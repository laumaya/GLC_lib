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

//! \file glc_primitivegroup.h interface for the GLC_PrimitiveGroup class.

#ifndef GLC_PRIMITIVEGROUP_H_
#define GLC_PRIMITIVEGROUP_H_

#include "glc_ext.h"
#include "glc_enum.h"
#include "glc_geomengine.h"

typedef QList<GLuint> IndexList;
typedef QVector<GLsizei> IndexSizes;
typedef QVector<GLvoid*> OffsetVector;

class GLC_PrimitiveGroup
{
public:
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

	//! Default constructor
	GLC_PrimitiveGroup(GLC_uint= 0);

	//! Copy constructor
	GLC_PrimitiveGroup(const GLC_PrimitiveGroup&);

	~GLC_PrimitiveGroup();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Return the group id
	inline GLC_uint id() const
	{return m_ID;}

	//! Return true if the group contains triangles
	inline bool containsTriangles() const
	{return not m_TrianglesIndex.isEmpty();}

	//! Return the list of triangles index of the group
	inline const IndexList& trianglesIndex() const
	{ return m_TrianglesIndex;}

	//! Return the offset of triangles index
	inline const GLvoid* trianglesIndexOffset() const
	{return m_pBaseTrianglesOffset;}

	//! Return true if the group contains strips
	inline bool containsStrip() const
	{return not m_StripsIndex.isEmpty();}

	//! Return the list of index of strips
	inline const IndexList& stripsIndex() const
	{return m_StripsIndex;}

	//! Return the vector of strips sizes
	inline const IndexSizes& stripsSizes() const
	{return m_StripIndexSizes;}

	//! Return the vector of strip offset
	inline const OffsetVector& stripsOffset() const
	{return m_StripIndexOffset;}

	//! Return true if the group contains fans
	inline bool containsFan() const
	{return not m_FansIndex.isEmpty();}

	//! Return the list of index of fans
	inline const IndexList& fansIndex() const
	{return m_FansIndex;}

	//! Return the vector of fans sizes
	inline const IndexSizes& fansSizes() const
	{return m_FansIndexSizes;}

	//! Return the vector of strip offset
	inline const OffsetVector& fansOffset() const
	{return m_FanIndexOffset;}


//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Add triangles to the group
	inline void addTriangles(const IndexList& input)
	{m_TrianglesIndex+= input;}

	//! Set the triangle index offset
	inline void setTrianglesOffset(GLvoid* pOffset)
	{m_pBaseTrianglesOffset= pOffset;}

	//! Add triangle strip to the group
	void addTrianglesStrip(const IndexList&);

	//! Set base triangle strip offset
	void setBaseTrianglesStripOffset(GLvoid*);

	//! Add triangle fan to the group
	void addTrianglesFan(const IndexList&);

	//! Set base triangle fan offset
	void setBaseTrianglesFanOffset(GLvoid*);


//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Grouped material id
	GLC_uint m_ID;

	//! Triangles index list
	IndexList m_TrianglesIndex;

	//! The base triangle index offset
	GLvoid* m_pBaseTrianglesOffset;

	//! Strips index list
	IndexList m_StripsIndex;

	//! Strips index size
	IndexSizes m_StripIndexSizes;

	//! Vector of strips offset
	OffsetVector m_StripIndexOffset;

	//! Fans index list
	IndexList m_FansIndex;

	//! Fans index size
	IndexSizes m_FansIndexSizes;

	//! Vector of fan Offset
	OffsetVector m_FanIndexOffset;

};

#endif /* GLC_PRIMITIVEGROUP_H_ */
