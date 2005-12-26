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

//! \file glc_light.cpp implementation of the GLC_Light class.

#include <QtDebug>
#include "glc_light.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Light::GLC_Light(const char *pName ,const GLfloat *pAmbiantColor)
:GLC_Object(pName)
, m_LumiereID(GL_LIGHT1)	// Default Light ID
, m_ListeID(0)				// By default display list ID= 0
, m_bListeIsValid(false)	// By default display list is not valid
{
	//! \todo modify class to support multi light

	// Color Initialisation
	if (pAmbiantColor != 0)
	{
		m_fColAmbiente[0]= pAmbiantColor[0];
		m_fColAmbiente[1]= pAmbiantColor[1];
		m_fColAmbiente[2]= pAmbiantColor[2];
		m_fColAmbiente[3]= pAmbiantColor[3];
	}
	else
	{	// By default light's color is dark grey
		m_fColAmbiente[0]= 0.2f;
		m_fColAmbiente[1]= 0.2f;
		m_fColAmbiente[2]= 0.2f;
		m_fColAmbiente[3]= 1.0f;
	}
	// Other are white
	// Diffuse Color
	m_fColDiffuse[0]= 1.0f;
	m_fColDiffuse[1]= 1.0f;
	m_fColDiffuse[2]= 1.0f;
	m_fColDiffuse[3]= 1.0f;

	// Specular Color
	m_fColSpeculaire[0]= 1.0f;
	m_fColSpeculaire[1]= 1.0f;
	m_fColSpeculaire[2]= 1.0f;
	m_fColSpeculaire[3]= 1.0f;

	// Color position
	m_fPos[0]= 0.0f;
	m_fPos[1]= 0.0f;
	m_fPos[2]= 0.0f;
	m_fPos[3]= 1.0f;
}

GLC_Light::~GLC_Light(void)
{
	DeleteListe();
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set the lihgt position by a 4D vector
void GLC_Light::SetPos(const GLC_Vector4d &VectPos)
{
	m_fPos[0]= static_cast<GLfloat>(VectPos.GetX());
	m_fPos[1]= static_cast<GLfloat>(VectPos.GetY());
	m_fPos[2]= static_cast<GLfloat>(VectPos.GetZ());
	m_fPos[3]= static_cast<GLfloat>(VectPos.GetW());

	m_bListeIsValid = false;
}

// Set the lihgt position by a 3 GLfloat
void GLC_Light::SetPos(GLfloat x, GLfloat y, GLfloat z)
{
	m_fPos[0]= x;
	m_fPos[1]= y;
	m_fPos[2]= z;

	m_bListeIsValid = false;
}

// Set light's ambiant color by an array of GLfloat
void GLC_Light::SetColAmbiante(const GLfloat* pfCol)
{
	m_fColAmbiente[0]= pfCol[0];
	m_fColAmbiente[1]= pfCol[1];
	m_fColAmbiente[2]= pfCol[2];
	m_fColAmbiente[3]= pfCol[3];

	m_bListeIsValid = false;
}

// Set light's diffuse color by an array of GLfloat
void GLC_Light::SetColDiffuse(const GLfloat* pfCol)
{
	m_fColDiffuse[0]= pfCol[0];
	m_fColDiffuse[1]= pfCol[1];
	m_fColDiffuse[2]= pfCol[2];
	m_fColDiffuse[3]= pfCol[3];

	m_bListeIsValid = false;
}

// Set light's specular color by an array of GLfloat
void GLC_Light::SetColSpeculaire(const GLfloat* pfCol)
{
	m_fColSpeculaire[0]= pfCol[0];
	m_fColSpeculaire[1]= pfCol[1];
	m_fColSpeculaire[2]= pfCol[2];
	m_fColSpeculaire[3]= pfCol[3];

	m_bListeIsValid = false;
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Create light's OpenGL list
bool GLC_Light::CreationListe(GLenum Mode)
{
	if(!m_ListeID)		// OpenGL list not created
	{
		m_ListeID= glGenLists(1);

		if (!m_ListeID)	// OpenGL List Id not get
		{
			GlDraw();
			qDebug("GLC_Lumiere::CreationListe Display list not create");
			return false;	// Light execution without OpenGL list creation
		}
	}
	// OpenGL list creation and execution
	glNewList(m_ListeID, Mode);
				
	// Light execution
	GlDraw();

	glEndList();
	
	// Indicateur de la validité de la liste
	m_bListeIsValid= true;
	
	//! \todo add error handler
	return true;	// OpenGL list created
}

// Execute OpenGL light
bool GLC_Light::GlExecute(GLenum Mode)
{
	// Object Name
	glLoadName(GetID());


	if (!m_bListeIsValid)
	{
		// OpenGL list is not valid

		CreationListe(Mode);
	}
	else
	{
		glCallList(m_ListeID);
	}

	//! \todo Add error handler here
	return true;
}

// OpenGL light set up
void GLC_Light::GlDraw(void)
{
	// Color
	glLightfv(m_LumiereID, GL_AMBIENT, m_fColAmbiente);		// Setup The Ambient Light
	glLightfv(m_LumiereID, GL_DIFFUSE, m_fColDiffuse);		// Setup The Diffuse Light
	glLightfv(m_LumiereID, GL_SPECULAR, m_fColSpeculaire);	// Setup The specular Light
	// Position
	glLightfv(m_LumiereID, GL_POSITION, m_fPos);	// Position The Light
}
