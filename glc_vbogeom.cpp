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
, m_GeometryIsValid(false)	// By default geometry is invalid
, m_pBoundingBox(NULL)
, m_MaterialHash()
, m_UseColorPerVertex(false)
, m_VertexVector()
, m_IndexVector()
, m_VboId(0)
, m_IboId(0)
, m_IsWire(typeIsWire)		// the geometry type
, m_TransparentMaterialNumber(0)
{

}
// Copy constructor
GLC_VboGeom::GLC_VboGeom(const GLC_VboGeom& sourceGeom)
:GLC_Object(sourceGeom)
, m_GeometryIsValid(false)	// By default geometry is invalid
, m_pBoundingBox(NULL)
, m_MaterialHash(sourceGeom.m_MaterialHash)
, m_UseColorPerVertex(sourceGeom.m_UseColorPerVertex)
, m_VertexVector(sourceGeom.m_VertexVector)
, m_IndexVector(sourceGeom.m_IndexVector)
, m_VboId(0)
, m_IboId(0)
, m_IsWire(sourceGeom.m_IsWire)
, m_TransparentMaterialNumber(sourceGeom.m_TransparentMaterialNumber)
{
	m_Uid= glc::GLC_GenID();

	// Add this mesh to inner material
	MaterialHash::const_iterator i= m_MaterialHash.begin();
    while (i != m_MaterialHash.constEnd())
    {
        // update inner material use table
        i.value()->addGLC_Geom(this);
        ++i;
    }

	if (NULL != sourceGeom.m_pBoundingBox)
	{
		m_pBoundingBox= new GLC_BoundingBox(*sourceGeom.m_pBoundingBox);
	}
}

GLC_VboGeom::~GLC_VboGeom()
{
	if (GLC_State::vboIsSupported())
	{
		// VBO
		if (0 != m_VboId)
			glDeleteBuffers(1, &m_VboId);
		// IBO
		if (0 != m_IboId)
			glDeleteBuffers(1, &m_IboId);
	}
	// delete mesh inner material
	{
		MaterialHash::const_iterator i= m_MaterialHash.begin();
	    while (i != m_MaterialHash.constEnd())
	    {
	        // delete the material if necessary
	        i.value()->delGLC_Geom(id());
	        if (i.value()->isUnused()) delete i.value();
	        ++i;
	    }
	}
	m_MaterialHash.clear();

	delete m_pBoundingBox;

}

/////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

//! Return material index if Material is the same than a material already in the mesh
GLC_uint GLC_VboGeom::materialIndex(const GLC_Material& mat) const
{
	int index= 0;
	MaterialHash::const_iterator iEntry= m_MaterialHash.begin();

    while ((iEntry != m_MaterialHash.constEnd()) and !(*(iEntry.value()) == mat))
    {
        ++iEntry;
    }
    if (iEntry != m_MaterialHash.constEnd())
    {
    	index= iEntry.key();
    }
	return index;
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Replace the Master material
void GLC_VboGeom::replaceMasterMaterial(GLC_Material* pMaterial)
{
	if (not m_MaterialHash.isEmpty())
	{
		Q_ASSERT(1 == m_MaterialHash.size());
		if (pMaterial != firstMaterial())
		{
			// Remove the first material
			MaterialHash::iterator iMaterial= m_MaterialHash.begin();
	        // delete the material if necessary
			iMaterial.value()->delGLC_Geom(id());
			if (iMaterial.value()->isTransparent())
			{
				--m_TransparentMaterialNumber;
			}
	        if (iMaterial.value()->isUnused()) delete iMaterial.value();
			m_MaterialHash.erase(iMaterial);
			addMaterial(pMaterial);
		}
	}
	else
	{
		addMaterial(pMaterial);
	}
}

// Add material to mesh
void GLC_VboGeom::addMaterial(GLC_Material* pMaterial)
{
	if (pMaterial != NULL)
	{
		const GLC_uint materialID= pMaterial->id();
		MaterialHash::const_iterator iMaterial= m_MaterialHash.find(materialID);
		// Check if there is a material at specified index
		Q_ASSERT(iMaterial == m_MaterialHash.end());

		// Add this geometry in the material use table
		pMaterial->addGLC_Geom(this);
		// Add the Material to Material hash table
		m_MaterialHash.insert(materialID, pMaterial);

		// Test if the material is transparent
		if (pMaterial->isTransparent())
		{
			qDebug() << "Add transparent material";
			++m_TransparentMaterialNumber;
		}
		// Invalid the geometry
		m_GeometryIsValid = false;
	}
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// if the geometry have a texture, load it
void GLC_VboGeom::glLoadTexture(void)
{
	MaterialHash::iterator iMaterial= m_MaterialHash.begin();

    while (iMaterial != m_MaterialHash.constEnd())
    {
        // Load texture of mesh materials
        iMaterial.value()->glLoadTexture();
        ++iMaterial;
    }
}

// Geometry display
void GLC_VboGeom::glExecute(bool isSelected, bool transparent)
{
	if (m_MaterialHash.isEmpty())
	{
		GLC_Material* pMaterial= new GLC_Material();
		pMaterial->setName(name());
		pMaterial->addGLC_Geom(this);
		m_MaterialHash.insert(pMaterial->id(), pMaterial);

	}
	// Define Geometry's property
	if(not GLC_State::isInSelectionMode())
	{
		glPropGeom(isSelected);
	}
	else
	{
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
	}

	const bool vboIsSupported= GLC_State::vboIsSupported();

	if (vboIsSupported and (0 == m_VboId))
	{
		createVBOs();
	}
	if (vboIsSupported)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IboId);

		glDraw(transparent);

		m_GeometryIsValid= true;

		// Unbind VBOs
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else // VBO not supported
	{
		glDraw(transparent);
		m_GeometryIsValid= true;
	}

	// OpenGL error handler
	GLenum error= glGetError();
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_VboGeom::GlExecute ", error);
		throw(OpenGlException);
	}
}

// Virtual interface for OpenGL Geometry properties.
void GLC_VboGeom::glPropGeom(bool isSelected)
{
	Q_ASSERT(not m_MaterialHash.isEmpty());

	if(m_IsWire and (m_MaterialHash.size() == 1))
	{
		GLC_Material* pCurrentMaterial= m_MaterialHash.begin().value();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		const GLfloat red= pCurrentMaterial->getDiffuseColor().redF();
		const GLfloat green= pCurrentMaterial->getDiffuseColor().greenF();
		const GLfloat blue= pCurrentMaterial->getDiffuseColor().blueF();
		const GLfloat alpha= pCurrentMaterial->getDiffuseColor().alphaF();

		glColor4f(red, green, blue, alpha);
		if (isSelected) GLC_SelectionMaterial::glExecute();
	}
	else if (m_MaterialHash.size() == 1)
	{
		GLC_Material* pCurrentMaterial= m_MaterialHash.begin().value();
		if (pCurrentMaterial->getAddRgbaTexture())
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			pCurrentMaterial->glExecute();
			if (isSelected) GLC_SelectionMaterial::glExecute();
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			if (isSelected) GLC_SelectionMaterial::glExecute();
			else pCurrentMaterial->glExecute();
		}
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
	glGenBuffers(1, &m_IboId);
}
