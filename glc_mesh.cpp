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

//! \file glc_mesh.cpp implementation of the GLC_Mesh class DEPRECATED SEE GLC_Mesh2.

#include <QtDebug>
#include "glc_mesh.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Mesh::GLC_Mesh(const char *pName, const GLfloat *pColor)
:GLC_Geometry(pName, pColor)
, m_nNbrFaces(0)
{

}

// Copy Constructor
GLC_Mesh::GLC_Mesh(GLC_Mesh *pMesh)
:GLC_Geometry(pMesh->GetName(), pMesh->GetfRGBA())
, m_nNbrFaces(0)

{
	GLC_Face *pFace;	
	for (int i = 0; i < pMesh->m_pFaceList.size(); ++i)
	{
		pFace= new GLC_Face(pMesh->m_pFaceList.at(i));
		AddFace(pFace);
	}
	
	SetMaterial(pMesh->GetMaterial());	
	SetMatrix(pMesh->GetMatrix());
}


GLC_Mesh::~GLC_Mesh(void)
{
	while (!m_pFaceList.isEmpty())
		delete m_pFaceList.takeFirst();
	m_nNbrFaces= 0;
	
}
/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Add a face to the Mesh
void GLC_Mesh::AddFace(GLC_Face* pFace)
{
	m_pFaceList.append(pFace);
	m_nNbrFaces++;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Virtual interface for OpenGL Geometry set up.
void GLC_Mesh::GlDraw()
{
    for (int i = 0; i < m_pFaceList.size(); ++i)
    {
    	m_pFaceList.at(i)->GlDraw();
    }
    
}

// Virtual interface for OpenGL Geomtry properties. (Color, thiknness, position..)
void GLC_Mesh::GlPropGeom()
{
		//! Change the current matrix
		glMultMatrixd(m_MatPos.Return_dMat());
		
		if (m_IsBlended)
		{
			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
		}
		else
		{
			glDisable(GL_BLEND);
		}
		
		
		if(!m_pMaterial || (m_PolyMode != GL_FILL))
		{
			qDebug() << "GLC_Mesh::GlPropGeom : Wireframe Mode";
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glColor4fv(GetfRGBA());			// is color
		}
		else if (m_pMaterial->GetAddRgbaTexture())
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			m_pMaterial->GlExecute();
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			m_pMaterial->GlExecute();
		}
		

		glLineWidth(GetThickness());	// is thikness

		// Polygons display mode
		glPolygonMode(m_PolyFace, m_PolyMode);
		
		// OpenGL error management
		if (glGetError() != GL_NO_ERROR)
		{
			qDebug("GLC_Cylinder::GlPropGeom ERROR OPENGL\n");
		}
		

}
