/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_meshdata.cpp Implementation for the GLC_MeshData class.

#include "../glc_exception.h"
#include "glc_meshdata.h"
#include "../glc_state.h"
#include "../glc_contextmanager.h"

// Class chunk id
quint32 GLC_MeshData::m_ChunkId= 0xA704;

// Default constructor
GLC_MeshData::GLC_MeshData()
    : m_VertexBuffer()
    , m_Positions()
    , m_Normals()
    , m_Texels()
    , m_Colors()
    , m_NormalBuffer()
    , m_TexelBuffer()
    , m_ColorBuffer()
    , m_LodList()
    , m_PositionSize(-1)
    , m_TexelsSize(-1)
    , m_ColorSize(-1)
    , m_UseVbo(false)
{

}

// Copy constructor
GLC_MeshData::GLC_MeshData(const GLC_MeshData& meshData)
    : m_VertexBuffer()
    , m_Positions(meshData.positionVector())
    , m_Normals(meshData.normalVector())
    , m_Texels(meshData.texelVector())
    , m_Colors(meshData.colorVector())
    , m_NormalBuffer()
    , m_TexelBuffer()
    , m_ColorBuffer()
    , m_LodList()
    , m_PositionSize(meshData.m_PositionSize)
    , m_TexelsSize(meshData.m_TexelsSize)
    , m_ColorSize(meshData.m_ColorSize)
    , m_UseVbo(meshData.m_UseVbo)
{
	// Copy meshData LOD list
	const int size= meshData.m_LodList.size();
	for (int i= 0; i < size; ++i)
	{
		m_LodList.append(new GLC_Lod(*meshData.m_LodList.at(i)));
	}
}

// Overload "=" operator
GLC_MeshData& GLC_MeshData::operator=(const GLC_MeshData& meshData)
{
	if (this != &meshData)
	{
		// Clear the content of the mesh Data
		clear();

		// Copy mesh Data members
		m_Positions= meshData.positionVector();
		m_Normals= meshData.normalVector();
		m_Texels= meshData.texelVector();
		m_Colors= meshData.colorVector();
		m_PositionSize= meshData.m_PositionSize;
		m_TexelsSize= meshData.m_TexelsSize;
		m_ColorSize= meshData.m_ColorSize;
		m_UseVbo= meshData.m_UseVbo;

		// Copy meshData LOD list
		const int size= meshData.m_LodList.size();
		for (int i= 0; i < size; ++i)
		{
			m_LodList.append(new GLC_Lod(*meshData.m_LodList.at(i)));
		}
	}
	return *this;
}

GLC_MeshData::~GLC_MeshData()
{
	clear();
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////
// Return the class Chunk ID
quint32 GLC_MeshData::chunckID()
{
	return m_ChunkId;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// If the there is more than 2 LOD Swap the first and last
void GLC_MeshData::finishLod()
{
	// PLace the master LOD at the beginning of the list
	const int size= m_LodList.size();
	if (size > 1)
	{
		GLC_Lod* PMasterLod= m_LodList.at(size - 1);
		m_LodList.removeAt(size - 1);
		m_LodList.prepend(PMasterLod);
	}
}

// Clear the content of the meshData and makes it empty
void GLC_MeshData::clear()
{
	m_Positions.clear();
	m_Normals.clear();
	m_Texels.clear();
	m_Colors.clear();
	m_PositionSize= -1;
	m_TexelsSize= -1;
	m_ColorSize= -1;

	// Delete Main Vbo ID
	if (m_VertexBuffer.isCreated())
	{
		m_VertexBuffer.destroy();
	}

	// Delete Normal VBO
	if (m_NormalBuffer.isCreated())
	{
		m_NormalBuffer.destroy();
	}

	// Delete Texel VBO
	if (m_TexelBuffer.isCreated())
	{
		m_TexelBuffer.destroy();
	}
	// Delete color index
	if (m_ColorBuffer.isCreated())
	{
		m_ColorBuffer.destroy();
	}

	const int size= m_LodList.size();
	for (int i= 0; i < size; ++i)
	{
		delete m_LodList.at(i);
	}
	m_LodList.clear();
}

void GLC_MeshData::releaseVboClientSide(bool update)
{
	if (m_VertexBuffer.isCreated() && !m_Positions.isEmpty())
	{
		if (update)
		{
			fillVbo(GLC_MeshData::GLC_Vertex);
			fillVbo(GLC_MeshData::GLC_Normal);
			fillVbo(GLC_MeshData::GLC_Texel);
			fillVbo(GLC_MeshData::GLC_Color);
            QOpenGLBuffer::release(QOpenGLBuffer::VertexBuffer);
		}
		m_PositionSize= m_Positions.size();
		m_TexelsSize= m_Texels.size();
		m_ColorSize= m_Colors.size();
	}
}

void GLC_MeshData::setVboUsage(bool usage)
{
	if (usage && (m_PositionSize != -1) && (!m_Positions.isEmpty()) && (!m_VertexBuffer.isCreated()))
	{
		createVBOs();

		fillVbo(GLC_MeshData::GLC_Vertex);
		fillVbo(GLC_MeshData::GLC_Normal);
		fillVbo(GLC_MeshData::GLC_Texel);
		fillVbo(GLC_MeshData::GLC_Color);
        QOpenGLBuffer::release(QOpenGLBuffer::VertexBuffer);

		const int lodCount= m_LodList.count();
		for (int i= 0; i < lodCount; ++i)
		{
			m_LodList.at(i)->setIboUsage(usage);
		}

	}
	else if (!usage && m_VertexBuffer.isCreated())
	{
		m_Positions= positionVector();
		m_PositionSize= m_Positions.size();
		m_VertexBuffer.destroy();

		m_Normals= normalVector();
		m_NormalBuffer.destroy();

		if (m_TexelBuffer.isCreated())
		{
			m_Texels= texelVector();
			m_TexelsSize= m_Texels.size();
			m_TexelBuffer.destroy();
		}
		if (m_ColorBuffer.isCreated())
		{
			m_Colors= colorVector();
			m_ColorSize= m_Colors.size();
			m_ColorBuffer.destroy();
		}

		const int lodCount= m_LodList.count();
		for (int i= 0; i < lodCount; ++i)
		{
			m_LodList.at(i)->setIboUsage(usage);
		}
	}
	m_UseVbo= usage;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
// Vbo creation
bool GLC_MeshData::createVBOs()
{
    bool subject= false;

	// Create position VBO
    if (!m_VertexBuffer.isCreated() && GLC_ContextManager::instance()->currentContext())
	{
		m_VertexBuffer.create();
		m_NormalBuffer.create();

		// Create Texel VBO
		if (!m_TexelBuffer.isCreated() && !m_Texels.isEmpty())
		{
			m_TexelBuffer.create();
		}

		// Create Color VBO
		if (!m_ColorBuffer.isCreated() && !m_Colors.isEmpty())
		{
			m_ColorBuffer.create();
		}

		const int size= m_LodList.size();
		for (int i= 0; i < size; ++i)
		{
			m_LodList.at(i)->createIBO();
		}
        subject= true;
	}
    else if (!m_VertexBuffer.isCreated())
    {
        qDebug() << "GLC_MeshData::createVBOs() No current context";
    }

    return subject;
}

// Ibo Usage
bool GLC_MeshData::useVBO(GLC_MeshData::VboType vboType)
{
	bool result= true;
    // Chose the right VBO
    if (vboType == GLC_MeshData::GLC_Vertex)
    {
        if (!m_VertexBuffer.bind())
        {
            GLC_Exception exception("GLC_MeshData::useVBO  Failed to bind vertex buffer");
            throw(exception);
        }
    }
    else if (vboType == GLC_MeshData::GLC_Normal)
    {
        if (!m_NormalBuffer.bind())
        {
            GLC_Exception exception("GLC_MeshData::useVBO  Failed to bind normal buffer");
            throw(exception);
        }
    }
    else if ((vboType == GLC_MeshData::GLC_Texel) && m_TexelBuffer.isCreated())
    {
        if (!m_TexelBuffer.bind())
        {
            GLC_Exception exception("GLC_MeshData::useVBO  Failed to bind texel buffer");
            throw(exception);
        }
    }
    else if ((vboType == GLC_MeshData::GLC_Color) && m_ColorBuffer.isCreated())
    {
        if (!m_ColorBuffer.bind())
        {
            GLC_Exception exception("GLC_MeshData::useVBO  Failed to bind color buffer");
            throw(exception);
        }
    }
    else result= false;

    return result;
}

void GLC_MeshData::fillVbo(GLC_MeshData::VboType type)
{
	// Chose the right VBO
	if (type == GLC_MeshData::GLC_Vertex)
	{
        useVBO(type);
		const GLsizei dataNbr= static_cast<GLsizei>(m_Positions.size());
		const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
		m_VertexBuffer.allocate(m_Positions.data(), dataSize);

		m_PositionSize= m_Positions.size();
	}
	else if (type == GLC_MeshData::GLC_Normal)
	{
        useVBO(type);
		const GLsizei dataNbr= static_cast<GLsizei>(m_Normals.size());
		const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
		m_NormalBuffer.allocate(m_Normals.data(), dataSize);
	}
	else if ((type == GLC_MeshData::GLC_Texel) && m_TexelBuffer.isCreated())
	{
        useVBO(type);
		const GLsizei dataNbr= static_cast<GLsizei>(m_Texels.size());
		const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
		m_TexelBuffer.allocate(m_Texels.data(), dataSize);

		m_TexelsSize= m_Texels.size();
	}
	else if ((type == GLC_MeshData::GLC_Color) && m_ColorBuffer.isCreated())
	{
        useVBO(type);
		const GLsizei dataNbr= static_cast<GLsizei>(m_Colors.size());
		const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
		m_ColorBuffer.allocate(m_Colors.data(), dataSize);

		m_ColorSize= m_Colors.size();
    }
}

void GLC_MeshData::fillLodIbo()
{
	const int lodCount= m_LodList.count();
	for (int i= 0; i < lodCount; ++i)
	{
		m_LodList.at(i)->fillIbo();
	}
}
// Non Member methods
// Non-member stream operator
QDataStream &operator<<(QDataStream &stream, const GLC_MeshData &meshData)
{
	quint32 chunckId= GLC_MeshData::m_ChunkId;
	stream << chunckId;

	stream << meshData.positionVector();
	stream << meshData.normalVector();
	stream << meshData.texelVector();
	stream << meshData.colorVector();

	// List of lod serialisation
	const int lodCount= meshData.m_LodList.size();
	QList<GLC_Lod> lodsList;
	for (int i= 0; i < lodCount; ++i)
	{
		lodsList.append(*(meshData.m_LodList[i]));
	}
	stream << lodsList;

	return stream;
}

QDataStream &operator>>(QDataStream &stream, GLC_MeshData &meshData)
{
	quint32 chunckId;
	stream >> chunckId;
	Q_ASSERT(chunckId == GLC_MeshData::m_ChunkId);

	meshData.clear();

	GLfloatVector position, normal, texel, color;

	stream >> meshData.m_Positions;
	stream >> meshData.m_Normals;
	stream >> meshData.m_Texels;
	stream >> meshData.m_Colors;

	// List of lod serialisation
	QList<GLC_Lod> lodsList;
	stream >> lodsList;
	const int lodCount= lodsList.size();
	for (int i= 0; i < lodCount; ++i)
	{
		meshData.m_LodList.append(new GLC_Lod(lodsList.at(i)));
	}

	return stream;
}

