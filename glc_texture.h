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

//! \file glc_texture.h interface for the GLC_Texture class.

#ifndef GLC_TEXTURE_H_
#define GLC_TEXTURE_H_

#include <QString>
#include <QtOpenGL>

/////////////////////////////////////////////////////////////////////
//! \class GLC_Texture
/*! \brief GLC_Texture : Image texture */

/*! Image texture define a texture map in 2 D coordinate system*/
//////////////////////////////////////////////////////////////////////


class GLC_Texture  
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
	//! Default constructor
	GLC_Texture(QGLWidget *GLWidget, const QString Filename);

	//! Default Destructor
	virtual ~GLC_Texture();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:	

	//! Bind texture in 2D mode
	void GlBindTexture(void)
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}
//@}
	
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////

private:
	//! OpenGL Texture ID
	GLuint	m_TextureID;
};

#endif //GLC_TEXTURE_H_
