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

//! \file glc_wiredata.cpp Implementation for the GLC_WireData class.

#include "glc_wiredata.h"
#include "glc_bsrep.h"
#include "../glc_ext.h"
#include "../glc_state.h"
#include "../glc_exception.h"
#include "../glc_contextmanager.h"

// Class chunk id
// Old chunkId = 0xA706
quint32 GLC_WireData::m_ChunkId= 0xA711;


GLC_WireData::GLC_WireData()
: m_VerticeBuffer()
, m_NextPrimitiveLocalId(1)
, m_Positions()
, m_ColorBuffer()
, m_Colors()
, m_IndexBuffer(QOpenGLBuffer::IndexBuffer)
, m_IndexVector()
, m_PositionSize(0)
, m_ColorSize(0)
, m_pBoundingBox(NULL)
, m_VerticeGrouprSizes()
, m_VerticeGroupOffseti()
, m_VerticeGroupOffset()
, m_VerticeGroupId()
, m_VerticeGroupCount(0)
, m_UseVbo(false)
{

}


GLC_WireData::GLC_WireData(const GLC_WireData& data)
: m_VerticeBuffer()
, m_NextPrimitiveLocalId(data.m_NextPrimitiveLocalId)
, m_Positions(data.positionVector())
, m_ColorBuffer()
, m_Colors(data.colorVector())
, m_IndexBuffer(QOpenGLBuffer::IndexBuffer)
, m_IndexVector(data.indexVector())
, m_PositionSize(data.m_PositionSize)
, m_ColorSize(data.m_ColorSize)
, m_pBoundingBox(NULL)
, m_VerticeGrouprSizes(data.m_VerticeGrouprSizes)
, m_VerticeGroupOffseti(data.m_VerticeGroupOffseti)
, m_VerticeGroupOffset(data.m_VerticeGroupOffset)
, m_VerticeGroupId(data.m_VerticeGroupId)
, m_VerticeGroupCount(data.m_VerticeGroupCount)
, m_UseVbo(data.m_UseVbo)
{
	if (NULL != data.m_pBoundingBox)
	{
		m_pBoundingBox= new GLC_BoundingBox(*(data.m_pBoundingBox));
	}
}


GLC_WireData& GLC_WireData::operator=(const GLC_WireData& data)
{
	if (this != &data)
	{
		clear();
		m_NextPrimitiveLocalId= data.m_NextPrimitiveLocalId;
		m_Positions= data.positionVector();
		m_Colors= data.colorVector();
		m_IndexVector= data.indexVector();
		m_PositionSize= data.m_PositionSize;
		m_ColorSize= data.m_ColorSize;
		if (NULL != data.m_pBoundingBox)
		{
			m_pBoundingBox= new GLC_BoundingBox(*(data.m_pBoundingBox));
		}
		m_VerticeGrouprSizes= data.m_VerticeGrouprSizes;
		m_VerticeGroupOffseti= data.m_VerticeGroupOffseti;
		m_VerticeGroupOffset= data.m_VerticeGroupOffset;
		m_VerticeGroupId= data.m_VerticeGroupId;
		m_VerticeGroupCount= data.m_VerticeGroupCount;
		m_UseVbo= data.m_UseVbo;
	}
	return *this;
}

GLC_WireData::~GLC_WireData()
{
	clear();
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////


quint32 GLC_WireData::chunckID()
{
	return m_ChunkId;
}

GLC_BoundingBox& GLC_WireData::boundingBox()
{
	if (NULL == m_pBoundingBox)
	{
		m_pBoundingBox= new GLC_BoundingBox();

		if (m_Positions.isEmpty())
		{
			//qDebug() << "GLC_WireData::getBoundingBox empty m_Positions";
		}
		else
		{
			const int max= m_Positions.size();
			if (max == 3) // Only One point
			{
				const double delta= 1e-2;
				GLC_Point3d lower(m_Positions[0] - delta,
						m_Positions[1] - delta,
						m_Positions[2] - delta);
				GLC_Point3d upper(m_Positions[0] + delta,
						m_Positions[1] + delta,
						m_Positions[2] + delta);
				m_pBoundingBox->combine(lower);
				m_pBoundingBox->combine(upper);

			}
			else
			{
				for (int i= 0; i < max; i= i + 3)
				{
					GLC_Point3d point(m_Positions[i], m_Positions[i + 1], m_Positions[i + 2]);
					m_pBoundingBox->combine(point);
				}
			}
		}

	}
	return *m_pBoundingBox;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////


GLC_uint GLC_WireData::addVerticeGroup(const GLfloatVector& floatVector)
{
	Q_ASSERT((floatVector.size() % 3) == 0);

	++m_VerticeGroupCount;
	m_Positions+= floatVector;

	m_VerticeGrouprSizes.append(static_cast<GLsizei>(floatVector.size() / 3));

	if (m_VerticeGroupOffseti.isEmpty())
	{
		m_VerticeGroupOffseti.append(0);
	}
	int offset= m_VerticeGroupOffseti.last() + m_VerticeGrouprSizes.last();
	m_VerticeGroupOffseti.append(offset);

	// The Polyline id
	m_VerticeGroupId.append(m_NextPrimitiveLocalId);
	return m_NextPrimitiveLocalId++;
}

void GLC_WireData::clear()
{
	m_VerticeBuffer.destroy();
    m_ColorBuffer.destroy();
    m_IndexBuffer.destroy();

	m_NextPrimitiveLocalId= 1;
	m_Positions.clear();
	m_PositionSize= 0;
	delete m_pBoundingBox;
	m_pBoundingBox= NULL;

	m_VerticeGrouprSizes.clear();
	m_VerticeGroupOffseti.clear();
	m_VerticeGroupId.clear();
	m_VerticeGroupCount= 0;
}

void GLC_WireData::releaseVboClientSide(bool update)
{
    if (m_VerticeBuffer.isCreated() && !m_Positions.isEmpty())
    {
        if (update)
        {
            finishVbo();
        }
        else
        {
            m_PositionSize= m_Positions.size();
            m_ColorSize= m_Colors.size();
        }
    }
}

void GLC_WireData::setVboUsage(bool usage)
{
	m_UseVbo= usage;
	if (!isEmpty())
	{
		if (m_UseVbo && (m_PositionSize != 0) && (!m_Positions.isEmpty())&& (!m_VerticeBuffer.isCreated()))
		{
			finishVbo();
		}
		else if (!m_UseVbo && m_VerticeBuffer.isCreated())
		{
			m_Positions= positionVector();
			m_VerticeBuffer.destroy();
			m_Colors= colorVector();
			m_ColorBuffer.destroy();
			m_IndexVector= indexVector();
			m_IndexBuffer.destroy();
		}
    }
}

void GLC_WireData::add(const GLC_WireData& other, const GLC_Matrix4x4& matrix)
{
    GLfloatVector position= other.positionVector();
    const int verticeGroupCount= other.verticeGroupCount();

    int startIndex= 0;
    for (int iGroup= 0; iGroup < verticeGroupCount; ++iGroup)
    {
        GLsizei verticeGroupSize= other.verticeGroupSize(iGroup);
        int size= 3 * static_cast<int>(verticeGroupSize);

        GLfloatVector currentPosition;

        for (int i= startIndex; i < (startIndex + size); i+=3)
        {
            GLC_Vector3d vect(static_cast<double>(position.at(i))
                              , static_cast<double>(position.at(i + 1))
                              , static_cast<double>(position.at(i + 2)));
            vect= matrix * vect;
            currentPosition.append(static_cast<float>(vect.x()));
            currentPosition.append(static_cast<float>(vect.y()));
            currentPosition.append(static_cast<float>(vect.z()));
        }
        startIndex+= size;
        addVerticeGroup(currentPosition);
    }
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

void GLC_WireData::finishVbo()
{
	if (!m_VerticeBuffer.isCreated())
	{
		m_VerticeBuffer.create();
	}
	if ((m_Colors.size() > 0) && !m_ColorBuffer.isCreated())
	{
		m_ColorBuffer.create();
	}
	if (!m_IndexBuffer.isCreated())
	{
		m_IndexBuffer.create();
	}
	fillVBOs();

	m_PositionSize= m_Positions.size();

	if (m_ColorBuffer.isCreated())
	{
		m_ColorSize= m_Colors.size();
	}
}

void GLC_WireData::useVBO(GLC_WireData::VboType type)
{
    // Chose the right VBO
    if (type == GLC_WireData::GLC_Vertex)
    {
        if (!m_VerticeBuffer.bind())
        {
            GLC_Exception exception("GLC_WireData::useVBO  Failed to bind vertex buffer");
            throw(exception);
        }
    }
    else if (type == GLC_WireData::GLC_Color)
    {
        Q_ASSERT(m_ColorSize > 0);
        if (!m_ColorBuffer.bind())
        {
            GLC_Exception exception("GLC_WireData::useVBO  Failed to bind color buffer");
            throw(exception);
        }
    }
    else if ((type == GLC_WireData::GLC_Index) && m_IndexBuffer.isCreated())
    {
        if (!m_IndexBuffer.bind())
        {
            GLC_Exception exception("GLC_WireData::useVBO  Failed to bind index buffer");
            throw(exception);
        }
    }
}

void GLC_WireData::glDraw(const GLC_RenderProperties&, GLenum mode)
{
    Q_ASSERT((NULL != QOpenGLContext::currentContext()) &&  QOpenGLContext::currentContext()->isValid());
	Q_ASSERT(!isEmpty());

    const bool vboIsUsed= m_UseVbo;

	if (vboIsUsed && ((m_PositionSize == 0) || !m_VerticeBuffer.isCreated()))
	{
		finishOffset();
		buidIndex();
		finishVbo();
	}
	else if (!vboIsUsed && (m_PositionSize == 0))
	{
		finishOffset();
		buidIndex();
		m_PositionSize= m_Positions.size();
		m_ColorSize= m_Colors.size();
	}

	// Activate VBO or Vertex Array
	if (vboIsUsed)
	{
		activateVboAndIbo();
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
		if (m_ColorSize > 0)
		{
			glColorPointer(4, GL_FLOAT, 0, 0);
			glEnableClientState(GL_COLOR_ARRAY);
		}

		// Render polylines
		for (int i= 0; i < m_VerticeGroupCount; ++i)
		{
			glDrawElements(mode, m_VerticeGrouprSizes.at(i), GL_UNSIGNED_INT, m_VerticeGroupOffset.at(i));
		}

        QOpenGLBuffer::release(QOpenGLBuffer::VertexBuffer);
        QOpenGLBuffer::release(QOpenGLBuffer::IndexBuffer);
    }
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, m_Positions.data());
		glEnableClientState(GL_VERTEX_ARRAY);
		if (m_ColorSize > 0)
		{
			glColorPointer(4, GL_FLOAT, 0, m_Colors.data());
			glEnableClientState(GL_COLOR_ARRAY);
		}
		// Render polylines
		for (int i= 0; i < m_VerticeGroupCount; ++i)
		{
			glDrawElements(mode, m_VerticeGrouprSizes.at(i), GL_UNSIGNED_INT, &(m_IndexVector.data()[m_VerticeGroupOffseti.at(i)]));
		}

	}

	if (m_ColorSize > 0)
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}

	glDisableClientState(GL_VERTEX_ARRAY);

	if (vboIsUsed)
	{
		QOpenGLBuffer::release(QOpenGLBuffer::IndexBuffer);
		QOpenGLBuffer::release(QOpenGLBuffer::VertexBuffer);
	}
}


void GLC_WireData::fillVBOs()
{
	{
		Q_ASSERT(m_VerticeBuffer.isCreated());
        useVBO(GLC_WireData::GLC_Vertex);
		const GLsizei dataNbr= static_cast<GLsizei>(m_Positions.size());
		const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
		m_VerticeBuffer.allocate(m_Positions.data(), dataSize);
	}

	{
		Q_ASSERT(m_IndexBuffer.isCreated());
        useVBO(GLC_WireData::GLC_Index);
		const GLsizei dataNbr= static_cast<GLsizei>(m_IndexVector.size());
		const GLsizeiptr dataSize= dataNbr * sizeof(GLuint);
		m_IndexBuffer.allocate(m_IndexVector.data(), dataSize);
	}

	if (m_ColorBuffer.isCreated())
	{
        useVBO(GLC_WireData::GLC_Color);
		const GLsizei dataNbr= static_cast<GLsizei>(m_Colors.size());
		const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
		m_ColorBuffer.allocate(m_Colors.data(), dataSize);
	}
}

void GLC_WireData::buidIndex()
{
	const int size= m_Positions.size();
	m_IndexVector.resize(size);
	for (int i= 0; i < size; ++i)
	{
		m_IndexVector[i]= i;
	}
}

void GLC_WireData::activateVboAndIbo()
{
	// Activate Vertices VBO
    useVBO(GLC_WireData::GLC_Vertex);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	// Activate Color VBO if needed
	if (m_ColorSize > 0)
	{
        useVBO(GLC_WireData::GLC_Color);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		glColorPointer(4, GL_FLOAT, 0, 0);
		glEnableClientState(GL_COLOR_ARRAY);
	}

	// Activate index Buffer object
    useVBO(GLC_WireData::GLC_Index);
}

void GLC_WireData::finishOffset()
{
	m_VerticeGroupOffseti.remove(m_VerticeGroupOffseti.size() - 1);
	m_VerticeGroupOffset.clear();
	const int offsetSize= m_VerticeGroupOffseti.size();
	for (int i= 0; i < offsetSize; ++i)
	{
		m_VerticeGroupOffset.append(BUFFER_OFFSET(static_cast<GLsizei>(m_VerticeGroupOffseti.at(i)) * sizeof(GLuint)));
    }
}

QDataStream &operator<<(QDataStream &stream, const GLC_WireData &wireData)
{
	quint32 chunckId= GLC_WireData::m_ChunkId;
	stream << chunckId;

	stream << wireData.m_NextPrimitiveLocalId;
    stream << wireData.m_Positions;
	stream << wireData.m_PositionSize;

	stream << wireData.m_VerticeGrouprSizes;
	stream << wireData.m_VerticeGroupOffseti;
	stream << wireData.m_VerticeGroupId;
	stream << wireData.m_VerticeGroupCount;

	// New version Data
    stream << wireData.m_Colors;
	stream << wireData.m_ColorSize;

	return stream;
}

QDataStream &operator>>(QDataStream &stream, GLC_WireData &wireData)
{
	quint32 chunckId;
	stream >> chunckId;
	Q_ASSERT((chunckId == GLC_WireData::m_ChunkId) || chunckId == 0xA706);

	wireData.clear();
	stream >> wireData.m_NextPrimitiveLocalId;
	stream >> wireData.m_Positions;
	stream >> wireData.m_PositionSize;

	stream >> wireData.m_VerticeGrouprSizes;
	stream >> wireData.m_VerticeGroupOffseti;
	stream >> wireData.m_VerticeGroupId;
	stream >> wireData.m_VerticeGroupCount;

	if (chunckId == GLC_WireData::m_ChunkId)
	{
		// New version Data
		stream >> wireData.m_Colors;
		stream >> wireData.m_ColorSize;
    }
    wireData.buidIndex();

	return stream;
}
