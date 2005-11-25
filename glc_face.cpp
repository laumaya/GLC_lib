/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9, packaged on Novemeber, 2005.

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

//! \file glc_face.cpp implementation of the GLC_Face class.

#include <QtDebug>
#include "glc_face.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Face::GLC_Face()
:m_NbrVertex(0)
{

}

GLC_Face::~GLC_Face(void)
{

	for (int i= m_NbrVertex - 1; i >= 0; i--)
	{
		delete m_Array_pVertex[i];
	}
}
/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Add a vertex to the face
void GLC_Face::AddVertex(GLC_Vertex* pVertex)
{
	m_Array_pVertex[m_NbrVertex]= pVertex;
	m_NbrVertex++;
}

// Add an array of vertex to the face
void GLC_Face::AddVertexs(GLC_Vertex** pVertex, int Nbr)
{
	//! \todo Check vertexs number
	for (int i= 0; i < Nbr; i++)
	{
		m_Array_pVertex[m_NbrVertex + i]= pVertex[i];
	}
	m_NbrVertex+= Nbr;	
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
// Face SetUp
void GLC_Face::GlDraw(void)
{
	glBegin(GL_POLYGON);
		for (int i= 0; i < m_NbrVertex; i++)
		{
			m_Array_pVertex[i]->GlDraw();
		}
	glEnd();

}
