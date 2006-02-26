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

//! \file glc_vertex.cpp implementation of the GLC_Vertex class DEPRECATED SEE GLC_Mesh2.

#include <QtDebug>
#include "glc_vertex.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
// Create an Vertex, with normal
GLC_Vertex::GLC_Vertex(const GLC_Vector4d &VectPos, const GLC_Vector4d &VectNormale)
: m_VectPos(VectPos)
, m_VectNormale(VectNormale)

{

}

// Create an Vertex, with normal and texture coordinate
GLC_Vertex::GLC_Vertex(const GLC_Vector4d &VectPos, const GLC_Vector4d &VectNormale
					   , const GLC_Vector4d &VectTexture)
: m_VectPos(VectPos)
, m_VectTexture(VectTexture)
, m_VectNormale(VectNormale)

{

}
// Copy Constructor
GLC_Vertex::GLC_Vertex(const GLC_Vertex *pVertex)
: m_VectPos(pVertex->m_VectPos)
, m_VectTexture(pVertex->m_VectTexture)
, m_VectNormale(pVertex->m_VectNormale)

{
	
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Vertex SetUp
void GLC_Vertex::GlDraw(void) const
{
	glTexCoord2dv(m_VectTexture.Return_dVect());
	glNormal3dv(m_VectNormale.Return_dVect());
	glVertex3dv(m_VectPos.Return_dVect());
}
