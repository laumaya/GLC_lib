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
, m_IsEmpty(true)
{
	
}

// Copy constructor
GLC_BoundingBox::GLC_BoundingBox(const GLC_BoundingBox& boundingBox)
: m_Lower(boundingBox.m_Lower)
, m_Upper(boundingBox.m_Upper)
, m_IsEmpty(boundingBox.m_IsEmpty)
{
}

// Constructor with 2 points.
GLC_BoundingBox::GLC_BoundingBox(const GLC_Vector4d& lower, const GLC_Vector4d& upper)
: m_Lower(lower)
, m_Upper(upper)
, m_IsEmpty(false)
{
	
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Test if a point is in the bounding Box
bool GLC_BoundingBox::intersect(const GLC_Vector4d& point) const
{
	if (!m_IsEmpty)
	{
		bool result= (point.GetX() < m_Upper.GetX()) && (point.GetY() < m_Upper.GetY())
		&& (point.GetZ() < m_Upper.GetZ()) && (point.GetX() > m_Lower.GetX())
		&& (point.GetY() > m_Lower.GetY()) && (point.GetZ() > m_Lower.GetZ());
		
		return result;
	}
	else
	{
		return false;
	}
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
	GLC_Vector4d vectResult = (m_Lower + m_Upper) * (1.0 / 2.0);
	return vectResult;
	
}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Combine the bounding Box with new geometry point
GLC_BoundingBox& GLC_BoundingBox::combine(const GLC_Vector4d& point)
{
	if (m_IsEmpty)
	{
		m_Lower= point;
		m_Upper= point;
		m_IsEmpty= false;
	}
	else
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
	return *this;
}

// Combine the bounding Box with new geometry point
GLC_BoundingBox& GLC_BoundingBox::combine(const GLC_Vector3d& point)
{
	if (m_IsEmpty)
	{
		m_Lower= point;
		m_Upper= point;
		m_IsEmpty= false;
	}
	else
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
	return *this;
}

// Combine the bounding Box with another bounding box
GLC_BoundingBox& GLC_BoundingBox::combine(const GLC_BoundingBox& box)
{
	if (m_IsEmpty)
	{
		m_Lower= box.m_Lower;
		m_Upper= box.m_Upper;
		m_IsEmpty= box.m_IsEmpty;
	}
	else
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
	
	return *this;
}

// Transform the bounding Box
GLC_BoundingBox& GLC_BoundingBox::transform(const GLC_Matrix4x4& matrix)
{
	// Compute Transformed BoundingBox Corner
	GLC_Vector4d corner1(m_Lower);
	GLC_Vector4d corner7(m_Upper);
	GLC_Vector4d corner2(corner7.GetX(), corner1.GetY(), corner1.GetZ());
	GLC_Vector4d corner3(corner7.GetX(), corner7.GetY(), corner1.GetZ());
	GLC_Vector4d corner4(corner1.GetX(), corner7.GetY(), corner1.GetZ());
	GLC_Vector4d corner5(corner1.GetX(), corner1.GetY(), corner7.GetZ());
	GLC_Vector4d corner6(corner7.GetX(), corner1.GetY(), corner7.GetZ());
	GLC_Vector4d corner8(corner1.GetX(), corner7.GetY(), corner7.GetZ());

	corner1 = (matrix * corner1);
	corner2 = (matrix * corner2);
	corner3 = (matrix * corner3);
	corner4 = (matrix * corner4);
	corner5 = (matrix * corner5);
	corner6 = (matrix * corner6);
	corner7 = (matrix * corner7);
	corner8 = (matrix * corner8);
	
	// Compute the new BoundingBox
	GLC_BoundingBox boundingBox;
	boundingBox.combine(corner1);
	boundingBox.combine(corner2);
	boundingBox.combine(corner3);
	boundingBox.combine(corner4);
	boundingBox.combine(corner5);
	boundingBox.combine(corner6);
	boundingBox.combine(corner7);
	boundingBox.combine(corner8);
	
	m_Lower= boundingBox.m_Lower;
	m_Upper= boundingBox.m_Upper;
	return *this;	
}

