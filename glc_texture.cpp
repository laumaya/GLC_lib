/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.8, packaged on January, 2008.

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
#include "glc_exception.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

GLC_Texture::GLC_Texture(const QGLContext *pContext, const QString &Filename)
: m_pQGLContext(const_cast<QGLContext*>(pContext))
, m_File(Filename)
, m_TextureID(0)
, m_pTextureImage(new QImage(m_File.fileName()))
{
	if (m_pTextureImage->isNull())
	{
		QString ErrorMess("GLC_Texture::GLC_Texture open image : ");
		ErrorMess.append(m_File.fileName()).append(" Failed");
		qDebug() << ErrorMess;
		GLC_Exception e(ErrorMess);
		throw(e);		
	}	
}

GLC_Texture::GLC_Texture(const QGLContext *pContext, const QFile &file)
: m_pQGLContext(const_cast<QGLContext*>(pContext))
, m_File(file.fileName())
, m_TextureID(0)
, m_pTextureImage(new QImage(file.fileName()))
{
	if (m_pTextureImage->isNull())
	{
		QString ErrorMess("GLC_Texture::GLC_Texture open image : ");
		ErrorMess.append(m_File.fileName()).append(" Failed");
		qDebug() << ErrorMess;
		GLC_Exception e(ErrorMess);
		throw(e);		
	}	
}

GLC_Texture::GLC_Texture(const GLC_Texture &TextureToCopy)
: m_pQGLContext(TextureToCopy.m_pQGLContext)
, m_File(TextureToCopy.m_File.fileName())
, m_TextureID(0)
, m_pTextureImage(new QImage(TextureToCopy.m_File.fileName()))

{
	if (m_pTextureImage->isNull())
	{
		QString ErrorMess("GLC_Texture::GLC_Texture open image : ");
		ErrorMess.append(m_File.fileName()).append(" Failed");
		qDebug() << ErrorMess;
		GLC_Exception e(ErrorMess);
		throw(e);		
	}	
	
}

GLC_Texture::~GLC_Texture()
{
	//qDebug() << "GLC_Texture::~GLC_Texture Texture ID : " << m_TextureID;
	if (m_TextureID != 0)
	{
		m_pQGLContext->deleteTexture(m_TextureID);
		m_TextureID= 0;
	}
	else
	{
		delete m_pTextureImage;
		m_pTextureImage = NULL;
	}
}
// Load the texture
void GLC_Texture::glLoadTexture(void)
{
	if (m_TextureID == 0)
	{
		m_TextureID= m_pQGLContext->bindTexture(*m_pTextureImage);
		delete m_pTextureImage;
		m_pTextureImage= NULL;
		//qDebug() << "GLC_Texture::glcBindTexture Texture ID = " << m_TextureID;
	}
}

// Bind texture in 2D mode
void GLC_Texture::glcBindTexture(void)
{
	if (m_TextureID != 0)
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}
	else
	{
		GLC_Exception e("GLC_Texture::glcBindTexture Texture not loaded");
		throw(e);
	}
	
}

