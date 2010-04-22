/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 2.0.0 Beta 1, packaged on April 2010.

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

//! \file glc_lod.cpp implementation of the GLC_Lod class.


#include "glc_lod.h"

// Class chunk id
quint32 GLC_Lod::m_ChunkId= 0xA708;

// Default Constructor
GLC_Lod::GLC_Lod()
: m_Accuracy(0.0)
, m_IboId(0)
, m_IndexVector()
, m_IndexSize(0)
{

}

// Construct an engine Lod with the specified accuracy
GLC_Lod::GLC_Lod(double accuracy)
: m_Accuracy(accuracy)
, m_IboId(0)
, m_IndexVector()
, m_IndexSize(0)
{

}

// Copy Constructor
GLC_Lod::GLC_Lod(const GLC_Lod& lod)
: m_Accuracy(lod.m_Accuracy)
, m_IboId(0)
, m_IndexVector(lod.indexVector())
, m_IndexSize(lod.m_IndexSize)
{


}

// Overload "=" operator
GLC_Lod& GLC_Lod::operator=(const GLC_Lod& lod)
{
	if (this != &lod)
	{
		m_Accuracy= lod.m_Accuracy;
		m_IboId= 0;
		m_IndexVector= lod.indexVector();
		m_IndexSize= lod.m_IndexSize;
	}

	return *this;
}

GLC_Lod::~GLC_Lod()
{
	// Delete IBO
	if (0 != m_IboId)
	{
		glDeleteBuffers(1, &m_IboId);
	}
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////
// Return the class Chunk ID
quint32 GLC_Lod::chunckID()
{
	return m_ChunkId;
}

// Return The unique index Vector
QVector<GLuint> GLC_Lod::indexVector() const
{
	if (0 != m_IboId)
	{
		// VBO created get data from VBO
		const int sizeOfIbo= m_IndexSize;
		const GLsizeiptr dataSize= sizeOfIbo * sizeof(GLuint);
		QVector<GLuint> indexVector(sizeOfIbo);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IboId);
		GLvoid* pIbo = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
		memcpy(indexVector.data(), pIbo, dataSize);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return indexVector;
	}
	else
	{
		return m_IndexVector;
	}
}

// Non-member stream operator
QDataStream &operator<<(QDataStream &stream, const GLC_Lod &lod)
{
	quint32 chunckId= GLC_Lod::m_ChunkId;
	stream << chunckId;

	stream << lod.m_Accuracy;
	stream << lod.indexVector();

	return stream;
}
QDataStream &operator>>(QDataStream &stream, GLC_Lod &lod)
{
	quint32 chunckId;
	stream >> chunckId;
	Q_ASSERT(chunckId == GLC_Lod::m_ChunkId);

	stream >> lod.m_Accuracy;

	QVector<GLuint> indexVector;
	stream >> indexVector;

	*(lod.indexVectorHandle())= indexVector;
	return stream;
}

