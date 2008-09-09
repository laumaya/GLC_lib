/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_camera.cpp Implementation of the GLC_Camera class.


#include "glc_camera.h"
#include "glc_openglexception.h"

#include <QtDebug>

using namespace glc;
//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Camera::GLC_Camera()
: GLC_Object("Camera")
, m_Eye(0,0,1)
, m_Target()
, m_VectUp(0,1,0)
{
	
}

GLC_Camera::GLC_Camera(const GLC_Point4d &Eye, const GLC_Point4d &Target, const GLC_Vector4d &Up)
: GLC_Object("Camera")
, m_Eye()
, m_Target()
, m_VectUp()

{
	setCam(Eye, Target, Up);	
	createMatComp();
}

// Copy constructor
GLC_Camera::GLC_Camera(const GLC_Camera& cam)
: GLC_Object(cam)
, m_Eye(cam.m_Eye)
, m_Target(cam.m_Target)
, m_VectUp(cam.m_VectUp)
, m_MatCompOrbit(cam.m_MatCompOrbit)
{
	
}

/////////////////////////////////////////////////////////////////////
// Get Functions
/////////////////////////////////////////////////////////////////////

// Get the distance between the eye and the target of camera
double GLC_Camera::getDistEyeTarget(void) const
{
	return (m_Eye - m_Target).getNorm();
}

// Get camera's eye coordinate point
const GLC_Point4d GLC_Camera::getEye(void) const
{
	return m_Eye;
}

// Get camera's target coordinate point
const GLC_Point4d GLC_Camera::getTarget(void) const
{
	return m_Target;
}

// Get camera's Up vector
const GLC_Vector4d GLC_Camera::getVectUp(void) const
{
	return m_VectUp;
}

// Get camera's Vector (from eye to target)
GLC_Vector4d GLC_Camera::getVectCam(void) const
{
	return m_Eye - m_Target;
}

// Get camera's orbit composition matrix
const GLC_Matrix4x4 GLC_Camera::getMatCompOrbit(void) const
{
	return m_MatCompOrbit;
}

// equality operator
bool GLC_Camera::operator==(const GLC_Camera& cam) const
{
	return (m_Eye == cam.m_Eye) && (m_Target == cam.m_Target)
			&& (m_VectUp == cam.m_VectUp);
}


/////////////////////////////////////////////////////////////////////
// Set Functions
/////////////////////////////////////////////////////////////////////
void GLC_Camera::orbit(GLC_Vector4d VectOldPoss, GLC_Vector4d VectCurPoss)
{
	// Map Vectors
	VectOldPoss= m_MatCompOrbit * VectOldPoss;
	VectCurPoss= m_MatCompOrbit * VectCurPoss;
	
	// Compute rotation matrix
	const GLC_Vector4d VectAxeRot(VectCurPoss ^ VectOldPoss);
	// Check if rotation vector is not null
	if (!VectAxeRot.isNull())
	{  // Ok, is not null
		const double Angle= acos(VectCurPoss * VectOldPoss);
		const GLC_Matrix4x4 MatOrbit(VectAxeRot, Angle);
	
		// Camera transformation
		m_Eye= (MatOrbit * (m_Eye - m_Target)) + m_Target;
		m_VectUp= MatOrbit * m_VectUp;
		m_MatCompOrbit= MatOrbit * m_MatCompOrbit;
	}
}

void GLC_Camera::pan(GLC_Vector4d VectDep)
{
	// Vector mapping
	VectDep= m_MatCompOrbit * VectDep;
	
	// Camera transformation
	m_Eye= m_Eye + VectDep;
	m_Target= m_Target + VectDep;
}

void GLC_Camera::zoom(double factor)
{
	Q_ASSERT(factor > 0);
	// Eye->target vector
	GLC_Vector4d VectCam(m_Eye - m_Target);
	
	// Compute new vector length
	const double Norme= VectCam.getNorm() * 1 / factor;
	VectCam.setNormal(Norme);
	
	m_Eye= VectCam + m_Target;
	
}

void GLC_Camera::move(const GLC_Matrix4x4 &MatMove)
{
	m_Eye= MatMove * m_Eye;
	m_Target= MatMove * m_Target;
	
	// Up vector computation	
	// In case of translation in matrix
	const GLC_Vector4d VectOrigine(0,0,0);
	// Backup m_VectUp
	const GLC_Vector4d VectUpOld(m_VectUp);
	m_VectUp= (MatMove * VectUpOld) - (MatMove * VectOrigine); // Up Vector Origin must be equal to 0,0,0
	createMatComp();
}

void GLC_Camera::translate(const GLC_Vector4d &VectTrans)
{
	m_Eye= m_Eye + VectTrans;
	m_Target= m_Target + VectTrans;
}

void GLC_Camera::setEyeCam(const GLC_Point4d &Eye)
{
	// Old camera's vector
	GLC_Vector4d VectOldCam(m_Eye - m_Target);
	// New camera's vector
	GLC_Vector4d VectCam(Eye - m_Target);
	if ( !(VectOldCam - VectCam).isNull() )
	{
		VectOldCam.setNormal(1);
		VectCam.setNormal(1);
		const double Angle= acos(VectOldCam * VectCam);
		if ( (Angle > EPSILON) && ( (PI - Angle) > EPSILON) )
		{
			const GLC_Vector4d VectAxeRot(VectOldCam ^ VectCam);
			const GLC_Matrix4x4 MatRot(VectAxeRot, Angle);
			m_VectUp= MatRot * m_VectUp;
		}
		else
		{
			if ( (PI - Angle) < EPSILON)
			{	// Angle de 180�
				m_VectUp.setInv();
			}
		}
		
		setCam(Eye, m_Target, m_VectUp);
	}
}

void GLC_Camera::setTargetCam(const GLC_Point4d &Target)
{
	// Old camera's vector
	GLC_Vector4d VectOldCam(m_Eye - m_Target);
	// New camera's vector
	GLC_Vector4d VectCam(m_Eye - Target);
	if ( !(VectOldCam - VectCam).isNull() )
	{
		VectOldCam.setNormal(1);
		VectCam.setNormal(1);
		const double Angle= acos(VectOldCam * VectCam);
		if ( (Angle > EPSILON) && ( (PI - Angle) > EPSILON) )
		{
			const GLC_Vector4d VectAxeRot(VectOldCam ^ VectCam);
			const GLC_Matrix4x4 MatRot(VectAxeRot, Angle);
			m_VectUp= MatRot * m_VectUp;
		}
		else
		{
			if ( (PI - Angle) < EPSILON)
			{	// Angle of 180�
				m_VectUp.setInv();
			}
		}
		
		setCam(m_Eye, Target, m_VectUp);
	}
}

void GLC_Camera::setUpCam(const GLC_Vector4d &Up)
{
	if ( !(m_VectUp - Up).isNull() )
	{
		setCam(m_Eye, m_Target, Up);
	}
}

void GLC_Camera::setCam(GLC_Point4d Eye, GLC_Point4d Target, GLC_Vector4d Up)
{
	Up.setNormal(1);

	const GLC_Vector4d VectCam((Eye - Target).setNormal(1));
	const double Angle= acos(VectCam * Up);
	
	/* m_VectUp and VectCam could not be parallel
	 * m_VectUp could not be NULL
	 * VectCam could not be NULL */
	//Q_ASSERT((Angle > EPSILON) && ((PI - Angle) > EPSILON));
	if ( fabs(Angle - (PI / 2)) > EPSILON)
	{	// Angle not equal to 90�
		const GLC_Vector4d AxeRot(VectCam ^ Up);
		GLC_Matrix4x4 MatRot(AxeRot, PI / 2);
		Up= MatRot * VectCam;
	}	

	m_Eye= Eye;
	m_Target= Target;
	m_VectUp= Up;
	createMatComp();
}

//! Set the camera by copying another camera
void GLC_Camera::setCam(const GLC_Camera& cam)
{
	m_Eye= cam.m_Eye;
	m_Target= cam.m_Target;
	m_VectUp= cam.m_VectUp;
	m_MatCompOrbit= cam.m_MatCompOrbit;
}


void GLC_Camera::setDistEyeTarget(double Longueur)
{
    GLC_Vector4d VectCam(m_Eye - m_Target);
    VectCam.setNormal(Longueur);
    m_Eye= VectCam + m_Target;
}
// Assignement operator
GLC_Camera &GLC_Camera::operator=(const GLC_Camera& cam)
{
	GLC_Object::operator=(cam);
	m_Eye= cam.m_Eye;
	m_Target= cam.m_Target;
	m_VectUp= cam.m_VectUp;
	m_MatCompOrbit= cam.m_MatCompOrbit;
	return *this;
}	

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
void GLC_Camera::glExecute(GLenum Mode)
{
	gluLookAt(m_Eye.getX(), m_Eye.getY(), m_Eye.getZ(),
		m_Target.getX(), m_Target.getY(), m_Target.getZ(),
		m_VectUp.getX(), m_VectUp.getY(), m_VectUp.getZ());
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Camera::GlExecute ", error);
		throw(OpenGlException);
	}
		
}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

void GLC_Camera::createMatComp(void)
{
	// Calcule de la matrice de rotation entre VectZ et VectCam
	// Compute rotation matrix between Z axis and camera
	const GLC_Vector4d VectCam((m_Eye - m_Target).setNormal(1));
	
	if ( fabs( fabs(VectCam.getZ()) - 1.0 ) > EPSILON)
	{ // Camera's vector not equal to Z axis
		const GLC_Vector4d VectAxeRot= (AxeZ ^ VectCam);
		const double Angle= acos(AxeZ * VectCam);
		m_MatCompOrbit.setMatRot(VectAxeRot, Angle);
	}
	else // Camera's Vector equal to Z axis
	{		
		if (VectCam.getZ() < 0)
		{
						
			m_MatCompOrbit.setMatRot(m_VectUp, PI);
		}
		else
			m_MatCompOrbit.setToIdentity();
	}

	// Angle between InitVectUp and m_VectUp
	GLC_Vector4d InitVectUp(0,1,0); // Par d�faut m_VectUp est Y
	InitVectUp= m_MatCompOrbit * InitVectUp;
	// Compute the angle
	const double AngleVectUp= acos(InitVectUp * m_VectUp);
	
	GLC_Matrix4x4 MatInt; // intermediate matrix

	if (( AngleVectUp > EPSILON) && ( (PI - AngleVectUp) > EPSILON) )

	{ // Angle not equal to 0 or 180�
		const GLC_Vector4d VectAxeRot(InitVectUp ^ m_VectUp);

		MatInt.setMatRot(VectAxeRot, AngleVectUp);
	}
	else	// Angle equal to 0 or 180�
	{
		MatInt.setMatRot(VectCam, AngleVectUp);
	}

	m_MatCompOrbit= MatInt * m_MatCompOrbit;	
}


