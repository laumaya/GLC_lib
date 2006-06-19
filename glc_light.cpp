/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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

//! \file glc_light.cpp implementation of the GLC_Light class.

#include <QtDebug>
#include "glc_light.h"
#include "glc_openglexception.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Light::GLC_Light(const char *pName ,const GLfloat *pAmbiantColor)
:GLC_Object(pName)
, m_LightID(GL_LIGHT1)	// Default Light ID
, m_ListID(0)				// By default display list ID= 0
, m_ListIsValid(false)	// By default display list is not valid
{
	//! \todo modify class to support multi light

	// Color Initialisation
	if (pAmbiantColor != 0)
	{
		m_AmbientColor[0]= pAmbiantColor[0];
		m_AmbientColor[1]= pAmbiantColor[1];
		m_AmbientColor[2]= pAmbiantColor[2];
		m_AmbientColor[3]= pAmbiantColor[3];
	}
	else
	{	// By default light's color is dark grey
		m_AmbientColor[0]= 0.2f;
		m_AmbientColor[1]= 0.2f;
		m_AmbientColor[2]= 0.2f;
		m_AmbientColor[3]= 1.0f;
	}
	// Other are white
	// Diffuse Color
	m_DiffuseColor[0]= 1.0f;
	m_DiffuseColor[1]= 1.0f;
	m_DiffuseColor[2]= 1.0f;
	m_DiffuseColor[3]= 1.0f;

	// Specular Color
	m_SpecularColor[0]= 1.0f;
	m_SpecularColor[1]= 1.0f;
	m_SpecularColor[2]= 1.0f;
	m_SpecularColor[3]= 1.0f;

	// Color position
	m_Position[0]= 0.0f;
	m_Position[1]= 0.0f;
	m_Position[2]= 0.0f;
	m_Position[3]= 1.0f;
}

GLC_Light::~GLC_Light(void)
{
	deleteList();
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set the lihgt position by a 4D vector
void GLC_Light::setPosition(const GLC_Vector4d &VectPos)
{
	m_Position[0]= static_cast<GLfloat>(VectPos.getX());
	m_Position[1]= static_cast<GLfloat>(VectPos.getY());
	m_Position[2]= static_cast<GLfloat>(VectPos.getZ());
	m_Position[3]= static_cast<GLfloat>(VectPos.getW());

	m_ListIsValid = false;
}

// Set the lihgt position by a 3 GLfloat
void GLC_Light::setPosition(GLfloat x, GLfloat y, GLfloat z)
{
	m_Position[0]= x;
	m_Position[1]= y;
	m_Position[2]= z;

	m_ListIsValid = false;
}

// Set light's ambiant color by an array of GLfloat
void GLC_Light::setAmbientColor(const GLfloat* pfCol)
{
	m_AmbientColor[0]= pfCol[0];
	m_AmbientColor[1]= pfCol[1];
	m_AmbientColor[2]= pfCol[2];
	m_AmbientColor[3]= pfCol[3];

	m_ListIsValid = false;
}

// Set light's diffuse color by an array of GLfloat
void GLC_Light::setDiffuseColor(const GLfloat* pfCol)
{
	m_DiffuseColor[0]= pfCol[0];
	m_DiffuseColor[1]= pfCol[1];
	m_DiffuseColor[2]= pfCol[2];
	m_DiffuseColor[3]= pfCol[3];

	m_ListIsValid = false;
}

// Set light's specular color by an array of GLfloat
void GLC_Light::setSpecularColor(const GLfloat* pfCol)
{
	m_SpecularColor[0]= pfCol[0];
	m_SpecularColor[1]= pfCol[1];
	m_SpecularColor[2]= pfCol[2];
	m_SpecularColor[3]= pfCol[3];

	m_ListIsValid = false;
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Create light's OpenGL list
void GLC_Light::creationList(GLenum Mode)
{
	if(!m_ListID)		// OpenGL list not created
	{
		m_ListID= glGenLists(1);

		if (!m_ListID)	// OpenGL List Id not get
		{
			glDraw();
			qDebug("GLC_Lumiere::CreationListe Display list not create");
		}
	}
	// OpenGL list creation and execution
	glNewList(m_ListID, Mode);
				
	// Light execution
	glDraw();

	glEndList();
	
	// Indicateur de la validité de la liste
	m_ListIsValid= true;
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Light::CreationList ", error);
		throw(OpenGlException);
	}
}

// Execute OpenGL light
void GLC_Light::glExecute(GLenum Mode)
{
	// Object Name
	glLoadName(getID());


	if (!m_ListIsValid)
	{
		// OpenGL list is not valid

		creationList(Mode);
	}
	else
	{
		glCallList(m_ListID);
	}

	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Light::GlExecute ", error);
		throw(OpenGlException);
	}

}

// OpenGL light set up
void GLC_Light::glDraw(void)
{
	// Color
	glLightfv(m_LightID, GL_AMBIENT, m_AmbientColor);		// Setup The Ambient Light
	glLightfv(m_LightID, GL_DIFFUSE, m_DiffuseColor);		// Setup The Diffuse Light
	glLightfv(m_LightID, GL_SPECULAR, m_SpecularColor);	// Setup The specular Light
	// Position
	glLightfv(m_LightID, GL_POSITION, m_Position);	// Position The Light
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Light::GlDraw ", error);
		throw(OpenGlException);
	}

}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

// Delete OpenGL Display list
void GLC_Light::deleteList(void)
{
	//! if the list is valid, the list is deleted
	if (glIsList(m_ListID))
	{
		glDeleteLists(m_ListID, 1);
	}
}
