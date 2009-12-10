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

//! \file glc_wiredata.cpp Implementation for the GLC_WireData class.

#include "glc_wiredata.h"
#include "../glc_state.h"

// Default constructor
GLC_WireData::GLC_WireData()
: m_VboId(0)
, m_Positions()
, m_PositionSize(0)
, m_pBoundingBox(NULL)
{

}

// Copy constructor
GLC_WireData::GLC_WireData(const GLC_WireData& data)
: m_VboId(0)
, m_Positions(data.positionVector())
, m_PositionSize(data.m_PositionSize)
, m_pBoundingBox(NULL)
{

}

// Overload "=" operator
GLC_WireData& GLC_WireData::operator=(const GLC_WireData& data)
{
	clear();

	if (this != &data)
	{
		m_Positions= data.positionVector();
		m_PositionSize= data.m_PositionSize;
	}
	return *this;
}

GLC_WireData::~GLC_WireData()
{
	clear();

	// Delete Main Vbo ID
	if (0 != m_VboId)
	{
		glDeleteBuffers(1, &m_VboId);
		m_VboId= 0;
	}
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Return the Position Vector
GLfloatVector GLC_WireData::positionVector() const
{
	if (0 != m_VboId)
	{
		// VBO created get data from VBO
		const int sizeOfVbo= m_PositionSize;
		const GLsizeiptr dataSize= sizeOfVbo * sizeof(float);
		GLfloatVector positionVector(sizeOfVbo);

		glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
		GLvoid* pVbo = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		memcpy(positionVector.data(), pVbo, dataSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return positionVector;
	}
	else
	{
		return m_Positions;
	}
}

// Return the wire bounding box
GLC_BoundingBox& GLC_WireData::boundingBox()
{
	if (NULL == m_pBoundingBox)
	{
		m_pBoundingBox= new GLC_BoundingBox();

		if (m_Positions.isEmpty())
		{
			qDebug() << "GLC_WireData::getBoundingBox empty m_Positions";
		}
		else
		{
			const int max= m_Positions.size();
			for (int i= 0; i < max; i= i + 3)
			{
				GLC_Vector3d vector(m_Positions[i], m_Positions[i + 1], m_Positions[i + 2]);
				m_pBoundingBox->combine(vector);
			}
		}

	}
	return *m_pBoundingBox;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Clear the content of the meshData and makes it empty
void GLC_WireData::clear()
{
	m_Positions.clear();
	m_PositionSize= 0;
	delete m_pBoundingBox;
	m_pBoundingBox= NULL;
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
// The mesh wich use the data is finished
void GLC_WireData::finishVbo()
{
	createVBOs();
	useVBO(true);
	fillVBOs();
	useVBO(false);

	m_PositionSize= m_Positions.size();
	m_Positions.clear();
}

// Ibo Usage
void GLC_WireData::useVBO(bool use)
{
	if (use)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VboId);	}
	else
	{
		// Unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

// Display wires data
void GLC_WireData::glDraw(const GLC_RenderProperties&)
{
	Q_ASSERT(!isEmpty());
	const bool vboIsUsed= GLC_State::vboUsed();

	if (vboIsUsed && ((m_PositionSize == 0) || (0 == m_VboId)))
	{
		finishVbo();
	}
	else if (m_PositionSize == 0)
	{
		m_PositionSize= m_Positions.size();
	}

	// Activate VBO or Vertex Array
	if (vboIsUsed)
	{
		useVBO(true);
		glVertexPointer(3, GL_FLOAT, 0, 0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, m_Positions.data());
	}
	glEnableClientState(GL_VERTEX_ARRAY);

	// Render wires
	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(m_PositionSize / 3));

	// Desactivate VBO or Vertex Array
	if (vboIsUsed)
	{
		useVBO(false);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
}


// Vbo creation
void GLC_WireData::createVBOs()
{
	// Create position VBO
	if (0 == m_VboId)
	{
		glGenBuffers(1, &m_VboId);
	}
}

// Fill the VBO
void GLC_WireData::fillVBOs()
{
	const GLsizei dataNbr= static_cast<GLsizei>(m_Positions.size());
	const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER, dataSize, m_Positions.data(), GL_STATIC_DRAW);
}
