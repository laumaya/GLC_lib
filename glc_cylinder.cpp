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

//! \file glc_cylinder.cpp implementation of the GLC_Cylinder class.

#include "glc_cylinder.h"
#include "glc_openglexception.h"
#include "glc_selectionmaterial.h"

#include <QVector>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace glc;
//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////


GLC_Cylinder::GLC_Cylinder(double dRadius, double dLength)
:GLC_VboGeom("Cylinder", false)
, m_Radius(dRadius)
, m_Length(dLength)
, m_Discret(GLC_POLYDISCRET)	// Default discretion
, m_EndedIsCaped(true)				// Cylinder ended are closed	
{
	Q_ASSERT((m_Radius > 0.0) && (m_Length > 0.0));
}

GLC_Cylinder::GLC_Cylinder(const GLC_Cylinder& sourceCylinder)
:GLC_VboGeom(sourceCylinder)
, m_Radius(sourceCylinder.m_Radius)
, m_Length(sourceCylinder.m_Length)
, m_Discret(sourceCylinder.m_Discret)
, m_EndedIsCaped(sourceCylinder.m_EndedIsCaped)
{
	Q_ASSERT((m_Radius > 0.0) && (m_Length > 0.0) && (m_Discret > 0));
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// return the cylinder bounding box
GLC_BoundingBox* GLC_Cylinder::getBoundingBox(void) const
{
	GLC_BoundingBox* pBoundingBox= new GLC_BoundingBox();
	
	GLC_Point3d lower(-m_Radius, -m_Radius, 0.0);
	GLC_Point3d upper(m_Radius, m_Radius, m_Length);
	pBoundingBox->combine(lower);
	pBoundingBox->combine(upper);
	
	return pBoundingBox;	
}

// Return a copy of the current geometry
GLC_VboGeom* GLC_Cylinder::clone() const
{
	return new GLC_Cylinder(*this);
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Set Cylinder length
void GLC_Cylinder::setLength(double Length)
{
	Q_ASSERT(Length > 0.0);
	m_Length= Length;

	m_GeometryIsValid= false;	 
}

// Set Cylinder radius
void GLC_Cylinder::setRadius(double Radius)
{
	Q_ASSERT(Radius > 0.0);
	m_Radius= Radius;
	
	m_GeometryIsValid= false;
}

// Set Discretion
void GLC_Cylinder::setDiscretion(int TargetDiscret)
{
	Q_ASSERT(TargetDiscret > 0);
	if (TargetDiscret != m_Discret)
	{
		m_Discret= TargetDiscret;
		if (m_Discret < 6) m_Discret= 6;

		m_GeometryIsValid= false;
	}
}

// End Caps
void GLC_Cylinder::setEndedCaps(bool CapsEnded)
{
	if (m_EndedIsCaped != CapsEnded)
	{
		m_EndedIsCaped= CapsEnded;
	}
}

//////////////////////////////////////////////////////////////////////
// Private Opengl functions
//////////////////////////////////////////////////////////////////////

// Dessin du GLC_Cylinder
void GLC_Cylinder::glDraw(void)
{
	
	if (!m_GeometryIsValid)
	{
		double cosArray[m_Discret];
		double sinArray[m_Discret];
		for (int i= 0; i < m_Discret; ++i)
		{
			cosArray[i]= m_Radius * cos(i * (2 * PI) / m_Discret);
			sinArray[i]= m_Radius * sin(i * (2 * PI) / m_Discret);
		}
		
		const GLsizei dataNbr= m_Discret * 4;
		const GLsizeiptr dataSize= dataNbr * sizeof(GLC_Vertex);
		GLC_Vertex positionData[dataNbr];
		for (GLsizei i= 0; i < (dataNbr / 4); ++i)
		{
			// Bottom
			positionData[i].x= static_cast<GLfloat>(cosArray[i]);
			positionData[i].y= static_cast<GLfloat>(sinArray[i]);
			positionData[i].z= 0.0f;
			GLC_Vector4d normal(cosArray[i], sinArray[i], 0.0);
			normal.setNormal(1.0);
			positionData[i].nx= static_cast<GLfloat>(normal.getX());
			positionData[i].ny= static_cast<GLfloat>(normal.getY());
			positionData[i].nz= 0.0f;
			positionData[i].s= static_cast<float>(i) / static_cast<float>(m_Discret);
			positionData[i].t= 0.0f;
			
			// Top
			positionData[i + m_Discret].x= positionData[i].x;
			positionData[i + m_Discret].y= positionData[i].y;
			positionData[i + m_Discret].z= static_cast<GLfloat>(m_Length);
			positionData[i + m_Discret].nx= positionData[i].nx;
			positionData[i + m_Discret].ny= positionData[i].ny;
			positionData[i + m_Discret].nz= 0.0f;
			positionData[i + m_Discret].s= positionData[i].s;
			positionData[i + m_Discret].t= 1.0f;
			
			// Bottom Cap
			positionData[i + 2 * m_Discret].x= positionData[i].x;
			positionData[i + 2 * m_Discret].y= positionData[i].y;
			positionData[i + 2 * m_Discret].z= 0.0f;
			positionData[i + 2 * m_Discret].nx= 0.0f;
			positionData[i + 2 * m_Discret].ny= 0.0f;
			positionData[i + 2 * m_Discret].nz= -1.0f;
			positionData[i + 2 * m_Discret].s= positionData[i].s;
			positionData[i + 2 * m_Discret].t= 1.0f;
	
			// Top Cap
			positionData[i + 3 * m_Discret].x= positionData[i].x;
			positionData[i + 3 * m_Discret].y= positionData[i].y;
			positionData[i + 3 * m_Discret].z= static_cast<GLfloat>(m_Length);
			positionData[i + 3 * m_Discret].nx= 0.0f;
			positionData[i + 3 * m_Discret].ny= 0.0f;
			positionData[i + 3 * m_Discret].nz= 1.0f;
			positionData[i + 3 * m_Discret].s= positionData[i].s;
			positionData[i + 3 * m_Discret].t= 1.0f;
		}
		glBufferData(GL_ARRAY_BUFFER, dataSize, positionData, GL_STATIC_DRAW);
		
		// Create IBO
		const GLsizei indexNbr= 2 * (m_Discret + 1) + 2 * m_Discret;
		const GLsizeiptr indexSize = indexNbr * sizeof(GLuint);
		GLuint indexData[indexNbr];
		GLsizei j= 0;
		for (GLsizei i= 0; i < indexNbr / 2; i+=2, ++j)
		{
			if (i < (indexNbr / 2) - 2)
			{
				indexData[i]= j;
				indexData[i + 1]= j + m_Discret;				
			}
			else
			{
				indexData[i]= 0;
				indexData[i + 1]= m_Discret;								
			}
		}
		
		// Caps end
		j= 2.5 * m_Discret;
		GLsizei k= j - 1;
		GLsizei max = 2 * (m_Discret + 1) + m_Discret;
		for (GLsizei i= 2 * (m_Discret + 1); i < max; i+= 2, ++j, --k)
		{
			indexData[i]= j;
			if (i < (max - 1))
				indexData[i + 1]= k;
		}
		
		j= 3.5 * m_Discret;
		k= j - 1;
		for (GLsizei i= max; i < indexNbr; i+= 2, ++j, --k)
		{
			indexData[i]= j;
			if (i < (indexNbr - 1))
				indexData[i + 1]= k;
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData, GL_STATIC_DRAW);
	}
	
	glVertexPointer(3, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(12));
	glTexCoordPointer(2, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(24));

	GLuint max= (m_Discret + 1) * 2;
	// Draw cylinder 
	glDrawRangeElements(GL_TRIANGLE_STRIP, 0, max, max, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	
	// Fill ended if needed
	if (m_EndedIsCaped)
	{
		// Draw bottom cap
		glDrawRangeElements(GL_TRIANGLE_STRIP, 0, m_Discret, m_Discret, GL_UNSIGNED_INT, BUFFER_OFFSET((max) * sizeof(unsigned int)));
		max+= m_Discret;
		// Draw top cap
		glDrawRangeElements(GL_TRIANGLE_STRIP, 0, m_Discret, m_Discret, GL_UNSIGNED_INT, BUFFER_OFFSET((max) * sizeof(unsigned int)));		
	}
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Cylinder::GlDraw ", error);
		throw(OpenGlException);
	}

}
