/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.7, packaged on September, 2007.

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

#include "glc_selectionmaterial.h"


GLC_SelectionMaterial::GLC_SelectionMaterial()
{
}

// Execute OpenGL Material
void GLC_SelectionMaterial::glExecute()
{
	static GLfloat pAmbientColor[4]= {1.0f, 0.376f, 0.223f, 1.0f};
								
	static GLfloat pDiffuseColor[4]= {1.0f, 0.376f, 0.223f, 1.0f};
	
	static GLfloat pSpecularColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};

	static GLfloat pLightEmission[4]= {0.0f, 0.0f, 0.0f, 1.0f};
	
	static float shininess= 50.0f;							
	
	glColor4fv(pAmbientColor);
								
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pAmbientColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pDiffuseColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pSpecularColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, pLightEmission);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);
	
}
