/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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

#include <QtDebug>

#include "glc_maths.h"
#include "glc_camera.h"
#include "glc_openglexception.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Camera::GLC_Camera()
:GLC_Object("Caméra"),
VectEye(0,0,1), VectUp(0,1,0)
{
	
}

GLC_Camera::GLC_Camera(const GLC_Vector4d &Eye, const GLC_Vector4d &Target, const GLC_Vector4d &Up
					   , const char *pName)
:GLC_Object(pName)
{
	SetCam(Eye, Target, Up);	
	CreateMatComp();
}

/////////////////////////////////////////////////////////////////////
// Get Functions
/////////////////////////////////////////////////////////////////////

// Get the distance between the eye and the target of camera
double GLC_Camera::GetDistEyeTarget(void) const
{
	return (VectEye - VectTarget).GetNorme();
}

// Get camera's eye coordinate vector
const GLC_Vector4d GLC_Camera::GetVectEye(void) const
{
	return VectEye;
}

// Get camera's target coordinate vector
const GLC_Vector4d GLC_Camera::GetVectTarget(void) const
{
	return VectTarget;
}

// Get camera's Up vector
const GLC_Vector4d GLC_Camera::GetVectUp(void) const
{
	return VectUp;
}

// Get camera's Vector (from eye to target)
GLC_Vector4d GLC_Camera::GetVectCam(void) const
{
	return VectEye - VectTarget;
}

// Get camera's orbit composition matrix
const GLC_Matrix4x4 GLC_Camera::GetMatCompOrbit(void) const
{
	return MatCompOrbit;
}


/////////////////////////////////////////////////////////////////////
// Set Functions
/////////////////////////////////////////////////////////////////////
void GLC_Camera::Orbit(GLC_Vector4d VectOldPoss, GLC_Vector4d VectCurPoss)
{
	// Map Vectors
	VectOldPoss= MatCompOrbit * VectOldPoss;
	VectCurPoss= MatCompOrbit * VectCurPoss;
	
	// Compute rotation matrix
	const GLC_Vector4d VectAxeRot(VectCurPoss ^ VectOldPoss);
	// Check if rotation vector is not null
	if (!VectAxeRot.IsNull())
	{  // Ok, is not null
		const double Angle= acos(VectCurPoss * VectOldPoss);
		const GLC_Matrix4x4 MatOrbit(VectAxeRot, Angle);
	
		// Camera transformation
		VectEye= (MatOrbit * (VectEye - VectTarget)) + VectTarget;
		VectUp= MatOrbit * VectUp;
		MatCompOrbit= MatOrbit * MatCompOrbit;
	}
}

void GLC_Camera::Pan(GLC_Vector4d VectDep)
{
	// Vector mapping
	VectDep= MatCompOrbit * VectDep;
	
	// Camera transformation
	VectEye= VectEye + VectDep;
	VectTarget= VectTarget + VectDep;
}

void GLC_Camera::Zoom(double factor)
{
	assert(factor > 0);
	// Eye->target vector
	GLC_Vector4d VectCam(VectEye - VectTarget);
	
	// Compute new vector length
	const double Norme= VectCam.GetNorme() * 1 / factor;
	VectCam.SetNormal(Norme);
	
	VectEye= VectCam + VectTarget;
	
}

void GLC_Camera::Move(const GLC_Matrix4x4 &MatMove)
{
	VectEye= MatMove * VectEye;
	VectTarget= MatMove * VectTarget;
	
	// Up vector computation	
	// In case of translation in matrix
	const GLC_Vector4d VectOrigine(0,0,0);
	// Backup VectUp
	const GLC_Vector4d VectUpOld(VectUp);
	VectUp= (MatMove * VectUpOld) - (MatMove * VectOrigine); // Up Vector Origin must be equal to 0,0,0
	CreateMatComp();
}

void GLC_Camera::Translate(const GLC_Vector4d &VectTrans)
{
	VectEye= VectEye + VectTrans;
	VectTarget= VectTarget + VectTrans;
}

void GLC_Camera::SetEyeCam(const GLC_Vector4d &Eye)
{
	// Old camera's vector
	GLC_Vector4d VectOldCam(VectEye - VectTarget);
	// New camera's vector
	GLC_Vector4d VectCam(Eye - VectTarget);
	if ( !(VectOldCam - VectCam).IsNull() )
	{
		VectOldCam.SetNormal(1);
		VectCam.SetNormal(1);
		const double Angle= acos(VectOldCam * VectCam);
		if ( (Angle > EPSILON) && ( (PI - Angle) > EPSILON) )
		{
			const GLC_Vector4d VectAxeRot(VectOldCam ^ VectCam);
			const GLC_Matrix4x4 MatRot(VectAxeRot, Angle);
			VectUp= MatRot * VectUp;
		}
		else
		{
			if ( (PI - Angle) < EPSILON)
			{	// Angle de 180°
				VectUp.SetInv();
			}
		}
		
		SetCam(Eye, VectTarget, VectUp);
	}
}

void GLC_Camera::SetTargetCam(const GLC_Vector4d &Target)
{
	// Old camera's vector
	GLC_Vector4d VectOldCam(VectEye - VectTarget);
	// New camera's vector
	GLC_Vector4d VectCam(VectEye - Target);
	if ( !(VectOldCam - VectCam).IsNull() )
	{
		VectOldCam.SetNormal(1);
		VectCam.SetNormal(1);
		const double Angle= acos(VectOldCam * VectCam);
		if ( (Angle > EPSILON) && ( (PI - Angle) > EPSILON) )
		{
			const GLC_Vector4d VectAxeRot(VectOldCam ^ VectCam);
			const GLC_Matrix4x4 MatRot(VectAxeRot, Angle);
			VectUp= MatRot * VectUp;
		}
		else
		{
			if ( (PI - Angle) < EPSILON)
			{	// Angle of 180°
				VectUp.SetInv();
			}
		}
		
		SetCam(VectEye, Target, VectUp);
	}
}

void GLC_Camera::SetUpCam(const GLC_Vector4d &Up)
{
	if ( !(VectUp - Up).IsNull() )
	{
		SetCam(VectEye, VectTarget, Up);
	}
}

void GLC_Camera::SetCam(GLC_Vector4d Eye, GLC_Vector4d Target, GLC_Vector4d Up)
{
	Up.SetNormal(1);

	const GLC_Vector4d VectCam((Eye - Target).SetNormal(1));
	const double Angle= acos(VectCam * Up);
	
	/* VectUp and VectCam could not be parallel
	 * VectUp could not be NULL
	 * VectCam could not be NULL */
	assert((Angle > EPSILON) && ((PI - Angle) > EPSILON));
	if ( fabs(Angle - (PI / 2)) > EPSILON)
	{	// Angle not equal to 90°
		const GLC_Vector4d AxeRot(VectCam ^ Up);
		GLC_Matrix4x4 MatRot(AxeRot, PI / 2);
		Up= MatRot * VectCam;
	}	

	VectEye= Eye;
	VectTarget= Target;
	VectUp= Up;
	CreateMatComp();
}


void GLC_Camera::SetDistEyeTarget(double Longueur)
{
    GLC_Vector4d VectCam(VectEye - VectTarget);
    VectCam.SetNormal(Longueur);
    VectEye= VectCam + VectTarget;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
void GLC_Camera::GlExecute(GLenum Mode)
{
	gluLookAt(VectEye.GetX(), VectEye.GetY(), VectEye.GetZ(),
		VectTarget.GetX(), VectTarget.GetY(), VectTarget.GetZ(),
		VectUp.GetX(), VectUp.GetY(), VectUp.GetZ());
	
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

void GLC_Camera::CreateMatComp(void)
{
	// Calcule de la matrice de rotation entre VectZ et VectCam
	// Compute rotation matrix between Z axis and camera
	const GLC_Vector4d VectCam((VectEye - VectTarget).SetNormal(1));
	
	const GLC_Vector4d VectZ(0,0,1);
	if ( fabs( fabs(VectCam.GetZ()) - 1.0 ) > EPSILON)
	{ // Camera's vector not equal to Z axis
		const GLC_Vector4d VectAxeRot= (VectZ ^ VectCam);
		const double Angle= acos(VectZ * VectCam);
		MatCompOrbit.SetMatRot(VectAxeRot, Angle);
	}
	else // Camera's Vector equal to Z axis
	{		
		if (VectCam.GetZ() < 0)
		{
						
			MatCompOrbit.SetMatRot(VectUp, PI);
		}
		else
			MatCompOrbit.SetIdentite();
	}

	// Angle between InitVectUp and VectUp
	GLC_Vector4d InitVectUp(0,1,0); // Par défaut VectUp est Y
	InitVectUp= MatCompOrbit * InitVectUp;
	// Compute the angle
	const double AngleVectUp= acos(InitVectUp * VectUp);
	
	GLC_Matrix4x4 MatInt; // intermediate matrix

	if (( AngleVectUp > EPSILON) && ( (PI - AngleVectUp) > EPSILON) )

	{ // Angle not equal to 0 or 180°
		const GLC_Vector4d VectAxeRot(InitVectUp ^ VectUp);

		MatInt.SetMatRot(VectAxeRot, AngleVectUp);
	}
	else	// Angle equal to 0 or 180°
	{
		MatInt.SetMatRot(VectCam, AngleVectUp);
	}

	MatCompOrbit= MatInt * MatCompOrbit;	
}


