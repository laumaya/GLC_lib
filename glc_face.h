/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.5, packaged on February, 2006.

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

//! \file glc_face.h interface for the GLC_Face class DEPRECATED SEE GLC_mesh2.

#ifndef GLC_FACE_H_
#define GLC_FACE_H_

#include "glc_vertex.h"
//! \todo Replace static array by a dynamic one
#define GLC_FACEVERTEXNBR 20

//////////////////////////////////////////////////////////////////////
//! \class GLC_Face
/*! \brief GLC_Face : OpenGL Face DEPRECATED SEE GLC_mesh2*/

/*! An GLC_Face polygonal face composed by vertexs (GLC_Vertex)
 */
//////////////////////////////////////////////////////////////////////
class GLC_Face
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Construct an empty Face
	GLC_Face();
	//! Copy constructor
	GLC_Face(const GLC_Face *pFaceToCopy);
	//! Delete Face's Vertexs (GLC_Vertex)
	~GLC_Face(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! return the number of Face's vertexs
	int GetVertexNumber(void) const
	{
		return m_NbrVertex;
	}

//@}	

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Add a vertex to the face
	void AddVertex(GLC_Vertex* pVertex);

	//! Add an array of vertex to the face
	void AddVertexs(GLC_Vertex** pVertex, int Nbr);
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Face SetUp
	void GlDraw(void);

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Array of Vertex pointer
	GLC_Vertex* m_Array_pVertex[GLC_FACEVERTEXNBR];

	//! Number of Face's Vertex
	int m_NbrVertex;

};
#endif //GLC_FACE_H_
