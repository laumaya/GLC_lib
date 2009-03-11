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

typedef QList<GLuint> IndexList;
typedef QList<GLsizei> IndexSizes;

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

	//! Return true if the group contains triangles
	inline bool containsTriangles() const
	{return not m_TrianglesIndex.isEmpty();}

	//! Return the list of triangles index of the group
	inline const IndexList& trianglesIndex() const
	{ return m_TrianglesIndex;}

	//! Return true if the group contains strips
	inline bool containsStrip() const
	{return not m_StripsIndex.isEmpty();}

	//! Return the list of index of strips
	inline const IndexList& stripsIndex() const
	{return m_StripsIndex;}

	//! Return the list of strips sizes
	inline const IndexSizes& stripsSizes() const
	{return m_StripIndexSizes;}

	//! Return true if the group contains fans
	inline bool containsFan() const
	{return not m_FansIndex.isEmpty();}

	//! Return the list of index of fans
	inline const IndexList& fansIndex() const
	{return m_FansIndex;}

	//! Return the list of fans sizes
	inline const IndexSizes& fansSizes() const
	{return m_FansIndexSizes;}


//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Add triangles to the group
	inline void addTriangles(const IndexList& input)
	{m_TrianglesIndex+= input;}

	//! Add triangle strip to the group
	inline void addTrianglesStrip(const IndexList& input)
	{
		m_StripsIndex+= input;
		m_StripIndexSizes+= static_cast<GLsizei>(input.size());
	}

	//! Add triangle fan to the group
	inline void addTrianglesFan(const IndexList& input)
	{
		m_FansIndex+= input;
		m_FansIndexSizes+= static_cast<GLsizei>(input.size());
	}

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Grouped material id
	GLC_uint m_ID;

	//! Triangles index list
	IndexList m_TrianglesIndex;

	//! Strips index list
	IndexList m_StripsIndex;

	//! Strips index size
	IndexSizes m_StripIndexSizes;

	//! Fans index list
	IndexList m_FansIndex;

	//! Fans index size
	IndexSizes m_FansIndexSizes;

};

#endif /* GLC_PRIMITIVEGROUP_H_ */
