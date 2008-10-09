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

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

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
{
	if (!m_GeometryIsValid)
	{
		GLC_Vertex positionData[24]; // 3 normals per vertex and 8 Vertex => 3 x 8 = 24
		const GLsizeiptr size= 24 * sizeof(GLC_Vertex);
		
		const GLfloat lgX= static_cast<const GLfloat>(m_dLgX / 2.0);
		const GLfloat lgY= static_cast<const GLfloat>(m_dLgY / 2.0);
		const GLfloat lgZ= static_cast<const GLfloat>(m_dLgZ / 2.0);
	
		// Face 1
		positionData[0].x= -lgX; positionData[0].y= -lgY; positionData[0].z= lgZ;
		positionData[0].nx= 0.0f; positionData[0].ny= 0.0f; positionData[0].nz= 1.0f;
		positionData[0].s= 0.0f; positionData[0].t= 0.0f;
		
		positionData[1].x= lgX; positionData[1].y= -lgY; positionData[1].z= lgZ;
		positionData[1].nx= 0.0f; positionData[1].ny= 0.0f; positionData[1].nz= 1.0f;
		positionData[1].s= 1.0f; positionData[1].t= 0.0f;
		
		positionData[2].x= lgX; positionData[2].y= lgY; positionData[2].z= lgZ;
		positionData[2].nx= 0.0f; positionData[2].ny= 0.0f; positionData[2].nz= 1.0f;
		positionData[2].s= 1.0f; positionData[2].t= 1.0f;
	
		positionData[3].x= -lgX; positionData[3].y= lgY; positionData[3].z= lgZ;
		positionData[3].nx= 0.0f; positionData[3].ny= 0.0f; positionData[3].nz= 1.0f;
		positionData[3].s= 0.0f; positionData[3].t= 1.0f;
	
		// Face 2
		positionData[4].x= lgX; positionData[4].y= -lgY; positionData[4].z= lgZ;
		positionData[4].nx= 1.0f; positionData[4].ny= 0.0f; positionData[4].nz= 0.0f;
		positionData[4].s= 0.0f; positionData[4].t= 0.0f;
		
		positionData[5].x= lgX; positionData[5].y= -lgY; positionData[5].z= -lgZ;
		positionData[5].nx= 1.0f; positionData[5].ny= 0.0f; positionData[5].nz= 0.0f;
		positionData[5].s= 1.0f; positionData[5].t= 0.0f;
		
		positionData[6].x= lgX; positionData[6].y= lgY; positionData[6].z= -lgZ;
		positionData[6].nx= 1.0f; positionData[6].ny= 0.0f; positionData[6].nz= 0.0f;
		positionData[6].s= 1.0f; positionData[6].t= 1.0f;
	
		positionData[7].x= lgX; positionData[7].y= lgY; positionData[7].z= lgZ;
		positionData[7].nx= 1.0f; positionData[7].ny= 0.0f; positionData[7].nz= 0.0f;
		positionData[7].s= 0.0f; positionData[7].t= 1.0f;
	
		// Face 3
		positionData[8].x= -lgX; positionData[8].y= -lgY; positionData[8].z= -lgZ;
		positionData[8].nx= -1.0f; positionData[8].ny= 0.0f; positionData[8].nz= 0.0f;
		positionData[8].s= 0.0f; positionData[8].t= 0.0f;
		
		positionData[9].x= -lgX; positionData[9].y= -lgY; positionData[9].z= lgZ;
		positionData[9].nx= -1.0f; positionData[9].ny= 0.0f; positionData[9].nz= 0.0f;
		positionData[9].s= 1.0f; positionData[9].t= 0.0f;
		
		positionData[10].x= -lgX; positionData[10].y= lgY; positionData[10].z= lgZ;
		positionData[10].nx= -1.0f; positionData[10].ny= 0.0f; positionData[10].nz= 0.0f;
		positionData[10].s= 1.0f; positionData[10].t= 1.0f;
	
		positionData[11].x= -lgX; positionData[11].y= lgY; positionData[11].z= -lgZ;
		positionData[11].nx= -1.0f; positionData[11].ny= 0.0f; positionData[11].nz= 0.0f;
		positionData[11].s= 0.0f; positionData[11].t= 1.0f;
	
		// Face 4
		positionData[12].x= lgX; positionData[12].y= -lgY; positionData[12].z= -lgZ;
		positionData[12].nx= 0.0f; positionData[12].ny= 0.0f; positionData[12].nz= -1.0f;
		positionData[12].s= 0.0f; positionData[12].t= 0.0f;
		
		positionData[13].x= -lgX; positionData[13].y= -lgY; positionData[13].z= -lgZ;
		positionData[13].nx= 0.0f; positionData[13].ny= 0.0f; positionData[13].nz= -1.0f;
		positionData[13].s= 1.0f; positionData[13].t= 0.0f;
		
		positionData[14].x= -lgX; positionData[14].y= lgY; positionData[14].z= -lgZ;
		positionData[14].nx= 0.0f; positionData[14].ny= 0.0f; positionData[14].nz= -1.0f;
		positionData[14].s= 1.0f; positionData[14].t= 1.0f;
	
		positionData[15].x= lgX; positionData[15].y= lgY; positionData[15].z= -lgZ;
		positionData[15].nx= 0.0f; positionData[15].ny= 0.0f; positionData[15].nz= -1.0f;
		positionData[15].s= 0.0f; positionData[15].t= 1.0f;
	
		// Face 5
		positionData[16].x= -lgX; positionData[16].y= lgY; positionData[16].z= lgZ;
		positionData[16].nx= 0.0f; positionData[16].ny= 1.0f; positionData[16].nz= 0.0f;
		positionData[16].s= 0.0f; positionData[16].t= 0.0f;
		
		positionData[17].x= lgX; positionData[17].y= lgY; positionData[17].z= lgZ;
		positionData[17].nx= 0.0f; positionData[17].ny= 1.0f; positionData[17].nz= 0.0f;
		positionData[17].s= 1.0f; positionData[17].t= 0.0f;
		
		positionData[18].x= lgX; positionData[18].y= lgY; positionData[18].z= -lgZ;
		positionData[18].nx= 0.0f; positionData[18].ny= 1.0f; positionData[18].nz= 0.0f;
		positionData[18].s= 1.0f; positionData[18].t= 1.0f;
	
		positionData[19].x= -lgX; positionData[19].y= lgY; positionData[19].z= -lgZ;
		positionData[19].nx= 0.0f; positionData[19].ny= 1.0f; positionData[19].nz= 0.0f;
		positionData[19].s= 0.0f; positionData[19].t= 1.0f;
	
		// Face 6
		positionData[20].x= -lgX; positionData[20].y= -lgY; positionData[20].z= -lgZ;
		positionData[20].nx= 0.0f; positionData[20].ny= -1.0f; positionData[20].nz= 0.0f;
		positionData[20].s= 0.0f; positionData[20].t= 0.0f;
		
		positionData[21].x= lgX; positionData[21].y= -lgY; positionData[21].z= -lgZ;
		positionData[21].nx= 0.0f; positionData[21].ny= -1.0f; positionData[21].nz= 0.0f;
		positionData[21].s= 1.0f; positionData[21].t= 0.0f;
		
		positionData[22].x= lgX; positionData[22].y= -lgY; positionData[22].z= lgZ;
		positionData[22].nx= 0.0f; positionData[22].ny= -1.0f; positionData[22].nz= 0.0f;
		positionData[22].s= 1.0f; positionData[22].t= 1.0f;
	
		positionData[23].x= -lgX; positionData[23].y= -lgY; positionData[23].z= lgZ;
		positionData[23].nx= 0.0f; positionData[23].ny= -1.0f; positionData[23].nz= 0.0f;
		positionData[23].s= 0.0f; positionData[23].t= 1.0f;
		
		glBufferData(GL_ARRAY_BUFFER, size, positionData, GL_STATIC_DRAW);
		
		// Create IBO
		const GLsizei indexNbr= 36; // 6 face * 2 * 3 Vertexs
		const GLsizeiptr IndexSize = indexNbr * sizeof(GLuint);
		GLuint IndexData[] =
		{
				0, 1, 2, 2, 3, 0,		// Face 1
				4, 5, 6, 6, 7, 4,		// Face 2
				8, 9, 10, 10, 11, 8,	// Face 3	
				12, 13, 14, 14, 15, 12,	// Face 4
				16, 17, 18, 18, 19, 16,	// Face 5
				20, 21, 22, 22, 23, 20	// Face 6
				
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexSize, IndexData, GL_STATIC_DRAW);
	}
	glVertexPointer(3, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(12));
	glTexCoordPointer(2, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(24));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawRangeElements(GL_TRIANGLES, 0, 36, 36, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Box::GlDraw ", error);
		throw(OpenGlException);
	}
}
