/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.5, packaged on February, 2006.

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

//! \file glc_collectionnode.cpp implementation of the GLC_CollectionNode class.

#include "glc_collectionnode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Default constructor
GLC_CollectionNode::GLC_CollectionNode()
: m_pGeom(NULL)
, m_ListID(0)
, m_pBoundingBox(NULL)
{
	
}

// Contruct node with a geometry
GLC_CollectionNode::GLC_CollectionNode(GLC_Geometry* pGeom)
: m_pGeom(pGeom)
, m_ListID(0)
, m_pBoundingBox(NULL)
{
	
}

// Destructor
GLC_CollectionNode::~GLC_CollectionNode()
{
	erase();	
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get the geometry of the node
GLC_Geometry* GLC_CollectionNode::getGeometry(void)
{
	return m_pGeom;
}

// Get the validity of the OpenGL list
bool GLC_CollectionNode::getListValidity(void) const
{
	if ((m_pGeom != NULL) && (m_ListID != 0))
	{
		return m_pGeom->GetValidity();
	}
	else return false;
}

// Get the bounding box
GLC_BoundingBox GLC_CollectionNode::getBoundingBox(void) const
{
	if (m_pBoundingBox != NULL)
	{
		return *m_pBoundingBox;
	}
	else
	{
		GLC_BoundingBox nullBoundingBox;
		return nullBoundingBox;
	}
}

// Get the validity of the Bounding Box
bool GLC_CollectionNode::getBoundingBoxValidity(void) const
{
	if ((m_pGeom != NULL) && (m_pBoundingBox != NULL))
	{
		return (m_pGeom->GetListIsValid()) && (m_pGeom->GetValidity());
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set the node Geometry
void GLC_CollectionNode::setGeometry(GLC_Geometry* pGeom)
{
		erase();
		m_pGeom= pGeom;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Display the Node
void GLC_CollectionNode::GlExecute(GLenum Mode)
{
	bool computeBox= false;
	
	if (!m_pGeom->GetListIsValid())
	{
		m_pGeom->CreateList(GL_COMPILE);
		computeBox= true;
	}
	
	if ((!m_pGeom->GetValidity()) || (m_ListID == 0))
	{
		if (m_ListID == 0)
		{
			qDebug() << "GLC_CollectionNode::GlExecute: List not found";
			m_ListID= glGenLists(1);
		}
		glNewList(m_ListID, Mode);
			m_pGeom->GlExecute(Mode);
		glEndList();
		qDebug() << "GLC_Collection::CreateSubLists : Display list " << m_ListID << " created";
		computeBox= true;	
	}
	else
	{
		glCallList(m_ListID);
	}
	
	if (computeBox)
	{
		computeBoundingBox();
	}
	
}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////

// Erase the Node
void GLC_CollectionNode::erase(void)
{
	if (m_pGeom != NULL)
	{
		delete m_pGeom;
		m_pGeom= NULL;
	}
	
	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}
	
	if (m_ListID != 0)
	{
		glDeleteLists(m_ListID, 1);
		m_ListID= 0;
	}
	
}

// compute the node bounding box
void GLC_CollectionNode::computeBoundingBox(void)
{
	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}
	
	m_pBoundingBox= m_pGeom->getBoundingBox();
	
}



