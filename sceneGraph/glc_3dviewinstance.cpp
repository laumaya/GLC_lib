/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.1.0, packaged on March, 2009.

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

//! \file glc_instance.cpp implementation of the GLC_3DViewInstance class.

#include "glc_3dviewinstance.h"
#include "../shading/glc_selectionmaterial.h"
#include "../viewport/glc_viewport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Default constructor
GLC_3DViewInstance::GLC_3DViewInstance()
: GLC_Object()
, m_pGeomList(NULL)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(new int(1))
, m_MatPos()
, m_IsBoundingBoxValid(false)
, m_IsSelected(false)
, m_PolyFace(GL_FRONT_AND_BACK)
, m_PolyMode(GL_FILL)
, m_IsVisible(true)
, m_colorId()
, m_DefaultLOD(10)
{
	// Encode Color Id
	encodeIdInRGBA();

	//qDebug() << "GLC_3DViewInstance::GLC_3DViewInstance null instance ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
}

// Contruct instance with a geometry
GLC_3DViewInstance::GLC_3DViewInstance(GLC_VboGeom* pGeom)
: GLC_Object()
, m_pGeomList(new QList<GLC_VboGeom*>())
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(new int(1))
, m_MatPos()
, m_IsBoundingBoxValid(false)
, m_IsSelected(false)
, m_PolyFace(GL_FRONT_AND_BACK)
, m_PolyMode(GL_FILL)
, m_IsVisible(true)
, m_colorId()
, m_DefaultLOD(10)
{
	m_pGeomList->append(pGeom);
	// Encode Color Id
	encodeIdInRGBA();

	setName(pGeom->name());

	//qDebug() << "GLC_3DViewInstance::GLC_3DViewInstance ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
}

// Copy constructor
GLC_3DViewInstance::GLC_3DViewInstance(const GLC_3DViewInstance& inputNode)
: GLC_Object(inputNode)
, m_pGeomList(inputNode.m_pGeomList)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(inputNode.m_pNumberOfInstance)
, m_MatPos(inputNode.m_MatPos)
, m_IsBoundingBoxValid(inputNode.m_IsBoundingBoxValid)
, m_IsSelected(inputNode.m_IsSelected)
, m_PolyFace(inputNode.m_PolyFace)
, m_PolyMode(inputNode.m_PolyMode)
, m_IsVisible(inputNode.m_IsVisible)
, m_colorId()
, m_DefaultLOD(inputNode.m_DefaultLOD)
{
	// Encode Color Id
	encodeIdInRGBA();

	if (NULL != inputNode.m_pBoundingBox)
	{
		m_pBoundingBox= new GLC_BoundingBox(*inputNode.m_pBoundingBox);
	}
	// Increment the number of instance
	++(*m_pNumberOfInstance);
	//qDebug() << "GLC_3DViewInstance::GLC_3DViewInstance CopyConstructor ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
}


// Assignement operator
GLC_3DViewInstance& GLC_3DViewInstance::operator=(const GLC_3DViewInstance& inputNode)
{
	if (this != &inputNode)
	{
		// Clear this instance
		clear();
		GLC_Object::operator=(inputNode);
		// Encode Color Id
		encodeIdInRGBA();

		m_pGeomList= inputNode.m_pGeomList;
		if (NULL != inputNode.m_pBoundingBox)
		{
			m_pBoundingBox= new GLC_BoundingBox(*inputNode.m_pBoundingBox);
		}
		m_pNumberOfInstance= inputNode.m_pNumberOfInstance;
		++(*m_pNumberOfInstance);
		m_MatPos= inputNode.m_MatPos;
		m_IsBoundingBoxValid= inputNode.m_IsBoundingBoxValid;
		m_IsSelected= inputNode.m_IsSelected;
		m_PolyFace= inputNode.m_PolyFace;
		m_PolyMode= inputNode.m_PolyMode;
		m_IsVisible= inputNode.m_IsVisible;
		m_DefaultLOD= inputNode.m_DefaultLOD;

		//qDebug() << "GLC_3DViewInstance::operator= :ID = " << m_Uid;
		//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
	}

	return *this;
}

// Destructor
GLC_3DViewInstance::~GLC_3DViewInstance()
{
	clear();
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get the bounding box
GLC_BoundingBox GLC_3DViewInstance::getBoundingBox(void)
{
	GLC_BoundingBox resultBox;
	if (getBoundingBoxValidity())
	{
		resultBox= *m_pBoundingBox;
	}
	else if (m_pGeomList != NULL)
	{
		computeBoundingBox();
		m_IsBoundingBoxValid= true;
		resultBox= *m_pBoundingBox;
	}

	return resultBox;
}

// Get the validity of the Bounding Box
const bool GLC_3DViewInstance::getBoundingBoxValidity(void) const
{
	bool result= true;
	if ((m_pGeomList != NULL) && (m_pBoundingBox != NULL))
	{
		const int size= m_pGeomList->size();
		for (int i= 0; i < size; ++i)
		{
			result= result and m_pGeomList->at(i)->boundingBoxIsValid() and m_IsBoundingBoxValid;
		}
	}
	else result= false;

	return result;
}

// Clone the instance
GLC_3DViewInstance GLC_3DViewInstance::clone() const
{
	GLC_3DViewInstance cloneInstance;
	if (m_pGeomList != NULL)
	{
		cloneInstance.m_pGeomList= new QList<GLC_VboGeom*>();
		int size= m_pGeomList->size();
		for (int i= 0; i < size; ++i)
		{
			cloneInstance.m_pGeomList->append(m_pGeomList->at(i)->clone());
		}
	}
	if (NULL != m_pBoundingBox)
	{
		cloneInstance.m_pBoundingBox= new GLC_BoundingBox(*m_pBoundingBox);
	}

	cloneInstance.m_MatPos= m_MatPos;
	cloneInstance.m_IsBoundingBoxValid= m_IsBoundingBoxValid;
	cloneInstance.m_IsSelected= m_IsSelected;
	cloneInstance.m_PolyFace= m_PolyFace;
	cloneInstance.m_PolyMode= m_PolyMode;
	cloneInstance.m_IsVisible= m_IsVisible;
	return cloneInstance;
}

// Instanciate the instance
GLC_3DViewInstance GLC_3DViewInstance::instanciate()
{
	GLC_3DViewInstance instance(*this);
	instance.m_Uid= glc::GLC_GenID();
	// Encode Color Id
	encodeIdInRGBA();

	return instance;
}

// Return the GLC_uint decoded ID from RGBA encoded ID
GLC_uint GLC_3DViewInstance::decodeRgbId(const GLubyte* pcolorId)
{
	GLC_uint returnId= 0;
	returnId|= (GLC_uint)pcolorId[0] << (0 * 8);
	returnId|= (GLC_uint)pcolorId[1] << (1 * 8);
	returnId|= (GLC_uint)pcolorId[2] << (2 * 8);
	// Only get first 24 bits
	//returnId|= (GLC_uint)pcolorId[3] << (3 * 8);

	return returnId;
}
// Get number of faces
unsigned int GLC_3DViewInstance::numberOfFaces() const
{
	unsigned int result= 0;
	if (NULL != m_pGeomList)
	{
		const int size= m_pGeomList->size();
		for (int i= 0; i < size; ++i)
		{
			result+= m_pGeomList->at(i)->numberOfFaces();
		}
	}

	return result;
}

// Get number of vertex
unsigned int GLC_3DViewInstance::numberOfVertex() const
{
	unsigned int result= 0;
	if (NULL != m_pGeomList)
	{
		const int size= m_pGeomList->size();
		for (int i= 0; i < size; ++i)
		{
			result+= m_pGeomList->at(i)->numberOfVertex();
		}
	}

	return result;
}

// Get number of materials
unsigned int GLC_3DViewInstance::numberOfMaterials() const
{
	unsigned int result= 0;
	if (NULL != m_pGeomList)
	{
		const int size= m_pGeomList->size();
		for (int i= 0; i < size; ++i)
		{
			result+= m_pGeomList->at(i)->numberOfMaterials();
		}
	}

	return result;
}

// Get materials List
QSet<GLC_Material*> GLC_3DViewInstance::materialSet() const
{
	QSet<GLC_Material*> result;
	if (NULL != m_pGeomList)
	{
		const int size= m_pGeomList->size();
		for (int i= 0; i < size; ++i)
		{
			result.unite(m_pGeomList->at(i)->materialSet());
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////


// Set the instance Geometry
bool GLC_3DViewInstance::setGeometry(GLC_VboGeom* pGeom)
{
	if (NULL != m_pGeomList)
	{
		if (m_pGeomList->contains(pGeom))
		{
			return false;
		}
		else
		{
			m_pGeomList->append(pGeom);
			return true;
		}
	}
	else
	{
		m_pGeomList= new QList<GLC_VboGeom*>();
		m_pGeomList->append(pGeom);
		return true;
	}
}

// Remove empty geometries
void GLC_3DViewInstance::removeEmptyGeometry()
{
	if (NULL != m_pGeomList)
	{
		QList<GLC_VboGeom*>::iterator iGeomList= m_pGeomList->begin();
		while(iGeomList != m_pGeomList->constEnd())
		{
			if ((*iGeomList)->numberOfVertex() == 0)
			{
				delete (*iGeomList);
				iGeomList= m_pGeomList->erase(iGeomList);
			}
			else
			{
				++iGeomList;
			}
		}
		if (m_pGeomList->isEmpty())
		{
			delete m_pGeomList;
			m_pGeomList= NULL;
		}
	}
}

// Reverse geometry normals
void GLC_3DViewInstance::reverseGeometriesNormals()
{
	if (NULL != m_pGeomList)
	{
		const int size= m_pGeomList->size();
		for (int i= 0; i < size; ++i)
		{
			(*m_pGeomList)[i]->reverseNormals();
		}
	}
}

// Instance translation
GLC_3DViewInstance& GLC_3DViewInstance::translate(double Tx, double Ty, double Tz)
{
	multMatrix(GLC_Matrix4x4(Tx, Ty, Tz));

	return *this;
}


// Move instance with a 4x4Matrix
GLC_3DViewInstance& GLC_3DViewInstance::multMatrix(const GLC_Matrix4x4 &MultMat)
{
	m_MatPos= MultMat * m_MatPos;
	m_IsBoundingBoxValid= false;

	return *this;
}

// Replace the instance Matrix
GLC_3DViewInstance& GLC_3DViewInstance::setMatrix(const GLC_Matrix4x4 &SetMat)
{
	m_MatPos= SetMat;
	m_IsBoundingBoxValid= false;

	return *this;
}

// Reset the instance Matrix
GLC_3DViewInstance& GLC_3DViewInstance::resetMatrix(void)
{
	m_MatPos.setToIdentity();
	m_IsBoundingBoxValid= false;

	return *this;
}

// Polygon's display style
void GLC_3DViewInstance::setPolygonMode(GLenum Face, GLenum Mode)
{
	// Change the polygon mode only if there is a change
	if ((m_PolyFace != Face) || (m_PolyMode != Mode))
	{
		m_PolyFace= Face;
		m_PolyMode= Mode;
	}
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Display the instance
void GLC_3DViewInstance::glExecute(bool transparent, bool useLoad, GLC_Viewport* pView)
{
	if (NULL == m_pGeomList) return;
	// Save current OpenGL Matrix
	glPushMatrix();
	glVisProperties();
	if(GLC_State::isInSelectionMode())
	{
		glColor3ubv(m_colorId); // D'ont use Alpha component
	}
	const int size= m_pGeomList->size();
	if (useLoad and (NULL != pView))
	{
		for (int i= 0; i < size; ++i)
		{
			const int lodValue= choseLod(m_pGeomList->at(i)->boundingBox(), pView);
			if (lodValue <= 100)
			{
				m_pGeomList->at(i)->setCurrentLod(lodValue);
				m_pGeomList->at(i)->glExecute(m_IsSelected, transparent);
			}
		}
	}
	else
	{
		for (int i= 0; i < size; ++i)
		{
			const int lodValue= choseLod(m_pGeomList->at(i)->boundingBox(), pView);
			if (lodValue <= 100)
			{
				m_pGeomList->at(i)->setCurrentLod(m_DefaultLOD);
				m_pGeomList->at(i)->glExecute(m_IsSelected, transparent);
			}

		}
	}
	// Restore OpenGL Matrix
	glPopMatrix();
}



//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////


// compute the instance bounding box
// m_pGeomList should be not null
void GLC_3DViewInstance::computeBoundingBox(void)
{
	if (NULL == m_pGeomList) return;

	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}
	m_pBoundingBox= new GLC_BoundingBox();
	const int size= m_pGeomList->size();
	for (int i= 0; i < size; ++i)
	{
		m_pBoundingBox->combine(m_pGeomList->at(i)->boundingBox());
	}

	m_pBoundingBox->transform(m_MatPos);
}

// Clear current instance
void GLC_3DViewInstance::clear()
{
	Q_ASSERT(m_pNumberOfInstance != NULL);

	if ((--(*m_pNumberOfInstance)) == 0)
	{
		// this is the last instance, delete the geometry
		if (m_pGeomList != NULL)
		{
			const int size= m_pGeomList->size();
			for (int i= 0; i < size; ++i)
			{
				delete (*m_pGeomList)[i];
			}
			delete m_pGeomList;
			m_pGeomList= NULL;
		}
		// delete instance counter
		//qDebug() << "GLC_3DViewInstance::clear ID = " << m_Uid;
		//qDebug() << "- Number of instance" << (*m_pNumberOfInstance);
		delete m_pNumberOfInstance;
		m_pNumberOfInstance= NULL;

	}
	else
	{
		//qDebug() << "GLC_3DViewInstance::clear ID = " << m_Uid;
		//qDebug() << " - Number of instance" << (*m_pNumberOfInstance);
	}


	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}

	// invalidate the bounding box
	m_IsBoundingBoxValid= false;

}

//! Encode Id to RGBA color
void GLC_3DViewInstance::encodeIdInRGBA()
{
	m_colorId[0]= static_cast<GLubyte>((m_Uid >> (0 * 8)) & 0xFF);
	m_colorId[1]= static_cast<GLubyte>((m_Uid >> (1 * 8)) & 0xFF);
	m_colorId[2]= static_cast<GLubyte>((m_Uid >> (2 * 8)) & 0xFF);
	m_colorId[3]= static_cast<GLubyte>((m_Uid >> (3 * 8)) & 0xFF);
}

// Compute LOD
int GLC_3DViewInstance::choseLod(const GLC_BoundingBox& boundingBox, GLC_Viewport* pView)
{
	if (NULL == pView) return 0;
	const double diameter= boundingBox.boundingSphereRadius() * 2.0;
	GLC_Vector4d center(m_MatPos * boundingBox.getCenter());

	const double dist= (center - pView->cameraHandle()->getEye()).norm();
	const double cameraCover= dist * (2.0 * tan((pView->getFov() * glc::PI / 180.0) / 2.0));
	double ratio= diameter / cameraCover * 150.0;

	if (ratio > 100.0) ratio= 100.0;
	ratio= 100.0 - ratio;
	if (ratio > 98.0) ratio= 110.0;
	if (ratio < static_cast<double>(m_DefaultLOD)) ratio= static_cast<double>(m_DefaultLOD);
	//qDebug() << "RATIO = " << static_cast<int>(ratio);

	return static_cast<int>(ratio);
}


