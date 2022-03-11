/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Copyright (C) 2009 Laurent Bauer
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_camera.cpp Implementation of the GLC_Camera class.

#include "glc_camera.h"
#include "../glc_context.h"
#include "../glc_contextmanager.h"

#include <QtDebug>

using namespace glc;
//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Camera::GLC_Camera()
: QObject()
, m_Eye(0,0,1)
, m_Target()
, m_VectUp(Y_AXIS)
, m_ModelViewMatrix()
, m_DefaultVectUp(Y_AXIS)
{

}

GLC_Camera::GLC_Camera(const GLC_Point3d &Eye, const GLC_Point3d &Target, const GLC_Vector3d &Up)
: QObject()
, m_Eye()
, m_Target()
, m_VectUp()
, m_ModelViewMatrix()
, m_DefaultVectUp(Y_AXIS)
{
	setCam(Eye, Target, Up);
	createMatComp();
}

// Copy constructor
GLC_Camera::GLC_Camera(const GLC_Camera& cam)
: QObject()
, m_Eye(cam.m_Eye)
, m_Target(cam.m_Target)
, m_VectUp(cam.m_VectUp)
, m_ModelViewMatrix(cam.m_ModelViewMatrix)
, m_DefaultVectUp(cam.m_DefaultVectUp)
{

}

/////////////////////////////////////////////////////////////////////
// Get Functions
/////////////////////////////////////////////////////////////////////

bool GLC_Camera::operator==(const GLC_Camera& other) const
{
    return (m_Eye == other.m_Eye) && (m_Target == other.m_Target)
            && (m_VectUp == other.m_VectUp) && (m_DefaultVectUp == other.m_DefaultVectUp);
}


/////////////////////////////////////////////////////////////////////
// Set Functions
/////////////////////////////////////////////////////////////////////
void GLC_Camera::orbit(GLC_Vector3d vectOldPoss, GLC_Vector3d vectCurPoss)
{
	// Compute rotation matrix
    const GLC_Vector3d rotationAxis(vectCurPoss ^ vectOldPoss);
    // Check if rotation axis is not null
    if (!rotationAxis.isNull())
    {
        const double angle= acos(vectCurPoss * vectOldPoss);
        const GLC_Matrix4x4 invMat(m_ModelViewMatrix.inverted());
        const GLC_Matrix4x4 orbitMatrix(invMat * rotationAxis, angle);

		// Camera transformation
        m_Eye= (orbitMatrix * (m_Eye - m_Target)) + m_Target;
        m_VectUp= orbitMatrix * m_VectUp;
		createMatComp();
	}

    emit changed();
}

void GLC_Camera::pan(GLC_Vector3d VectDep)
{
	// Vector mapping
	GLC_Matrix4x4 invMat(m_ModelViewMatrix);
	invMat.invert();
	VectDep= invMat * VectDep;

	// Camera transformation
	m_Eye= m_Eye + VectDep;
	m_Target= m_Target + VectDep;

    emit changed();
}

void GLC_Camera::zoom(double factor)
{
	Q_ASSERT(factor > 0);
	// Eye->target vector
	GLC_Vector3d VectCam(m_Eye - m_Target);

	// Compute new vector length
	const double Norme= VectCam.length() * 1 / factor;
	VectCam.setLength(Norme);

	m_Eye= VectCam + m_Target;

    emit changed();
}

void GLC_Camera::move(const GLC_Matrix4x4 &MatMove)
{
	m_Eye= MatMove * m_Eye;
	m_Target= MatMove * m_Target;
	m_VectUp= MatMove.rotationMatrix() * m_VectUp;
	createMatComp();

    emit changed();
}

void GLC_Camera::rotateAround(const GLC_Vector3d& axis, const double& angle, const GLC_Point3d& point)
{
	const GLC_Matrix4x4 rotationMatrix(axis, angle);
	translate(-point);
	move(rotationMatrix);
	translate(point);

    emit changed();
}

void GLC_Camera::rotateAroundTarget(const GLC_Vector3d& axis, const double& angle)
{
	GLC_Point3d target(m_Target);
	rotateAround(axis, angle, target);

    emit changed();
}

void GLC_Camera::translate(const GLC_Vector3d &VectTrans)
{
	m_Eye= m_Eye + VectTrans;
	m_Target= m_Target + VectTrans;

    emit changed();
}

void GLC_Camera::setEyeCam(const GLC_Point3d &Eye)
{
	// Old camera's vector
	GLC_Vector3d VectOldCam(m_Eye - m_Target);
	// New camera's vector
	GLC_Vector3d VectCam(Eye - m_Target);
	if ( !(VectOldCam - VectCam).isNull() )
	{
		VectOldCam.setLength(1);
		VectCam.setLength(1);
		const double Angle= acos(VectOldCam * VectCam);
		if ( !glc::fuzzyCompare(Angle, 0.0) && !glc::fuzzyCompare(PI - Angle, 0.0))
		{
			const GLC_Vector3d VectAxeRot(VectOldCam ^ VectCam);
			const GLC_Matrix4x4 MatRot(VectAxeRot, Angle);
			m_VectUp= MatRot * m_VectUp;
		}
		else
		{
			if ( glc::fuzzyCompare(PI - Angle, 0.0))
			{	// Angle de 180%
				m_VectUp.invert();
			}
		}

		setCam(Eye, m_Target, m_VectUp);
	}
}

void GLC_Camera::setTargetCam(const GLC_Point3d &Target)
{
	// Old camera's vector
	GLC_Vector3d VectOldCam(m_Eye - m_Target);
	// New camera's vector
	GLC_Vector3d VectCam(m_Eye - Target);
	if ( !(VectOldCam - VectCam).isNull() )
	{
		VectOldCam.setLength(1);
		VectCam.setLength(1);
		const double Angle= acos(VectOldCam * VectCam);
		if ( !glc::fuzzyCompare(Angle, 0.0) && !glc::fuzzyCompare(PI - Angle, 0.0))
		{
			const GLC_Vector3d VectAxeRot(VectOldCam ^ VectCam);
			const GLC_Matrix4x4 MatRot(VectAxeRot, Angle);
			m_VectUp= MatRot * m_VectUp;
		}
		else
		{
			if ( glc::fuzzyCompare(PI - Angle, 0.0))
			{	// Angle of 180%
				m_VectUp.invert();
			}
		}

		setCam(m_Eye, Target, m_VectUp);
	}
}

void GLC_Camera::setUpCam(const GLC_Vector3d &Up)
{
	if ( !(m_VectUp - Up).isNull() )
	{
        if (!glc::fuzzyCompare(forward().angleWithVect(Up), 0.0))
		{
			setCam(m_Eye, m_Target, Up);
		}
	}
}

void GLC_Camera::setCam(GLC_Point3d Eye, GLC_Point3d Target, GLC_Vector3d Up)
{
	Up.setLength(1);

	const GLC_Vector3d VectCam((Eye - Target).setLength(1));
	const double Angle= acos(VectCam * Up);

	/* m_VectUp and VectCam could not be parallel
	 * m_VectUp could not be NULL
	 * VectCam could not be NULL */
	//Q_ASSERT((Angle > EPSILON) && ((PI - Angle) > EPSILON));

	if ( !glc::fuzzyCompare(Angle - (PI / 2), 0.0))
	{	// Angle not equal to 90
		const GLC_Vector3d AxeRot(VectCam ^ Up);
		GLC_Matrix4x4 MatRot(AxeRot, PI / 2);
		Up= MatRot * VectCam;
	}

	m_Eye= Eye;
	m_Target= Target;
	m_VectUp= Up;
	createMatComp();

    emit changed();
}

//! Set the camera by copying another camera
void GLC_Camera::setCam(const GLC_Camera& cam)
{
	m_Eye= cam.m_Eye;
	m_Target= cam.m_Target;
	m_VectUp= cam.m_VectUp;
	m_ModelViewMatrix= cam.m_ModelViewMatrix;

    emit changed();
}


void GLC_Camera::setDistEyeTarget(double Longueur)
{
    GLC_Vector3d VectCam(forward());
    VectCam.setLength(Longueur);
    m_Eye= m_Target - VectCam;

    emit changed();
}

void GLC_Camera::setDistTargetEye(double Longueur)
{
    GLC_Vector3d VectCam(forward());
    VectCam.setLength(Longueur);
    m_Target= m_Eye + VectCam;

    emit changed();
}

// Assignment operator
GLC_Camera &GLC_Camera::operator=(const GLC_Camera& cam)
{
    QObject::setObjectName(cam.objectName());
	m_Eye= cam.m_Eye;
	m_Target= cam.m_Target;
	m_VectUp= cam.m_VectUp;
	m_ModelViewMatrix= cam.m_ModelViewMatrix;
	m_DefaultVectUp= cam.m_DefaultVectUp;

    emit changed();

    return *this;
}

void GLC_Camera::setDefaultUpVector(const GLC_Vector3d& up)
{
    m_DefaultVectUp= up;
    emit changed();
}

void GLC_Camera::setDefaultUpVectorByName(const QString &vectorName)
{
    GLC_Vector3d upVector= m_DefaultVectUp;

    if (vectorName == "x")
    {
        upVector= glc::X_AXIS;
    }
    else if (vectorName == "y")
    {
        upVector= glc::Y_AXIS;
    }
    else if (vectorName == "z")
    {
        upVector= glc::Z_AXIS;
    }

    setDefaultUpVector(upVector);
}

bool GLC_Camera::isAlmostEqualTo(const GLC_Camera& cam, const double distanceAccuracy) const
{
      GLC_Vector3d incident1 = m_Target - m_Eye;
      GLC_Vector3d incident2 = cam.m_Target - cam.m_Eye;

      double allowedGap =  incident1.length() * distanceAccuracy;
      GLC_Point3d left1 = incident1 ^ m_VectUp;
      GLC_Point3d left2 = incident2 ^ cam.m_VectUp;

      return ((m_Eye - cam.m_Eye).length() < allowedGap ) && ( (m_Target - cam.m_Target).length() < allowedGap)
              && ((left1 - left2).length() < allowedGap) ;
}

QString GLC_Camera::defaultUpVectorName() const
{
    QString subject;
    if (m_DefaultVectUp == glc::X_AXIS)
    {
        subject= "x";
    }
    else if (m_DefaultVectUp == glc::Y_AXIS)
    {
        subject= "y";
    }
    else
    {
        Q_ASSERT(m_DefaultVectUp == glc::Z_AXIS);
        subject= "z";
    }

    return subject;
}

GLC_Camera GLC_Camera::frontView() const
{
	GLC_Vector3d eye;

	if (m_DefaultVectUp == glc::Z_AXIS)
	{
		eye.setVect(0.0, -1.0, 0.0);
	}
	else // Y_AXIS or X_AXIS
	{
		eye.setVect(0.0, 0.0, 1.0);
	}
	eye= eye + m_Target;

	GLC_Camera newCam(eye, m_Target, m_DefaultVectUp);
	newCam.setDistEyeTarget(distEyeTarget());
	newCam.setDefaultUpVector(m_DefaultVectUp);
	return newCam;
}

GLC_Camera GLC_Camera::rearView() const
{
    GLC_Camera subject= frontView();
    subject.rotateAroundTarget(m_DefaultVectUp, glc::PI);

    return subject;
}

GLC_Camera GLC_Camera::rightView() const
{
    GLC_Camera subject= frontView();
    subject.rotateAroundTarget(m_DefaultVectUp, glc::PI / 2.0);

    return subject;
}



GLC_Camera GLC_Camera::leftView() const
{
    GLC_Camera subject= frontView();
    subject.rotateAroundTarget(m_DefaultVectUp, - glc::PI / 2.0);

    return subject;
}

GLC_Camera GLC_Camera::topView() const
{
	GLC_Vector3d eye= m_DefaultVectUp;
	eye= eye + m_Target;
	GLC_Vector3d up;

	if (m_DefaultVectUp == glc::Y_AXIS)
	{
		up.setVect(0.0, 0.0, -1.0);
	}
	else // Z_AXIS or X_AXIS
	{
		up.setVect(0.0, 1.0, 0.0);
	}

	GLC_Camera newCam(eye, m_Target, up);
	newCam.setDistEyeTarget(distEyeTarget());
	newCam.setDefaultUpVector(m_DefaultVectUp);

	return newCam;
}

GLC_Camera GLC_Camera::bottomView() const
{
	GLC_Camera newCam(topView());
	newCam.rotateAroundTarget(newCam.upVector(), glc::PI);

	return newCam;
}

GLC_Camera GLC_Camera::isoView() const
{
	GLC_Vector3d eye;
	if (m_DefaultVectUp == glc::Z_AXIS)
	{
		eye.setVect(-1.0, -1.0, 1.0);
	}
	else if (m_DefaultVectUp == glc::Y_AXIS)
	{
		eye.setVect(-1.0, 1.0, 1.0);
	}
	else
	{
		eye.setVect(1.0, 1.0, 1.0);
	}

	eye= eye + m_Target;

	GLC_Camera newCam(eye, m_Target, m_DefaultVectUp);
	newCam.setDistEyeTarget(distEyeTarget());
	newCam.setDefaultUpVector(m_DefaultVectUp);
	return newCam;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
void GLC_Camera::glExecute()
{
    GLC_ContextManager::instance()->currentContext()->glcMultMatrix(modelViewMatrix());
}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

void GLC_Camera::createMatComp(void)
{
	const GLC_Vector3d forward((m_Target - m_Eye).normalize());
	const GLC_Vector3d side((forward ^ m_VectUp).normalize());

	// Update camera matrix
	m_ModelViewMatrix.setData()[0]= side.x();
	m_ModelViewMatrix.setData()[4]= side.y();
	m_ModelViewMatrix.setData()[8]= side.z();
	m_ModelViewMatrix.setData()[12]= 0.0;

	// Vector Up is Y Axis
	m_ModelViewMatrix.setData()[1]= m_VectUp.x();
	m_ModelViewMatrix.setData()[5]= m_VectUp.y();
	m_ModelViewMatrix.setData()[9]= m_VectUp.z();
	m_ModelViewMatrix.setData()[13]= 0.0;

	// Vector Cam is Z axis
	m_ModelViewMatrix.setData()[2]= - forward.x();
	m_ModelViewMatrix.setData()[6]= - forward.y();
	m_ModelViewMatrix.setData()[10]= - forward.z();
	m_ModelViewMatrix.setData()[14]= 0.0;

	m_ModelViewMatrix.setData()[3]= 0.0;
	m_ModelViewMatrix.setData()[7]= 0.0;
	m_ModelViewMatrix.setData()[11]= 0.0;
	m_ModelViewMatrix.setData()[15]= 1.0;

}
