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

//! \file glc_point.cpp implementation of the GLC_Point class.

#include "glc_point.h"
#include "glc_openglexception.h"


//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////


GLC_Point::GLC_Point(const GLC_Vector4d &VectSetCoord, const QColor &color)
:GLC_Geometry("Point", color), m_VectCoord(VectSetCoord)
{
	
}
//! Construct an GLC_Point
GLC_Point::GLC_Point(double x, double y, double z, const QColor &color)
:GLC_Geometry("Point", color), m_VectCoord(x, y, z)
{
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get a 4D vector represent point coordinate
GLC_Vector4d GLC_Point::getVectCoord(void) const
{
	return m_VectCoord;
}

// return the point bounding box
GLC_BoundingBox* GLC_Point::getBoundingBox(void) const
{
	GLC_BoundingBox* pBoundingBox= new GLC_BoundingBox();
	
	GLC_Vector3d lower(m_VectCoord.getX() - EPSILON,
						 m_VectCoord.getY() - EPSILON,
						  m_VectCoord.getZ() - EPSILON);
	GLC_Vector3d upper(m_VectCoord.getX() + EPSILON,
						 m_VectCoord.getY() + EPSILON,
						  m_VectCoord.getZ() + EPSILON);
	pBoundingBox->combine(lower);
	pBoundingBox->combine(upper);
	
    
    pBoundingBox->transform(m_MatPos);	
	return pBoundingBox;	
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Point coordinate by 4D Vector
void GLC_Point::setCoord(const GLC_Vector4d &Vect)
{
	m_VectCoord= Vect;
	m_ListIsValid = false;
}
// Set Point coordinate by 3 double
void GLC_Point::setCoord(double x, double y, double z)
{
	m_VectCoord.setVect(x, y, z);
	m_ListIsValid = false;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

void GLC_Point::glDraw(void)
{
	// Point Display
	glBegin(GL_POINTS);
		glVertex3dv(m_VectCoord.return_dVect());
	glEnd();
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Point::GlDraw ", error);
		throw(OpenGlException);
	}
	
}

void GLC_Point::glPropGeom(void)
{
	// Change current Matrix
	glMultMatrixd(m_MatPos.return_dMat());
	
	// Disable lighting
	glDisable(GL_LIGHTING);
	// Disable blending
	glDisable(GL_BLEND);

	glColor4d(getdRed(), getdGreen(), getdBlue(), getdAlpha());	// Color
	glPointSize(getThickness());	// Thikness
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Point::GlPropGeom ", error);
		throw(OpenGlException);
	}
	
}

