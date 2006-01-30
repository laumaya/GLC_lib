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

//! \file glc_box.cpp implementation of the GLC_Box class.

#include "glc_box.h"


//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////


GLC_Box::GLC_Box(double dLx, double dLy, double dlz
					   , const char *pName, const GLfloat *pColor)

:GLC_Geometry(pName, pColor)
, m_dLgX(dLx)
, m_dLgY(dLy)
, m_dLgZ(dlz)
{

}

//////////////////////////////////////////////////////////////////////
// Private OpenGL functions
//////////////////////////////////////////////////////////////////////

// Box Set Up
void GLC_Box::GlDraw(void)
{
	// Polygon 1	
	glBegin(GL_QUADS);		
		glNormal3d(0.0, 0.0, 1.0);	// Z
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0,		0.0,	m_dLgZ);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,	0.0,	m_dLgZ);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	m_dLgY, m_dLgZ);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0,		m_dLgY, m_dLgZ);
	// Polygon 2
		glNormal3d(1.0, 0.0, 0.0);	// X
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_dLgX,	0.0,	m_dLgZ);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_dLgX,	0.0,	0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,	m_dLgY, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	m_dLgY, m_dLgZ);
	// Polygon 3	
		glNormal3d(0.0, 0.0, -1.0);	// -Z
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0,		0.0,	0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0,		m_dLgY,	0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	m_dLgY,	0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,	0.0,	0.0);
	// Polygon 4
		glNormal3d(-1.0, 0.0, 0.0);	// -X	
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0,		0.0,	0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0,		0.0,	m_dLgZ);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(0.0,		m_dLgY,	m_dLgZ);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(0.0,		m_dLgY,	0.0);
	// Polygon 5
		glNormal3d(0.0, -1.0, 0.0);	// -Y
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0,		0.0,	0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,	0.0,	0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	0.0,	m_dLgZ);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0,		0.0,	m_dLgZ);
	// Polygon 6
		glNormal3d(0.0, 1.0, 0.0);	// Y
		glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0,		m_dLgY,	0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0,		m_dLgY,	m_dLgZ);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_dLgX,	m_dLgY,	m_dLgZ);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_dLgX,		m_dLgY,	0.0);
	glEnd();

}
// Virtual interface for OpenGL Geomtry properties. (Color, thiknness, position..)
void GLC_Box::GlPropGeom(void)
{
		// Update Current matrix
		glMultMatrixd(m_MatPos.Return_dMat());
				
		if(!m_pMaterial || (m_PolyMode != GL_FILL))
		{
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);

			glColor4fv(GetfRGBA());			// Sa Couleur
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

		glLineWidth(GetThickness());	// Son Epaisseur

		// Polygons display mode
		glPolygonMode(m_PolyFace, m_PolyMode);
		
		// OpenGL error management
		if (glGetError() != GL_NO_ERROR)
		{
			qDebug("GLC_Cylinder::GlPropGeom ERROR OPENGL\n");
		}
		
}
