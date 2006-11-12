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

//! \file glc_material.cpp implementation of the GLC_Material class.

#include <QtDebug>
#include <assert.h>
#include "glc_material.h"
#include "glc_collection.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

GLC_Material::GLC_Material()
:GLC_Object("Material")
, m_fShininess(50.0)		// By default shininess 50
, m_pTexture(NULL)			// no texture 
{
	// Ambient Color
	initAmbientColor();

	// Others
	initOtherColor();
}

GLC_Material::GLC_Material(const QColor &ambientColor)
:GLC_Object("Material")
, m_fShininess(50.0)		// default : shininess 50
, m_pTexture(NULL)			// no texture
{

	m_AmbientColor= ambientColor;
	
	// Others
	initOtherColor();
}


GLC_Material::GLC_Material(const char *pName ,const GLfloat *pAmbientColor)
:GLC_Object(pName)
, m_fShininess(50.0)		// default : shininess 50
, m_pTexture(NULL)			// no texture
{

	// Init Ambiant Color
	if (pAmbientColor != 0)
	{
		m_AmbientColor.setRgbF(static_cast<qreal>(pAmbientColor[0]),
								static_cast<qreal>(pAmbientColor[1]), 
								static_cast<qreal>(pAmbientColor[2]),
								static_cast<qreal>(pAmbientColor[3]));
	}
	else
	{	
		initAmbientColor();
	}
	// Others
	initOtherColor();
}
GLC_Material::GLC_Material(GLC_Texture* pTexture, const char *pName)
:GLC_Object(pName)
, m_fShininess(50.0)		// By default shininess 50
, m_pTexture(pTexture)		// Init texture 
{
	// Ambiente Color
	initAmbientColor();

	// Others
	initOtherColor();
}

// Copy constructor
GLC_Material::GLC_Material(const GLC_Material &InitMaterial)
:GLC_Object(InitMaterial.getName())
, m_fShininess(InitMaterial.m_fShininess)
, m_pTexture(NULL)
{
	if (NULL != InitMaterial.m_pTexture)
	{
		m_pTexture= new GLC_Texture(*(InitMaterial.m_pTexture));
		assert(m_pTexture != NULL);
	}
	
	// Ambient Color
	m_AmbientColor= InitMaterial.m_AmbientColor;

	// Diffuse Color
	m_DiffuseColor= InitMaterial.m_DiffuseColor;

	// Specular Color
	m_SpecularColor= InitMaterial.m_SpecularColor;

	// Lighting emit
	m_LightEmission= InitMaterial.m_LightEmission;
		
}

// Destructor
GLC_Material::~GLC_Material(void)
{

	// Remove material from Geometries
	CWhereUsed::iterator iEntry= m_WhereUsed.begin();
	
    while (iEntry != m_WhereUsed.constEnd())
    {
        // Remove material from geometry
        iEntry.value()->delMaterial(iEntry.value());
        ++iEntry;
    }
    // clear whereUSED Hash table
    m_WhereUsed.clear();
    
    if (NULL != m_pTexture)
    {
   		delete m_pTexture;
    	m_pTexture= NULL;
    }
    

}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get Ambiant color
void GLC_Material::getAmbientColor(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= static_cast<GLfloat>(m_AmbientColor.redF());
	pfCol[1]= static_cast<GLfloat>(m_AmbientColor.greenF());
	pfCol[2]= static_cast<GLfloat>(m_AmbientColor.blueF());
	pfCol[3]= static_cast<GLfloat>(m_AmbientColor.alphaF());
}

// Get diffuse color
void GLC_Material::getDiffuseColor(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= static_cast<GLfloat>(m_DiffuseColor.redF());
	pfCol[1]= static_cast<GLfloat>(m_DiffuseColor.greenF());
	pfCol[2]= static_cast<GLfloat>(m_DiffuseColor.blueF());
	pfCol[3]= static_cast<GLfloat>(m_DiffuseColor.alphaF());
}

// Get specular color
void GLC_Material::getSpecularColor(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= static_cast<GLfloat>(m_SpecularColor.redF());
	pfCol[1]= static_cast<GLfloat>(m_SpecularColor.greenF());
	pfCol[2]= static_cast<GLfloat>(m_SpecularColor.blueF());
	pfCol[3]= static_cast<GLfloat>(m_SpecularColor.alphaF());
}

// Get the emissive color
void GLC_Material::getLightEmission(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= static_cast<GLfloat>(m_LightEmission.redF());
	pfCol[1]= static_cast<GLfloat>(m_LightEmission.greenF());
	pfCol[2]= static_cast<GLfloat>(m_LightEmission.blueF());
	pfCol[3]= static_cast<GLfloat>(m_LightEmission.alphaF());
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Ambiant Color
void GLC_Material::setAmbientColor(const GLfloat* pfCol) // pfCol[4]
{
	m_AmbientColor.setRgbF(static_cast<qreal>(pfCol[0]),
							static_cast<qreal>(pfCol[1]), 
							static_cast<qreal>(pfCol[2]),
							static_cast<qreal>(pfCol[3]));
	
	updateUsed();
}

// Set Diffuse color
void GLC_Material::setDiffuseColor(const GLfloat* pfCol) // pfCol[4]
{
	m_DiffuseColor.setRgbF(static_cast<qreal>(pfCol[0]),
							static_cast<qreal>(pfCol[1]), 
							static_cast<qreal>(pfCol[2]),
							static_cast<qreal>(pfCol[3]));
	
	updateUsed();
}

// Set Specular color
void GLC_Material::setSpecularColor(const GLfloat* pfCol) // pfCol[4]
{
	m_SpecularColor.setRgbF(static_cast<qreal>(pfCol[0]),
							static_cast<qreal>(pfCol[1]), 
							static_cast<qreal>(pfCol[2]),
							static_cast<qreal>(pfCol[3]));
	
	updateUsed();
}

// Set Emissive
void GLC_Material::setLightEmission(const GLfloat* pfCol) // pfCol[4]
{
	m_LightEmission.setRgbF(static_cast<qreal>(pfCol[0]),
							static_cast<qreal>(pfCol[1]), 
							static_cast<qreal>(pfCol[2]),
							static_cast<qreal>(pfCol[3]));
	
	updateUsed();
}

// Add Geometry to where used hash table
bool GLC_Material::addGLC_Geom(GLC_Geometry* pGeom)
{
	CWhereUsed::iterator iGeom= m_WhereUsed.find(pGeom->getID());
	
	if (iGeom == m_WhereUsed.end())
	{	// Ok, ID doesn't exist		
		// Add Geometry to where used hash table
		m_WhereUsed.insert(pGeom->getID(), pGeom);		
		return true;
	}
	else
	{	// KO, ID exist
		qDebug("GLC_Material::AddGLC_Geom : Geometry not added");
		return false;
	}
	
}
// Supprime une géométrie de la collection
bool GLC_Material::delGLC_Geom(GLC_uint Key)
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
void GLC_Material::glExecute(GLenum Mode)
{
	GLfloat pAmbientColor[4];
	getAmbientColor(pAmbientColor);
	GLfloat pDiffuseColor[4];
	getDiffuseColor(pDiffuseColor);
	GLfloat pSpecularColor[4];
	getSpecularColor(pSpecularColor);
	GLfloat pLightEmission[4];
	getLightEmission(pLightEmission);
	
	if (m_pTexture != NULL)
	{
		// for blend managing
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		m_pTexture->glcBindTexture();
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pAmbientColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pDiffuseColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pSpecularColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, pLightEmission);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &m_fShininess);

	// OpenGL Error handler
	GLenum errCode;
	if ((errCode= glGetError()) != GL_NO_ERROR)
	{
		const GLubyte* errString;
		errString = gluErrorString(errCode);
		qDebug("GLC_Material::GlExecute OpenGL Error %s", errString);
	}

}
//////////////////////////////////////////////////////////////////////
// Private servicies Functions
//////////////////////////////////////////////////////////////////////

// Update geometries which used material
void GLC_Material::updateUsed(void)
{
	CWhereUsed::iterator iEntry= m_WhereUsed.begin();
	
    while (iEntry != m_WhereUsed.constEnd())
    {
        // Indique aux géométrie utilisant la matière que celle ci à changée
        iEntry.value()->setMaterial(this);
        ++iEntry;
    }
	
}

// Init Ambiant Color
void GLC_Material::initAmbientColor(void)
{
	m_AmbientColor.setRgbF(0.2, 0.2, 0.2, 1.0);
}

// Init default color
void GLC_Material::initOtherColor(void)
{
	// Diffuse Color
	m_DiffuseColor.setRgbF(1.0, 1.0, 1.0, 1.0);

	// Specular Color
	m_SpecularColor.setRgbF(1.0, 1.0, 1.0, 1.0);

	// Lighting emit
	m_LightEmission.setRgbF(0.0, 0.0, 0.0, 1.0);
	
}

