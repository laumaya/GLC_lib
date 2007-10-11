/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.7, packaged on September, 2007.

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
, m_pNumberOfInstance(new int(1))
, m_MatPos()
, m_NodeIsValid(false)
{
	
}

// Contruct node with a geometry
GLC_CollectionNode::GLC_CollectionNode(GLC_Geometry* pGeom)
: m_pGeom(pGeom)
, m_ListID(0)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(new int(1))
, m_MatPos()
, m_NodeIsValid(false)
{
	
}

// Copy constructor
GLC_CollectionNode::GLC_CollectionNode(const GLC_CollectionNode& inputNode)
: m_pGeom(inputNode.m_pGeom)
, m_ListID(0)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(inputNode.m_pNumberOfInstance)
, m_MatPos(inputNode.m_MatPos)
, m_NodeIsValid(false)

{
	// Increment the number of instance
	++(*m_pNumberOfInstance);
}

// Assignement operator
GLC_CollectionNode& GLC_CollectionNode::operator=(const GLC_CollectionNode& inputNode)
{	
	if (m_pGeom != inputNode.m_pGeom)
	{
		clear();
		m_pGeom= inputNode.m_pGeom;
		m_pNumberOfInstance= inputNode.m_pNumberOfInstance;
		++(*m_pNumberOfInstance);
	}
	return *this;
}

// Destructor
GLC_CollectionNode::~GLC_CollectionNode()
{
	clear();
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
		return m_pGeom->getValidity();
	}
	else return false;
}

// Get the bounding box
GLC_BoundingBox GLC_CollectionNode::getBoundingBox(void)
{
	if (getBoundingBoxValidity())
	{
		return *m_pBoundingBox;
	}
	else if (m_pGeom != NULL)
	{
		computeBoundingBox();
		if (m_pBoundingBox != NULL)
		{
			return *m_pBoundingBox;
		}
		
	}
	
	GLC_BoundingBox nullBoundingBox;
	return nullBoundingBox;
}

// Get the validity of the Bounding Box
bool GLC_CollectionNode::getBoundingBoxValidity(void) const
{
	if ((m_pGeom != NULL) && (m_pBoundingBox != NULL))
	{
		return (m_pGeom->getListIsValid()) && (m_pGeom->getValidity());
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set the node Geometry
bool GLC_CollectionNode::setGeometry(GLC_Geometry* pGeom)
{
		if (NULL == m_pGeom) return false;
		else
		{
			m_pGeom= pGeom;
			return true;
		}
}

// Geometry translation
void GLC_CollectionNode::translate(double Tx, double Ty, double Tz)
{
	GLC_Matrix4x4 MatTrans(Tx, Ty, Tz);
	
	multMatrix(MatTrans);
}


// move Geometry with a 4x4Matrix
void GLC_CollectionNode::multMatrix(const GLC_Matrix4x4 &MultMat)
{
	m_MatPos= MultMat * m_MatPos;

	m_NodeIsValid= false;
}

// Replace the Geometry Matrix
void GLC_CollectionNode::setMatrix(const GLC_Matrix4x4 &SetMat)
{
	m_MatPos= SetMat;

	m_NodeIsValid= false;
}

// Reset the Geometry Matrix
void GLC_CollectionNode::resetMatrix(void)
{
	m_MatPos.setToIdentity();

	m_NodeIsValid= false;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Display the Node
void GLC_CollectionNode::glExecute(GLenum Mode)
{
	bool computeBox= false;
	
	// Geometry OpenGl list invalid
	if (!m_pGeom->getListIsValid())
	{
		m_pGeom->glLoadTexture();
		m_pGeom->createList(Mode);
		computeBox= true;
	}
	
	// Geometry invalid or collection node list ID == 0
	if ((!m_pGeom->getValidity()) || (!m_NodeIsValid))
	{
		qDebug() << "GLC_CollectionNode::GlExecute: geometry validity : " << m_pGeom->getValidity();
		qDebug() << "GLC_CollectionNode::GlExecute: list ID : " << m_ListID;
		
		if (m_ListID == 0)
		{
			qDebug() << "GLC_CollectionNode::GlExecute: List not found";
			m_ListID= glGenLists(1);
		}		
		glNewList(m_ListID, Mode);
			// Save current OpenGL Matrix
			glPushMatrix();
			
			// Change the current matrix
			glMultMatrixd(m_MatPos.return_dMat());
			m_pGeom->glExecute(Mode);
			
			glPopMatrix();
		glEndList();
		qDebug() << "GLC_CollectionNode::GlExecute : Display list " << m_ListID << " created";
		m_NodeIsValid= true;
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


// compute the node bounding box
// m_pGeom should be not null
void GLC_CollectionNode::computeBoundingBox(void)
{
	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}
	
	m_pBoundingBox= m_pGeom->getBoundingBox();
	
}

// Clear current node
void GLC_CollectionNode::clear()
{
	Q_ASSERT(m_pNumberOfInstance != NULL);
	
	if ((--(*m_pNumberOfInstance)) == 0)
	{
		// this is the last instance, delete the geometry
		if (m_pGeom != NULL)
		{
			delete m_pGeom;
			m_pGeom= NULL;
		}
		// delete instance counter
		delete m_pNumberOfInstance;
		m_pNumberOfInstance= NULL;
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
	
	m_NodeIsValid= false;

}



