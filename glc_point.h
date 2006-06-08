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

//! \file glc_point.h interface for the GLC_Point class.

#ifndef GLC_POINT_H_
#define GLC_POINT_H_


#include "glc_geometry.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Point
/*! \brief GLC_Point : OpenGL 3D Point*/

/*! An GLC_Point is just a simple 3D Point*/
//////////////////////////////////////////////////////////////////////

class GLC_Point : public GLC_Geometry  
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:	
	//! Construct an GLC_Point
	GLC_Point(const GLC_Vector4d &VectSetCoord, const char *pName= "Point", const GLfloat *pColor= 0);

	//! Construct an GLC_Point
	GLC_Point(double x, double y, double z, const char *pName= "Point", const GLfloat *pColor= 0);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Get a 4D vector represent point coordinate
	GLC_Vector4d GetVectCoord(void) const;
	
	//! return the point bounding box
	virtual GLC_BoundingBox* getBoundingBox(void) const;
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set Point coordinate by 4D Vector
	void SetCoord(const GLC_Vector4d &Vect);

	//! Set Point coordinate by 3 double
	void SetCoord(double x, double y, double z);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////

private:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n*/
	virtual void GlDraw(void);

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function is implemented here.*/
	virtual void GlPropGeom(void);

//@}

//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////

private:
	//! 4d Vector for point coordinate
	GLC_Vector4d m_VectCoord;
	
};
#endif //GLC_POINT_H_
