/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

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

//! \file glc_vbogeom.cpp Implementation of the GLC_VboGeom class.

#include "glc_vbogeom.h"
#include "glc_openglexception.h"
#include "glc_selectionmaterial.h"
#include "glc_state.h"

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////
// Default constructor
GLC_VboGeom::GLC_VboGeom(const QString& name, const bool typeIsWire)
:GLC_Object(name)
, m_VboId(0)
, m_IboId(0)
, m_GeometryIsValid(false)	// By default geometry is invalid
, m_pMaterial(NULL)			// have to be set later in constructor
, m_UseColorPerVertex(false)
, m_IsWire(typeIsWire)		// the geometry type
, m_IsTransparent(false)	// Not transparent by default
{	
	// Material is set here
	setMaterial(new GLC_Material());
}
// Copy constructor
GLC_VboGeom::GLC_VboGeom(const GLC_VboGeom& sourceGeom)
:GLC_Object(sourceGeom)
, m_VboId(0)
, m_IboId(0)
, m_GeometryIsValid(false)	// By default geometry is invalid
, m_pMaterial(NULL)			// have to be set later in constructor
, m_UseColorPerVertex(sourceGeom.m_UseColorPerVertex)
, m_IsWire(sourceGeom.m_IsWire)
, m_IsTransparent(sourceGeom.m_IsTransparent)
{
	m_Uid= glc::GLC_GenID();
	// Material is copy here
	setMaterial(new GLC_Material());
	m_pMaterial->setMaterial(sourceGeom.getMaterial());
}

GLC_VboGeom::~GLC_VboGeom()
{
	// VBO
	if (0 != m_VboId)
		glDeleteBuffers(1, &m_VboId);
	// IBO
	if (0 != m_IboId)
		glDeleteBuffers(1, &m_IboId);
	// Material	
	if (NULL != m_pMaterial)
	{
		m_pMaterial->delGLC_Geom(getID());	//Remove Geometry from the material usage collection
		if (m_pMaterial->isUnused()) delete m_pMaterial;
	}
}

/////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// return the geometry bounding box
GLC_BoundingBox* GLC_VboGeom::getBoundingBox(void) const
{
	return NULL;	
}


/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Material
void GLC_VboGeom::setMaterial(GLC_Material* pMat)
{
	if (pMat != m_pMaterial)
	{
		if (pMat != NULL)
		{
			pMat->addGLC_Geom(this);
		}
	
		if (m_pMaterial != NULL)
		{
			m_pMaterial->delGLC_Geom(getID());
			if (m_pMaterial->isUnused()) delete m_pMaterial;
		}

		m_pMaterial= pMat;
		
		// Test if the material is transparent
		if (m_pMaterial->isTransparent())
		{
			setTransparency(true);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// if the geometry have a texture, load it
void GLC_VboGeom::glLoadTexture(void)
{
	m_pMaterial->glLoadTexture();
}

// Geometry display
void GLC_VboGeom::glExecute(bool isSelected, bool forceWire)
{
	// Define Geometry's property
	glPropGeom(isSelected, forceWire);

	if (0 == m_VboId)
	{
		createVBOs();
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IboId);
	}
	if (isSelected and GLC_State::selectionShaderUsed())
	{
		GLC_SelectionMaterial::useShader();
	}
		
	glDraw();
	
	if (isSelected and GLC_State::selectionShaderUsed())
	{
		GLC_SelectionMaterial::unUseShader();
	}
		
	m_GeometryIsValid= true;
	
	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_VboGeom::GlExecute ", error);
		throw(OpenGlException);
	}
}

// Virtual interface for OpenGL Geometry properties.
void GLC_VboGeom::glPropGeom(bool isSelected, bool forceWire)
{
	
	if(m_IsWire || forceWire)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		
		if (isSelected) GLC_SelectionMaterial::glExecute();
		else glColor4f(getdRed(), getdGreen(), getdBlue(), getdAlpha());			// is color
	}
	else if (m_pMaterial->getAddRgbaTexture())
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		m_pMaterial->glExecute();
		if (isSelected) GLC_SelectionMaterial::glExecute();		
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		if (isSelected) GLC_SelectionMaterial::glExecute();
		else m_pMaterial->glExecute();
	}
		
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_VboGeom::GlPropGeom ", error);
		throw(OpenGlException);
	}	
}

// Vbo creation
void GLC_VboGeom::createVBOs()
{
	glGenBuffers(1, &m_VboId);
	glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
	glGenBuffers(1, &m_IboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IboId);
}
