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

//! \file glc_enginelod.cpp implementation of the GLC_EngineLod class.


#include "glc_enginelod.h"

// Default constructor
GLC_EngineLod::GLC_EngineLod(double accuracy)
: m_Accuracy(accuracy)
, m_TrianglesIboId(0)
, m_TrianglesIbo()
, m_TrianglesStripIboId(0)
, m_TrianglesStripIbo()
, m_TrianglesFanIboId(0)
, m_TrianglesFanIbo()
{


}

// Copy Constructor
GLC_EngineLod::GLC_EngineLod(const GLC_EngineLod& lod)
: m_Accuracy(lod.m_Accuracy)
, m_TrianglesIboId(0)
, m_TrianglesIbo(lod.m_TrianglesIbo)
, m_TrianglesStripIboId(0)
, m_TrianglesStripIbo(lod.m_TrianglesStripIbo)
, m_TrianglesFanIboId(0)
, m_TrianglesFanIbo(lod.m_TrianglesFanIbo)
{


}

GLC_EngineLod::~GLC_EngineLod()
{
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

// IBO creation
void GLC_EngineLod::createIBOs()
{
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

// Ibo Usage
void GLC_EngineLod::useIBO(GLC_EngineLod::IboType type)
{
	// Chose the right IBO
	if (type == GLC_EngineLod::GLC_Triangles)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TrianglesIboId);
	}
	else if (type == GLC_EngineLod::GLC_TrianglesStrip)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TrianglesStripIboId);
	}
	else if (type == GLC_EngineLod::GLC_TrianglesFan)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TrianglesFanIboId);
	}
}
