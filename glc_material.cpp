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
QColor GLC_Material::getAmbientColor() const
{
	return m_AmbientColor;
}

// Get diffuse color
QColor GLC_Material::getDiffuseColor() const
{
	return m_DiffuseColor; 
}

// Get specular color
QColor GLC_Material::getSpecularColor() const
{
	return m_SpecularColor;
}

// Get the emissive color
QColor GLC_Material::getLightEmission() const
{
	return m_LightEmission;
}
// Get the texture File Name
QString GLC_Material::getTextureFileName() const
{
	if (m_pTexture != NULL)
	{
		return m_pTexture->getTextureFileName();
	}
	else
	{
		return "";
	}
}

// Get Texture Id
GLuint GLC_Material::getTextureID() const
{
	if (m_pTexture != NULL)
	{
		return m_pTexture->getTextureID();
	}
	else
	{
		return 0;
	}
	
}

// return true if the texture is loaded
bool GLC_Material::textureIsLoaded() const
{
	if (m_pTexture != NULL)
	{
		return m_pTexture->isLoaded();
	}
	else
	{
		return false;
	}	
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Material properties
 void GLC_Material::setMaterial(const GLC_Material* pMat)
 {
	if (NULL != pMat->m_pTexture)
	{
		GLC_Texture* pTexture= new GLC_Texture(*(pMat->m_pTexture));
		setTexture(pTexture);
	}	
	// Ambient Color
	m_AmbientColor= pMat->m_AmbientColor;
	// Diffuse Color
	m_DiffuseColor= pMat->m_DiffuseColor;
	// Specular Color
	m_SpecularColor= pMat->m_SpecularColor;
	// Lighting emit
	m_LightEmission= pMat->m_LightEmission;
 	
 	m_fShininess= pMat->m_fShininess;
 	
 	updateUsed();
 }

// Set Ambiant Color
void GLC_Material::setAmbientColor(const QColor& ambientColor)
{
	m_AmbientColor= ambientColor;
	updateUsed();
}

// Set Diffuse color
void GLC_Material::setDiffuseColor(const QColor& diffuseColor)
{
	m_DiffuseColor= diffuseColor;
	updateUsed();
}

// Set Specular color
void GLC_Material::setSpecularColor(const QColor& specularColor)
{
	m_SpecularColor= specularColor;
	updateUsed();
}

// Set Emissive
void GLC_Material::setLightEmission(const QColor& lightEmission)
{
	m_LightEmission= lightEmission;
	updateUsed();
}

// Set Texture
void GLC_Material::setTexture(GLC_Texture* pTexture)
{
	qDebug() << "GLC_Material::SetTexture";
	if (m_pTexture != NULL)
	{
		delete m_pTexture;
		m_pTexture= pTexture;
		glLoadTexture();
	}
	else
	{
		// It is not sure that there is OpenGL context
		m_pTexture= pTexture;
	}
		
	updateUsed();
}

// remove Material Texture
void GLC_Material::removeTexture()
{
	if (m_pTexture != NULL)
	{
		delete m_pTexture;
		m_pTexture= NULL;
		updateUsed();
	}
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

// Load the texture
void GLC_Material::glLoadTexture(void)
{
	if (m_pTexture != NULL)
	{
		m_pTexture->glLoadTexture();
	}
}

// Execute OpenGL Material
void GLC_Material::glExecute(GLenum Mode)
{
	GLfloat pAmbientColor[4]= {getAmbientColor().redF(),
								getAmbientColor().greenF(),
								getAmbientColor().blueF(),
								getAmbientColor().alphaF()};
	
	GLfloat pDiffuseColor[4]= {getDiffuseColor().redF(),
								getDiffuseColor().greenF(),
								getDiffuseColor().blueF(),
								getDiffuseColor().alphaF()};
	
	GLfloat pSpecularColor[4]= {getSpecularColor().redF(),
								getSpecularColor().greenF(),
								getSpecularColor().blueF(),
								getSpecularColor().alphaF()};

	GLfloat pLightEmission[4]= {getLightEmission().redF(),
								getLightEmission().greenF(),
								getLightEmission().blueF(),
								getLightEmission().alphaF()};

	if (m_pTexture != NULL)
	{
		// for blend managing
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		m_pTexture->glcBindTexture();
	}
	glColor4fv(pAmbientColor);
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
	m_AmbientColor.setRgbF(0.8, 0.8, 0.8, 1.0);
}

// Init default color
void GLC_Material::initOtherColor(void)
{
	// Diffuse Color
	m_DiffuseColor.setRgbF(1.0, 1.0, 1.0, 1.0);

	// Specular Color
	m_SpecularColor.setRgbF(0.5, 0.5, 0.5, 1.0);

	// Lighting emit
	m_LightEmission.setRgbF(0.0, 0.0, 0.0, 1.0);
	
}

