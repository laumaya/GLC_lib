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

//! \file glc_boundingbox.h interface for the GLC_BoundingBox class.

#ifndef GLC_BOUNDINGBOX_
#define GLC_BOUNDINGBOX_

#include "glc_maths.h"
#include "glc_utils_maths.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_BoundingBox
/*! \brief GLC_BoundingBox : Geometry bounding box*/

/*! An GLC_BoundingBox is a non oriented bounding box
*/
 
//////////////////////////////////////////////////////////////////////

class GLC_BoundingBox
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor */
//@{
//////////////////////////////////////////////////////////////////////
public:	
	//! Default constructor
	GLC_BoundingBox();
	
	//! Copy constructor
	GLC_BoundingBox(const GLC_BoundingBox& boundingBox);
	
	//! Constructor with 2 points.
	GLC_BoundingBox(const GLC_Vector4d& lower, const GLC_Vector4d& upper);
	
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////

	//! Get the empty state of the bounding Box
	bool isEmpty(void) const
	{
		return m_IsEmpty;
	}
	
	//! Test if a point is in the bounding Box
	bool intersect(const GLC_Vector4d& point) const;

	//! Return the max distance between a point and a corner of the bounding box
	//double maxDistance(const GLC_Vector4d& point) const;
	
	//! Get the lower corner of the bounding box
	GLC_Vector4d getLower(void) const;
	
	//! Get the upper corner of the bounding box
	GLC_Vector4d getUpper(void) const;
	
	//! Get the center of the bounding box
	GLC_Vector4d getCenter(void) const;
		
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Combine the bounding Box with new geometry point
	GLC_BoundingBox& combine(const GLC_Vector4d& point);

	//! Combine the bounding Box with new geometry point
	GLC_BoundingBox& combine(const GLC_Vector3d& point);

	//! Combine the bounding Box with another bounding box
	GLC_BoundingBox& combine(const GLC_BoundingBox& box);

	//! Transform the bounding Box
	GLC_BoundingBox& transform(const GLC_Matrix4x4& matrix);
	

//@}


//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	GLC_Vector4d m_Lower;
	GLC_Vector4d m_Upper;
	bool m_IsEmpty;

};
#endif /*GLC_BOUNDINGBOX_*/
