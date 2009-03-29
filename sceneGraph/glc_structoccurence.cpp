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

//! \file glc_structoccurence.cpp implementation of the GLC_StructOccurence class.

#include "glc_structoccurence.h"
#include "glc_collection.h"
#include "glc_structinstance.h"
#include "glc_structreference.h"

// Default constructor
GLC_StructOccurence::GLC_StructOccurence(GLC_Collection* pCollection, GLC_StructInstance* pStructInstance, GLuint shaderId)
: GLC_Object()
, m_pCollection(pCollection)
, m_pNumberOfOccurence(NULL)
, m_pStructInstance(pStructInstance)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_HaveRepresentation(pStructInstance->structReference()->haveRepresentation())
{
	// Update the number of occurences
	if (pStructInstance->haveStructOccurence())
	{
		m_pNumberOfOccurence= pStructInstance->firstOccurenceHandle()->m_pNumberOfOccurence;
		++(*m_pNumberOfOccurence);
		QList<GLC_StructOccurence*> childs= pStructInstance->firstOccurenceHandle()->m_Childs;
		const int size= childs.size();
		for (int i= 0; i < size; ++i)
		{
			addChild(childs.at(i)->clone(m_pCollection));
		}
	}
	else
	{
		m_pNumberOfOccurence= new int(1);
	}

	// Add instance representation in the collection
	if (m_HaveRepresentation)
	{
		GLC_Instance representation(pStructInstance->structReference()->instanceRepresentation());
		// Force instance representation id
		representation.setId(id());
		m_pCollection->add(representation, shaderId);
	}

	setName(m_pStructInstance->name());

	// Update Absolute matrix
	updateAbsoluteMatrix();

	// Update instance
	m_pStructInstance->structOccurenceCreated(this);
}
// Copy constructor
GLC_StructOccurence::GLC_StructOccurence(GLC_Collection* pCollection, const GLC_StructOccurence& structOccurence)
: GLC_Object(structOccurence)
, m_pCollection(pCollection)
, m_pNumberOfOccurence(structOccurence.m_pNumberOfOccurence)
, m_pStructInstance(structOccurence.m_pStructInstance)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix(structOccurence.m_AbsoluteMatrix)
, m_HaveRepresentation(structOccurence.m_HaveRepresentation)
{
	// Change object id
	setId(glc::GLC_GenID());
	++(*m_pNumberOfOccurence);

	if (m_HaveRepresentation)
	{
		// Add occurence instance representation to the collection
		GLC_Instance newRep= structOccurence.m_pCollection->getInstanceHandle(structOccurence.id())->instanciate();
		newRep.setId(id());
		if(structOccurence.m_pCollection->isInAShadingGroup(structOccurence.id()))
		{
			GLuint shaderId= structOccurence.m_pCollection->shadingGroup(structOccurence.id());
			pCollection->bindShader(shaderId);
			m_pCollection->add(newRep, shaderId);
		}
		else
		{
			m_pCollection->add(newRep);
		}
		//qDebug() << "Add instance " << newRep.name() << " in collection";

	}

	// Update Absolute matrix
	updateAbsoluteMatrix();


	// Create childs
	const int size= structOccurence.childCount();
	for (int i= 0; i < size; ++i)
	{
		GLC_StructOccurence* pChild= structOccurence.child(i)->clone(pCollection);
		addChild(pChild);
		pChild->updateAbsoluteMatrix();
	}

	// Update instance
	m_pStructInstance->structOccurenceCreated(this);
}

// Destructor
GLC_StructOccurence::~GLC_StructOccurence()
{
	Q_ASSERT(m_pNumberOfOccurence != NULL);
	// Remove collection representation if necessary
	if (m_HaveRepresentation and not m_pCollection->isEmpty())
	{
		m_pCollection->remove(id());
	}

	// Remove Childs
	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
		GLC_StructOccurence* pChild= m_Childs.at(i);
		delete pChild;
	}
	// Update number of occurence and instance
	if ((--(*m_pNumberOfOccurence)) == 0)
	{
		delete m_pStructInstance;
		delete m_pNumberOfOccurence;
	}
	else
	{
		m_pStructInstance->structOccurenceDeleted(this);
	}
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get number of faces
unsigned int GLC_StructOccurence::numberOfFaces() const
{
	unsigned int result= 0;
	if (m_HaveRepresentation)
	{
		result= structInstance()->structReference()->numberOfFaces();
	}
	else
	{
		const int size= m_Childs.size();
		for (int i= 0; i < size; ++i)
		{
			result+= m_Childs.at(i)->numberOfFaces();
		}
	}
	return result;
}

// Get number of vertex
unsigned int GLC_StructOccurence::numberOfVertex() const
{
	unsigned int result= 0;
	if (m_HaveRepresentation)
	{
		result= structInstance()->structReference()->numberOfVertex();
	}
	else
	{
		const int size= m_Childs.size();
		for (int i= 0; i < size; ++i)
		{
			result+= m_Childs.at(i)->numberOfVertex();
		}
	}
	return result;
}

// Get number of materials
unsigned int GLC_StructOccurence::numberOfMaterials() const
{
	unsigned int result= 0;
	QSet<GLC_Material*> materialSet;
	if (m_HaveRepresentation)
	{
		result= structInstance()->structReference()->numberOfMaterials();
	}
	else
	{
		const int size= m_Childs.size();
		for (int i= 0; i < size; ++i)
		{
			materialSet.unite(m_Childs.at(i)->materialSet());
		}
		result= static_cast<unsigned int>(materialSet.size());
	}
	return result;
}

// Get materials List
QSet<GLC_Material*> GLC_StructOccurence::materialSet() const
{
	QSet<GLC_Material*> materialSet;
	if (m_HaveRepresentation)
	{
		materialSet= structInstance()->structReference()->materialSet();
	}
	else
	{
		const int size= m_Childs.size();
		for (int i= 0; i < size; ++i)
		{
			materialSet.unite(m_Childs.at(i)->materialSet());
		}
	}
	return materialSet;
}

// Clone the occurence
GLC_StructOccurence* GLC_StructOccurence::clone(GLC_Collection* pCollection) const
{
	return new GLC_StructOccurence(pCollection, *this);
}

// Return true if the occurence is visible
bool GLC_StructOccurence::isVisible() const
{
	bool result= true;
	if (m_HaveRepresentation)
	{
		result= m_pCollection->getInstanceHandle(id())->isVisible();
	}
	else
	{
		const int size= childCount();
		int i= 0;
		while ((i < size) and result)
		{
			result= result and child(i)->isVisible();
			++i;
		}
	}
	return result;
}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Update the absolute matrix
GLC_StructOccurence* GLC_StructOccurence::updateAbsoluteMatrix()
{
	if (NULL != m_pParent)
	{
		m_AbsoluteMatrix= m_pParent->absoluteMatrix() * m_pStructInstance->relativeMatrix();
	}
	else
	{
		m_AbsoluteMatrix= m_pStructInstance->relativeMatrix();
	}
	// If the occurence have a representation, update it.
	if (m_HaveRepresentation)
	{
		m_pCollection->getInstanceHandle(id())->setMatrix(m_AbsoluteMatrix);
	}
	return this;
}

// Update childs obsolute Matrix
GLC_StructOccurence* GLC_StructOccurence::updateChildsAbsoluteMatrix()
{
	updateAbsoluteMatrix();
	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
		m_Childs[i]->updateChildsAbsoluteMatrix();
	}
	return this;
}

// Add Child
void GLC_StructOccurence::addChild(GLC_StructOccurence* pChild)
{
	Q_ASSERT(pChild->isOrphan());
	//qDebug() << "Add Child " << pChild->name() << "id=" << pChild->id() << " to " << name() << " id=" << id();
	// Add the child to the list of child
	m_Childs.append(pChild);
	pChild->m_pParent= this;
	pChild->updateAbsoluteMatrix();
}

// Add Child instance (the occurence is created)
void GLC_StructOccurence::addChild(GLC_StructInstance* pInstance)
{
	GLC_StructOccurence* pOccurence;
	if (not pInstance->haveStructOccurence())
	{
		pOccurence= new GLC_StructOccurence(m_pCollection, pInstance);
	}
	else
	{
		pOccurence= pInstance->firstOccurenceHandle()->clone(m_pCollection);
	}

	addChild(pOccurence);
}

// Add Children
void GLC_StructOccurence::addChildren(const QList<GLC_StructOccurence*>& children)
{
	const int size= children.size();
	for (int i= 0; i < size; ++i)
	{
		Q_ASSERT(children.at(i)->isOrphan());
		// Add the child to the list of child
		m_Childs.append(children.at(i));
		children.at(i)->m_pParent= this;
	}
}

// make the occurence orphan
void GLC_StructOccurence::makeOrphan()
{
	Q_ASSERT(not isOrphan());
	m_pParent->removeChild(this);
}

// Reverse Normals of this Occurence and childs
void GLC_StructOccurence::reverseNormals()
{
	if (m_HaveRepresentation)
	{
		m_pCollection->getInstanceHandle(id())->getGeometry()->reverseNormals();
	}
}

// Check the presence of representation
void GLC_StructOccurence::checkForRepresentation()
{
	if (NULL != m_pStructInstance)
	{
		GLC_StructReference* pRef= m_pStructInstance->structReference();
		if (NULL != pRef)
		{
			if (pRef->haveRepresentation())
			{
				GLC_Instance representation(pRef->instanceRepresentation());
				representation.setName(name());
				// Force instance representation id
				representation.setId(id());
				m_pCollection->add(representation);
			}
			m_HaveRepresentation= true;
		}
	}
}
