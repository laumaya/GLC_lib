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

//! \file glc_box.cpp implementation of the GLC_Box class.

#include "glc_box.h"
#include "glc_openglexception.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

GLC_Box::GLC_Box(double dLx, double dLy, double dlz)
:GLC_VboGeom("Box", false)
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
	    
	return pBoundingBox;	
}

// Return a copy of the current geometry
GLC_VboGeom* GLC_Box::clone() const
{
	return new GLC_Box(*this);
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set X length
void GLC_Box::setLgX(double LgX)
{
	Q_ASSERT(LgX > 0);
	m_dLgX= LgX;
	m_GeometryIsValid= false;
}

// Set Y length
void GLC_Box::setLgY(double LgY)
{
	Q_ASSERT(LgY > 0);
	m_dLgY= LgY;
	m_GeometryIsValid= false;
}

// Set Z length
void GLC_Box::setLgZ(double LgZ)
{
	Q_ASSERT(LgZ > 0);
	m_dLgZ= LgZ;
	m_GeometryIsValid= false;
}

//////////////////////////////////////////////////////////////////////
// Private OpenGL functions
//////////////////////////////////////////////////////////////////////

// Box Set Up
void GLC_Box::glDraw(void)
{/*
	const GLsizei numberOfVertex= 8;
	const GLsizei numberOfFace= 6;
	if (!m_GeometryIsValid)
	{
		
		// Vertex Data
		const GLsizeiptr positionSize= numberOfVertex * 3 * sizeof(GLfloat);
		GLfloat positionData[numberOfVertex * 3];
		const float lgX= static_cast<float>(m_dLgX / 2.0);
		const float lgY= static_cast<float>(m_dLgY / 2.0);
		const float lgZ= static_cast<float>(m_dLgZ / 2.0);
		
		positionData[0]= -lgX; positionData[1]= -lgY; positionData[2]= lgZ;
		positionData[3]= lgX; positionData[4]= -lgY; positionData[5]= lgZ;
		positionData[6]= lgX; positionData[7]= -lgY; positionData[8]= -lgZ;
		positionData[9]= -lgX; positionData[10]= -lgY; positionData[11]= -lgZ;
		
		positionData[12]= -lgX; positionData[13]= lgY; positionData[14]= lgZ;
		positionData[15]= lgX; positionData[16]= lgY; positionData[17]= lgZ;
		positionData[18]= lgX; positionData[19]= lgY; positionData[20]= -lgZ;
		positionData[21]= -lgX; positionData[22]= lgY; positionData[23]= -lgZ;
		
		// Normals
		const GLsizeiptr normalSize= numberOfFace * 3 * sizeof(GLfloat);
		GLfloat normalData[numberOfFace * 3];
		
		
	}
	// Polygon 1	
	glBegin(GL_QUADS);		
		glNormal3d(0.0, 0.0, 1.0);	// Z
		glTexCoord2f(0.0f, 0.0f); glVertex3d(-m_dLgX / 2, -m_dLgY / 2, m_dLgZ / 2);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX / 2, -m_dLgY / 2, m_dLgZ / 2);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX / 2, m_dLgY / 2, m_dLgZ / 2);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(-m_dLgX / 2, m_dLgY / 2, m_dLgZ / 2);
	// Polygon 2
		glNormal3d(1.0, 0.0, 0.0);	// X
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_dLgX / 2, -m_dLgY / 2, m_dLgZ / 2);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_dLgX / 2, -m_dLgY / 2, -m_dLgZ / 2);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX / 2, m_dLgY / 2, -m_dLgZ / 2);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX / 2, m_dLgY / 2, m_dLgZ / 2);
	// Polygon 3	
		glNormal3d(0.0, 0.0, -1.0);	// -Z
		glTexCoord2f(0.0f, 0.0f); glVertex3d(-m_dLgX / 2, -m_dLgY / 2, -m_dLgZ / 2);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(-m_dLgX / 2, m_dLgY / 2, -m_dLgZ / 2);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX / 2, m_dLgY / 2, -m_dLgZ / 2);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX / 2, -m_dLgY / 2, -m_dLgZ / 2);
	// Polygon 4
		glNormal3d(-1.0, 0.0, 0.0);	// -X	
		glTexCoord2f(0.0f, 0.0f); glVertex3d(-m_dLgX / 2, -m_dLgY / 2, -m_dLgZ / 2);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(-m_dLgX / 2, -m_dLgY / 2, m_dLgZ / 2);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(-m_dLgX / 2, m_dLgY / 2, m_dLgZ / 2);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(-m_dLgX / 2, m_dLgY / 2, -m_dLgZ / 2);
	// Polygon 5
		glNormal3d(0.0, -1.0, 0.0);	// -Y
		glTexCoord2f(0.0f, 0.0f); glVertex3d(-m_dLgX / 2, -m_dLgY / 2, -m_dLgZ / 2);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX / 2, -m_dLgY / 2, -m_dLgZ / 2);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX / 2, -m_dLgY / 2,	m_dLgZ / 2);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(-m_dLgX / 2, -m_dLgY / 2,	m_dLgZ / 2);
	// Polygon 6
		glNormal3d(0.0, 1.0, 0.0);	// Y
		glTexCoord2f(0.0f, 0.0f); glVertex3d(-m_dLgX / 2, m_dLgY / 2, -m_dLgZ / 2);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(-m_dLgX / 2, m_dLgY / 2, m_dLgZ / 2);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX / 2, m_dLgY / 2, m_dLgZ / 2);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX / 2, m_dLgY / 2, -m_dLgZ / 2);
	glEnd();
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Box::GlDraw ", error);
		throw(OpenGlException);
	}
	*/

}
