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
//! \file glc_plane.cpp Implementation of the GLC_Plane class.

#include "glc_plane.h"

// Default constructor
GLC_Plane::GLC_Plane()
: m_A(0.0)
, m_B(0.0)
, m_C(1.0)
, m_D(0.0)
{

}
// Contruct a plan with specified parameter
GLC_Plane::GLC_Plane(double a, double b, double c, double d)
: m_A(a)
, m_B(b)
, m_C(c)
, m_D(d)
{

}

// Construct a plane with normal vector and lenght
GLC_Plane::GLC_Plane(const GLC_Vector3d& normal, double d)
: m_A(normal.x())
, m_B(normal.y())
, m_C(normal.z())
, m_D(d)
{

}

// Copy constructor
GLC_Plane::GLC_Plane(const GLC_Plane& plane)
: m_A(plane.m_A)
, m_B(plane.m_B)
, m_C(plane.m_C)
, m_D(plane.m_D)
{

}

// Assignement operator
GLC_Plane& GLC_Plane::operator=(const GLC_Plane& p)
{
	if ((this != &p) && (*this != p))
	{
		m_A= p.m_A;
		m_B= p.m_B;
		m_C= p.m_C;
		m_D= p.m_D;
	}
	return *this;
}

// Destructor
GLC_Plane::~GLC_Plane()
{

}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Equality operator
bool GLC_Plane::operator==(GLC_Plane p2) const
{
	GLC_Plane p1(*this);
	p1.normalize();
	p2.normalize();
	bool areEqual= qFuzzyCompare(p1.m_A, p2.m_A);
	areEqual= areEqual && qFuzzyCompare(p1.m_B, p2.m_B);
	areEqual= areEqual && qFuzzyCompare(p1.m_C, p2.m_C);
	areEqual= areEqual && qFuzzyCompare(p1.m_D, p2.m_D);

	return areEqual;
}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Normalize the plane
void GLC_Plane::normalize()
{
	double mag;
	mag= sqrt(m_A * m_A + m_B * m_B + m_C * m_C);

	m_A= m_A / mag;
	m_B= m_B / mag;
	m_C= m_C / mag;
	m_D= m_D / mag;
}
