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

//! \file glc_primitivegroup.cpp implementation of the GLC_PrimitiveGroup class.

#include "glc_primitivegroup.h"
#include "../glc_state.h"

GLC_PrimitiveGroup::GLC_PrimitiveGroup(GLC_uint materialId)
: m_ID(materialId)
, m_TrianglesIndex()
, m_pBaseTrianglesOffset(NULL)
, m_BaseTrianglesOffseti(0)
, m_StripsIndex()
, m_StripIndexSizes()
, m_StripIndexOffset()
, m_StripIndexOffseti()
, m_FansIndex()
, m_FansIndexSizes()
, m_FanIndexOffset()
, m_FanIndexOffseti()
, m_IsFinished(false)
, m_TrianglesIndexSize(0)
, m_TrianglesStripSize(0)
, m_TrianglesFanSize(0)
{


}
//! Copy constructor
GLC_PrimitiveGroup::GLC_PrimitiveGroup(const GLC_PrimitiveGroup& group)
: m_ID(group.m_ID)
, m_TrianglesIndex(group.m_TrianglesIndex)
, m_pBaseTrianglesOffset(group.m_pBaseTrianglesOffset)
, m_BaseTrianglesOffseti(group.m_BaseTrianglesOffseti)
, m_StripsIndex(group.m_StripsIndex)
, m_StripIndexSizes(group.m_StripIndexSizes)
, m_StripIndexOffset(group.m_StripIndexOffset)
, m_StripIndexOffseti(group.m_StripIndexOffseti)
, m_FansIndex(group.m_FansIndex)
, m_FansIndexSizes(group.m_FansIndexSizes)
, m_FanIndexOffset(group.m_FanIndexOffset)
, m_FanIndexOffseti(group.m_FanIndexOffseti)
, m_IsFinished(group.m_IsFinished)
, m_TrianglesIndexSize(group.m_TrianglesIndexSize)
, m_TrianglesStripSize(group.m_TrianglesStripSize)
, m_TrianglesFanSize(group.m_TrianglesFanSize)
{


}

//! Copy constructor
GLC_PrimitiveGroup::GLC_PrimitiveGroup(const GLC_PrimitiveGroup& group, GLC_uint id)
: m_ID(id)
, m_TrianglesIndex(group.m_TrianglesIndex)
, m_pBaseTrianglesOffset(group.m_pBaseTrianglesOffset)
, m_BaseTrianglesOffseti(group.m_BaseTrianglesOffseti)
, m_StripsIndex(group.m_StripsIndex)
, m_StripIndexSizes(group.m_StripIndexSizes)
, m_StripIndexOffset(group.m_StripIndexOffset)
, m_StripIndexOffseti(group.m_StripIndexOffseti)
, m_FansIndex(group.m_FansIndex)
, m_FansIndexSizes(group.m_FansIndexSizes)
, m_FanIndexOffset(group.m_FanIndexOffset)
, m_FanIndexOffseti(group.m_FanIndexOffseti)
, m_IsFinished(group.m_IsFinished)
, m_TrianglesIndexSize(group.m_TrianglesIndexSize)
, m_TrianglesStripSize(group.m_TrianglesStripSize)
, m_TrianglesFanSize(group.m_TrianglesFanSize)
{


}

// = operator
GLC_PrimitiveGroup& GLC_PrimitiveGroup::operator=(const GLC_PrimitiveGroup& group)
{
	if (this != &group)
	{
		m_ID= group.m_ID;
		m_TrianglesIndex= group.m_TrianglesIndex;
		m_pBaseTrianglesOffset= group.m_pBaseTrianglesOffset;
		m_BaseTrianglesOffseti= group.m_BaseTrianglesOffseti;
		m_StripsIndex= group.m_StripsIndex;
		m_StripIndexSizes= group.m_StripIndexSizes;
		m_StripIndexOffset= group.m_StripIndexOffset;
		m_StripIndexOffseti= group.m_StripIndexOffseti;
		m_FansIndex= group.m_FansIndex;
		m_FansIndexSizes= group.m_FansIndexSizes;
		m_FanIndexOffset= group.m_FanIndexOffset;
		m_FanIndexOffseti= group.m_FanIndexOffseti;
		m_IsFinished= group.m_IsFinished;
		m_TrianglesIndexSize= group.m_TrianglesIndexSize;
		m_TrianglesStripSize= group.m_TrianglesStripSize;
		m_TrianglesFanSize= group.m_TrianglesFanSize;
	}
	return *this;
}

GLC_PrimitiveGroup::~GLC_PrimitiveGroup()
{

}

// Add triangle strip to the group
void GLC_PrimitiveGroup::addTrianglesStrip(const IndexList& input)
{
	m_StripsIndex+= input;
	m_TrianglesStripSize= m_StripsIndex.size();

	m_StripIndexSizes.append(static_cast<GLsizei>(input.size()));

	if (m_StripIndexOffseti.isEmpty())
	{
		m_StripIndexOffseti.append(0);
	}
	int offset= m_StripIndexOffseti.last() + m_StripIndexSizes.last();
	m_StripIndexOffseti.append(offset);
}

// Set base triangle strip offset
void GLC_PrimitiveGroup::setBaseTrianglesStripOffset(GLvoid* pOffset)
{
	m_StripIndexOffseti.pop_back();
	const int size= m_StripIndexOffseti.size();
	for (int i= 0; i < size; ++i)
	{
		m_StripIndexOffset.append(BUFFER_OFFSET(static_cast<GLsizei>(m_StripIndexOffseti[i]) * sizeof(GLuint) + reinterpret_cast<GLsizeiptr>(pOffset)));
	}
	m_StripIndexOffseti.clear();
}

// Set base triangle strip offset
void GLC_PrimitiveGroup::setBaseTrianglesStripOffseti(int offset)
{
	m_StripIndexOffseti.pop_back();
	const int size= m_StripIndexOffseti.size();
	for (int i= 0; i < size; ++i)
	{
		m_StripIndexOffseti[i]= m_StripIndexOffseti[i] + offset;
	}
}

//! Add triangle fan to the group
void GLC_PrimitiveGroup::addTrianglesFan(const IndexList& input)
{
	m_FansIndex+= input;
	m_TrianglesFanSize= m_FansIndex.size();

	m_FansIndexSizes.append(static_cast<GLsizei>(input.size()));

	if (m_FanIndexOffseti.isEmpty())
	{
		m_FanIndexOffseti.append(0);
	}
	int offset= m_FanIndexOffseti.last() + m_FansIndexSizes.last();
	m_FanIndexOffseti.append(offset);

}

// Set base triangle fan offset
void GLC_PrimitiveGroup::setBaseTrianglesFanOffset(GLvoid* pOffset)
{
	m_FanIndexOffseti.pop_back();
	const int size= m_FanIndexOffseti.size();
	for (int i= 0; i < size; ++i)
	{
		m_FanIndexOffset.append(BUFFER_OFFSET(static_cast<GLsizei>(m_FanIndexOffseti[i]) * sizeof(GLuint) + reinterpret_cast<GLsizeiptr>(pOffset)));
	}
	m_FanIndexOffseti.clear();
}

// Set base triangle fan offset
void GLC_PrimitiveGroup::setBaseTrianglesFanOffseti(int offset)
{
	m_FanIndexOffseti.pop_back();
	const int size= m_FanIndexOffseti.size();
	for (int i= 0; i < size; ++i)
	{
		m_FanIndexOffseti[i]= m_FanIndexOffseti[i] + offset;
	}
}

// Change index to VBO mode
void GLC_PrimitiveGroup::changeToVboMode()
{
	m_pBaseTrianglesOffset= BUFFER_OFFSET(static_cast<GLsizei>(m_BaseTrianglesOffseti) * sizeof(GLuint));
	m_BaseTrianglesOffseti= 0;

	m_StripIndexOffset.clear();
	const int stripOffsetSize= m_StripIndexOffseti.size();
	for (int i= 0; i < stripOffsetSize; ++i)
	{
		m_StripIndexOffset.append(BUFFER_OFFSET(static_cast<GLsizei>(m_StripIndexOffseti.at(i)) * sizeof(GLuint)));
	}
	m_StripIndexOffseti.clear();

	m_FanIndexOffset.clear();
	const int fanOffsetSize= m_FanIndexOffseti.size();
	for (int i= 0; i < fanOffsetSize; ++i)
	{
		m_FanIndexOffset.append(BUFFER_OFFSET(static_cast<GLsizei>(m_FanIndexOffseti.at(i)) * sizeof(GLuint)));
	}
	m_FanIndexOffseti.clear();
}

// Clear the group
void GLC_PrimitiveGroup::clear()
{
	m_TrianglesIndex.clear();
	m_pBaseTrianglesOffset= NULL;
	m_BaseTrianglesOffseti= 0;
	m_StripsIndex.clear();
	m_StripIndexSizes.clear();
	m_StripIndexOffset.clear();
	m_StripIndexOffseti.clear();
	m_FansIndex.clear();
	m_FansIndexSizes.clear();
	m_FanIndexOffset.clear();
	m_FanIndexOffseti.clear();
	m_IsFinished= false;
	m_TrianglesIndexSize= 0;
	m_TrianglesStripSize= 0;
	m_TrianglesFanSize= 0;
}

// Non-member stream operator
QDataStream &operator<<(QDataStream &stream, const GLC_PrimitiveGroup &primitiveGroup)
{
	Q_ASSERT(primitiveGroup.isFinished());

	// Primitive group id
	stream << primitiveGroup.m_ID;

	// Triangles, strips and fan offset index
	GLuint baseTrianglesOffseti;
	OffsetVectori stripIndexOffseti;
	OffsetVectori fanIndexOffseti;

	// Get triangles, strips and fans offset
	if (GLC_State::vboUsed())
	{
		// Convert offset to index
		// Triangles offset
		baseTrianglesOffseti= static_cast<GLuint>(reinterpret_cast<GLsizeiptr>(primitiveGroup.m_pBaseTrianglesOffset) / sizeof(GLuint));

		// Trips offsets
		for (int i= 0; i < primitiveGroup.m_TrianglesStripSize; ++i)
		{
			stripIndexOffseti.append(static_cast<GLuint>(reinterpret_cast<GLsizeiptr>(primitiveGroup.m_StripIndexOffset.at(i)) / sizeof(GLuint)));
		}
		// Fans offsets
		for (int i= 0; i < primitiveGroup.m_TrianglesFanSize; ++i)
		{
			fanIndexOffseti.append(static_cast<GLuint>(reinterpret_cast<GLsizeiptr>(primitiveGroup.m_FanIndexOffset.at(i)) / sizeof(GLuint)));
		}
	}
	else
	{
		baseTrianglesOffseti= primitiveGroup.m_BaseTrianglesOffseti;
		stripIndexOffseti= primitiveGroup.m_StripIndexOffseti;
		fanIndexOffseti= primitiveGroup.m_FanIndexOffseti;
	}
	// Triangles index
	stream << primitiveGroup.m_TrianglesIndexSize;
	stream << baseTrianglesOffseti;

	// Triangles strips index
	stream << primitiveGroup.m_TrianglesStripSize;
	stream << stripIndexOffseti;
	stream << primitiveGroup.m_StripIndexSizes;

	// Triangles fans index
	stream << primitiveGroup.m_TrianglesFanSize;
	stream << fanIndexOffseti;
	stream << primitiveGroup.m_FansIndexSizes;

	return stream;
}
QDataStream &operator>>(QDataStream &stream, GLC_PrimitiveGroup &primitiveGroup)
{
	stream >> primitiveGroup.m_ID;

	// Triangles index
	stream >> primitiveGroup.m_TrianglesIndexSize;
	stream >> primitiveGroup.m_BaseTrianglesOffseti;

	// Triangles strips index
	stream >> primitiveGroup.m_TrianglesStripSize;
	stream >> primitiveGroup.m_StripIndexOffseti;
	stream >> primitiveGroup.m_StripIndexSizes;

	// Triangles fans index
	stream >> primitiveGroup.m_TrianglesFanSize;
	stream >> primitiveGroup.m_FanIndexOffseti;
	stream >> primitiveGroup.m_FansIndexSizes;


	primitiveGroup.finished();

	return stream;
}

