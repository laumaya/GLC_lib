/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.9, packaged on May, 2008.

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

//! \file glc_camera.h Interface for the GLC_Camera class.

#ifndef GLC_CAMERA_H_
#define GLC_CAMERA_H_

#include "glc_object.h"
#include "glc_vector4d.h"
#include "glc_matrix4x4.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Camera
/*! \brief GLC_Camera : OpenGL perpective viewpoint */

/*! An GLC_Camera define Viewpoint and orientation
 * of an OpenGL perpective camera*/
//////////////////////////////////////////////////////////////////////

class GLC_Camera : 
	public GLC_Object
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
   //! Default constructor
   /*! Point of view (0, 0, 1) Up Vector (0, 1, 0)*/
   GLC_Camera();
   //! Explicit constructor
	/* VectUp and VectCam could not be parallel
	 * VectUp could not be NULL
	 * VectCam could not be NULL */   
   GLC_Camera(const GLC_Point4d &, const GLC_Point4d &, const GLC_Vector4d &);
	//! Copy constructor
	GLC_Camera(const GLC_Camera&);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Get the distance between the eye and the target of camera
	double getDistEyeTarget(void) const;

	//! Get camera's eye coordinate point
	const GLC_Point4d getEye(void) const;

	//! Get camera's target coordinate point
	const GLC_Point4d getTarget(void) const;

	//! Get camera's Up vector
	const GLC_Vector4d getVectUp(void) const;

	//! Get camera's Vector (from eye to target)
	GLC_Vector4d getVectCam(void) const;

	//! Get camera's orbit composition matrix
	const GLC_Matrix4x4 getMatCompOrbit(void) const;
	
	//! equality operator
	bool operator==(const GLC_Camera&) const;	
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:	
	//! Camera orbiting
	void orbit(GLC_Vector4d VectOldPoss, GLC_Vector4d VectCurPoss);

   //! panoramic movement
   void pan(GLC_Vector4d VectDep);

   //! move camera's eye along camera vector (eye -> target)
   /*! Factor must be > 0*/
   void zoom(double factor);

   //! move camera
	void move(const GLC_Matrix4x4 &MatMove);

 	//! Camera translation
  	void translate(const GLC_Vector4d &VectTrans);

   //! Set the camera
	/* VectUp and VectCam could not be parallel
	 * VectUp could not be NULL
	 * VectCam could not be NULL */   
   void setCam(GLC_Point4d Eye, GLC_Point4d Target, GLC_Vector4d Up);
   
   //! Set the camera by copying another camera
   void setCam(const GLC_Camera&);

   //! Set camera's eye coordinate vector
   void setEyeCam(const GLC_Point4d &Eye);
  
   //! Set camera's target coordinate vector
   void setTargetCam(const GLC_Point4d &Target);
 
   //! Set camera's Up vector
   void setUpCam(const GLC_Vector4d &Up);
 
   //! Set the distance between eye and target (move eye)
   void setDistEyeTarget(double Longueur);
   
	//! Assignement operator
	GLC_Camera &operator=(const GLC_Camera&);	
   

//@}
   
//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
   //! Execute OpenGL Camera
   /*! Throw GLC_OpenGlException*/
   void glExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);

//@}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
private:
	//! compute composition matrix
 	void createMatComp(void);


//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////
private:
	//! Camera's eye point
	GLC_Point4d m_Eye;
	//! Camera's target point
	GLC_Point4d m_Target;
	//! Camera's Up vector
	GLC_Vector4d m_VectUp;
	
	//! Camera orbit composition matrix
	GLC_Matrix4x4 m_MatCompOrbit;
};
#endif //GLC_CAMERA_H_
