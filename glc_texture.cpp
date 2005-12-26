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

//! \file glc_texture.cpp implementation of the GLC_Texture class.

#include <QtDebug>
#include "glc_texture.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

GLC_Texture::GLC_Texture(QGLWidget *GLWidget, const QString Filename)
: m_nTextureID(0)
{
	QImage image(Filename);
	if (image.isNull())
	{
		qDebug() << "GLC_Texture::GLC_Texture open image : " << Filename << " Failed";		
	}	
	else
	{
		m_nTextureID= GLWidget->bindTexture(image);
		qDebug() << "GLC_Texture::GLC_Texture Texture ID = " << m_nTextureID;
	}
	//! \todo Add Error handler
}

GLC_Texture::~GLC_Texture()
{
	glDeleteTextures(1, &m_nTextureID);
}

