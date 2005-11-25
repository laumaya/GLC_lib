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

//! \file glc_point.cpp implementation of the GLC_Point class.

#include "glc_point.h"


//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////


GLC_Point::GLC_Point(const GLC_Vector4d &VectSetCoord, const char *pName, const GLfloat *pColor)

:GLC_Geometrie(pName, pColor), m_VectCoord(VectSetCoord)
{
	
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

void GLC_Point::GlDraw(void)
{
	// Point Display
	glBegin(GL_POINTS);
		glVertex3dv(m_VectCoord.Return_dVect());
	glEnd();
}

void GLC_Point::GlPropGeom(void)
{
		// Change current Matrix
		glMultMatrixd(m_MatPos.Return_dMat());
		
		// Disable lighting
		glDisable(GL_LIGHTING);
		// Disable blending
		glDisable(GL_BLEND);

		glColor4fv(GetfRGBA());			// Sa Couleur
		glPointSize(GetEpaisseur());	// Son Epaisseur
}

