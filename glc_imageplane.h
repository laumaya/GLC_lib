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

//! \file glc_imagePlane.h interface for the GLC_ImagePlane class.

#ifndef GLC_IMAGEPLANE_H_
#define GLC_IMAGEPLANE_H_

#include "glc_geometry.h"


class GLC_Viewport;

//////////////////////////////////////////////////////////////////////
//! \class GLC_ImagePlane
/*! \brief GLC_ImagePlane : Viewport background image*/

/*! An GLC_ImagePlane is just a plane with a image texture.*/
//////////////////////////////////////////////////////////////////////

class GLC_ImagePlane : public GLC_Geometry
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Construct an Image plane linked to a viewport
	GLC_ImagePlane(GLC_Viewport* pViewport, const QColor& color);

	//! Remove OpenGL Texture from memmory
	virtual ~GLC_ImagePlane(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Load image
	bool loadImageFile(QGLWidget *GLWidget, const QString ImageName);

	//! Update image plane size
	void updatePlaneSize(void);
	
	//! Update Plane Z position
	void updateZPosition(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n*/
	virtual void glDraw(void);

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function is implemented here.*/
	virtual void glPropGeom(void);
//@}

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
	
	//! Polygons display style
	GLenum m_PolyFace;
	GLenum m_PolyMode;

};

#endif //GLC_IMAGEPLANE_H_
