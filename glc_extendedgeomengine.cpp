/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.1.0, packaged on March, 2009.

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

//! \file glc_extendedgeomengine.cpp Implementation for the GLC_ExtendedGeomEngine class.

#include "glc_extendedgeomengine.h"
#include "glc_state.h"

// Default constructor
GLC_ExtendedGeomEngine::GLC_ExtendedGeomEngine()
: GLC_GeomEngine()
, m_Positions()
, m_Normals()
, m_Texels()
, m_NormalVboId(0)
, m_TexelVboId(0)
, m_TrianglesIboId(0)
, m_TrianglesStripIboId(0)
, m_TrianglesFanIboId(0)
{


}

// Copy constructor
GLC_ExtendedGeomEngine::GLC_ExtendedGeomEngine(const GLC_ExtendedGeomEngine& engine)
: GLC_GeomEngine(engine)
, m_Positions(engine.m_Positions)
, m_Normals(engine.m_Normals)
, m_Texels(engine.m_Texels)
, m_NormalVboId(0)
, m_TexelVboId(0)
, m_TrianglesIboId(0)
, m_TrianglesStripIboId(0)
, m_TrianglesFanIboId(0)
{


}

GLC_ExtendedGeomEngine::~GLC_ExtendedGeomEngine()
{
	if (GLC_State::vboUsed())
	{
		// Delete Texel VBO
		if (0 != m_TexelVboId)
		{
			glDeleteBuffers(1, &m_TexelVboId);
		}

		// Delete Triangles IBO
		if (0 != m_TrianglesIboId)
		{
			glDeleteBuffers(1, &m_TrianglesIboId);
		}

		// Delete Triangles strip IBO
		if (0 != m_TrianglesStripIboId)
		{
			glDeleteBuffers(1, &m_TrianglesStripIboId);
		}

		// Delete Triangle fan IBO
		if (0 != m_TrianglesFanIboId)
		{
			glDeleteBuffers(1, &m_TrianglesFanIboId);
		}
	}
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
//! Vbo creation
void GLC_ExtendedGeomEngine::createVBOs()
{
	// Create position VBO
	if (0 == m_VboId)
	{
		glGenBuffers(1, &m_VboId);
		glGenBuffers(1, &m_NormalVboId);

		// Create Texel VBO
		if (0 == m_TexelVboId and not m_Texels.isEmpty())
		{
			glGenBuffers(1, &m_TexelVboId);
		}

		// Create Triangles IBO
		if (0 == m_TrianglesIboId and not m_TrianglesIbo.isEmpty())
		{
			glGenBuffers(1, &m_TrianglesIboId);
		}

		// Create Triangles strip IBO
		if (0 == m_TrianglesStripIboId and not m_TrianglesStripIbo.isEmpty())
		{
			glGenBuffers(1, &m_TrianglesStripIboId);
		}

		// Create Triangle fan IBO
		if (0 == m_TrianglesFanIboId and not m_TrianglesFanIbo.isEmpty())
		{
			glGenBuffers(1, &m_TrianglesFanIboId);
		}
	}
}

//! Vbo Usage
void GLC_ExtendedGeomEngine::useVBOs(bool use, GLC_GeomEngine::IboType type)
{
	if (use)
	{
		// Position VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
		// Normal VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_NormalVboId);
		// Texture VBO if needed
		if (0 != m_TexelVboId)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_NormalVboId);
		}
		// Chose the right IBO
		if (type == GLC_GeomEngine::GLC_Triangles)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TrianglesIboId);
		}
		else if (type == GLC_GeomEngine::GLC_TrianglesStrip)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TrianglesStripIboId);
		}
		else if (type == GLC_GeomEngine::GLC_TrianglesFan)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TrianglesFanIboId);
		}
	}
	else
	{
		// Unbind VBOs
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}

