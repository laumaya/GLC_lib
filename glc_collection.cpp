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

#include <QtDebug>

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

GLC_Collection::GLC_Collection()
: m_NodeMap()
, m_CollectionIsValid(false)
, m_pBoundingBox(NULL)
, m_SelectedNodes()
, m_NotTransparentNodes()
, m_TransparentNodes()
, m_IsInShowSate(true)
, m_CheckInstanceTransparency(true)
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
// Add GLC_Instance in the collection
bool GLC_Collection::add(GLC_Instance& node)
{
	const GLC_uint key= node.getID();
	m_NodeMap.insert(key, node);
	// Create an GLC_Instance pointer of the inserted instance
	CNodeMap::iterator iNode= m_NodeMap.find(key);
	GLC_Instance* pInstance= &(iNode.value());
	// Chose the hash where instance is
	if (!node.getGeometry()->isTransparent())
	{
		m_NotTransparentNodes.insert(key, pInstance);
	}
	else
	{
		m_TransparentNodes.insert(key, pInstance);
	}
	
	m_CollectionIsValid= false;
	return true;	
}

// Delete geometry from the collection
bool GLC_Collection::remove(GLC_uint Key)
{

	CNodeMap::iterator iNode= m_NodeMap.find(Key);
		
	if (iNode != m_NodeMap.end())
	{	// Ok, the key exist
		
		if (getNumberOfSelectedNode() > 0)
		{
			// if the geometry is selected, unselect it
			unselect(Key);
		}
		
		// test if the node is transparent
		if(!iNode.value().getGeometry()->isTransparent())
		{
			m_NotTransparentNodes.remove(Key);
		}
		else
		{
			m_TransparentNodes.remove(Key);
		}
		m_NodeMap.remove(Key);		// Delete the conteneur
			
		// Collection validity
		m_CollectionIsValid= false;
		
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
	// Clear the not tranparent Hash Table
	m_NotTransparentNodes.clear();
	// Clear the transparent Hash Table
	m_TransparentNodes.clear();
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
		m_SelectedNodes.insert(pSelectedNode->getID(), pSelectedNode);
				
		pSelectedNode->select();		
		m_CollectionIsValid= false;
		
		//qDebug("GLC_Collection::selectNode : Element succesfuly selected");
		return true;
		
	}
	else
	{	// KO, key doesn't exist or node allready selected
		//qDebug("GLC_Collection::selectNode : Element not selected");
		return false;
	}
	
}

// unselect a node
bool GLC_Collection::unselect(GLC_uint key)
{
	PointerNodeHash::iterator iSelectedNode= m_SelectedNodes.find(key);
		
	if (iSelectedNode != m_SelectedNodes.end())
	{	// Ok, the key exist and the node is selected
		iSelectedNode.value()->unselect();
		
		m_SelectedNodes.remove(key);
		
		m_CollectionIsValid= false;
		
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
		iSelectedNode.value()->unselect();    	
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
		m_CollectionIsValid= false;
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
    m_CollectionIsValid= false;
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
    m_CollectionIsValid= false;
}
// Update instance transparency
void GLC_Collection::updateInstancesTransparency()
{
	// Check instance visibility
	PointerNodeHash::iterator iEntry= m_NotTransparentNodes.begin();
    while (iEntry != m_NotTransparentNodes.constEnd())
    {
        if (iEntry.value()->getGeometry()->isTransparent())
        {
        	GLC_Instance* pInstance= iEntry.value();
        	m_TransparentNodes.insert(pInstance->getID(), pInstance);
        	iEntry= m_NotTransparentNodes.erase(iEntry);
        }
        else ++iEntry;
    }
    
	iEntry= m_TransparentNodes.begin();
    while (iEntry != m_TransparentNodes.constEnd())
    {
        if (not iEntry.value()->getGeometry()->isTransparent())
        {
        	GLC_Instance* pInstance= iEntry.value();
        	m_NotTransparentNodes.insert(pInstance->getID(), pInstance);
        	iEntry= m_TransparentNodes.erase(iEntry);
        }
        else ++iEntry;
    }
	// End of checking instance visibility
	
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

// Return a GLC_Instance pointer from the collection
GLC_Instance* GLC_Collection::getInstanceHandle(GLC_uint Key)
{
	return &(m_NodeMap[Key]);
}

//! return the collection Bounding Box
GLC_BoundingBox GLC_Collection::getBoundingBox(void)
{
	setBoundingBoxValidity();
	
	if (((m_pBoundingBox == NULL) || !m_CollectionIsValid) && (getNumber() > 0))
	{
		CNodeMap::iterator iEntry= m_NodeMap.begin();
		if (m_pBoundingBox != NULL)
		{
			delete m_pBoundingBox;
			m_pBoundingBox= NULL;
		}
		m_pBoundingBox= new GLC_BoundingBox();
		
	    while (iEntry != m_NodeMap.constEnd())
	    {
	        if(iEntry.value().isVisible() == m_IsInShowSate)
	        {
	        	// Combine Collection BoundingBox with element Bounding Box
	        	m_pBoundingBox->combine(iEntry.value().getBoundingBox());
	        }
	        ++iEntry;
	    }
	    if (m_pBoundingBox->isEmpty())
	    {
	    	delete m_pBoundingBox;
			GLC_Vector4d lower(-0.5, -0.5, -0.5);
			GLC_Vector4d upper(0.5, 0.5, 0.5);
			m_pBoundingBox= new GLC_BoundingBox(lower, upper);
	    }
				
	}
	else if ((m_pBoundingBox == NULL) || !m_CollectionIsValid)
	{
		if (m_pBoundingBox != NULL)
		{
			delete m_pBoundingBox;
			m_pBoundingBox= NULL;
		}		
		GLC_Vector4d lower(-0.5, -0.5, -0.5);
		GLC_Vector4d upper(0.5, 0.5, 0.5);
		m_pBoundingBox= new GLC_BoundingBox(lower, upper);		
	}
	
	return *m_pBoundingBox;
}


//////////////////////////////////////////////////////////////////////
// Fonctions OpenGL
//////////////////////////////////////////////////////////////////////
void GLC_Collection::glExecute(void)
{
	//qDebug() << "GLC_Collection::glExecute";
	if (getNumber() > 0)
	{
		if (m_CheckInstanceTransparency)
		{
			updateInstancesTransparency();
		}
		if (m_CollectionIsValid)
		{	// La collection OK
			glDraw();
		}
		else
		{
			glDraw();
			m_CollectionIsValid= true;
			// delete the boundingBox
			if (m_pBoundingBox != NULL)
			{
				delete m_pBoundingBox;
				m_pBoundingBox= NULL;
			}				
		}

		// Gestion erreur OpenGL
		GLenum errCode;
		if ((errCode= glGetError()) != GL_NO_ERROR)
		{
			const GLubyte* errString;
			errString = gluErrorString(errCode);
			qDebug("GLC_Collection::GlExecute OPENGL ERROR %s", errString);
		}
	}
}

// Affiche les éléments de la collection
void GLC_Collection::glDraw(void)
{
	// Display non transparent instance
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

	PointerNodeHash::iterator iEntry= m_NotTransparentNodes.begin();
    while (iEntry != m_NotTransparentNodes.constEnd())
    {
    	//qDebug() << "no transparent";
        if (iEntry.value()->isVisible() == m_IsInShowSate)
        {
        	iEntry.value()->glExecute();
        }        
        ++iEntry;
    }
    
    // Set attributes for blending activation
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Display transparent instance
	iEntry= m_TransparentNodes.begin();
    while (iEntry != m_TransparentNodes.constEnd())
    {
    	//qDebug() << "transparent";
        if (iEntry.value()->isVisible() == m_IsInShowSate)
        {
        	iEntry.value()->glExecute();
        }        
        ++iEntry;
    }
    // Restore attributtes
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    
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
	if (m_pBoundingBox == NULL)
	{
		return;
	}
	CNodeMap::iterator iEntry= m_NodeMap.begin();
	
    while (iEntry != m_NodeMap.constEnd())
    {
    	// Update Instance validity
    	iEntry.value().setInstanceValidity();
    	iEntry++;
    }
	iEntry= m_NodeMap.begin();
    while (iEntry != m_NodeMap.constEnd() and (m_pBoundingBox != NULL))
    {
    	if(!iEntry.value().getValidity())
    	{
 			if (m_pBoundingBox != NULL)
			{
				delete m_pBoundingBox;
				m_pBoundingBox= NULL;
			}				
    	}
    	// Passe au Suivant
    	iEntry++;
    }
}
