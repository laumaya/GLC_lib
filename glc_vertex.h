/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.4, packaged on January, 2006.

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

//! \file glc_vertex.h interface for the GLC_Vertex class.

#ifndef GLC_VERTEX_H_
#define GLC_VERTEX_H_

#include <QtOpenGL>

#include "glc_maths.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Vertex
/*! \brief GLC_Vertex : OpenGL Vertex, Normal, Texture coordinate*/

/*! An GLC_Vertex polygonal face composed by vertexs (GLC_Vertex)
 */
//////////////////////////////////////////////////////////////////////
class GLC_Vertex
{

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an Vertex, with normal
	GLC_Vertex(const GLC_Vector4d &VectPos, const GLC_Vector4d &VectNormale);
	//! Create an Vertex, with normal and texture coordinate
	GLC_Vertex(const GLC_Vector4d &VectPos, const GLC_Vector4d &VectNormale, const GLC_Vector4d &VectTexture);
	//! Copy Constructor
	GLC_Vertex(const GLC_Vertex *pVertex);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Vertex SetUp
	void GlDraw(void) const;

//@}

//////////////////////////////////////////////////////////////////////
// Private Members
//////////////////////////////////////////////////////////////////////
private:

	//! Vertex 3D coordinate
	GLC_Vector4d m_VectPos;

	//! Vertex's 2D texture coordinate
	GLC_Vector4d m_VectTexture;

	//! Vertex's normal vector
	GLC_Vector4d m_VectNormale;

};
#endif //GLC_VERTEX_H_
