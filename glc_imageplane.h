/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9, packaged on Novemeber, 2005.

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

//! \file glc_imagePlane.h interface for the GLC_ImagePlane class.

#ifndef GLC_IMAGEPLANE_H_
#define GLC_IMAGEPLANE_H_

#include "glc_geometrie.h"


class GLC_Viewport;

//////////////////////////////////////////////////////////////////////
//! \class GLC_ImagePlane
/*! \brief GLC_ImagePlane : Viewport background image*/

/*! An GLC_ImagePlane is just a plane with a image texture.*/
//////////////////////////////////////////////////////////////////////

class GLC_ImagePlane : public GLC_Geometrie
{
//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
public:
	//! Construct an Image plane linked to a viewport
	GLC_ImagePlane(GLC_Viewport* pViewport, const char* pName="Image Plane"
		, const GLfloat *pColor= NULL);

	//! Remove OpenGL Texture from memmory
	virtual ~GLC_ImagePlane(void);

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Load image
	bool ChargeImage(QGLWidget *GLWidget, const QString ImageName);

	//! Update image plane size
	void UpdatePlaneSize(void);

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n*/
	virtual void GlDraw(void);

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function is implemented here.*/
	virtual void GlPropGeom(void);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////

private:

	//! Image texture to bind on plane
	GLC_Texture* m_pImgTexture;

	//! ViewPort
	GLC_Viewport* m_pViewport;

	//! Image with
	double m_dLgImage;

	//! Plane Position in Z direction
	double m_dZpos;

};

#endif //GLC_IMAGEPLANE_H_
