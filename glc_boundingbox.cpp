/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.5, packaged on February, 2006.

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

//! \file glc_boundingbox.cpp implementation of the GLC_BoundingBox class.

#include "glc_boundingbox.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

// Default constructor
GLC_BoundingBox::GLC_BoundingBox()
: m_Lower(0, 0, 0)
, m_Upper(0, 0, 0)
{
	
}

// Copy constructor
GLC_BoundingBox::GLC_BoundingBox(const GLC_BoundingBox& boundingBox)
: m_Lower(boundingBox.m_Lower)
, m_Upper(boundingBox.m_Upper)
{
}

// Constructor with 2 points.
GLC_BoundingBox::GLC_BoundingBox(const GLC_Vector4d& lower, const GLC_Vector4d& upper)
: m_Lower(lower)
, m_Upper(upper)
{
	
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Test if a point is in the bounding Box
bool GLC_BoundingBox::intersect(const GLC_Vector4d& point) const
{
	bool result= (point.GetX() < m_Upper.GetX()) && (point.GetY() < m_Upper.GetY())
	&& (point.GetZ() < m_Upper.GetZ()) && (point.GetX() > m_Lower.GetX())
	&& (point.GetY() > m_Lower.GetY()) && (point.GetZ() > m_Lower.GetZ());
	
	return result;
}

// Get the lower corner of the bounding box
GLC_Vector4d GLC_BoundingBox::getLower(void) const
{
	return m_Lower;
}

// Get the upper corner of the bounding box
GLC_Vector4d GLC_BoundingBox::getUpper(void) const
{
	return m_Upper;
}

// Get the center of the bounding box
GLC_Vector4d GLC_BoundingBox::getCenter(void) const
{
	const double x= (m_Lower.GetX() + m_Upper.GetX()) / 2.0;
	const double y= (m_Lower.GetY() + m_Upper.GetY()) / 2.0;
	const double z= (m_Lower.GetZ() + m_Upper.GetZ()) / 2.0;
	
	GLC_Vector4d vectResult(x, y, z);
	return vectResult;
	
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Combine the bounding Box with new geometry point
void GLC_BoundingBox::combine(const GLC_Vector4d& point)
{
	double lowerX= fmin(point.GetX(), m_Lower.GetX());
	double lowerY= fmin(point.GetY(), m_Lower.GetY());
	double lowerZ= fmin(point.GetZ(), m_Lower.GetZ());
	m_Lower.SetVect(lowerX, lowerY, lowerZ);
	
	double upperX= fmax(point.GetX(), m_Upper.GetX());
	double upperY= fmax(point.GetY(), m_Upper.GetY());
	double upperZ= fmax(point.GetZ(), m_Upper.GetZ());	
	m_Upper.SetVect(upperX, upperY, upperZ);

}

// Combine the bounding Box with new geometry point
void GLC_BoundingBox::combine(const GLC_Vector3d& point)
{
	double lowerX= fmin(point.GetX(), m_Lower.GetX());
	double lowerY= fmin(point.GetY(), m_Lower.GetY());
	double lowerZ= fmin(point.GetZ(), m_Lower.GetZ());
	m_Lower.SetVect(lowerX, lowerY, lowerZ);
	
	double upperX= fmax(point.GetX(), m_Upper.GetX());
	double upperY= fmax(point.GetY(), m_Upper.GetY());
	double upperZ= fmax(point.GetZ(), m_Upper.GetZ());	
	m_Upper.SetVect(upperX, upperY, upperZ);

}

// Combine the bounding Box with another bounding box
void GLC_BoundingBox::combine(const GLC_BoundingBox& box)
{
	if (m_Lower.IsNull() && m_Upper.IsNull())
	{
		m_Lower= box.m_Lower;
		m_Upper= box.m_Upper;
	}
	else if (!box.m_Lower.IsNull() && !box.m_Upper.IsNull())
	{
		double lowerX= fmin(box.m_Lower.GetX(), m_Lower.GetX());
		double lowerY= fmin(box.m_Lower.GetY(), m_Lower.GetY());
		double lowerZ= fmin(box.m_Lower.GetZ(), m_Lower.GetZ());
		m_Lower.SetVect(lowerX, lowerY, lowerZ);
		
		double upperX= fmax(box.m_Upper.GetX(), m_Upper.GetX());
		double upperY= fmax(box.m_Upper.GetY(), m_Upper.GetY());
		double upperZ= fmax(box.m_Upper.GetZ(), m_Upper.GetZ());	
		m_Upper.SetVect(upperX, upperY, upperZ);
	}
	
}

