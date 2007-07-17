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

//! \file glc_box.cpp implementation of the GLC_Box class.

#include "glc_box.h"
#include <assert.h>
#include "glc_openglexception.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////


GLC_Box::GLC_Box(double dLx, double dLy, double dlz, const QColor& color)

:GLC_Geometry("Box", color)
, m_dLgX(dLx)
, m_dLgY(dLy)
, m_dLgZ(dlz)
{

}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get X length
double GLC_Box::getLgX(void) const
{
	return m_dLgX;
}

// Get Y length
double GLC_Box::getLgY(void) const
{
	return m_dLgY;
}

// Get Z length
double GLC_Box::getLgZ(void) const
{
	return m_dLgZ;
}

// return the box bounding box
GLC_BoundingBox* GLC_Box::getBoundingBox(void) const
{
	GLC_BoundingBox* pBoundingBox= new GLC_BoundingBox();
	
	GLC_Vector3d lower(-m_dLgX / 2.0, -m_dLgY / 2.0, -m_dLgZ / 2.0);
	GLC_Vector3d upper(m_dLgX / 2.0, m_dLgY / 2.0, m_dLgZ / 2.0);
	pBoundingBox->combine(lower);
	pBoundingBox->combine(upper);
	
    
    pBoundingBox->transform(m_MatPos);	
	return pBoundingBox;	
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set X length
void GLC_Box::setLgX(double LgX)
{
	assert(LgX > 0);
	m_dLgX= LgX;
	m_ListIsValid= false;
}

// Set Y length
void GLC_Box::setLgY(double LgY)
{
	assert(LgY > 0);
	m_dLgY= LgY;
	m_ListIsValid= false;
}

// Set Z length
void GLC_Box::setLgZ(double LgZ)
{
	assert(LgZ > 0);
	m_dLgZ= LgZ;
	m_ListIsValid= false;
}

//////////////////////////////////////////////////////////////////////
// Private OpenGL functions
//////////////////////////////////////////////////////////////////////

// Box Set Up
void GLC_Box::glDraw(void)
{
	// Polygon 1	
	glBegin(GL_QUADS);		
		glNormal3d(0.0, 0.0, 1.0);	// Z
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0,		0.0,	m_dLgZ);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,	0.0,	m_dLgZ);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	m_dLgY, m_dLgZ);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0,		m_dLgY, m_dLgZ);
	// Polygon 2
		glNormal3d(1.0, 0.0, 0.0);	// X
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_dLgX,	0.0,	m_dLgZ);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_dLgX,	0.0,	0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,	m_dLgY, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	m_dLgY, m_dLgZ);
	// Polygon 3	
		glNormal3d(0.0, 0.0, -1.0);	// -Z
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0,		0.0,	0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0,		m_dLgY,	0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	m_dLgY,	0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,	0.0,	0.0);
	// Polygon 4
		glNormal3d(-1.0, 0.0, 0.0);	// -X	
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0,		0.0,	0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0,		0.0,	m_dLgZ);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(0.0,		m_dLgY,	m_dLgZ);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(0.0,		m_dLgY,	0.0);
	// Polygon 5
		glNormal3d(0.0, -1.0, 0.0);	// -Y
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0,		0.0,	0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,	0.0,	0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	0.0,	m_dLgZ);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0,		0.0,	m_dLgZ);
	// Polygon 6
		glNormal3d(0.0, 1.0, 0.0);	// Y
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0,		m_dLgY,	0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0,		m_dLgY,	m_dLgZ);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	m_dLgY,	m_dLgZ);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,	m_dLgY,	0.0);
	glEnd();
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Box::GlDraw ", error);
		throw(OpenGlException);
	}
	

}
// Virtual interface for OpenGL Geomtry properties. (Color, thiknness, position..)
void GLC_Box::glPropGeom(void)
{
	// Update Current matrix
	glMultMatrixd(m_MatPos.return_dMat());
			
	if(!m_pMaterial || (m_PolyMode != GL_FILL))
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);

		glColor4d(getdRed(), getdGreen(), getdBlue(), getdAlpha());	// is color
	}
	else if (m_pMaterial->getAddRgbaTexture())
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);


		m_pMaterial->glExecute();
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		
		m_pMaterial->glExecute();

	}

	glLineWidth(getThickness());	// Is thikness

	// Polygons display mode
	glPolygonMode(m_PolyFace, m_PolyMode);
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Box::GlDraw ", error);
		throw(OpenGlException);
	}
		
}
