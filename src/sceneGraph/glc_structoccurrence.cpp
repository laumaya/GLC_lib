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

//! \file glc_structoccurence.cpp implementation of the GLC_StructOccurence class.

#include "glc_structoccurrence.h"
#include "glc_3dviewcollection.h"
#include "glc_structreference.h"
#include "glc_worldhandle.h"
#include "../glc_errorlog.h"

GLC_StructOccurrence::GLC_StructOccurrence()
: m_Uid(glc::GLC_GenID())
, m_pWorldHandle(NULL)
, m_pNumberOfOccurence(new int(1))
, m_pStructInstance(new GLC_StructInstance())
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurenceNumber(0)
, m_IsVisible(true)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(true)
, m_pRelativeMatrix(NULL)
{
	// Update instance
	m_pStructInstance->structOccurenceCreated(this);
}

GLC_StructOccurrence::GLC_StructOccurrence(GLC_StructInstance* pStructInstance, GLC_WorldHandle* pWorldHandle, GLuint shaderId)
: m_Uid(glc::GLC_GenID())
, m_pWorldHandle(pWorldHandle)
, m_pNumberOfOccurence(NULL)
, m_pStructInstance(pStructInstance)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurenceNumber(0)
, m_IsVisible(true)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(true)
, m_pRelativeMatrix(NULL)
{
	doCreateOccurrenceFromInstance(shaderId);
}

GLC_StructOccurrence::GLC_StructOccurrence(GLC_StructInstance* pStructInstance, GLC_uint id, GLC_WorldHandle* pWorldHandle, GLuint shaderId)
: m_Uid(id)
, m_pWorldHandle(pWorldHandle)
, m_pNumberOfOccurence(NULL)
, m_pStructInstance(pStructInstance)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurenceNumber(0)
, m_IsVisible(true)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(true)
, m_pRelativeMatrix(NULL)
{
	doCreateOccurrenceFromInstance(shaderId);
}

GLC_StructOccurrence::GLC_StructOccurrence(GLC_3DRep* pRep)
: m_Uid(glc::GLC_GenID())
, m_pWorldHandle(NULL)
, m_pNumberOfOccurence(new int(1))
, m_pStructInstance(NULL)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurenceNumber(0)
, m_IsVisible(true)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(true)
, m_pRelativeMatrix(NULL)
{
	m_pStructInstance= new GLC_StructInstance(pRep);

	// Update instance
	m_pStructInstance->structOccurenceCreated(this);
}

GLC_StructOccurrence::GLC_StructOccurrence(GLC_3DRep* pRep, GLC_uint id)
: m_Uid(id)
, m_pWorldHandle(NULL)
, m_pNumberOfOccurence(new int(1))
, m_pStructInstance(NULL)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurenceNumber(0)
, m_IsVisible(true)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(true)
, m_pRelativeMatrix(NULL)
{
	m_pStructInstance= new GLC_StructInstance(pRep);

	// Update instance
	m_pStructInstance->structOccurenceCreated(this);
}

GLC_StructOccurrence::GLC_StructOccurrence(GLC_WorldHandle* pWorldHandle, const GLC_StructOccurrence& structOccurence, bool shareInstance)
: m_Uid(glc::GLC_GenID())
, m_pWorldHandle(pWorldHandle)
, m_pNumberOfOccurence(NULL)
, m_pStructInstance(NULL)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix(structOccurence.m_AbsoluteMatrix)
, m_OccurenceNumber(0)
, m_IsVisible(structOccurence.m_IsVisible)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(structOccurence.m_AutomaticCreationOf3DViewInstance)
, m_pRelativeMatrix(NULL)
{
	if (shareInstance)
	{
		m_pStructInstance= structOccurence.m_pStructInstance;
		m_pNumberOfOccurence= structOccurence.m_pNumberOfOccurence;
		++(*m_pNumberOfOccurence);
	}
	else
	{
		m_pNumberOfOccurence= new int(1);
		m_pStructInstance= new GLC_StructInstance(structOccurence.m_pStructInstance);
	}


	// Test if structOccurence has representation and has a shader
	GLuint shaderId= 0;
	bool instanceIsSelected= false;
	if ((NULL != m_pWorldHandle) && (NULL != structOccurence.m_pWorldHandle) && structOccurence.m_pWorldHandle->collection()->contains(structOccurence.id()))
	{
		GLC_3DViewInstance* p3DViewInstance= structOccurence.m_pWorldHandle->collection()->instanceHandle(structOccurence.id());

		if(structOccurence.m_pWorldHandle->collection()->isInAShadingGroup(structOccurence.id()))
		{
			shaderId= structOccurence.m_pWorldHandle->collection()->shadingGroup(structOccurence.id());
		}

		instanceIsSelected= p3DViewInstance->isSelected();
		// Test the rendering properties
		if (! p3DViewInstance->renderPropertiesHandle()->isDefault())
		{
			m_pRenderProperties= new GLC_RenderProperties(*(p3DViewInstance->renderPropertiesHandle()));
		}
	}
	else if (NULL != structOccurence.m_pRenderProperties)
	{
		m_pRenderProperties= new GLC_RenderProperties(*(structOccurence.m_pRenderProperties));
	}

	// Inform the world Handle
	if (NULL != m_pWorldHandle)
	{
		m_pWorldHandle->addOccurence(this, instanceIsSelected, shaderId);
		if (NULL != m_pRenderProperties && this->has3DViewInstance())
		{
			m_pWorldHandle->collection()->instanceHandle(id())->setRenderProperties(*m_pRenderProperties);
			delete m_pRenderProperties;
			m_pRenderProperties= NULL;
		}
	}

	// Check flexibility
	if (NULL != structOccurence.m_pRelativeMatrix)
	{
		m_pRelativeMatrix= new GLC_Matrix4x4(*(structOccurence.m_pRelativeMatrix));
	}

	// Update Absolute matrix
	updateAbsoluteMatrix();


	// Create childs
	const int size= structOccurence.childCount();
	for (int i= 0; i < size; ++i)
	{
        GLC_StructOccurrence* pChild= structOccurence.child(i)->clone(m_pWorldHandle, true);
		addChild(pChild);
	}
	updateChildrenAbsoluteMatrix();
	// Update instance
	m_pStructInstance->structOccurenceCreated(this);
}

GLC_StructOccurrence::~GLC_StructOccurrence()
{
	//qDebug() << "Delete " << id();
	Q_ASSERT(m_pNumberOfOccurence != NULL);

	// Remove Childs
	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
        GLC_StructOccurrence* pChild= m_Childs.first();
		removeChild(pChild);
		delete pChild;
	}

    makeOrphan();

	// Update number of occurence and instance
	if ((--(*m_pNumberOfOccurence)) == 0)
	{
		delete m_pStructInstance;
		delete m_pNumberOfOccurence;
	}
	else
	{
		m_pStructInstance->structOccurenceDeleted(this);
		if (!m_pStructInstance->hasStructOccurence())
		{

			QStringList errorList;
			errorList << "StructOccurence count error";
			errorList << ("ref name = " + m_pStructInstance->structReference()->name());
			GLC_ErrorLog::addError(errorList);

			delete m_pStructInstance;
			//delete m_pNumberOfOccurence;
		}
	}

	delete m_pRenderProperties;
	delete m_pRelativeMatrix;
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

GLC_Matrix4x4 GLC_StructOccurrence::occurrenceRelativeMatrix() const
{
	GLC_Matrix4x4 matrix;
	if (NULL != m_pRelativeMatrix)
	{
		matrix= *m_pRelativeMatrix;
	}
	return matrix;
}

bool GLC_StructOccurrence::hasRepresentation() const
{
	if ((NULL != m_pStructInstance) && (m_pStructInstance->structReference() != NULL))
	{
		return this->structReference()->hasRepresentation();
	}
	else return false;
}

bool GLC_StructOccurrence::has3DViewInstance() const
{
	if ( NULL != m_pWorldHandle)
	{
		return m_pWorldHandle->collection()->contains(m_Uid);
	}
	else return false;
}

bool GLC_StructOccurrence::canBeAddedToChildren(GLC_StructOccurrence* pOccurence) const
{
	bool canBeAdded= false;
	if ((NULL != m_pStructInstance) && (m_pStructInstance->hasStructOccurence()) && (NULL != pOccurence->m_pStructInstance) && (NULL != pOccurence->structReference()))
	{
		if (this->structReference() != pOccurence->structReference())
		{
            QSet<GLC_StructReference*> thisRefSet= GLC_StructOccurrence::parentsReferences(this);
			thisRefSet << this->structReference();
			QSet<GLC_StructReference*> childRefSet= pOccurence->childrenReferences();

			canBeAdded= thisRefSet == (thisRefSet - childRefSet);
		}
	}
	else
	{
		canBeAdded= true;
	}
    return canBeAdded;
}

QList<GLC_StructOccurrence *> GLC_StructOccurrence::ancestorList() const
{
    QList<GLC_StructOccurrence *> subject;
    if (NULL != m_pParent)
    {
        subject.append(m_pParent);
        subject.append(m_pParent->ancestorList());
    }

    return subject;
}

QList<GLC_StructOccurrence*> GLC_StructOccurrence::subOccurenceList() const
{
    QList<GLC_StructOccurrence*> subOccurence;
	const int childCount= m_Childs.size();
	for (int i= 0; i < childCount; ++i)
	{
        GLC_StructOccurrence* pCurrentChild= m_Childs.at(i);
		subOccurence.append(pCurrentChild);
		if (pCurrentChild->hasChild())
		{
			subOccurence.append(pCurrentChild->subOccurenceList());
		}
	}

	return subOccurence;
}

unsigned int GLC_StructOccurrence::numberOfFaces() const
{
	unsigned int result= 0;
	if (hasRepresentation())
	{
		result= structInstance()->structReference()->numberOfFaces();
	}

	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
		result+= m_Childs.at(i)->numberOfFaces();
	}

	return result;
}

unsigned int GLC_StructOccurrence::numberOfVertex() const
{
	unsigned int result= 0;
	if (hasRepresentation())
	{
		result= structInstance()->structReference()->numberOfVertex();
	}
	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
		result+= m_Childs.at(i)->numberOfVertex();
	}

	return result;
}

unsigned int GLC_StructOccurrence::numberOfMaterials() const
{
	unsigned int result= 0;
	QSet<GLC_Material*> materialSet;
	if (hasRepresentation())
	{
		result= structInstance()->structReference()->numberOfMaterials();
	}

	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
		materialSet.unite(m_Childs.at(i)->materialSet());
	}
	result= static_cast<unsigned int>(materialSet.size());

	return result;
}

QSet<GLC_Material*> GLC_StructOccurrence::materialSet() const
{
	QSet<GLC_Material*> materialSet;
	if (hasRepresentation())
	{
		materialSet= structInstance()->structReference()->materialSet();
	}

	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
		materialSet.unite(m_Childs.at(i)->materialSet());
	}

	return materialSet;
}

GLC_StructOccurrence* GLC_StructOccurrence::clone(GLC_WorldHandle* pWorldHandle, bool shareInstance) const
{
    return new GLC_StructOccurrence(pWorldHandle, *this, shareInstance);
}

bool GLC_StructOccurrence::isVisible() const
{
	bool isHidden= true;

	if ((NULL != m_pWorldHandle) && m_pWorldHandle->collection()->contains(m_Uid))
	{
		isHidden= !m_pWorldHandle->collection()->instanceHandle(m_Uid)->isVisible();
	}
	else if (childCount() > 0)
	{
		const int size= childCount();
		int i= 0;
		while ((i < size) && isHidden)
		{
			isHidden= isHidden && !child(i)->isVisible();
			++i;
		}
	}
	else
	{
		isHidden= !m_IsVisible;
	}
	return !isHidden;
}

GLC_BoundingBox GLC_StructOccurrence::boundingBox() const
{
	GLC_BoundingBox boundingBox;

	if (NULL != m_pWorldHandle)
	{
		if (has3DViewInstance())
		{
			Q_ASSERT(m_pWorldHandle->collection()->contains(id()));
			boundingBox= m_pWorldHandle->collection()->instanceHandle(id())->boundingBox();
		}
		else
		{
			if (hasChild())
			{
                QList<GLC_StructOccurrence*> childrenList= children();
				const int size= childrenList.size();

				for (int i= 0; i < size; ++i)
				{
					boundingBox.combine(childrenList.at(i)->boundingBox());
				}
			}
		}
	}

	return boundingBox;
}

unsigned int GLC_StructOccurrence::nodeCount() const
{
	unsigned int result= 1;
	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
		result+= m_Childs.at(i)->nodeCount();
	}
	return result;
}

QSet<GLC_StructReference*> GLC_StructOccurrence::childrenReferences() const
{
	QSet<GLC_StructReference*> refChildrenSet;
	const int childCount= m_Childs.size();
	for (int i= 0; i < childCount; ++i)
	{
        GLC_StructOccurrence* pCurrentChild= m_Childs.at(i);
		if ((NULL != pCurrentChild->structInstance()) && (NULL != pCurrentChild->structReference()))
		{
			refChildrenSet << pCurrentChild->structReference();
		}
	}

	return refChildrenSet;
}

QSet<GLC_StructReference*> GLC_StructOccurrence::parentsReferences(const GLC_StructOccurrence* pOccurence)
{
	QSet<GLC_StructReference*> parentSet;
    GLC_StructOccurrence* pParent= pOccurence->parent();
	if (NULL != pParent)
	{
		if ((NULL != pParent->structInstance()) && (NULL != pParent->structReference()))
		{
			parentSet << pParent->structReference();
            parentSet.unite(GLC_StructOccurrence::parentsReferences(pParent));
		}
	}

	return parentSet;
}

int GLC_StructOccurrence::indexOf(const GLC_StructOccurrence* pOcc) const
{
	int subject= -1;
	const int childCount= m_Childs.count();
	bool containsOcc= false;
	int i= 0;
	while (!containsOcc && (i < childCount))
	{
		if (m_Childs.at(i) == pOcc)
		{
			containsOcc= true;
			subject= i;
		}
		++i;
	}
	return subject;
}

bool GLC_StructOccurrence::containsChild(const GLC_StructOccurrence* pOcc) const
{
	return pOcc->parent() == this;
}
//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

GLC_StructOccurrence* GLC_StructOccurrence::updateAbsoluteMatrix()
{
	GLC_Matrix4x4 relativeMatrix;
	if (NULL == m_pRelativeMatrix)
	{
		relativeMatrix= m_pStructInstance->relativeMatrix();
	}
	else
	{
		relativeMatrix= *m_pRelativeMatrix;
	}

	if (NULL != m_pParent)
	{
		m_AbsoluteMatrix= m_pParent->absoluteMatrix() * relativeMatrix;
	}
	else
	{
		m_AbsoluteMatrix= relativeMatrix;
	}
	// If the occurence have a representation, update it.

	if ((NULL != m_pWorldHandle) && m_pWorldHandle->collection()->contains(m_Uid))
	{
		m_pWorldHandle->collection()->instanceHandle(m_Uid)->setMatrix(m_AbsoluteMatrix);
	}
	return this;
}

GLC_StructOccurrence* GLC_StructOccurrence::updateChildrenAbsoluteMatrix()
{
	updateAbsoluteMatrix();
	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
		m_Childs[i]->updateChildrenAbsoluteMatrix();
	}
	return this;
}

void GLC_StructOccurrence::addChild(GLC_StructOccurrence* pChild)
{
	Q_ASSERT(pChild->isOrphan());
	Q_ASSERT((NULL == pChild->m_pWorldHandle) || (m_pWorldHandle == pChild->m_pWorldHandle));

	//qDebug() << "Add Child " << pChild->name() << "id=" << pChild->id() << " to " << name() << " id=" << id();
	// Add the child to the list of child
	// Get occurence reference
	m_Childs.append(pChild);
	pChild->m_pParent= this;
	if (NULL == pChild->m_pWorldHandle)
	{
		pChild->setWorldHandle(m_pWorldHandle);
	}
	pChild->updateChildrenAbsoluteMatrix();
}

void GLC_StructOccurrence::insertChild(int index, GLC_StructOccurrence* pChild)
{
	Q_ASSERT(pChild->isOrphan());
	Q_ASSERT((NULL == pChild->m_pWorldHandle) || (m_pWorldHandle == pChild->m_pWorldHandle));
	Q_ASSERT(m_Childs.count() >= index);

	//qDebug() << "Add Child " << pChild->name() << "id=" << pChild->id() << " to " << name() << " id=" << id();
	// Add the child to the list of child
	// Get occurence reference
	m_Childs.insert(index, pChild);
	pChild->m_pParent= this;
	if (NULL == pChild->m_pWorldHandle)
	{
		pChild->setWorldHandle(m_pWorldHandle);
	}
	pChild->updateChildrenAbsoluteMatrix();
}

GLC_StructOccurrence* GLC_StructOccurrence::addChild(GLC_StructInstance* pInstance)
{
    GLC_StructOccurrence* pOccurence;
    pOccurence= new GLC_StructOccurrence(pInstance, m_pWorldHandle);

	addChild(pOccurence);

	return pOccurence;
}

GLC_StructOccurrence* GLC_StructOccurrence::insertChild(int index, GLC_StructInstance* pInstance)
{
    GLC_StructOccurrence* pOccurence;
    pOccurence= new GLC_StructOccurrence(pInstance, m_pWorldHandle);

	insertChild(index, pOccurence);

	return pOccurence;
}

void GLC_StructOccurrence::makeOrphan()
{
	if(!isOrphan())
	{
		m_pParent->removeChild(this);
	}
	else
	{
		detach();
	}
}

bool GLC_StructOccurrence::removeChild(GLC_StructOccurrence* pChild)
{
	Q_ASSERT(pChild->m_pParent == this);
	pChild->m_pParent= NULL;
	pChild->detach();

	return m_Childs.removeOne(pChild);
}


void GLC_StructOccurrence::reverseNormals()
{
	if (has3DViewInstance())
	{
		m_pWorldHandle->collection()->instanceHandle(id())->reverseGeometriesNormals();
	}
}

bool GLC_StructOccurrence::create3DViewInstance(GLuint shaderId)
{
	bool subject= false;
	if (!has3DViewInstance() && (NULL != m_pWorldHandle) && hasRepresentation())
	{
		GLC_3DRep* p3DRep= dynamic_cast<GLC_3DRep*>(structReference()->representationHandle());
		if (NULL != p3DRep)
		{
			GLC_3DViewInstance instance(*p3DRep, m_Uid);
			instance.setName(name());

			if (NULL != m_pRenderProperties)
			{
				instance.setRenderProperties(*m_pRenderProperties);
				delete m_pRenderProperties;
				m_pRenderProperties= NULL;
			}

			if (0 != shaderId) m_pWorldHandle->collection()->bindShader(shaderId);
			subject= m_pWorldHandle->collection()->add(instance, shaderId);
			m_pWorldHandle->collection()->setVisibility(m_Uid, m_IsVisible);
			if (m_pWorldHandle->selectionSetHandle()->contains(m_Uid))
			{
				m_pWorldHandle->collection()->select(m_Uid);
			}
		}
	}
	return subject;
}

bool GLC_StructOccurrence::remove3DViewInstance()
{
	if (NULL != m_pWorldHandle)
	{
		return m_pWorldHandle->collection()->remove(m_Uid);
	}
	else return false;
}

// Set the occurence world Handle
void GLC_StructOccurrence::setWorldHandle(GLC_WorldHandle* pWorldHandle)
{
	// Check if world handles are equal
	if (m_pWorldHandle == pWorldHandle) return;

	if (NULL != m_pWorldHandle)
	{
		m_pWorldHandle->removeOccurence(this);
	}

	m_pWorldHandle= pWorldHandle;

	if (NULL != m_pWorldHandle)
	{
		m_pWorldHandle->addOccurence(this);
		m_pWorldHandle->collection()->setVisibility(m_Uid, m_IsVisible);

		const int size= m_Childs.size();
		for (int i= 0; i < size; ++i)
		{
			m_Childs[i]->setWorldHandle(m_pWorldHandle);
		}
	}
}

// Load the representation and return true if success
bool GLC_StructOccurrence::loadRepresentation()
{
	Q_ASSERT(!this->has3DViewInstance());

	bool loadSuccess= false;
	if (hasRepresentation())
	{
		GLC_StructReference* pReference= this->structReference();
		if (pReference->representationIsLoaded())
		{
			loadSuccess= create3DViewInstance();
		}
		else
		{
			loadSuccess=  m_pStructInstance->structReference()->loadRepresentation();
			if (loadSuccess && !m_AutomaticCreationOf3DViewInstance)
			{
				loadSuccess= create3DViewInstance();
			}
		}
	}

	return loadSuccess;
}

// UnLoad the representation and return true if success
bool GLC_StructOccurrence::unloadRepresentation()
{
	bool unloadResult= false;
	if (hasRepresentation())
	{
		GLC_StructReference* pRef= this->structReference();
		if (pRef->representationIsLoaded())
		{
			if (this->has3DViewInstance())
			{
				unloadResult= m_pWorldHandle->collection()->remove(m_Uid);
                QSet<GLC_StructOccurrence*> occurenceSet= pRef->setOfStructOccurence();
                QSet<GLC_StructOccurrence*>::const_iterator iOcc= occurenceSet.constBegin();
				bool unloadReferenceRep= true;
				while (occurenceSet.constEnd() != iOcc)
				{
					unloadReferenceRep= unloadReferenceRep && !(*iOcc)->has3DViewInstance();
					++iOcc;
				}
				if (unloadReferenceRep)
				{
					pRef->unloadRepresentation();
				}
			}
		}
	}
	return unloadResult;
}

unsigned int GLC_StructOccurrence::updateOccurenceNumber(unsigned int n)
{
	m_OccurenceNumber= n++;
	const int childCount= m_Childs.size();
	for (int i= 0; i < childCount; ++i)
	{
		n= m_Childs[i]->updateOccurenceNumber(n);
	}
	return n;
}

void GLC_StructOccurrence::setVisibility(bool visibility)
{
	m_IsVisible= visibility;
	if (has3DViewInstance())
	{
		m_pWorldHandle->collection()->setVisibility(m_Uid, m_IsVisible);
	}
	const int childCount= m_Childs.size();
	for (int i= 0; i < childCount; ++i)
	{
		m_Childs[i]->setVisibility(m_IsVisible);
	}
}

void GLC_StructOccurrence::setRenderProperties(const GLC_RenderProperties& renderProperties)
{
	qDebug() << "GLC_StructOccurence::setRenderProperties";
	delete m_pRenderProperties;
	m_pRenderProperties= NULL;
	if (has3DViewInstance())
	{
		m_pWorldHandle->collection()->instanceHandle(m_Uid)->setRenderProperties(renderProperties);
	}

	if (hasChild())
	{
		const int childCount= m_Childs.size();
		for (int i= 0; i < childCount; ++i)
		{
			m_Childs[i]->setRenderProperties(renderProperties);
		}
	}
	else if (!has3DViewInstance())
	{
		m_pRenderProperties= new GLC_RenderProperties(renderProperties);
	}
}

void GLC_StructOccurrence::removeEmptyChildren()
{
    QList<GLC_StructOccurrence*>::iterator iChild= m_Childs.begin();
	while (m_Childs.constEnd() != iChild)
	{
		if (!((*iChild)->hasChild()) && !((*iChild)->hasRepresentation()))
		{
            GLC_StructOccurrence* pOcc= *iChild;
            iChild= m_Childs.erase(iChild);
            delete pOcc;
		}
		else
		{
			(*iChild)->removeEmptyChildren();
			++iChild;
		}
	}
}

void GLC_StructOccurrence::setReference(GLC_StructReference* pRef)
{
	Q_ASSERT(m_pStructInstance->structReference() == NULL);
	Q_ASSERT((*m_pNumberOfOccurence) == 1);

	if (pRef->hasStructInstance())
	{
		GLC_StructInstance* pExistingInstance= pRef->firstInstanceHandle();
		if (pExistingInstance->hasStructOccurence())
		{
            GLC_StructOccurrence* pFirstOccurence= pExistingInstance->firstOccurenceHandle();
            QList<GLC_StructOccurrence*> childs= pFirstOccurence->m_Childs;
			const int size= childs.size();
			for (int i= 0; i < size; ++i)
			{
                GLC_StructOccurrence* pChild= childs.at(i)->clone(m_pWorldHandle, true);
				addChild(pChild);
			}

			QList<GLC_StructInstance*> instances= pRef->listOfStructInstances();
			const int instanceCount= instances.size();
			int i= 0;
			bool continu= true;
			while (continu && (i < instanceCount))
			{
				if (m_pStructInstance == instances.at(i))
				{
					continu= false;
					delete m_pNumberOfOccurence;
					m_pNumberOfOccurence= instances.at(i)->firstOccurenceHandle()->m_pNumberOfOccurence;
					++(*m_pNumberOfOccurence);
				}
				++i;
			}
		}
	}

	m_pStructInstance->setReference(pRef);
}

void GLC_StructOccurrence::makeFlexible(const GLC_Matrix4x4& relativeMatrix)
{
	delete m_pRelativeMatrix;
	m_pRelativeMatrix= new GLC_Matrix4x4(relativeMatrix);

	updateChildrenAbsoluteMatrix();
}

void GLC_StructOccurrence::makeRigid()
{
	delete m_pRelativeMatrix;
	m_pRelativeMatrix= NULL;

	updateChildrenAbsoluteMatrix();
}

void GLC_StructOccurrence::swap(int i, int j)
{
	Q_ASSERT(i != j);

	GLC_StructReference* pRef= this->structReference();
    QList<GLC_StructOccurrence*> occurences= pRef->listOfStructOccurence();
	const int count= occurences.count();
	for (int i= 0; i < count; ++i)
	{
        GLC_StructOccurrence* pOcc= occurences.at(i);
		Q_ASSERT(i <= pOcc->m_Childs.count());
		Q_ASSERT(j <= pOcc->m_Childs.count());
		pOcc->m_Childs.swap(i, j);
	}

}

//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////

void GLC_StructOccurrence::detach()
{
	if (NULL != m_pWorldHandle)
	{
		// retrieve renderProperties if needed
		if (m_pWorldHandle->collection()->contains(m_Uid))
		{
			GLC_3DViewInstance* pInstance= m_pWorldHandle->collection()->instanceHandle(m_Uid);
			if (!pInstance->renderPropertiesHandle()->isDefault())
			{
				Q_ASSERT(NULL == m_pRenderProperties);
				m_pRenderProperties= new GLC_RenderProperties(*(pInstance->renderPropertiesHandle()));
			}
		}
		m_pWorldHandle->removeOccurence(this);
		m_pWorldHandle= NULL;
		if (!m_Childs.isEmpty())
		{
			const int size= m_Childs.size();
			for (int i= 0; i < size; ++i)
			{
				m_Childs[i]->detach();
			}
		}
	}
}

void GLC_StructOccurrence::doCreateOccurrenceFromInstance(GLuint shaderId)
{
	// Update the number of occurences
	if (m_pStructInstance->hasStructOccurence())
	{
        GLC_StructOccurrence* pFirstOccurence= m_pStructInstance->firstOccurenceHandle();
		m_pNumberOfOccurence= pFirstOccurence->m_pNumberOfOccurence;
		++(*m_pNumberOfOccurence);
        QList<GLC_StructOccurrence*> childs= pFirstOccurence->m_Childs;
		const int size= childs.size();
		for (int i= 0; i < size; ++i)
		{
            GLC_StructOccurrence* pChild= childs.at(i)->clone(m_pWorldHandle, true);
			addChild(pChild);
		}
	}
	else
	{
		m_pNumberOfOccurence= new int(1);
	}

	// Inform the world Handle
	if (NULL != m_pWorldHandle)
	{
		m_pWorldHandle->addOccurence(this, shaderId);
	}

	// Update Absolute matrix
	updateAbsoluteMatrix();

	// Update instance
	m_pStructInstance->structOccurenceCreated(this);
}
