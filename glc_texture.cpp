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

//! \file glc_texture.cpp implementation of the GLC_Texture class.

#include <QtDebug>
#include "glc_texture.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

GLC_Texture::GLC_Texture(QGLWidget *GLWidget, const QString &Filename)
: m_pQGLWidget(GLWidget)
, m_FileName(Filename)
, m_TextureID(0)
{
	QImage image(m_FileName);
	if (image.isNull())
	{
		qDebug() << "GLC_Texture::GLC_Texture open image : " << m_FileName << " Failed";		
	}	
	else
	{
		m_TextureID= m_pQGLWidget->bindTexture(image);
		qDebug() << "GLC_Texture::GLC_Texture Texture ID = " << m_TextureID;
	}
	//! \todo Add Error handler
}

GLC_Texture::GLC_Texture(const GLC_Texture &TextureToCopy)
: m_pQGLWidget(TextureToCopy.m_pQGLWidget)
, m_FileName(TextureToCopy.m_FileName)
, m_TextureID(0)

{
	QImage image(m_FileName);
	if (image.isNull())
	{
		qDebug() << "GLC_Texture::GLC_Texture open image : " << m_FileName << " Failed";		
	}	
	else
	{
		m_TextureID= m_pQGLWidget->bindTexture(image);
		qDebug() << "GLC_Texture::GLC_Texture Texture ID = " << m_TextureID;
	}
	
}

GLC_Texture::~GLC_Texture()
{
	qDebug() << "GLC_Texture::~GLC_Texture Texture ID : " << m_TextureID;
	m_pQGLWidget->deleteTexture(m_TextureID);
}

