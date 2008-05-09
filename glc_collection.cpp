/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.9, packaged on May, 2008.

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

#include <QtDebug>

#include "glc_collection.h"
#include "glc_material.h"
#include "glc_openglexception.h"

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

GLC_Collection::GLC_Collection()
: m_ListID(0)
, m_ListIsValid(false)
, m_pBoundingBox(NULL)
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
	m_NodeMap.insert(node.getID(), node);
	m_ListIsValid= false;
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
		
		m_NodeMap.remove(Key);		// Delete the conteneur
		// Search the list
			
		// List validity
		m_ListIsValid= false;
		
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
	// Clear main Hash table
    m_NodeMap.clear();

	// Delete display list
	if (0 != m_ListID)
	{
		glDeleteLists(m_ListID, 1);
		//qDebug() << "GLC_Collection::deleteList : Display list " << m_ListID << " Deleted";
		m_ListID= 0;			
	}
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
	SelectedNodeHash::iterator iSelectedNode= m_SelectedNodes.find(key);
		
	if ((iNode != m_NodeMap.end()) && (iSelectedNode == m_SelectedNodes.end()))
	{	// Ok, the key exist and the node is not selected
		pSelectedNode= &(iNode.value());
		m_SelectedNodes.insert(pSelectedNode->getID(), pSelectedNode);
				
		pSelectedNode->select();		
		m_ListIsValid= false;
		
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
	SelectedNodeHash::iterator iSelectedNode= m_SelectedNodes.find(key);
		
	if (iSelectedNode != m_SelectedNodes.end())
	{	// Ok, the key exist and the node is selected
		iSelectedNode.value()->unselect();
		
		m_SelectedNodes.remove(key);
		
		m_ListIsValid= false;
		
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
	SelectedNodeHash::iterator iSelectedNode= m_SelectedNodes.begin();
	
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
		m_ListIsValid= false;
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
    m_ListIsValid= false;
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
    m_ListIsValid= false;
}

// Return a GLC_Instance pointer from the collection
GLC_Instance* GLC_Collection::getInstanceHandle(GLC_uint Key)
{
	return &(m_NodeMap[Key]);
}

//! return the collection Bounding Box
GLC_BoundingBox GLC_Collection::getBoundingBox(void)
{
	if (((m_pBoundingBox == NULL) || !m_ListIsValid) && (getNumber() > 0))
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
	        if(iEntry.value().isVisible())
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
	else if ((m_pBoundingBox == NULL) || !m_ListIsValid)
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
		createMemberLists();		// Si nécessaire

		//CreateSubLists();		// Si nécessaire

		if (m_ListIsValid)
		{	// La liste de la collection OK
			glCallList(m_ListID);
		}
		else
		{
			createList();
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
	CNodeMap::iterator iEntry= m_NodeMap.begin();
    while (iEntry != m_NodeMap.constEnd())
    {
        if (iEntry.value().isVisible())
        {
        	iEntry.value().glExecute();
        }        
        ++iEntry;
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

// Création des listes d'affichages des membres
void GLC_Collection::createMemberLists(void)
{
	CNodeMap::iterator iEntry= m_NodeMap.begin();
	//qDebug("GLC_Collection::CreateMemberList ENTER");
	
    while (iEntry != m_NodeMap.constEnd())
    {
    	// Update Instance validity
    	iEntry.value().setInstanceValidity();
    	iEntry++;
    }
    
	iEntry= m_NodeMap.begin();
    while (iEntry != m_NodeMap.constEnd())
    {
    	if(!iEntry.value().getValidity())
    	{
    		iEntry.value().glExecute(GL_COMPILE);
			if (m_pBoundingBox != NULL)
			{
				delete m_pBoundingBox;
				m_pBoundingBox= NULL;
			}				
    	}
    	// Passe au Suivant
    	iEntry++;
    }

	// Gestion erreur OpenGL
	if (glGetError() != GL_NO_ERROR)
	{
		qDebug("GLC_Collection::CreateMemberList OPENGL ERROR");
	}


}
//////////////////////////////////////////////////////////////////////
// Fonctions de services privées
//////////////////////////////////////////////////////////////////////

// Création de la liste d'affichage de la collection
bool GLC_Collection::createList(void)
{
	//qDebug("GLC_Collection::CreateList");
	
	if(0 == m_ListID)		// list doesn't exist
	{
		m_ListID= glGenLists(1);
	}

	// Création de la liste
	glNewList(m_ListID, GL_COMPILE_AND_EXECUTE);				
		glDraw();
	glEndList();
	
	m_ListIsValid= true;

	//qDebug("GLC_Collection::createList : Liste d'affichage %u créé", m_ListID);	

	// OpenGl error handling
	GLenum error= glGetError();
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Geometry::GlExecute ", error);
		throw(OpenGlException);
	}

	return true;
}
