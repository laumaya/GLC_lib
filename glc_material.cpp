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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

//! \file glc_material.cpp implementation of the GLC_Material class.

#include <QtDebug>

#include "glc_material.h"
#include "glc_collection.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Material::GLC_Material(const char *pName ,const GLfloat *pAmbiantColor)
:GLC_Object(pName)
, m_fShininess(50.0)		// Par défaut, shininess 50
, m_pTexture(NULL)			// Pas de texture
{

	// Initialisation de tableau contenant la couleur
	if (pAmbiantColor != 0)
	{
		m_fColAmbiente[0]= pAmbiantColor[0];
		m_fColAmbiente[1]= pAmbiantColor[1];
		m_fColAmbiente[2]= pAmbiantColor[2];
		m_fColAmbiente[3]= pAmbiantColor[3];
	}
	else
	{	// By default light's color is dark grey.
		m_fColAmbiente[0]= 0.2f;
		m_fColAmbiente[1]= 0.2f;
		m_fColAmbiente[2]= 0.2f;
		m_fColAmbiente[3]= 1.0f;
	}
	// Les autres 
	// Couleur diffuse
	m_fColDiffuse[0]= 1.0f;
	m_fColDiffuse[1]= 1.0f;
	m_fColDiffuse[2]= 1.0f;
	m_fColDiffuse[3]= 1.0f;

	// Couleur spéculaire
	m_fColSpeculaire[0]= 1.0f;
	m_fColSpeculaire[1]= 1.0f;
	m_fColSpeculaire[2]= 1.0f;
	m_fColSpeculaire[3]= 1.0f;

	// Emission de lumière
	m_fColEmission[0]= 0.0f;
	m_fColEmission[1]= 0.0f;
	m_fColEmission[2]= 0.0f;
	m_fColEmission[3]= 1.0f;

}
GLC_Material::GLC_Material(GLC_Texture* pTexture, const char *pName)
:GLC_Object(pName)
, m_fShininess(50.0)		// By default shininess 50
, m_pTexture(pTexture)			// Init texture
{
	// Ambiente Color
	m_fColAmbiente[0]= 0.2f;
	m_fColAmbiente[1]= 0.2f;
	m_fColAmbiente[2]= 0.2f;
	m_fColAmbiente[3]= 1.0f;

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

	// Lighting emit
	m_fColEmission[0]= 0.0f;
	m_fColEmission[1]= 0.0f;
	m_fColEmission[2]= 0.0f;
	m_fColEmission[3]= 1.0f;

}

GLC_Material::~GLC_Material(void)
{

	// Remove material from Geometries
	CGeomMap::iterator iEntry= m_WhereUsed.begin();
	
    while (iEntry != m_WhereUsed.constEnd())
    {
        // Remove material from geometry
        iEntry.value()->DelMatiere(iEntry.value());
        ++iEntry;
    }
    // clear whereUSED Hash table
    m_WhereUsed.clear();

}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get Ambiant color
void GLC_Material::GetColAmbiante(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= m_fColAmbiente[0];
	pfCol[1]= m_fColAmbiente[1];
	pfCol[2]= m_fColAmbiente[2];
	pfCol[3]= m_fColAmbiente[3];
}

// Get diffuse color
void GLC_Material::GetColDiffuse(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= m_fColDiffuse[0];
	pfCol[1]= m_fColDiffuse[1];
	pfCol[2]= m_fColDiffuse[2];
	pfCol[3]= m_fColDiffuse[3];
}

// Get specular color
void GLC_Material::GetColSpeculaire(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= m_fColSpeculaire[0];
	pfCol[1]= m_fColSpeculaire[1];
	pfCol[2]= m_fColSpeculaire[2];
	pfCol[3]= m_fColSpeculaire[3];
}

// Get the emissive color
void GLC_Material::GetLightEmission(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= m_fColEmission[0];
	pfCol[1]= m_fColEmission[1];
	pfCol[2]= m_fColEmission[2];
	pfCol[3]= m_fColEmission[3];
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Ambiant Color
void GLC_Material::SetColAmbiante(const GLfloat* pfCol)
{
	m_fColAmbiente[0]= pfCol[0];
	m_fColAmbiente[1]= pfCol[1];
	m_fColAmbiente[2]= pfCol[2];
	m_fColAmbiente[3]= pfCol[3];

	UpdateUsed();
}

// Set Diffuse color
void GLC_Material::SetColDiffuse(const GLfloat* pfCol)
{
	m_fColDiffuse[0]= pfCol[0];
	m_fColDiffuse[1]= pfCol[1];
	m_fColDiffuse[2]= pfCol[2];
	m_fColDiffuse[3]= pfCol[3];

	UpdateUsed();
}

// Set Specular color
void GLC_Material::SetColSpeculaire(const GLfloat* pfCol)
{
	m_fColSpeculaire[0]= pfCol[0];
	m_fColSpeculaire[1]= pfCol[1];
	m_fColSpeculaire[2]= pfCol[2];
	m_fColSpeculaire[3]= pfCol[3];

	UpdateUsed();
}

// Set Emissive
void GLC_Material::SetLightEmission(const GLfloat* pfCol)
{
	m_fColEmission[0]= pfCol[0];
	m_fColEmission[1]= pfCol[1];
	m_fColEmission[2]= pfCol[2];
	m_fColEmission[3]= pfCol[3];

	UpdateUsed();
}

// Add Geometry to where used hash table
bool GLC_Material::AddGLC_Geom(GLC_Geometrie* pGeom)
{
	CWhereUsed::iterator iGeom= m_WhereUsed.find(pGeom->GetID());
	
	if (iGeom == m_WhereUsed.end())
	{	// Ok, ID doesn't exist		
		// Add Geometry to where used hash table
		m_WhereUsed.insert(pGeom->GetID(), pGeom);		
		return true;
	}
	else
	{	// KO, ID exist
		qDebug("GLC_Material::AddGLC_Geom : Geometry not added");
		return false;
	}
	
}
// Supprime une géométrie de la collection
bool GLC_Material::DelGLC_Geom(GLC_uint Key)
{
	CWhereUsed::iterator iGeom= m_WhereUsed.find(Key);
	
	if (iGeom != m_WhereUsed.end())
	{	// Ok, ID exist		
		m_WhereUsed.remove(Key);	// Remove container
		
		return true;
	}
	else
	{	// KO doesn't exist
		qDebug("GLC_Material::DelGLC_Geom : Geometry not remove");
		return false;
	}
	
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Execute OpenGL Material
bool GLC_Material::GlExecute(GLenum Mode)
{
	if (m_pTexture != NULL)
	{
		// for blend managing
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		m_pTexture->GlBindTexture();
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_fColAmbiente);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_fColDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_fColSpeculaire);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_fColEmission);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &m_fShininess);

	// OpenGL Error handler
	GLenum errCode;
	if ((errCode= glGetError()) != GL_NO_ERROR)
	{
		const GLubyte* errString;
		errString = gluErrorString(errCode);
		qDebug("GLC_Material::GlExecute OpenGL Error %s", errString);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////
// Private servicies Functions
//////////////////////////////////////////////////////////////////////

// Update geometries which used material
void GLC_Material::UpdateUsed(void)
{
	CGeomMap::iterator iEntry= m_WhereUsed.begin();
	
    while (iEntry != m_WhereUsed.constEnd())
    {
        // Indique aux géométrie utilisant la matière que celle ci à changée
        iEntry.value()->SetMatiere(this);
        ++iEntry;
    }
	

}

