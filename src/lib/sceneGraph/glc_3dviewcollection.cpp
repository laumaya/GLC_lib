/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_3dviewcollection.cpp implementation of the GLC_3DViewCollection class.

#include <QtDebug>

#include "glc_3dviewcollection.h"
#include "../shading/glc_selectionmaterial.h"
#include "../glc_state.h"
#include "../shading/glc_shader.h"
#include "../viewport/glc_viewport.h"
#include "glc_spacepartitioning.h"
#include "../glc_context.h"
#include "../glc_contextmanager.h"

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

GLC_3DViewCollection::GLC_3DViewCollection()
    : m_3DViewInstanceHash()
    , m_SelectedInstances()
    , m_ShadedPointerViewInstanceHash()
    , m_ShaderGroup()
    , m_MainInstances()
    , m_IsInShowSate(true)
    , m_UseLod(false)
    , m_pViewport(nullptr)
    , m_pSpacePartitioning(nullptr)
    , m_UseSpacePartitioning(false)
    , m_IsViewable(true)
    , m_UseOrderRendering(false)
{
}

GLC_3DViewCollection::~GLC_3DViewCollection()
{
	// Delete all collection's elements and the collection bounding box
	clear();
}
//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

bool GLC_3DViewCollection::bindShader(GLC_uint shaderId)
{
    bool subject;
	if (m_ShadedPointerViewInstanceHash.contains(shaderId))
	{
        subject= false;
	}
	else
	{
		PointerViewInstanceHash* pNodeHash= new PointerViewInstanceHash;
		m_ShadedPointerViewInstanceHash.insert(shaderId, pNodeHash);
        subject= true;
	}
    return subject;
}

bool GLC_3DViewCollection::unBindShader(GLC_uint shaderId)
{
    bool subject= false;
	if (m_ShadedPointerViewInstanceHash.contains(shaderId))
	{
		// Find node which use the shader
        const QList<GLC_uint> nodeId(m_ShaderGroup.keys(shaderId));

		// Move these node in the standard hash and remove them from shader group
		PointerViewInstanceHash* pShaderNodeHash= m_ShadedPointerViewInstanceHash.take(shaderId);
		for (int i= 0; i < nodeId.size(); ++i)
		{
			const GLC_uint id= nodeId[i];
			GLC_3DViewInstance* pInstance= pShaderNodeHash->value(id);

			if (!pInstance->isSelected())
			{
				m_MainInstances.insert(id, pInstance);
			}
			else
			{
				m_SelectedInstances.insert(id, pInstance);
			}
			m_ShaderGroup.remove(id);
		}
		pShaderNodeHash->clear();
		delete pShaderNodeHash;
        subject= true;
	}
	Q_ASSERT(!m_ShadedPointerViewInstanceHash.contains(shaderId));
    return subject;
}

bool GLC_3DViewCollection::unBindAllShader()
{
    bool subject= true;
    HashList::const_iterator iEntry= m_ShadedPointerViewInstanceHash.constBegin();
	QList<GLuint> shaderList;
    while (iEntry != m_ShadedPointerViewInstanceHash.constEnd())
    {
    	shaderList.append(iEntry.key());
    	++iEntry;
    }
    const int size= shaderList.size();
    for (int i=0; i < size; ++i)
    {
        subject= subject && unBindShader(shaderList[i]);
    }
    return subject;
}

bool GLC_3DViewCollection::add(const GLC_3DViewInstance& node, GLC_uint shaderID)
{
    bool subject= false;
	const GLC_uint key= node.id();
	if (m_3DViewInstanceHash.contains(key))
	{
		return false;
	}

	m_3DViewInstanceHash.insert(key, node);
	// Create an GLC_3DViewInstance pointer of the inserted instance
	ViewInstancesHash::iterator iNode= m_3DViewInstanceHash.find(key);
	GLC_3DViewInstance* pInstance= &(iNode.value());
	// Chose the hash where instance is
	if(0 != shaderID)
	{
		// Test if shaderId group exist
		if (m_ShadedPointerViewInstanceHash.contains(shaderID))
		{
			m_ShaderGroup.insert(key, shaderID);

			if(pInstance->isSelected())
			{
				m_SelectedInstances.insert(key, pInstance);
			}
			else
			{
				m_ShadedPointerViewInstanceHash.value(shaderID)->insert(key, pInstance);
			}
            subject=true;
		}
	}
	else if (!pInstance->isSelected())
	{
		m_MainInstances.insert(key, pInstance);
        subject=true;
	}
	else
	{
		m_SelectedInstances.insert(key, pInstance);
        subject=true;
	}

    return subject;
}

void GLC_3DViewCollection::changeShadingGroup(GLC_uint instanceId, GLC_uint shaderId)
{
	// Test if the specified instance exist
	Q_ASSERT(m_3DViewInstanceHash.contains(instanceId));
	// Get the instance shading group
	const GLuint instanceShadingGroup= shadingGroup(instanceId);
	// Get a pointer to the instance
    GLC_3DViewInstance* pInstance= nullptr;
	if (0 == instanceShadingGroup)
	{
		// The instance is not in a shading group
		if (m_MainInstances.contains(instanceId))
		{
			pInstance= m_MainInstances.take(instanceId);
		}
		else if (m_SelectedInstances.contains(instanceId))
		{
			// The instance is selected don't take it
			pInstance= m_SelectedInstances.value(instanceId);
		}
		else
		{
			Q_ASSERT(false);
		}
	}
	else
	{
		m_ShaderGroup.remove(instanceId);
		// The instance is in a shading group
		if (m_SelectedInstances.contains(instanceId))
		{
			// The instance is selected don't take it
			pInstance= m_SelectedInstances.value(instanceId);
		}
		else
		{
			pInstance= m_ShadedPointerViewInstanceHash.value(instanceShadingGroup)->take(instanceId);
		}

	}
	// Put the instance in specified shading group
	if (0 != shaderId)
	{
		m_ShaderGroup.insert(instanceId, shaderId);
		if (!pInstance->isSelected())
		{
			m_ShadedPointerViewInstanceHash.value(shaderId)->insert(instanceId, pInstance);
		}
	}
	else if (!pInstance->isSelected())
	{
		m_MainInstances.insert(instanceId, pInstance);
	}
}

bool GLC_3DViewCollection::remove(GLC_uint key)
{
    bool subject;

    if (m_3DViewInstanceHash.contains(key))
	{	// Ok, the key exist

        if (m_SelectedInstances.contains(key))
		{
			// if the geometry is selected, unselect it
            unselect(key);
		}

        m_MainInstances.remove(key);
        m_3DViewInstanceHash.remove(key);		// Delete the conteneur

        subject= true;
	}
	else
	{	// KO, key doesn't exist
        subject= false;
	}

    return subject;
}

void GLC_3DViewCollection::clear(void)
{
	// Clear Selected node Hash Table
	m_SelectedInstances.clear();
	// Clear the not transparent Hash Table
	m_MainInstances.clear();
	// Clear Other Node Hash List
    qDeleteAll(m_ShadedPointerViewInstanceHash);
    m_ShadedPointerViewInstanceHash.clear();
	m_ShaderGroup.clear();

	// Clear main Hash table
    m_3DViewInstanceHash.clear();

	// delete the space partitioning
	delete m_pSpacePartitioning;
    m_pSpacePartitioning= nullptr;
}

bool GLC_3DViewCollection::select(GLC_uint key, bool primitive)
{
    bool subject= false;

    if (m_3DViewInstanceHash.contains(key))
    {
        ViewInstancesHash::iterator iNode= m_3DViewInstanceHash.find(key);
        PointerViewInstanceHash::iterator iSelectedNode= m_SelectedInstances.find(key);

        if ((iNode != m_3DViewInstanceHash.end()) && (iSelectedNode == m_SelectedInstances.end()))
        {	// Ok, the key exist and the node is not selected
            GLC_3DViewInstance* pSelectedInstance= &(iNode.value());
            m_SelectedInstances.insert(pSelectedInstance->id(), pSelectedInstance);

            // Remove Selected Node from is previous collection
            if (isInAShadingGroup(key))
            {
                m_ShadedPointerViewInstanceHash.value(shadingGroup(key))->remove(key);
            }
            else
            {
                m_MainInstances.remove(key);
            }
            pSelectedInstance->select(primitive);

            subject= true;
        }

    }
    return subject;
}

void GLC_3DViewCollection::selectAll(bool allShowState)
{
	unselectAll();
	ViewInstancesHash::iterator iNode= m_3DViewInstanceHash.begin();
	while (iNode != m_3DViewInstanceHash.end())
	{
		GLC_3DViewInstance *pCurrentInstance= &(iNode.value());
		const GLC_uint instanceId= pCurrentInstance->id();

		if (allShowState || (pCurrentInstance->isVisible() == m_IsInShowSate))
		{
			pCurrentInstance->select(false);
			m_SelectedInstances.insert(instanceId, pCurrentInstance);
			m_MainInstances.remove(instanceId);
			if(isInAShadingGroup(instanceId))
			{
				m_ShadedPointerViewInstanceHash.value(shadingGroup(instanceId))->remove(instanceId);
			}
		}
        ++iNode;
	}
}

bool GLC_3DViewCollection::unselect(GLC_uint key)
{
    bool subject;

	PointerViewInstanceHash::iterator iSelectedNode= m_SelectedInstances.find(key);

	if (iSelectedNode != m_SelectedInstances.end())
	{	// Ok, the key exist and the node is selected
		iSelectedNode.value()->unselect();

        GLC_3DViewInstance* pSelectedNode= iSelectedNode.value();
		m_SelectedInstances.remove(key);

		// Insert Selected Node to the right collection
		if (isInAShadingGroup(key))
		{
			m_ShadedPointerViewInstanceHash.value(shadingGroup(key))->insert(key, pSelectedNode);
		}
		else
		{
			m_MainInstances.insert(key, pSelectedNode);
		}

        subject= true;

	}
	else
	{	// KO, key doesn't exist or node allready selected
        subject= false;
	}
    return subject;
}

void GLC_3DViewCollection::unselectAll()
{
	PointerViewInstanceHash::iterator iSelectedNode= m_SelectedInstances.begin();

    while (iSelectedNode != m_SelectedInstances.end())
    {
    	GLC_3DViewInstance* pInstance= iSelectedNode.value();
    	pInstance->unselect();
		if (isInAShadingGroup(pInstance->id()))
		{
			m_ShadedPointerViewInstanceHash.value(shadingGroup(pInstance->id()))->insert(pInstance->id(), pInstance);
		}
		else
		{
			m_MainInstances.insert(pInstance->id(), pInstance);
		}

        ++iSelectedNode;
    }
    // Clear selected node hash table
    m_SelectedInstances.clear();
}

void GLC_3DViewCollection::setPolygonModeForAll(GLenum face, GLenum mode)
{
	ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();

    while (iEntry != m_3DViewInstanceHash.end())
    {
    	iEntry.value().setPolygonMode(face, mode);
        ++iEntry;
    }

}

void GLC_3DViewCollection::setVisibility(const GLC_uint key, const bool visibility)
{
	ViewInstancesHash::iterator iNode= m_3DViewInstanceHash.find(key);
	if (iNode != m_3DViewInstanceHash.end())
	{	// Ok, the key exist
		iNode.value().setVisibility(visibility);
	}
}

void GLC_3DViewCollection::showAll()
{
	ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();

    while (iEntry != m_3DViewInstanceHash.end())
    {
     	iEntry.value().setVisibility(true);
        ++iEntry;
    }
}

void GLC_3DViewCollection::hideAll()
{
	ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();

    while (iEntry != m_3DViewInstanceHash.end())
    {
    	iEntry.value().setVisibility(false);
        ++iEntry;
    }
}

void GLC_3DViewCollection::bindSpacePartitioning(GLC_SpacePartitioning* pSpacePartitioning)
{
    Q_ASSERT(nullptr != pSpacePartitioning);

	delete m_pSpacePartitioning;
	m_pSpacePartitioning= pSpacePartitioning;
    m_pSpacePartitioning->set3DViewCollection(this);
}

void GLC_3DViewCollection::unbindSpacePartitioning()
{
	delete m_pSpacePartitioning;
    m_pSpacePartitioning= nullptr;
	m_UseSpacePartitioning= false;

	ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();
    while (iEntry != m_3DViewInstanceHash.end())
    {
    	// Update Instance viewable flag
    	iEntry.value().setViewable(GLC_3DViewInstance::FullViewable);
        ++iEntry;
    }
}

void GLC_3DViewCollection::updateInstanceViewableState(GLC_Matrix4x4* pMatrix)
{
    if ((nullptr != m_pViewport) && m_UseSpacePartitioning && (nullptr != m_pSpacePartitioning))
	{
		if (m_pViewport->updateFrustum(pMatrix))
        {
            m_pSpacePartitioning->updateViewableInstances(m_pViewport->frustum());
        }
	}
}

void GLC_3DViewCollection::updateInstanceViewableState(const GLC_Frustum& frustum)
{
    if (nullptr != m_pSpacePartitioning)
    {
        m_pSpacePartitioning->updateViewableInstances(frustum);
    }
}

void GLC_3DViewCollection::updateSpacePartitionning()
{
    if (nullptr != m_pSpacePartitioning)
    {
        m_pSpacePartitioning->updateSpacePartitioning();
    }
}

void GLC_3DViewCollection::setVboUsage(bool usage)
{
	ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();

    while (iEntry != m_3DViewInstanceHash.end())
    {
    	iEntry.value().setVboUsage(usage);
        ++iEntry;
    }
}

void GLC_3DViewCollection::setMeshWireColorAndLineWidth(const QColor& color, GLfloat lineWidth)
{
    ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();

    while (iEntry != m_3DViewInstanceHash.end())
    {
        iEntry.value().setMeshWireColorAndLineWidth(color, lineWidth);
        ++iEntry;
    }
}

QList<GLC_3DViewInstance*> GLC_3DViewCollection::instancesHandle()
{
	QList<GLC_3DViewInstance*> instancesList;

	ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();

    while (iEntry != m_3DViewInstanceHash.end())
    {
    	instancesList.append(&(iEntry.value()));
        ++iEntry;
    }
	return instancesList;
}

QList<GLC_3DViewInstance*> GLC_3DViewCollection::visibleInstancesHandle()
{
    QList<GLC_3DViewInstance*> subject;

	ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();

    while (iEntry != m_3DViewInstanceHash.end())
    {
    	if (iEntry.value().isVisible())
    	{
            subject.append(&(iEntry.value()));
    	}
        ++iEntry;
    }
    return subject;

}

bool GLC_3DViewCollection::hasVisibleInstance() const
{
    bool subject= false;
    ViewInstancesHash::const_iterator iEntry= m_3DViewInstanceHash.constBegin();
    while (!subject && (iEntry != m_3DViewInstanceHash.constEnd()))
    {
        subject= iEntry.value().isVisible();
        ++iEntry;
    }
    return subject;
}

QList<GLC_3DViewInstance*> GLC_3DViewCollection::viewableInstancesHandle()
{
    QList<GLC_3DViewInstance*> subject;

	ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();

    while (iEntry != m_3DViewInstanceHash.end())
    {
    	if (iEntry.value().isVisible() == m_IsInShowSate)
    	{
            subject.append(&(iEntry.value()));
    	}
        ++iEntry;
    }
    return subject;
}

GLC_3DViewInstance* GLC_3DViewCollection::instanceHandle(GLC_uint Key)
{
	Q_ASSERT(m_3DViewInstanceHash.contains(Key));
	return &(m_3DViewInstanceHash[Key]);
}

GLC_BoundingBox GLC_3DViewCollection::boundingBox(bool allObject)
{
	GLC_BoundingBox boundingBox;
	// Check if the bounding box have to be updated
	if (!m_3DViewInstanceHash.isEmpty())
	{
		ViewInstancesHash::iterator iEntry= m_3DViewInstanceHash.begin();
        while (iEntry != m_3DViewInstanceHash.end())
	    {
	        if(allObject || iEntry.value().isVisible() == m_IsInShowSate)
	        {
	        	// Combine Collection BoundingBox with element Bounding Box
	        	boundingBox.combine(iEntry.value().boundingBox());
	        }
	        ++iEntry;
	    }
	}
	return boundingBox;
}

int GLC_3DViewCollection::drawableObjectsSize() const
{
	// The number of object to drw
	int numberOffDrawnHit= 0;

	// Count the number off instance to draw
	ViewInstancesHash::const_iterator i= m_3DViewInstanceHash.begin();
	while (i != m_3DViewInstanceHash.constEnd())
	{
		if (i.value().isVisible() == m_IsInShowSate)
		{
			++numberOffDrawnHit;
		}
		++i;
	}
	return numberOffDrawnHit;
}

QList<QString> GLC_3DViewCollection::instanceNamesFromShadingGroup(GLuint shaderId) const
{
	QList<QString> listOfInstanceName;
	QList<GLC_uint> listOfInstanceNameId= m_ShaderGroup.keys(shaderId);
	if (!listOfInstanceNameId.isEmpty())
	{
		const int size= listOfInstanceNameId.size();
		for (int i= 0; i < size; ++i)
		{
			listOfInstanceName << m_3DViewInstanceHash.value(listOfInstanceNameId[i]).name();
		}
	}
	return listOfInstanceName;
}

int GLC_3DViewCollection::numberOfUsedShadingGroup() const
{
    const QList<GLC_uint> values(m_ShaderGroup.values());
    return QSet<GLC_uint>(values.begin(), values.end()).size();
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

void GLC_3DViewCollection::render(GLuint groupId, glc::RenderFlag renderFlag)
{
    GLC_Context* pContext= GLC_ContextManager::instance()->currentContext();
	if (!isEmpty() && m_IsViewable)
	{
		if (renderFlag == glc::WireRenderFlag)
		{
	        glEnable(GL_POLYGON_OFFSET_FILL);
	        glPolygonOffset (1.0, 1.0);
		}
		if (GLC_State::isInSelectionMode())
		{
			glDisable(GL_BLEND);
            pContext->glcEnableLighting(false);
			glDisable(GL_TEXTURE_2D);
		}
		else
		{
            pContext->glcEnableLighting(true);
		}
		glDraw(groupId, renderFlag);

		if (renderFlag == glc::WireRenderFlag)
		{
	        glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}
}
void GLC_3DViewCollection::renderShaderGroup(glc::RenderFlag renderFlag)
{
	if (!isEmpty() && m_IsViewable)
	{
		if (GLC_State::isInSelectionMode())
		{
			glDisable(GL_BLEND);
            GLC_ContextManager::instance()->currentContext()->glcEnableLighting(false);
			glDisable(GL_TEXTURE_2D);
		}

        HashList::const_iterator iEntry= m_ShadedPointerViewInstanceHash.constBegin();
	    while (iEntry != m_ShadedPointerViewInstanceHash.constEnd())
	    {
	    	glDraw(iEntry.key(), renderFlag);
	    	++iEntry;
	    }
	}
}

void GLC_3DViewCollection::glDraw(GLC_uint groupId, glc::RenderFlag renderFlag)
{
	// Set render Mode and OpenGL state
	if (!GLC_State::isInSelectionMode() && (groupId == 0))
	{
		if (renderFlag == glc::TransparentRenderFlag)
		{
	        glEnable(GL_BLEND);
	        glDepthMask(GL_FALSE);
	        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		}
		else
		{
		    glDisable(GL_BLEND);
		    glDepthMask(GL_TRUE);
		    glEnable(GL_DEPTH_TEST);
		}
	}

	// Normal GLC_3DViewInstance
	if ((groupId == 0) && !m_MainInstances.isEmpty())
	{
		glDrawInstancesOf(&m_MainInstances, renderFlag);

	}
	// Selected GLC_3DVIewInstance
	else if ((groupId == 1) && !m_SelectedInstances.isEmpty())
	{
		if (GLC_State::selectionShaderUsed()) GLC_SelectionMaterial::useShader();

		glDrawInstancesOf(&m_SelectedInstances, renderFlag);

		if (GLC_State::selectionShaderUsed()) GLC_SelectionMaterial::unUseShader();
	}
	// GLC_3DViewInstance with shader
	else if (!m_ShadedPointerViewInstanceHash.isEmpty())
	{
	    if(m_ShadedPointerViewInstanceHash.contains(groupId) && !m_ShadedPointerViewInstanceHash.value(groupId)->isEmpty())
	    {
	    	PointerViewInstanceHash* pNodeHash= m_ShadedPointerViewInstanceHash.value(groupId);

	    	GLC_Shader::use(groupId);
	    	glDrawInstancesOf(pNodeHash, renderFlag);
	    	GLC_Shader::unuse();
	    }
	}

	// Restore OpenGL state
	if (renderFlag && !GLC_State::isInSelectionMode() && (groupId == 0))
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}
