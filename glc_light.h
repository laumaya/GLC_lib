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

//! \file glc_light.h interface for the GLC_Light class.

#ifndef GLC_LIGHT_H_
#define GLC_LIGHT_H_

#include <QColor>
#include "glc_object.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Light
/*! \brief GLC_Light : OpenGL Point Light*/

/*! An GLC_Light is an OpenGL point Light source at a 3D location\n
 * Point light is omnidirectional and have color*/
//////////////////////////////////////////////////////////////////////

class GLC_Light :
	public GLC_Object
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Construct a default GLC_Light
	/*! By default, ambient color is black, diffuse Color is white and specular color is white*/
	GLC_Light();
	//* Construct GLC_Light with specified diffuse color
	GLC_Light(const QColor& );
	
	//! Delete OpenGL list
	virtual ~GLC_Light(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Get a 4D Vector representing light position
	GLC_Vector4d getPosition(void) const {return m_Position;}

	//! Get a QColor of the light's ambiant color
	const QColor getAmbientColor()
	{
		return m_AmbientColor;
	}

	//! Get  a QColor of the light's Diffuse color
	const QColor getDiffuseColor()
	{
		return m_DiffuseColor;
	}
	//! Get  a QColor of the light's Specular color
	const QColor getSpecularColor()
	{
		return m_SpecularColor;
	}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set lihgt's position by a 4D vector
	void setPosition(const GLC_Vector4d &VectPos);

	//! Set lihgt's position by a 3 GLfloat
	void setPosition(GLfloat x, GLfloat y, GLfloat z);
	
	//! Set light's ambiant color by a QColor
	void setAmbientColor(const QColor &);

	//! Set light's diffuse color by a QColor
	void setDiffuseColor(const QColor &);

	//! Set light's specular color by a QColor
	void setSpecularColor(const QColor &);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Enable the light
	void enable(void)
	{
		glEnable(m_LightID);
	}
	
	// Disable the light
	void disable(void)
	{
		glDisable(m_LightID);
	}

	//! Execute OpenGL light
	virtual void glExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////

private:
	//! OpenGL light set up 
	void glDraw(void);
	
	//! Display List creation
	void creationList(GLenum Mode);

private:

	//! Delete OpenGL Display list
	void deleteList(void);
	
//@}

//////////////////////////////////////////////////////////////////////
// Private Members
//////////////////////////////////////////////////////////////////////

private:
	//! OpenGL light ID
	GLenum m_LightID;

	//! OpenGL Display list ID
	GLuint m_ListID;

	//! OpenGL list validity
	bool m_ListIsValid;

	//! Light ambiant color
	QColor m_AmbientColor;
	//! Light diffuse color
	QColor m_DiffuseColor;
	//! Light specular color
	QColor m_SpecularColor;
	
	//! Light position
	GLC_Vector4d m_Position;

};
#endif //GLC_LIGHT_H_
