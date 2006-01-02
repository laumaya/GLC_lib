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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_mesh.cpp implementation of the GLC_Mesh class.

#include <QtDebug>
#include "glc_mesh.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Mesh::GLC_Mesh(const char *pName, const GLfloat *pColor)
:GLC_Geometry(pName, pColor)
, m_nNbrFaces(0)
, m_PolyFace(GL_FRONT_AND_BACK)	// Default Faces style
, m_PolyMode(GL_FILL)			// Default polyganal mode
{

}

// Copy Constructor
GLC_Mesh::GLC_Mesh(GLC_Mesh *pMesh)
:GLC_Geometry(pMesh->GetName(), pMesh->GetfRGBA())
, m_nNbrFaces(0)
, m_PolyFace(pMesh->m_PolyFace)	// Default Faces style
, m_PolyMode(pMesh->m_PolyMode)	// Default polyganal mode

{
	GLC_Face *pFace;	
	for (int i = 0; i < pMesh->m_Liste_pFace.size(); ++i)
	{
		pFace= new GLC_Face(pMesh->m_Liste_pFace.at(i));
		AddFace(pFace);
	}
	
	SetMaterial(pMesh->GetMaterial());	
	SetMatrix(pMesh->GetMatrix());
}


GLC_Mesh::~GLC_Mesh(void)
{
	while (!m_Liste_pFace.isEmpty())
		delete m_Liste_pFace.takeFirst();
	m_nNbrFaces= 0;
	
}
/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Add a face to the Mesh
void GLC_Mesh::AddFace(GLC_Face* pFace)
{
	m_Liste_pFace.append(pFace);
	m_nNbrFaces++;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Virtual interface for OpenGL Geometry set up.
void GLC_Mesh::GlDraw()
{
    for (int i = 0; i < m_Liste_pFace.size(); ++i)
    {
    	m_Liste_pFace.at(i)->GlDraw();
    }
    
}

// Virtual interface for OpenGL Geomtry properties.
void GLC_Mesh::GlPropGeom()
{
		//! Change the current matrix
		glMultMatrixd(m_MatPos.Return_dMat());
		if(!m_pMaterial || (m_PolyMode != GL_FILL))
		{
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);

			if (m_IsBlended)
			{
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			glColor4fv(GetfRGBA());			// is color
		}
		else if (m_pMaterial->GetAddRgbaTexture())
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);

			if (m_IsBlended)
			{
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			m_pMaterial->GlExecute();
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);

			if (m_IsBlended)
			{
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			m_pMaterial->GlExecute();
		}

		glLineWidth(GetThickness());	// is thikness

		// Polygons display mode
		glPolygonMode(m_PolyFace, m_PolyMode);

}
