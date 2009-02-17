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

//! \file glc_collection.cpp implementation of the GLC_Collection class.


#include "glc_collection.h"
#include "glc_material.h"
#include "glc_openglexception.h"
#include "glc_selectionmaterial.h"
#include "glc_state.h"
#include "glc_shader.h"

#include <QtDebug>

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

GLC_Collection::GLC_Collection()
: m_NodeMap()
, m_pBoundingBox(NULL)
, m_SelectedNodes()
, m_OtherNodeHashList()
, m_ShaderGroup()
, m_NotTransparentNodes()
, m_IsInShowSate(true)
{
}

GLC_Collection::~GLC_Collection()
{
	// Delete all collection's elements and the collection bounding box
	clear();
}
//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Add the specified shader to the collection
bool GLC_Collection::bindShader(GLuint shaderId)
{
	if (m_OtherNodeHashList.contains(shaderId))
	{
		return false;
	}
	else
	{
		PointerNodeHash* pNodeHash= new PointerNodeHash;
		m_OtherNodeHashList.insert(shaderId, pNodeHash);
		return true;
	}
}

// Unbind the specified shader from the collection
bool GLC_Collection::unBindShader(GLuint shaderId)
{
	bool result= false;
	if (m_OtherNodeHashList.contains(shaderId))
	{
		// Find node which use the shader
		QList<GLC_uint> nodeId(m_ShaderGroup.keys(shaderId));

		// Move these node in the standard hash and remove them from shader group
		PointerNodeHash* pShaderNodeHash= m_OtherNodeHashList.take(shaderId);
		for (int i= 0; i < nodeId.size(); ++i)
		{
			const GLC_uint id= nodeId[i];
			GLC_Instance* pInstance= pShaderNodeHash->value(id);

			if (not pInstance->isSelected())
			{
				m_NotTransparentNodes.insert(id, pInstance);
			}
			else
			{
				m_SelectedNodes.insert(id, pInstance);
			}
			m_ShaderGroup.remove(id);
		}
		pShaderNodeHash->clear();
		delete pShaderNodeHash;
		result= true;
	}
	Q_ASSERT(not m_OtherNodeHashList.contains(shaderId));
	return result;
}

// Unbind All shader
bool GLC_Collection::unBindAllShader()
{
	bool result= true;
	HashList::iterator iEntry= m_OtherNodeHashList.begin();
	QList<GLuint> shaderList;
    while (iEntry != m_OtherNodeHashList.constEnd())
    {
    	shaderList.append(iEntry.key());
    	++iEntry;
    }
    const int size= shaderList.size();
    for (int i=0; i < size; ++i)
    {
    	result= result and unBindShader(shaderList[i]);
    }
    return result;
}

// Add GLC_Instance in the collection
bool GLC_Collection::add(GLC_Instance& node, GLuint shaderID)
{
	bool result= false;
	const GLC_uint key= node.id();
	if (m_NodeMap.contains(key))
	{
		qDebug() << "Instance already in collection";
		return false;
	}
	m_NodeMap.insert(key, node);
	// Create an GLC_Instance pointer of the inserted instance
	CNodeMap::iterator iNode= m_NodeMap.find(key);
	GLC_Instance* pInstance= &(iNode.value());
	// Chose the hash where instance is
	if(0 != shaderID)
	{
		// Test if shaderId group exist
		if (m_OtherNodeHashList.contains(shaderID))
		{
			m_ShaderGroup.insert(key, shaderID);

			if(pInstance->isSelected())
			{
				m_SelectedNodes.insert(key, pInstance);
			}
			else
			{
				m_OtherNodeHashList.value(shaderID)->insert(key, pInstance);
			}
			result=true;
		}
	}
	else if (not pInstance->isSelected())
	{
		m_NotTransparentNodes.insert(key, pInstance);
		result=true;
	}
	else
	{
		m_SelectedNodes.insert(key, pInstance);
		result=true;
	}

	if(result)
	{
		// Bounding box validity
		if (NULL != m_pBoundingBox)
		{
			delete m_pBoundingBox;
			m_pBoundingBox= NULL;
		}
	}
	return result;
}
// Change instance shading group
void GLC_Collection::changeShadingGroup(GLC_uint instanceId, GLuint shaderId)
{
	// Test if the specified instance exist
	Q_ASSERT(m_NodeMap.contains(instanceId));
	// Get the instance shading group
	const GLuint instanceShadingGroup= shadingGroup(instanceId);
	// Get a pointer to the instance
	GLC_Instance* pInstance= NULL;
	if (0 == instanceShadingGroup)
	{
		// The instance is not in a shading group
		if (m_NotTransparentNodes.contains(instanceId))
		{
			pInstance= m_NotTransparentNodes.take(instanceId);
		}
		else if (m_SelectedNodes.contains(instanceId))
		{
			// The instance is selected don't take it
			pInstance= m_SelectedNodes.value(instanceId);
		}
		else
		{
			Q_ASSERT(false);
		}
	}
	else
	{
		m_ShaderGroup.remove(instanceId);
		if (m_ShaderGroup.keys(instanceShadingGroup).isEmpty())
		{
			unBindShader(instanceShadingGroup);
		}
		// The instance is in a shading group
		if (m_SelectedNodes.contains(instanceId))
		{
			// The instance is selected don't take it
			pInstance= m_SelectedNodes.value(instanceId);
		}
		else
		{
			pInstance= m_OtherNodeHashList.value(instanceShadingGroup)->take(instanceId);
		}

	}
	// Put the instance in specified shading group
	if (0 != shaderId)
	{
		m_ShaderGroup.insert(instanceId, shaderId);
		if (not pInstance->isSelected())
		{
			m_OtherNodeHashList.value(shaderId)->insert(instanceId, pInstance);
		}
	}
	else if (not pInstance->isSelected())
	{
		m_NotTransparentNodes.insert(instanceId, pInstance);
	}
}

// Delete geometry from the collection
bool GLC_Collection::remove(GLC_uint Key)
{

	CNodeMap::iterator iNode= m_NodeMap.find(Key);

	if (iNode != m_NodeMap.end())
	{	// Ok, the key exist

		if (numberOfSelectedNode() > 0)
		{
			// if the geometry is selected, unselect it
			unselect(Key);
		}

		m_NotTransparentNodes.remove(Key);

		m_NodeMap.remove(Key);		// Delete the conteneur

		// Bounding box validity
		if (NULL != m_pBoundingBox)
		{
			delete m_pBoundingBox;
			m_pBoundingBox= NULL;
		}

		//qDebug("GLC_Collection::removeNode : Element succesfuly deleted");
		return true;

	}
	else
	{	// KO, key doesn't exist
		qDebug("GLC_Collection::removeNode : Element not deleted");
		return false;
	}

}

// Clear the collection
void GLC_Collection::clear(void)
{
	// Clear Selected node Hash Table
	m_SelectedNodes.clear();
	// Clear the not transparent Hash Table
	m_NotTransparentNodes.clear();
	// Clear Other Node Hash List
	HashList::iterator iEntry= m_OtherNodeHashList.begin();
    while (iEntry != m_OtherNodeHashList.constEnd())
    {
    	iEntry.value()->clear();
    	delete iEntry.value();
    	iEntry= m_OtherNodeHashList.erase(iEntry);
    }

	m_OtherNodeHashList.clear();
	m_ShaderGroup.clear();

	// Clear main Hash table
    m_NodeMap.clear();

	// delete the boundingBox
	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}
}

// Select a node
bool GLC_Collection::select(GLC_uint key)
{
	GLC_Instance* pSelectedNode;
	CNodeMap::iterator iNode= m_NodeMap.find(key);
	PointerNodeHash::iterator iSelectedNode= m_SelectedNodes.find(key);

	if ((iNode != m_NodeMap.end()) && (iSelectedNode == m_SelectedNodes.end()))
	{	// Ok, the key exist and the node is not selected
		pSelectedNode= &(iNode.value());
		m_SelectedNodes.insert(pSelectedNode->id(), pSelectedNode);

		// Remove Selected Node from is previous collection
		if (isInAShadingGroup(key))
		{
			m_OtherNodeHashList.value(shadingGroup(key))->remove(key);
			//qDebug() << "remove from shader list";
		}
		else
		{
			m_NotTransparentNodes.remove(key);
		}
		pSelectedNode->select();

		//qDebug("GLC_Collection::selectNode : Element succesfuly selected");
		return true;
	}
	else
	{	// KO, key doesn't exist or node allready selected
		qDebug("GLC_Collection::selectNode : Element not selected");
		return false;
	}
}

// Select all instances in current show state
void GLC_Collection::selectAll()
{
	unselectAll();
	CNodeMap::iterator iNode= m_NodeMap.begin();
	while (iNode != m_NodeMap.end())
	{
		GLC_Instance *pCurrentInstance= &(iNode.value());
		const GLC_uint instanceId= pCurrentInstance->id();

		if (pCurrentInstance->isVisible() == m_IsInShowSate)
		{
			pCurrentInstance->select();
			m_SelectedNodes.insert(instanceId, pCurrentInstance);
			m_NotTransparentNodes.remove(instanceId);
			if(isInAShadingGroup(instanceId))
			{
				m_OtherNodeHashList.value(shadingGroup(instanceId))->remove(instanceId);
			}
		}
		iNode++;
	}
}

// unselect a node
bool GLC_Collection::unselect(GLC_uint key)
{
	GLC_Instance* pSelectedNode;

	PointerNodeHash::iterator iSelectedNode= m_SelectedNodes.find(key);

	if (iSelectedNode != m_SelectedNodes.end())
	{	// Ok, the key exist and the node is selected
		iSelectedNode.value()->unselect();

		m_SelectedNodes.remove(key);

		pSelectedNode= iSelectedNode.value();

		// Insert Selected Node to the right collection
		if (isInAShadingGroup(key))
		{
			m_OtherNodeHashList.value(shadingGroup(key))->insert(key, pSelectedNode);
		}
		else
		{
			m_NotTransparentNodes.insert(key, pSelectedNode);
		}

		//qDebug("GLC_Collection::unselectNode : Node succesfuly unselected");
		return true;

	}
	else
	{	// KO, key doesn't exist or node allready selected
		//qDebug("GLC_Collection::unselectNode : Node not unselected");
		return false;
	}
}

// Unselect all Node
void GLC_Collection::unselectAll()
{
	PointerNodeHash::iterator iSelectedNode= m_SelectedNodes.begin();

    while (iSelectedNode != m_SelectedNodes.end())
    {
    	GLC_Instance* pInstance= iSelectedNode.value();
    	pInstance->unselect();
		if (isInAShadingGroup(pInstance->id()))
		{
			m_OtherNodeHashList.value(shadingGroup(pInstance->id()))->insert(pInstance->id(), pInstance);
		}
		else
		{
			m_NotTransparentNodes.insert(pInstance->id(), pInstance);
		}

        ++iSelectedNode;
    }
    // Clear selected node hash table
    m_SelectedNodes.clear();
}

// Set the polygon mode for all Instance
void GLC_Collection::setPolygonModeForAll(GLenum face, GLenum mode)
{
	CNodeMap::iterator iEntry= m_NodeMap.begin();

    while (iEntry != m_NodeMap.constEnd())
    {
    	// Update Instance Polygon Mode
    	iEntry.value().setPolygonMode(face, mode);
    	iEntry++;
    }

}

// Set Instance visibility
void GLC_Collection::setVisibility(const GLC_uint key, const bool visibility)
{
	CNodeMap::iterator iNode= m_NodeMap.find(key);
	if (iNode != m_NodeMap.end())
	{	// Ok, the key exist
		iNode.value().setVisibility(visibility);
		// Bounding box validity
		if (NULL != m_pBoundingBox)
		{
			delete m_pBoundingBox;
			m_pBoundingBox= NULL;
		}
	}
}

// Show all instance of the collection
void GLC_Collection::showAll()
{
	CNodeMap::iterator iEntry= m_NodeMap.begin();

    while (iEntry != m_NodeMap.constEnd())
    {
    	// Update Instance Polygon Mode
    	iEntry.value().setVisibility(true);
    	iEntry++;
    }

    // Bounding box validity
	if (NULL != m_pBoundingBox)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}

}

// Hide all instance of the collection
void GLC_Collection::hideAll()
{
	CNodeMap::iterator iEntry= m_NodeMap.begin();

    while (iEntry != m_NodeMap.constEnd())
    {
    	// Update Instance Polygon Mode
    	iEntry.value().setVisibility(false);
    	iEntry++;
    }

	// Bounding box validity
	if (NULL != m_pBoundingBox)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}

}

// Return all GLC_Instance from collection
QList<GLC_Instance*> GLC_Collection::getInstancesHandle()
{
	QList<GLC_Instance*> instancesList;

	CNodeMap::iterator iEntry= m_NodeMap.begin();

    while (iEntry != m_NodeMap.constEnd())
    {
    	instancesList.append(&(iEntry.value()));
    	iEntry++;
    }
	return instancesList;
}
// Return all visible GLC_Instance from the collection
QList<GLC_Instance*> GLC_Collection::getVisibleInstanceHandle()
{
	QList<GLC_Instance*> instancesList;

	CNodeMap::iterator iEntry= m_NodeMap.begin();

    while (iEntry != m_NodeMap.constEnd())
    {
    	if (iEntry.value().isVisible())
    	{
        	instancesList.append(&(iEntry.value()));
    	}
    	iEntry++;
    }
	return instancesList;

}

// Return list of invisible instance name
QList<QString> GLC_Collection::getInvisibleInstanceName() const
{
	QList<QString> listOfInstanceName;

	CNodeMap::const_iterator iEntry= m_NodeMap.constBegin();

    while (iEntry != m_NodeMap.constEnd())
    {
    	if (not iEntry.value().isVisible())
    	{
    		listOfInstanceName.append(iEntry.value().name());
    	}
    	iEntry++;
    }

    return listOfInstanceName;
}


// Return a GLC_Instance pointer from the collection
GLC_Instance* GLC_Collection::getInstanceHandle(GLC_uint Key)
{
	return &(m_NodeMap[Key]);
}

// return the entire collection Bounding Box
GLC_BoundingBox GLC_Collection::boundingBox(void)
{
	// If the bounding box is not valid delete it
	setBoundingBoxValidity();

	// Check if the bounding box have to be updated
	if ((m_pBoundingBox == NULL) and not m_NodeMap.isEmpty())
	{
		// There is objects in the collection and the collection or bounding box is not valid
		if (m_pBoundingBox != NULL)
		{
			delete m_pBoundingBox;
		}
		m_pBoundingBox= new GLC_BoundingBox();

		CNodeMap::iterator iEntry= m_NodeMap.begin();
	    while (iEntry != m_NodeMap.constEnd())
	    {
	        if(iEntry.value().isVisible() == m_IsInShowSate)
	        {
	        	// Combine Collection BoundingBox with element Bounding Box
	        	m_pBoundingBox->combine(iEntry.value().getBoundingBox());
	        }
	        ++iEntry;
	    }
	}
	else if (NULL == m_pBoundingBox)
	{
		// The collection is empty and m_pBoundingBox == NULL
		delete m_pBoundingBox;
		m_pBoundingBox= new GLC_BoundingBox();
	}

	return *m_pBoundingBox;
}

// Return the number of drawable objects
int GLC_Collection::numberOfDrawableObjects() const
{
	// The number of object to drw
	int numberOffDrawnHit= 0;

	// Count the number off instance to draw
	CNodeMap::const_iterator i= m_NodeMap.begin();
	while (i != m_NodeMap.constEnd())
	{
		//qDebug() << "transparent";
		if (i.value().isVisible() == m_IsInShowSate)
		{
			++numberOffDrawnHit;
		}
		++i;
	}
	return numberOffDrawnHit;
}

// Return instances name from the specified shading group
QList<QString> GLC_Collection::getInstanceNameFromShadingGroup(GLuint shaderId) const
{
	QList<QString> listOfInstanceName;
	QList<GLC_uint> listOfInstanceNameId= m_ShaderGroup.keys(shaderId);
	if (not listOfInstanceNameId.isEmpty())
	{
		const int size= listOfInstanceNameId.size();
		for (int i= 0; i < size; ++i)
		{
			listOfInstanceName << m_NodeMap.value(listOfInstanceNameId[i]).name();
		}
	}
	return listOfInstanceName;
}

// Return the number of used shading group
int GLC_Collection::numberOfUsedShadingGroup() const
{
	return m_ShaderGroup.values().toSet().size();
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

void GLC_Collection::glExecute(GLuint groupId)
{
	//qDebug() << "GLC_Collection::glExecute";
	if (getNumber() > 0)
	{

		glDraw(groupId);

		// OpenGL error handler
		GLenum errCode;
		if ((errCode= glGetError()) != GL_NO_ERROR)
		{
			const GLubyte* errString;
			errString = gluErrorString(errCode);
			qDebug("GLC_Collection::GlExecute OPENGL ERROR %s", errString);
		}
	}
}
// Display all shader group
void GLC_Collection::glExecuteShaderGroup()
{
	//qDebug() << "GLC_Collection::glExecuteShaderGroup";
	if (getNumber() > 0)
	{
		HashList::iterator iEntry= m_OtherNodeHashList.begin();
	    while (iEntry != m_OtherNodeHashList.constEnd())
	    {
	    	glDraw(iEntry.key());
	    	++iEntry;
	    }

		// OpenGL error handler
		GLenum errCode;
		if ((errCode= glGetError()) != GL_NO_ERROR)
		{
			const GLubyte* errString;
			errString = gluErrorString(errCode);
			qDebug("GLC_Collection::GlExecute OPENGL ERROR %s", errString);
		}
	}

}

// Display the specified collection group
void GLC_Collection::glDraw(GLuint groupId)
{
	if (groupId == 0)
	{
		// Display non transparent instance
	    glDisable(GL_BLEND);
	    //glDisable(GL_CULL_FACE);
	    glEnable(GL_DEPTH_TEST);

	    PointerNodeHash::iterator iEntry;
	    QList<GLC_Instance*> transparentInstances;
	    if (not m_NotTransparentNodes.empty())
	    {
	    	iEntry= m_NotTransparentNodes.begin();
	        while (iEntry != m_NotTransparentNodes.constEnd())
	        {
	        	//qDebug() << "no transparent";
	        	if ((iEntry.value()->isVisible() == m_IsInShowSate))
	        	{
		            if (not iEntry.value()->getGeometry()->isTransparent())
		            {
		            	iEntry.value()->glExecute();
		            }

		            if (iEntry.value()->getGeometry()->haveTransparentMaterials())
		            {
		            	transparentInstances.append(iEntry.value());
		            }

	        	}
	            ++iEntry;
	        }
	    }

	    if(not transparentInstances.isEmpty())
	    {
	        // Set attributes for blending activation
	        //glEnable(GL_CULL_FACE);
	        glEnable(GL_BLEND);
	        glDepthMask(GL_FALSE);
	        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	        //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	    	// Display transparent instance
	        const int size= transparentInstances.size();
	        for (int i= 0; i < size; ++i)
	        {
	        	//qDebug() << "transparent";
	            transparentInstances[i]->glExecute(true);
	        }
	        // Restore attributtes
	        glDepthMask(GL_TRUE);
	        glDisable(GL_BLEND);
	    }

	}
	else if (groupId == 1)
	{
	    if(not m_SelectedNodes.isEmpty())
	    {
			if (GLC_State::selectionShaderUsed())
			{
				GLC_SelectionMaterial::useShader();
			}
			PointerNodeHash::iterator iEntry= m_SelectedNodes.begin();
	        while (iEntry != m_SelectedNodes.constEnd())
	        {
	            if (iEntry.value()->isVisible() == m_IsInShowSate)
	            {
	            	iEntry.value()->glExecute();
	            }
	            ++iEntry;
	        }
			if (GLC_State::selectionShaderUsed())
			{
				GLC_SelectionMaterial::unUseShader();
			}
	    }

	}
	else
	{
	    if(m_OtherNodeHashList.contains(groupId) and not m_OtherNodeHashList.value(groupId)->isEmpty())
	    {
	    	QList<GLC_Instance*> transparentInstances;
	    	GLC_Shader::use(groupId);
	    	PointerNodeHash* pNodeHash= m_OtherNodeHashList.value(groupId);

			PointerNodeHash::iterator iEntry= pNodeHash->begin();
	        while (iEntry != pNodeHash->constEnd())
	        {
	        	//qDebug() << "transparent";
	            if (iEntry.value()->isVisible() == m_IsInShowSate)
	            {
		            if (not iEntry.value()->getGeometry()->isTransparent())
		            {
		            	iEntry.value()->glExecute();
		            }

		            if (iEntry.value()->getGeometry()->haveTransparentMaterials())
		            {
		            	transparentInstances.append(iEntry.value());
		            }

	            }
	            ++iEntry;
	        }
		    if(not transparentInstances.isEmpty())
		    {
		        // Set attributes for blending activation
		        //glEnable(GL_CULL_FACE);
		        glEnable(GL_BLEND);
		        glDepthMask(GL_FALSE);
		        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		        //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		    	// Display transparent instance
		        const int size= transparentInstances.size();
		        for (int i= 0; i < size; ++i)
		        {
		        	//qDebug() << "transparent";
		            transparentInstances[i]->glExecute(true);
		        }
		        // Restore attributtes
		        glDepthMask(GL_TRUE);
		        glDisable(GL_BLEND);
		    }

			GLC_Shader::unuse();
	    }
	}
	// OpenGL error handler
	GLenum errCode;
	if ((errCode= glGetError()) != GL_NO_ERROR)
	{
		const GLubyte* errString;
		errString = gluErrorString(errCode);
		qDebug("GLC_Collection::GlDraw OPENGL ERROR %s", errString);
	}

}

// Set the Bounding box validity
void GLC_Collection::setBoundingBoxValidity(void)
{
	if (NULL != m_pBoundingBox)
	{
		if (not m_NodeMap.isEmpty())
		{
			// Check instance bounding box validity
			CNodeMap::iterator iEntry= m_NodeMap.begin();
		    while (iEntry != m_NodeMap.constEnd())
		    {
		    	if (not iEntry.value().getBoundingBoxValidity())
		    	{
					delete m_pBoundingBox;
					m_pBoundingBox= NULL;
					return;
		    	}
		    	iEntry++;
		    }
		}
		else if (not m_pBoundingBox->isEmpty())
		{
			delete m_pBoundingBox;
			m_pBoundingBox= NULL;
		}
	}
}
