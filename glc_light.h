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

//! \file glc_light.h interface for the GLC_Light class.

#ifndef GLC_LIGHT_H_
#define GLC_LIGHT_H_

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
	//! Construct an GLC_Light
	/*! By default, Ambiant Color is dark grey*/
	GLC_Light(const char *pName, const GLfloat *pAmbiantColor = NULL);
	
	//! Delete OpenGL list
	virtual ~GLC_Light(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Get a 4D Vector representing light position
	GLC_Vector4d GetPosition(void) const
	{
		GLC_Vector4d ReturnVect(m_Position[0], m_Position[1], m_Position[2], m_Position[3]);
		return ReturnVect;
	}

	//! Get an array of GLfloat for the light's ambiant color
	const GLfloat *GetAmbientColor()
	{
		return m_AmbientColor;
	}

	//! Get an array of GLfloat for the light's Diffuse color
	const GLfloat *GetDiffuseColor()
	{
		return m_DiffuseColor;
	}
	//! Get an array of GLfloat for the light's Specular color
	const GLfloat *GetSpecularColor()
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
	void SetPosition(const GLC_Vector4d &VectPos);

	//! Set lihgt's position by a 3 GLfloat
	void SetPosition(GLfloat x, GLfloat y, GLfloat z);
	
	//! Set light's ambiant color by an array of GLfloat
	void SetAmbientColor(const GLfloat* pfCol);

	//! Set light's diffuse color by an array of GLfloat
	void SetDiffuseColor(const GLfloat* pfCol);

	//! Set light's specular color by an array of GLfloat
	void SetSpecularColor(const GLfloat* pfCol);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Enable the light
	void GlEnable(void)
	{
		glEnable(m_LightID);
	}
	
	// Disable the light
	void GlDisable(void)
	{
		glDisable(m_LightID);
	}

	//! Execute OpenGL light
	virtual bool GlExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////

private:
	//! OpenGL light set up 
	void GlDraw(void);
	
	//! Display List creation
	bool CreationList(GLenum Mode);

private:

	//! Delete OpenGL Display list
	void DeleteList(void)
	{
		//! if the list is valid, the list is deleted
		if (glIsList(m_ListID))
		{
			glDeleteLists(m_ListID, 1);
		}
		//! \todo Add error handler
	}
	
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
	GLfloat m_AmbientColor[4];
	//! Light diffuse color
	GLfloat m_DiffuseColor[4];
	//! Light specular color
	GLfloat m_SpecularColor[4];
	
	//! Light position
	GLfloat m_Position[4];

};
#endif //GLC_LIGHT_H_
