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
		m_Childs= pStructInstance->firstOccurenceHandle()->m_Childs;
	}
	else
	{
		m_pNumberOfOccurence= new int(1);
	}

	// Add instance representation in the collection
	if (m_HaveRepresentation)
	{
		GLC_Instance representation(pStructInstance->structReference()->instanceRepresentation());
		// Set occurence default name
		setName(representation.name());
		// Force instance representation id
		representation.setId(id());
		m_pCollection->add(representation, shaderId);
	}

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
	++(*m_pNumberOfOccurence);
	// Inform the instance of the creation
	m_pStructInstance->structOccurenceCreated(this);

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

	// Update instance
	m_pStructInstance->structOccurenceCreated(this);

	// Create childs
	const int size= structOccurence.childCount();
	for (int i= 0; i < size; ++i)
	{
		addChild(structOccurence.child(i)->clone(pCollection));
	}
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

	if ((--(*m_pNumberOfOccurence)) == 0)
	{
		delete m_pStructInstance;
		delete m_pNumberOfOccurence;
	}
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get number of faces
unsigned int GLC_StructOccurence::numberOfFaces() const
{
	return 0;
}

// Get number of vertex
unsigned int GLC_StructOccurence::numberOfVertex() const
{
	return 0;
}

// Get number of materials
unsigned int GLC_StructOccurence::numberOfMaterials() const
{
	return 0;
}

// Get materials List
QSet<GLC_Material*> GLC_StructOccurence::materialSet() const
{
	QSet<GLC_Material*> materialSet;
	if (m_HaveRepresentation)
	{
		materialSet= m_pCollection->getInstanceHandle(id())->materialSet();
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
void GLC_StructOccurence::updateAbsoluteMatrix()
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
}

// Add Child
void GLC_StructOccurence::addChild(GLC_StructOccurence* pChild)
{
	Q_ASSERT(pChild->isOrphan());

	// Add the child to the list of child
	m_Childs.append(pChild);
	pChild->m_pParent= this;
}

// Add Child instance (the occurence is created)
void GLC_StructOccurence::addChild(GLC_StructInstance* pInstance)
{
	GLC_StructOccurence* pOccurence= new GLC_StructOccurence(m_pCollection, pInstance);
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
