/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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
#include "maths/glc_matrix4x4.h"

// Class chunk id
quint32 GLC_BoundingBox::m_ChunkId= 0xA707;

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
GLC_BoundingBox::GLC_BoundingBox(const GLC_Point3d& lower, const GLC_Point3d& upper)
: m_Lower(lower)
, m_Upper(upper)
, m_IsEmpty(false)
{

}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////
// Return the class Chunk ID
quint32 GLC_BoundingBox::chunckID()
{
	return m_ChunkId;
}

// Test if a point is in the bounding Box
bool GLC_BoundingBox::intersect(const GLC_Point3d& point) const
{
	if (!m_IsEmpty)
	{
		bool result= (point.x() < m_Upper.x()) && (point.y() < m_Upper.y())
		&& (point.z() < m_Upper.z()) && (point.x() > m_Lower.x())
		&& (point.y() > m_Lower.y()) && (point.z() > m_Lower.z());

		return result;
	}
	else
	{
		return false;
	}
}

// Test if a point is in the bounding Sphere
bool GLC_BoundingBox::intersectBoundingSphere(const GLC_Point3d& point) const
{
	const double distance= (center() - point).lenght();
	return distance < boundingSphereRadius();
}

// Test if the bounding sphere of a bounding box interserct with the bounding sphere
bool GLC_BoundingBox::intersectBoundingSphere(const GLC_BoundingBox& boundingSphere) const
{
	const double distance= (center() - boundingSphere.center()).lenght();
	return distance < (boundingSphereRadius() + boundingSphere.boundingSphereRadius());
}

// Get the lower corner of the bounding box
GLC_Point3d GLC_BoundingBox::lowerCorner(void) const
{
	return m_Lower;
}

// Get the upper corner of the bounding box
GLC_Point3d GLC_BoundingBox::upperCorner(void) const
{
	return m_Upper;
}

// Get the center of the bounding box
GLC_Point3d GLC_BoundingBox::center(void) const
{
	GLC_Vector3d vectResult = (m_Lower + m_Upper) * (1.0 / 2.0);
	return vectResult;

}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Combine the bounding Box with new geometry point
GLC_BoundingBox& GLC_BoundingBox::combine(const GLC_Point3d& point)
{
	if (m_IsEmpty)
	{
		m_Lower= point;
		m_Upper= point;
		m_IsEmpty= false;
	}
	else
	{
		double lowerX= qMin(point.x(), m_Lower.x());
		double lowerY= qMin(point.y(), m_Lower.y());
		double lowerZ= qMin(point.z(), m_Lower.z());
		m_Lower.setVect(lowerX, lowerY, lowerZ);

		double upperX= qMax(point.x(), m_Upper.x());
		double upperY= qMax(point.y(), m_Upper.y());
		double upperZ= qMax(point.z(), m_Upper.z());
		m_Upper.setVect(upperX, upperY, upperZ);
	}
	return *this;
}

// Combine the bounding Box with new geometry point
GLC_BoundingBox& GLC_BoundingBox::combine(const GLC_Point3df& pointf)
{
	GLC_Point3d point(pointf);
	if (m_IsEmpty)
	{
		m_Lower= point;
		m_Upper= point;
		m_IsEmpty= false;
	}
	else
	{
		double lowerX= qMin(point.x(), m_Lower.x());
		double lowerY= qMin(point.y(), m_Lower.y());
		double lowerZ= qMin(point.z(), m_Lower.z());
		m_Lower.setVect(lowerX, lowerY, lowerZ);

		double upperX= qMax(point.x(), m_Upper.x());
		double upperY= qMax(point.y(), m_Upper.y());
		double upperZ= qMax(point.z(), m_Upper.z());
		m_Upper.setVect(upperX, upperY, upperZ);
	}
	return *this;
}

// Combine the bounding Box with another bounding box
GLC_BoundingBox& GLC_BoundingBox::combine(const GLC_BoundingBox& box)
{
	if (m_IsEmpty && !box.m_IsEmpty)
	{
		m_Lower= box.m_Lower;
		m_Upper= box.m_Upper;
		m_IsEmpty= box.m_IsEmpty;
	}
	else if (! box.m_IsEmpty)
	{
		double lowerX= qMin(box.m_Lower.x(), m_Lower.x());
		double lowerY= qMin(box.m_Lower.y(), m_Lower.y());
		double lowerZ= qMin(box.m_Lower.z(), m_Lower.z());
		m_Lower.setVect(lowerX, lowerY, lowerZ);

		double upperX= qMax(box.m_Upper.x(), m_Upper.x());
		double upperY= qMax(box.m_Upper.y(), m_Upper.y());
		double upperZ= qMax(box.m_Upper.z(), m_Upper.z());
		m_Upper.setVect(upperX, upperY, upperZ);
	}

	return *this;
}

// Transform the bounding Box
GLC_BoundingBox& GLC_BoundingBox::transform(const GLC_Matrix4x4& matrix)
{
	// Compute Transformed BoundingBox Corner
	GLC_Point3d corner1(m_Lower);
	GLC_Point3d corner7(m_Upper);
	GLC_Point3d corner2(corner7.x(), corner1.y(), corner1.z());
	GLC_Point3d corner3(corner7.x(), corner7.y(), corner1.z());
	GLC_Point3d corner4(corner1.x(), corner7.y(), corner1.z());
	GLC_Point3d corner5(corner1.x(), corner1.y(), corner7.z());
	GLC_Point3d corner6(corner7.x(), corner1.y(), corner7.z());
	GLC_Point3d corner8(corner1.x(), corner7.y(), corner7.z());

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

// Non-member stream operator
QDataStream &operator<<(QDataStream &stream, const GLC_BoundingBox &bBox)
{
	quint32 chunckId= GLC_BoundingBox::m_ChunkId;
	stream << chunckId;

	stream << bBox.m_IsEmpty;
	stream << bBox.lowerCorner();
	stream << bBox.upperCorner();

	return stream;
}
QDataStream &operator>>(QDataStream &stream, GLC_BoundingBox &bBox)
{
	quint32 chunckId;
	stream >> chunckId;
	Q_ASSERT(chunckId == GLC_BoundingBox::m_ChunkId);

	stream >> bBox.m_IsEmpty;
	stream >> bBox.m_Lower;
	stream >> bBox.m_Upper;

	return stream;
}

