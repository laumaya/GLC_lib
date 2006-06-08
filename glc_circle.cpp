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

//! \file glc_circle.cpp implementation of the GLC_Circle class.

#include "glc_circle.h"
#include "assert.h"
#include "glc_openglexception.h"

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////


GLC_Circle::GLC_Circle(const double &dRadius, double Angle,
const char *pName, const GLfloat *pColor)
:GLC_Geometry(pName, pColor)
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
int GLC_Circle::GetDiscretion(void) const
{
	return m_nDiscret;
}
// Return Circle radius
double GLC_Circle::GetRadius(void) const
{
	return m_Radius;
}
// return Circle diameter
double GLC_Circle::GetDiameter(void) const
{
	return 2 * m_Radius;
}

// return the circle bounding box
GLC_BoundingBox* GLC_Circle::getBoundingBox(void) const
{
	GLC_BoundingBox* pBoundingBox= new GLC_BoundingBox();
	
	GLC_Vector3d lower(-m_Radius, -m_Radius, -2.0 * EPSILON);
	GLC_Vector3d upper(m_Radius, m_Radius, 2.0 * EPSILON);
	pBoundingBox->combine(lower);
	pBoundingBox->combine(upper);
	
    
    pBoundingBox->transform(m_MatPos);	
	return pBoundingBox;	
}
//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Circle diameter
void GLC_Circle::SetDiameter(double D)
{
	assert(D > (2 * EPSILON));
	SetRadius(D / 2);
}

// Set Circle Radius
void GLC_Circle::SetRadius(double R)
{
	assert(R > EPSILON);
	if ( fabs(R - m_Radius) > EPSILON)
	{	// Radius is changing
		m_Radius= R;
		m_ListIsValid= false;
	}
}

// Set Circle discret
void GLC_Circle::SetDiscretion(int TargetDiscret)
{
	assert(TargetDiscret > 0);
	if (TargetDiscret != m_nDiscret)
	{
		m_nDiscret= TargetDiscret;
		if (m_nDiscret < 6) m_nDiscret= 6;
		m_ListIsValid= false;
	}
}

// Set Circle Angle
void GLC_Circle::SetAngle(double AngleRadians)	// Angle in Radians
{
	assert((AngleRadians > EPSILON) && (AngleRadians < 2 * PI));
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
void GLC_Circle::GlDraw(void)
{
	double MyCos;
	double MySin;
	GLC_Vector4d Vect;

	glBegin(GL_LINE_STRIP);

		for (int i= 0; i <= m_nDiscret; i++)
		{
			MyCos= m_Radius * cos(i * m_dAngle / m_nDiscret);
			MySin= m_Radius * sin(i * m_dAngle / m_nDiscret);
			Vect.SetVect(MyCos, MySin, 0);
			glVertex3dv(Vect.Return_dVect());
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
// Virtual interface for OpenGL Geomtry properties. (Color, thiknness, position..)
void GLC_Circle::GlPropGeom(void)
{
	// Update Current matrix
	glMultMatrixd(m_MatPos.Return_dMat());
	
	// Circle graphic properties
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	// Pas de transparence
	glDisable(GL_BLEND);
		
	glColor4fv(GetfRGBA());			// Color
	glLineWidth(GetThickness());	// Thikness
			
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Circle::GlPropGeom ", error);
		throw(OpenGlException);
	}				
}
