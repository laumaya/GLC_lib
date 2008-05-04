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

//! \file glc_instance.cpp implementation of the GLC_Instance class.

#include "glc_instance.h"
#include "glc_selectionmaterial.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Default constructor
GLC_Instance::GLC_Instance()
: m_pGeom(NULL)
, m_ListID(0)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(new int(1))
, m_MatPos()
, m_IsValid(false)
, m_IsSelected(false)
, m_PolyFace(GL_FRONT_AND_BACK)
, m_PolyMode(GL_FILL)

{
	//qDebug() << "GLC_Instance::GLC_Instance null instance ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
}

// Contruct instance with a geometry
GLC_Instance::GLC_Instance(GLC_Geometry* pGeom)
: m_pGeom(pGeom)
, m_ListID(0)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(new int(1))
, m_MatPos()
, m_IsValid(false)
, m_IsSelected(false)
, m_PolyFace(GL_FRONT_AND_BACK)
, m_PolyMode(GL_FILL)

{
	setName(pGeom->getName());
	//qDebug() << "GLC_Instance::GLC_Instance ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
}

// Copy constructor
GLC_Instance::GLC_Instance(const GLC_Instance& inputNode)
: GLC_Object(inputNode)
, m_pGeom(inputNode.m_pGeom)
, m_ListID(0)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(inputNode.m_pNumberOfInstance)
, m_MatPos(inputNode.m_MatPos)
, m_IsValid(false)
, m_IsSelected(false)
, m_PolyFace(inputNode.m_PolyFace)
, m_PolyMode(inputNode.m_PolyMode)


{
	// Increment the number of instance
	++(*m_pNumberOfInstance);
	//qDebug() << "GLC_Instance::GLC_Instance CopyConstructor ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
}


// Assignement operator
GLC_Instance& GLC_Instance::operator=(const GLC_Instance& inputNode)
{	
	
	clear();
	m_pGeom= inputNode.m_pGeom;
	m_Uid= inputNode.m_Uid;
	m_Name= inputNode.m_Name;
	
	m_pNumberOfInstance= inputNode.m_pNumberOfInstance;
	++(*m_pNumberOfInstance);
	//qDebug() << "GLC_Instance::operator= :ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
	m_MatPos= inputNode.m_MatPos;
	return *this;
}

// Destructor
GLC_Instance::~GLC_Instance()
{
	clear();
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get the geometry of the instance
GLC_Geometry* GLC_Instance::getGeometry(void)
{
	return m_pGeom;
}


// Get the validity of the OpenGL list
const bool GLC_Instance::getValidity(void) const
{
	if ((m_pGeom != NULL) && (m_IsValid))
	{
		return m_pGeom->getValidity();
	}
	else return false;
}

// Get the bounding box
GLC_BoundingBox GLC_Instance::getBoundingBox(void)
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
const bool GLC_Instance::getBoundingBoxValidity(void) const
{
	if ((m_pGeom != NULL) && (m_pBoundingBox != NULL))
	{
		return (m_pGeom->getListIsValid()) && (m_pGeom->getValidity());
	}
	else return false;
}

// Clone the instance
GLC_Instance GLC_Instance::clone() const
{
	GLC_Instance cloneInstance;
	cloneInstance.m_pGeom= m_pGeom->clone();
	if (NULL != m_pBoundingBox)
	{
		cloneInstance.m_pBoundingBox= new GLC_BoundingBox(*m_pBoundingBox);
	}
	cloneInstance.m_MatPos= m_MatPos;
	cloneInstance.m_PolyFace= m_PolyFace;
	cloneInstance.m_PolyMode= m_PolyMode;
	return cloneInstance;
}

// Instanciate the instance
GLC_Instance GLC_Instance::instanciate()
{
	GLC_Instance instance(*this);
	instance.m_Uid= GLC_GenID();
	return instance;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////


// Set the instance Geometry
bool GLC_Instance::setGeometry(GLC_Geometry* pGeom)
{
		if (NULL == m_pGeom) return false;
		else
		{
			m_pGeom= pGeom;
			return true;
		}
}

// Geometry translation
void GLC_Instance::translate(double Tx, double Ty, double Tz)
{
	GLC_Matrix4x4 MatTrans(Tx, Ty, Tz);
	
	multMatrix(MatTrans);
}


// move instance with a 4x4Matrix
void GLC_Instance::multMatrix(const GLC_Matrix4x4 &MultMat)
{
	m_MatPos= MultMat * m_MatPos;

	m_IsValid= false;
}

// Replace the instance Matrix
void GLC_Instance::setMatrix(const GLC_Matrix4x4 &SetMat)
{
	m_MatPos= SetMat;

	m_IsValid= false;
}

// Reset the instance Matrix
void GLC_Instance::resetMatrix(void)
{
	m_MatPos.setToIdentity();

	m_IsValid= false;
}

// Polygon's display style
void GLC_Instance::setPolygonMode(GLenum Face, GLenum Mode)
{
	// Change the polygon mode only if there is a change
	if ((m_PolyFace != Face) || (m_PolyMode != Mode))
	{
		m_PolyFace= Face;
		m_PolyMode= Mode;
		m_IsValid = false;
	}
}

// Select the instance
void GLC_Instance::select(void)
{
	m_IsSelected= true;
	m_IsValid= false;
}

// Unselect the instance
void GLC_Instance::unselect(void)
{
	m_IsSelected= false;
	m_IsValid= false;
}

// set Instance validity in case of multiple instance.
void GLC_Instance::setInstanceValidity(void)
{
	if ((m_pGeom != NULL) && (m_IsValid))
	{
		m_IsValid= m_pGeom->getValidity();
	}
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Display the instance
void GLC_Instance::glExecute(GLenum Mode)
{
	bool computeBox= false;
	if (NULL == m_pGeom) return;
	// Geometry OpenGl list invalid
	if (!m_pGeom->getListIsValid())
	{
		m_pGeom->glLoadTexture();
		m_pGeom->createList(GL_COMPILE);
		computeBox= true;
	}
	
	// Geometry invalid or instance list ID == 0
	if ((!m_pGeom->getValidity()) || (!m_IsValid))
	{
		//qDebug() << "GLC_Instance::GlExecute: geometry validity : " << m_pGeom->getValidity();
		//qDebug() << "GLC_Instance::GlExecute: list ID : " << m_ListID;
		
		if (m_ListID == 0)
		{
			//qDebug() << "GLC_Instance::GlExecute: List not found";
			m_ListID= glGenLists(1);
		}		
		glNewList(m_ListID, Mode);
			// Object ID for selection purpose
			glLoadName(getID());
			// Save current OpenGL Matrix
			glPushMatrix();
			glVisProperties();
			m_pGeom->glExecute(Mode, m_IsSelected, ((m_PolyMode != GL_FILL)));
			// Restore OpenGL Matrix
			glPopMatrix();
		glEndList();
		//qDebug() << "GLC_Instance::GlExecute : Display list " << m_ListID << " created";
		m_IsValid= true;
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

// Set instance visualisation properties
void GLC_Instance::glVisProperties()
{
	// Polygons display mode
	glPolygonMode(m_PolyFace, m_PolyMode);	
	// Change the current matrix
	glMultMatrixd(m_MatPos.return_dMat());

}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////


// compute the instance bounding box
// m_pGeom should be not null
void GLC_Instance::computeBoundingBox(void)
{
	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}
	
	m_pBoundingBox= m_pGeom->getBoundingBox();
	m_pBoundingBox->transform(m_MatPos);
	
}

// Clear current instance
void GLC_Instance::clear()
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
		//qDebug() << "GLC_Instance::clear ID = " << m_Uid;
		//qDebug() << "- Number of instance" << (*m_pNumberOfInstance);
		delete m_pNumberOfInstance;
		m_pNumberOfInstance= NULL;
		
	}
	else
	{
		//qDebug() << "GLC_Instance::clear ID = " << m_Uid;
		//qDebug() << " - Number of instance" << (*m_pNumberOfInstance);
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
	
	// invalidate the instance
	m_IsValid= false;

}



