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

//! \file glc_rectangle.cpp Implementation for the GLC_Rectangle class.

#include "glc_rectangle.h"
#include "../glc_state.h"

// Default constructor
GLC_Rectangle::GLC_Rectangle()
:GLC_Mesh()
, m_L1(1.0)
, m_L2(1.0)
{

}

// Complete constructor
GLC_Rectangle::GLC_Rectangle(double l1, double l2)
:GLC_Mesh()
, m_L1(l1)
, m_L2(l2)
{

}

// Copy constructor
GLC_Rectangle::GLC_Rectangle(const GLC_Rectangle& rect)
:GLC_Mesh(rect)
, m_L1(rect.m_L1)
, m_L2(rect.m_L2)
{

}


GLC_Rectangle::~GLC_Rectangle()
{

}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// clone the rectangle
GLC_Geometry* GLC_Rectangle::clone() const
{
	return new GLC_Rectangle(*this);
}

// return the rectangle bounding box
GLC_BoundingBox& GLC_Rectangle::boundingBox()
{
	if (GLC_Mesh::isEmpty())
	{
		createMesh();
	}
	return GLC_Mesh::boundingBox();
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set the rectangle
GLC_Rectangle& GLC_Rectangle::setRectangle(double l1, double l2)
{
	m_L1= l1;
	m_L2= l2;

	delete m_pBoundingBox;
	m_pBoundingBox= NULL;

	GLC_Mesh::clearOnlyMesh();

	return *this;
}

// Set rectangle length 1
void GLC_Rectangle::setLength1(double value)
{
	m_L1= value;
	delete m_pBoundingBox;
	m_pBoundingBox= NULL;

	GLC_Mesh::clearOnlyMesh();
}

// Set rectangle length 2
void GLC_Rectangle::setLength2(double value)
{
	m_L2= value;
	delete m_pBoundingBox;
	m_pBoundingBox= NULL;

	GLC_Mesh::clearOnlyMesh();
}

//////////////////////////////////////////////////////////////////////
// Private OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Virtual interface for OpenGL Geometry set up.
void GLC_Rectangle::glDraw(const GLC_RenderProperties& renderProperties)
{
	if (GLC_Mesh::isEmpty())
	{
		createMesh();
	}

	GLC_Mesh::glDraw(renderProperties);
}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

// Create rectangle mesh
void GLC_Rectangle::createMesh()
{
	Q_ASSERT(GLC_Mesh::isEmpty());

	const GLfloat lgX= static_cast<const GLfloat>(m_L1 / 2.0);
	const GLfloat lgY= static_cast<const GLfloat>(m_L2 / 2.0);

	GLfloatVector verticeVector;
	GLfloatVector normalsVector;
	GLfloatVector texelVector;

	// Face 1
	verticeVector << -lgX; verticeVector << -lgY; verticeVector << 0.0f;
	normalsVector << 0.0f; normalsVector << 0.0f; normalsVector << 1.0f;
	texelVector << 0.0f; texelVector << 0.0f;

	verticeVector << lgX; verticeVector << -lgY; verticeVector << 0.0f;
	normalsVector << 0.0f; normalsVector << 0.0f; normalsVector << 1.0f;
	texelVector << 1.0f; texelVector << 0.0f;

	verticeVector << lgX; verticeVector << lgY; verticeVector << 0.0f;
	normalsVector << 0.0f; normalsVector << 0.0f; normalsVector << 1.0f;
	texelVector << 1.0f; texelVector << 1.0f;

	verticeVector << -lgX; verticeVector << lgY; verticeVector << 0.0f;
	normalsVector << 0.0f; normalsVector << 0.0f; normalsVector << 1.0f;
	texelVector << 0.0f; texelVector << 1.0f;

	// Add bulk data in to the mesh
	GLC_Mesh::addVertices(verticeVector);
	GLC_Mesh::addNormals(normalsVector);
	GLC_Mesh::addTexels(texelVector);

	// Set the material to use
	GLC_Material* pMaterial;
	if (hasMaterial())
	{
		pMaterial= this->firstMaterial();
	}
	else
	{
		pMaterial= new GLC_Material();
	}

	IndexList index;
	// Face 1
	index << 0 << 1 << 3 << 2;
	GLC_Mesh::addTrianglesStrip(pMaterial, index);

	GLC_Mesh::finish();
}


