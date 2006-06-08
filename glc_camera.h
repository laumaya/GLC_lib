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

//! \file glc_camera.h Interface for the GLC_Camera class.

#ifndef GLC_CAMERA_H_
#define GLC_CAMERA_H_


#include "glc_object.h"

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
   GLC_Camera(const GLC_Vector4d &Eye, const GLC_Vector4d &Target, const GLC_Vector4d &Up,
	   const char *pName="Caméra" );
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Get the distance between the eye and the target of camera
	double GetDistEyeTarget(void) const;

	//! Get camera's eye coordinate vector
	const GLC_Vector4d GetVectEye(void) const
	{
		return VectEye;
	}

	//! Get camera's target coordinate vector
	const GLC_Vector4d GetVectTarget(void) const
	{
		return VectTarget;
	}

	//! Get camera's Up vector
	const GLC_Vector4d GetVectUp(void) const
	{
		return VectUp;
	}

	//! Get camera's Vector (from eye to target)
	GLC_Vector4d GetVectCam(void) const
	{
		return VectEye - VectTarget;
	}

	//! Get camera's orbit composition matrix
	const GLC_Matrix4x4 GetMatCompOrbit(void) const
	{
		return MatCompOrbit;
	}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:	
	//! Camera orbiting
	void Orbit(GLC_Vector4d VectOldPoss, GLC_Vector4d VectCurPoss);

   //! panoramic movement
   void Pan(GLC_Vector4d VectDep);

   //! Move camera's eye along camera vector (eye -> target)
   void Zoom(double Facteur);

   //! Move camera
	void Move(const GLC_Matrix4x4 &MatMove);

 	//! Camera translation
  	void Translate(const GLC_Vector4d &VectTrans);

   //! Set the camera
   bool SetCam(GLC_Vector4d Eye, GLC_Vector4d Target, GLC_Vector4d Up);

   //! Set camera's eye coordinate vector
   bool SetEyeCam(const GLC_Vector4d &Eye);
  
   //! Set camera's target coordinate vector
   bool SetTargetCam(const GLC_Vector4d &Target);
 
   //! Set camera's Up vector
   bool SetUpCam(const GLC_Vector4d &Up);
 
   //! Set the distance between eye and target (Move eye)
   void SetDistEyeTarget(double Longueur);

//@}
   
//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
   //! Execute OpenGL Camera
   void GlExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);

//@}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
private:
	//! compute composition matrix
 	void CreateMatComp(void);


//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////
private:
	//! Camera's eye vector
	GLC_Vector4d VectEye;
	//! Camera's target vector
	GLC_Vector4d VectTarget;
	//! Camera's Up vector
	GLC_Vector4d VectUp;
	
	//! Camera orbit composition matrix
	GLC_Matrix4x4 MatCompOrbit;
};
#endif //GLC_CAMERA_H_
