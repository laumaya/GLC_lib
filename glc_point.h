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

//! \file glc_point.h interface for the GLC_Point class.

#ifndef GLC_POINT_H_
#define GLC_POINT_H_

#include "glc_vbogeom.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Point
/*! \brief GLC_Point : OpenGL 3D Point*/

/*! An GLC_Point is just a simple 3D Point*/
//////////////////////////////////////////////////////////////////////

class GLC_Point : public GLC_VboGeom 
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:	
	//! Construct an GLC_Point
	GLC_Point(const GLC_Point4d &);

	//! Construct an GLC_Point
	GLC_Point(double, double, double);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Get a 4D point represent point coordinate
	GLC_Point4d getCoord(void) const;
	
	//! return the point bounding box
	virtual GLC_BoundingBox* getBoundingBox(void) const;
	
	//! Return a copy of the geometry
	virtual GLC_VboGeom* clone() const;
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set Point coordinate by 4D point
	void setCoord(const GLC_Point4d &);

	//! Set Point coordinate by 3 double
	void setCoord(double x, double y, double z);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////

private:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n*/
	virtual void glDraw(void);

//@}

//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////

private:
	//! 4d point for point coordinate
	GLC_Point4d m_Coord;
	
};
#endif //GLC_POINT_H_
