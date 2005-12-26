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
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
public:
	//! Construct an GLC_Light
	/*! By default, Ambiant Color is dark grey*/
	GLC_Light(const char *pName, const GLfloat *pAmbiantColor = NULL);
	
	//! Delete OpenGL list
	virtual ~GLC_Light(void);

//////////////////////////////////////////////////////////////////////
// Fonctions Get
//////////////////////////////////////////////////////////////////////
public:
	//! Get a 4D Vector representing light position
	GLC_Vector4d GetPos(void) const
	{
		GLC_Vector4d ReturnVect(m_fPos[0], m_fPos[1], m_fPos[2], m_fPos[3]);
		return ReturnVect;
	}

	//! Get an array of GLfloat for the light's ambiant color
	const GLfloat *GetColAmbiante()
	{
		return m_fColAmbiente;
	}

	//! Get an array of GLfloat for the light's Diffuse color
	const GLfloat *GetColDiffuse()
	{
		return m_fColDiffuse;
	}
	//! Get an array of GLfloat for the light's Specular color
	const GLfloat *GetSpeculaire()
	{
		return m_fColSpeculaire;
	}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Set lihgt's position by a 4D vector
	void SetPos(const GLC_Vector4d &VectPos);

	//! Set lihgt's position by a 3 GLfloat
	void SetPos(GLfloat x, GLfloat y, GLfloat z);
	
	//! Set light's ambiant color by an array of GLfloat
	void SetColAmbiante(const GLfloat* pfCol);

	//! Set light's diffuse color by an array of GLfloat
	void SetColDiffuse(const GLfloat* pfCol);

	//! Set light's specular color by an array of GLfloat
	void SetColSpeculaire(const GLfloat* pfCol);

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Enable the light
	void GlEnable(void)
	{
		glEnable(m_LumiereID);
	}
	
	// Disable the light
	void GlDisable(void)
	{
		glDisable(m_LumiereID);
	}

	//! Execute OpenGL light
	virtual bool GlExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);

private:
	//! OpenGL light set up 
	void GlDraw(void);
	
	//! Display List creation
	bool CreationListe(GLenum Mode);


//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////
private:

	//! Delete OpenGL Display list
	void DeleteListe(void)
	{
		//! if the list is valid, the list is deleted
		if (glIsList(m_ListeID))
		{
			glDeleteLists(m_ListeID, 1);
		}
		//! \todo Add error handler
	}

//////////////////////////////////////////////////////////////////////
// Private Members
//////////////////////////////////////////////////////////////////////

private:
	//! OpenGL light ID
	GLenum m_LumiereID;

	//! OpenGL Display list ID
	GLuint m_ListeID;

	//! OpenGL list validity
	bool m_bListeIsValid;

	//! Light ambiant color
	GLfloat m_fColAmbiente[4];
	//! Light diffuse color
	GLfloat m_fColDiffuse[4];
	//! Light specular color
	GLfloat m_fColSpeculaire[4];
	
	//! Light position
	GLfloat m_fPos[4];

};
#endif //GLC_LIGHT_H_
