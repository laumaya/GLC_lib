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

//! \file glc_circle.cpp implementation of the GLC_Circle class.

#include "glc_circle.h"
#include "glc_openglexception.h"

using namespace glc;
//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////

GLC_Circle::GLC_Circle(const double &dRadius, double Angle)
:GLC_Geometry("Circle", true)
, m_Radius(dRadius)
, m_nDiscret(GLC_DISCRET)
, m_dAngle(Angle)
{
	
}

GLC_Circle::GLC_Circle(const GLC_Circle& sourceCircle)
:GLC_Geometry(sourceCircle)
, m_Radius(sourceCircle.m_Radius)
, m_nDiscret(sourceCircle.m_nDiscret)
, m_dAngle(sourceCircle.m_dAngle)
{
}
GLC_Circle::~GLC_Circle()
{
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Return Circle Discretion
int GLC_Circle::getDiscretion(void) const
{
	return m_nDiscret;
}
// Return Circle radius
double GLC_Circle::getRadius(void) const
{
	return m_Radius;
}
// return Circle diameter
double GLC_Circle::getDiameter(void) const
{
	return 2 * m_Radius;
}

// return the circle bounding box
GLC_BoundingBox* GLC_Circle::getBoundingBox(void) const
{
	GLC_BoundingBox* pBoundingBox= new GLC_BoundingBox();
	
	GLC_Point3d lower(-m_Radius, -m_Radius, -2.0 * EPSILON);
	GLC_Point3d upper(m_Radius, m_Radius, 2.0 * EPSILON);
	pBoundingBox->combine(lower);
	pBoundingBox->combine(upper);
	
	return pBoundingBox;	
}

// Return a copy of the current geometry
GLC_Geometry* GLC_Circle::clone() const
{
	return new GLC_Circle(*this);
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Circle diameter
void GLC_Circle::setDiameter(double D)
{
	Q_ASSERT(D > (2 * EPSILON));
	setRadius(D / 2);
}

// Set Circle Radius
void GLC_Circle::setRadius(double R)
{
	Q_ASSERT(R > EPSILON);
	if ( fabs(R - m_Radius) > EPSILON)
	{	// Radius is changing
		m_Radius= R;
		m_ListIsValid= false;
	}
}

// Set Circle discret
void GLC_Circle::setDiscretion(int TargetDiscret)
{
	Q_ASSERT(TargetDiscret > 0);
	if (TargetDiscret != m_nDiscret)
	{
		m_nDiscret= TargetDiscret;
		if (m_nDiscret < 6) m_nDiscret= 6;
		m_ListIsValid= false;
	}
}

// Set Circle Angle
void GLC_Circle::setAngle(double AngleRadians)	// Angle in Radians
{
	Q_ASSERT((AngleRadians > EPSILON) && (AngleRadians < 2 * PI));
	if ( fabs(AngleRadians - m_dAngle) > EPSILON)
	{	// Angle is changing
			m_dAngle= AngleRadians;
			m_ListIsValid= false;
	}
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Circle drawing
void GLC_Circle::glDraw(void)
{
	double coord[3];
	
	// Z is always set to 0.0
	coord[2]= 0.0;

	glBegin(GL_LINE_STRIP);

		for (int i= 0; i <= m_nDiscret; i++)
		{
			coord[0]= m_Radius * cos(i * m_dAngle / m_nDiscret);
			coord[1]= m_Radius * sin(i * m_dAngle / m_nDiscret);
			glVertex3dv(coord);
		}

	glEnd();
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Circle::GlDraw ", error);
		throw(OpenGlException);
	}
}
