/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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
//! \file glc_renderproperties.cpp implementation for the GLC_RenderProperties class.

#include "glc_renderproperties.h"

// Default constructor
GLC_RenderProperties::GLC_RenderProperties()
: m_Uid(glc::GLC_GenUserID())
, m_IsSelected(false)
, m_PolyFace(GL_FRONT_AND_BACK)
, m_PolyMode(GL_FILL)
, m_RenderMode(glc::NonTransparentMaterial)
, m_pOverwriteMaterial(NULL)
, m_OverwriteTransparency(0.0f)
, m_pSelectedPrimitvesId(NULL)
, m_pOverwritePrimitiveMaterialMap(NULL)
{

}

// Copy constructor
GLC_RenderProperties::GLC_RenderProperties(const GLC_RenderProperties& renderProperties)
: m_Uid(glc::GLC_GenUserID())
, m_IsSelected(renderProperties.m_IsSelected)
, m_PolyFace(renderProperties.m_PolyFace)
, m_PolyMode(renderProperties.m_PolyMode)
, m_RenderMode(renderProperties.m_RenderMode)
, m_pOverwriteMaterial(renderProperties.m_pOverwriteMaterial)
, m_OverwriteTransparency(renderProperties.m_OverwriteTransparency)
, m_pSelectedPrimitvesId(NULL)
, m_pOverwritePrimitiveMaterialMap(NULL)
{
	// Update overwrite material usage
	if (NULL != m_pOverwriteMaterial)
	{
		m_pOverwriteMaterial->addUsage(m_Uid);
	}

	// Copy the list of id of selected primitive
	if (NULL != renderProperties.m_pSelectedPrimitvesId)
	{
		m_pSelectedPrimitvesId= new QList<GLC_uint>(*m_pSelectedPrimitvesId);
	}

	// Update primitive overwrite material usage
	if (renderProperties.m_pOverwritePrimitiveMaterialMap)
	{
		// Copy the hash table of overwrite materials
		m_pOverwritePrimitiveMaterialMap= new QHash<GLC_uint, GLC_Material*>(*(renderProperties.m_pOverwritePrimitiveMaterialMap));
		QHash<GLC_uint, GLC_Material*>::iterator iMatMap= m_pOverwritePrimitiveMaterialMap->begin();
		while (m_pOverwritePrimitiveMaterialMap->constEnd() != iMatMap)
		{
			iMatMap.value()->addUsage(m_Uid);
			++iMatMap;
		}
	}
}

// Assignement operator
GLC_RenderProperties& GLC_RenderProperties::operator=(const GLC_RenderProperties& renderProperties)
{
	if (this != &renderProperties)
	{
		clear();
		m_IsSelected= renderProperties.m_IsSelected;
		m_PolyFace= renderProperties.m_PolyFace;
		m_PolyMode= renderProperties.m_PolyMode;
		m_RenderMode= renderProperties.m_RenderMode;
		m_pOverwriteMaterial= renderProperties.m_pOverwriteMaterial;
		m_OverwriteTransparency= renderProperties.m_OverwriteTransparency;
		m_pSelectedPrimitvesId= NULL;
		m_pOverwritePrimitiveMaterialMap= NULL;

		// Update overwrite material usage
		if (NULL != m_pOverwriteMaterial)
		{
			m_pOverwriteMaterial->addUsage(m_Uid);
		}

		// Copy the list of id of selected primitive
		if (NULL != renderProperties.m_pSelectedPrimitvesId)
		{
			m_pSelectedPrimitvesId= new QList<GLC_uint>(*m_pSelectedPrimitvesId);
		}

		// Update primitive overwrite material usage
		if (renderProperties.m_pOverwritePrimitiveMaterialMap)
		{
			// Copy the hash table of overwrite materials
			m_pOverwritePrimitiveMaterialMap= new QHash<GLC_uint, GLC_Material*>(*(renderProperties.m_pOverwritePrimitiveMaterialMap));
			QHash<GLC_uint, GLC_Material*>::iterator iMatMap= m_pOverwritePrimitiveMaterialMap->begin();
			while (m_pOverwritePrimitiveMaterialMap->constEnd() != iMatMap)
			{
				iMatMap.value()->addUsage(m_Uid);
				++iMatMap;
			}
		}
	}

	return *this;
}

// Destructor
GLC_RenderProperties::~GLC_RenderProperties()
{
	clear();
}

// Clear the content of the render properties and update materials usage
void GLC_RenderProperties::clear()
{
	if (NULL != m_pOverwriteMaterial)
	{
		// Delete the material if it is unused
		m_pOverwriteMaterial->delUsage(m_Uid);
		if (m_pOverwriteMaterial->isUnused()) delete m_pOverwriteMaterial;
		m_pOverwriteMaterial= NULL;
	}

	delete m_pSelectedPrimitvesId;
	m_pSelectedPrimitvesId= NULL;

	if (NULL != m_pOverwritePrimitiveMaterialMap)
	{
		// Delete unused materials from the Hash table
		QHash<GLC_uint, GLC_Material*>::iterator iMatMap= m_pOverwritePrimitiveMaterialMap->begin();
		while (m_pOverwritePrimitiveMaterialMap->constEnd() != iMatMap)
		{
			iMatMap.value()->delUsage(m_Uid);
			if (iMatMap.value()->isUnused()) delete iMatMap.value();

			++iMatMap;
		}
		delete m_pOverwritePrimitiveMaterialMap;
		m_pOverwritePrimitiveMaterialMap= NULL;
	}

}

// Set the overwrite material
void GLC_RenderProperties::setOverwriteMaterial(GLC_Material* pMaterial)
{
	Q_ASSERT(NULL != pMaterial);
	if (NULL != m_pOverwriteMaterial)
	{
		m_pOverwriteMaterial->delUsage(m_Uid);
		if (m_pOverwriteMaterial->isUnused()) delete m_pOverwriteMaterial;
	}
	m_pOverwriteMaterial= pMaterial;

	m_pOverwriteMaterial->addUsage(m_Uid);
}

// Set the list of selected primitives id
void GLC_RenderProperties::setlistOfSelectedPrimitivesId(const QList<GLC_uint>& list)
{
	if (NULL != m_pSelectedPrimitvesId) delete m_pSelectedPrimitvesId;
	m_pSelectedPrimitvesId= new QList<GLC_uint>(list);
}

// Set the overwrite primitive material Hash
void GLC_RenderProperties::setOfOverwritePrimitiveMaterials(const QHash<GLC_uint, GLC_Material*>& primitiveMaterialMap)
{
	// Delete the current primitive material map
	if (NULL != m_pOverwritePrimitiveMaterialMap)
	{
		// Delete unused materials from the Hash table
		QHash<GLC_uint, GLC_Material*>::iterator iMatMap= m_pOverwritePrimitiveMaterialMap->begin();
		while (m_pOverwritePrimitiveMaterialMap->constEnd() != iMatMap)
		{
			iMatMap.value()->delUsage(m_Uid);
			if (iMatMap.value()->isUnused()) delete iMatMap.value();

			++iMatMap;
		}
		delete m_pOverwritePrimitiveMaterialMap;
	}

	// Create the new primitive material map
	m_pOverwritePrimitiveMaterialMap= new QHash<GLC_uint, GLC_Material*>(primitiveMaterialMap);

	// Update usage off materials
	QHash<GLC_uint, GLC_Material*>::iterator iMatMap= m_pOverwritePrimitiveMaterialMap->begin();
	while (m_pOverwritePrimitiveMaterialMap->constEnd() != iMatMap)
	{
		iMatMap.value()->addUsage(m_Uid);
		++iMatMap;
	}
}

