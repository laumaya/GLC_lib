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

//! \file glc_structoccurrence.cpp implementation of the GLC_StructOccurrence class.

#include "glc_structoccurrence.h"
#include "glc_3dviewcollection.h"
#include "glc_structreference.h"
#include "glc_worldhandle.h"
#include "../glc_errorlog.h"

GLC_StructOccurrence::GLC_StructOccurrence()
: m_Uid(glc::GLC_GenID())
, m_pWorldHandle(NULL)
, m_pNumberOfOccurrence(new int(1))
, m_pStructInstance(new GLC_StructInstance())
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurrenceNumber(0)
, m_IsVisible(true)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(true)
, m_pRelativeMatrix(NULL)
{
	// Update instance
	m_pStructInstance->structOccurrenceCreated(this);
}

GLC_StructOccurrence::GLC_StructOccurrence(GLC_StructInstance* pStructInstance, GLC_WorldHandle* pWorldHandle, GLuint shaderId)
: m_Uid(glc::GLC_GenID())
, m_pWorldHandle(pWorldHandle)
, m_pNumberOfOccurrence(NULL)
, m_pStructInstance(pStructInstance)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurrenceNumber(0)
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
, m_pNumberOfOccurrence(NULL)
, m_pStructInstance(pStructInstance)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurrenceNumber(0)
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
, m_pNumberOfOccurrence(new int(1))
, m_pStructInstance(NULL)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurrenceNumber(0)
, m_IsVisible(true)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(true)
, m_pRelativeMatrix(NULL)
{
	m_pStructInstance= new GLC_StructInstance(pRep);

	// Update instance
	m_pStructInstance->structOccurrenceCreated(this);
}

GLC_StructOccurrence::GLC_StructOccurrence(GLC_3DRep* pRep, GLC_uint id)
: m_Uid(id)
, m_pWorldHandle(NULL)
, m_pNumberOfOccurrence(new int(1))
, m_pStructInstance(NULL)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_OccurrenceNumber(0)
, m_IsVisible(true)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(true)
, m_pRelativeMatrix(NULL)
{
	m_pStructInstance= new GLC_StructInstance(pRep);

	// Update instance
	m_pStructInstance->structOccurrenceCreated(this);
}

GLC_StructOccurrence::GLC_StructOccurrence(GLC_WorldHandle* pWorldHandle, const GLC_StructOccurrence& structOccurrence, bool shareInstance)
: m_Uid(glc::GLC_GenID())
, m_pWorldHandle(pWorldHandle)
, m_pNumberOfOccurrence(NULL)
, m_pStructInstance(NULL)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix(structOccurrence.m_AbsoluteMatrix)
, m_OccurrenceNumber(0)
, m_IsVisible(structOccurrence.m_IsVisible)
, m_pRenderProperties(NULL)
, m_AutomaticCreationOf3DViewInstance(structOccurrence.m_AutomaticCreationOf3DViewInstance)
, m_pRelativeMatrix(NULL)
{
	if (shareInstance)
	{
		m_pStructInstance= structOccurrence.m_pStructInstance;
		m_pNumberOfOccurrence= structOccurrence.m_pNumberOfOccurrence;
		++(*m_pNumberOfOccurrence);
	}
	else
	{
		m_pNumberOfOccurrence= new int(1);
		m_pStructInstance= new GLC_StructInstance(structOccurrence.m_pStructInstance);
	}


	// Test if structOccurrence has representation and has a shader
	GLuint shaderId= 0;
	bool instanceIsSelected= false;
	if ((NULL != m_pWorldHandle) && (NULL != structOccurrence.m_pWorldHandle) && structOccurrence.m_pWorldHandle->collection()->contains(structOccurrence.id()))
	{
		GLC_3DViewInstance* p3DViewInstance= structOccurrence.m_pWorldHandle->collection()->instanceHandle(structOccurrence.id());

		if(structOccurrence.m_pWorldHandle->collection()->isInAShadingGroup(structOccurrence.id()))
		{
			shaderId= structOccurrence.m_pWorldHandle->collection()->shadingGroup(structOccurrence.id());
		}

		instanceIsSelected= p3DViewInstance->isSelected();
		// Test the rendering properties
		if (! p3DViewInstance->renderPropertiesHandle()->isDefault())
		{
			m_pRenderProperties= new GLC_RenderProperties(*(p3DViewInstance->renderPropertiesHandle()));
		}
	}
	else if (NULL != structOccurrence.m_pRenderProperties)
	{
		m_pRenderProperties= new GLC_RenderProperties(*(structOccurrence.m_pRenderProperties));
	}

	// Inform the world Handle
	if (NULL != m_pWorldHandle)
	{
		m_pWorldHandle->addOccurrence(this, instanceIsSelected, shaderId);
		if (NULL != m_pRenderProperties && this->has3DViewInstance())
		{
			m_pWorldHandle->collection()->instanceHandle(id())->setRenderProperties(*m_pRenderProperties);
			delete m_pRenderProperties;
			m_pRenderProperties= NULL;
		}
	}

	// Check flexibility
	if (NULL != structOccurrence.m_pRelativeMatrix)
	{
		m_pRelativeMatrix= new GLC_Matrix4x4(*(structOccurrence.m_pRelativeMatrix));
	}

	// Update Absolute matrix
	updateAbsoluteMatrix();


	// Create childs
	const int size= structOccurrence.childCount();
	for (int i= 0; i < size; ++i)
	{
        GLC_StructOccurrence* pChild= structOccurrence.child(i)->clone(m_pWorldHandle, true);
		addChild(pChild);
	}
	updateChildrenAbsoluteMatrix();
	// Update instance
	m_pStructInstance->structOccurrenceCreated(this);
}

GLC_StructOccurrence::~GLC_StructOccurrence()
{
	//qDebug() << "Delete " << id();
	Q_ASSERT(m_pNumberOfOccurrence != NULL);

	// Remove Childs
	const int size= m_Childs.size();
	for (int i= 0; i < size; ++i)
	{
        GLC_StructOccurrence* pChild= m_Childs.first();
		removeChild(pChild);
		delete pChild;
	}

    makeOrphan();

	// Update number of occurrence and instance
	if ((--(*m_pNumberOfOccurrence)) == 0)
	{
		delete m_pStructInstance;
		delete m_pNumberOfOccurrence;
	}
	else
	{
		m_pStructInstance->structOccurrenceDeleted(this);
		if (!m_pStructInstance->hasStructOccurrence())
		{

			QStringList errorList;
			errorList << "StructOccurrence count error";
			errorList << ("ref name = " + m_pStructInstance->structReference()->name());
			GLC_ErrorLog::addError(errorList);

			delete m_pStructInstance;
			//delete m_pNumberOfOccurrence;
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

bool GLC_StructOccurrence::canBeAddedToChildren(GLC_StructOccurrence* pOccurrence) const
{
	bool canBeAdded= false;
	if ((NULL != m_pStructInstance) && (m_pStructInstance->hasStructOccurrence()) && (NULL != pOccurrence->m_pStructInstance) && (NULL != pOccurrence->structReference()))
	{
		if (this->structReference() != pOccurrence->structReference())
		{
            QSet<GLC_StructReference*> thisRefSet= GLC_StructOccurrence::parentsReferences(this);
			thisRefSet << this->structReference();
			QSet<GLC_StructReference*> childRefSet= pOccurrence->childrenReferences();

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

QList<GLC_StructOccurrence*> GLC_StructOccurrence::subOccurrenceList() const
{
    QList<GLC_StructOccurrence*> subOccurrence;
	const int childCount= m_Childs.size();
	for (int i= 0; i < childCount; ++i)
	{
        GLC_StructOccurrence* pCurrentChild= m_Childs.at(i);
		subOccurrence.append(pCurrentChild);
		if (pCurrentChild->hasChild())
		{
			subOccurrence.append(pCurrentChild->subOccurrenceList());
		}
	}

	return subOccurrence;
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

QSet<GLC_StructReference*> GLC_StructOccurrence::parentsReferences(const GLC_StructOccurrence* pOccurrence)
{
	QSet<GLC_StructReference*> parentSet;
    GLC_StructOccurrence* pParent= pOccurrence->parent();
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

GLC_OccurencePath GLC_StructOccurrence::path() const
{
    GLC_OccurencePath subject;
    QList<GLC_StructOccurrence*> listOfAncestor= ancestorList();
    listOfAncestor.prepend(const_cast<GLC_StructOccurrence*>(this));

    const int count= listOfAncestor.count();
    for (int i= (count - 2); i >= 0; --i) // Exclude root
    {
        GLC_StructOccurrence* pOcc= listOfAncestor.at(i);
        QPair<QString, uint> currentPathNode(pOcc->structReference()->name(), pOcc->parent()->indexOf(pOcc));
        subject.append(currentPathNode);
    }

    return subject;
}

GLC_StructOccurrence *GLC_StructOccurrence::occurrenceFromPath(GLC_OccurencePath path) const
{
    Q_ASSERT(!path.isEmpty());
    GLC_StructOccurrence* pSubject= NULL;

    QPair<QString, uint> firstNode= path.takeFirst();
    const int index= static_cast<int>(firstNode.second);
    const QString name= firstNode.first;
    if (m_Childs.count() > index)
    {
        GLC_StructOccurrence* pChild= m_Childs.at(index);
        if (pChild->structReference()->name() == name)
        {
            if (path.isEmpty())
            {
                pSubject= pChild;
            }
            else
            {
                pSubject= pChild->occurrenceFromPath(path);
            }
        }
    }

    return pSubject;
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
	// If the occurrence have a representation, update it.

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

    pChild->setVisibility(this->isVisible());
	m_Childs.append(pChild);
	pChild->m_pParent= this;
	if (NULL == pChild->m_pWorldHandle)
	{
		pChild->setWorldHandle(m_pWorldHandle);
	}

    if ((NULL != m_pWorldHandle) && m_pWorldHandle->selectionSetHandle()->contains(this))
    {
        m_pWorldHandle->select(pChild->id());
    }

	pChild->updateChildrenAbsoluteMatrix();
}

void GLC_StructOccurrence::insertChild(int index, GLC_StructOccurrence* pChild)
{
	Q_ASSERT(pChild->isOrphan());
	Q_ASSERT((NULL == pChild->m_pWorldHandle) || (m_pWorldHandle == pChild->m_pWorldHandle));
	Q_ASSERT(m_Childs.count() >= index);

    pChild->setVisibility(this->isVisible());

	//qDebug() << "Add Child " << pChild->name() << "id=" << pChild->id() << " to " << name() << " id=" << id();
	// Add the child to the list of child
	// Get occurrence reference
	m_Childs.insert(index, pChild);
	pChild->m_pParent= this;
	if (NULL == pChild->m_pWorldHandle)
	{
		pChild->setWorldHandle(m_pWorldHandle);
	}

    if ((NULL != m_pWorldHandle) && m_pWorldHandle->selectionSetHandle()->contains(this))
    {
        m_pWorldHandle->select(pChild->id());
    }

	pChild->updateChildrenAbsoluteMatrix();
}

GLC_StructOccurrence* GLC_StructOccurrence::addChild(GLC_StructInstance* pInstance)
{
    GLC_StructOccurrence* pOccurrence;
    pOccurrence= new GLC_StructOccurrence(pInstance, m_pWorldHandle);

	addChild(pOccurrence);

	return pOccurrence;
}

GLC_StructOccurrence* GLC_StructOccurrence::insertChild(int index, GLC_StructInstance* pInstance)
{
    GLC_StructOccurrence* pOccurrence;
    pOccurrence= new GLC_StructOccurrence(pInstance, m_pWorldHandle);

	insertChild(index, pOccurrence);

	return pOccurrence;
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

// Set the occurrence world Handle
void GLC_StructOccurrence::setWorldHandle(GLC_WorldHandle* pWorldHandle)
{
	// Check if world handles are equal
	if (m_pWorldHandle == pWorldHandle) return;

	if (NULL != m_pWorldHandle)
	{
		m_pWorldHandle->removeOccurrence(this);
	}

	m_pWorldHandle= pWorldHandle;

	if (NULL != m_pWorldHandle)
	{
		m_pWorldHandle->addOccurrence(this);
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
        if (pRef->representationIsLoaded() && has3DViewInstance())
		{
            // Save rendering properties of the 3DView instance of this occurrence
            GLC_RenderProperties savedRenderingProperties(*(m_pWorldHandle->collection()->instanceHandle(m_Uid)->renderPropertiesHandle()));
            savedRenderingProperties.unselect();

            // Remove this occurence 3DVIew instance
            unloadResult= m_pWorldHandle->collection()->remove(m_Uid);

            // Check if there is another Occurrence with the same representation
            QSet<GLC_StructOccurrence*> occurrenceSet= pRef->setOfStructOccurrence();
            QSet<GLC_StructOccurrence*>::const_iterator iOcc= occurrenceSet.constBegin();
            bool unloadReferenceRep= true;
            while (occurrenceSet.constEnd() != iOcc)
            {
                unloadReferenceRep= unloadReferenceRep && !(*iOcc)->has3DViewInstance();
                ++iOcc;
            }

            if (unloadReferenceRep)
            {
                pRef->unloadRepresentation();
            }

            // Assign the rendering properties of the instance to this Occurrence
            this->setRenderProperties(savedRenderingProperties, false);
        }
	}
	return unloadResult;
}

unsigned int GLC_StructOccurrence::updateOccurrenceNumber(unsigned int n)
{
	m_OccurrenceNumber= n++;
	const int childCount= m_Childs.size();
	for (int i= 0; i < childCount; ++i)
	{
		n= m_Childs[i]->updateOccurrenceNumber(n);
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

void GLC_StructOccurrence::setRenderProperties(const GLC_RenderProperties& renderProperties, bool propagate)
{
    delete m_pRenderProperties;
	m_pRenderProperties= NULL;
	if (has3DViewInstance())
	{
		m_pWorldHandle->collection()->instanceHandle(m_Uid)->setRenderProperties(renderProperties);
	}

    if (propagate && hasChild())
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

void GLC_StructOccurrence::removeRenderProperties()
{
    delete m_pRenderProperties;
    m_pRenderProperties= NULL;
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
	Q_ASSERT((*m_pNumberOfOccurrence) == 1);

	if (pRef->hasStructInstance())
	{
		GLC_StructInstance* pExistingInstance= pRef->firstInstanceHandle();
		if (pExistingInstance->hasStructOccurrence())
		{
            GLC_StructOccurrence* pFirstOccurrence= pExistingInstance->firstOccurrenceHandle();
            QList<GLC_StructOccurrence*> childs= pFirstOccurrence->m_Childs;
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
					delete m_pNumberOfOccurrence;
					m_pNumberOfOccurrence= instances.at(i)->firstOccurrenceHandle()->m_pNumberOfOccurrence;
					++(*m_pNumberOfOccurrence);
				}
				++i;
			}
		}
	}

	m_pStructInstance->setReference(pRef);
}

void GLC_StructOccurrence::makeFlexible(const GLC_Matrix4x4& relativeMatrix, bool update)
{
	delete m_pRelativeMatrix;
	m_pRelativeMatrix= new GLC_Matrix4x4(relativeMatrix);

    if (update) updateChildrenAbsoluteMatrix();
}

void GLC_StructOccurrence::makeRigid(bool update)
{
	delete m_pRelativeMatrix;
	m_pRelativeMatrix= NULL;

    if (update) updateChildrenAbsoluteMatrix();
}

void GLC_StructOccurrence::swap(int oldPos, int newPos)
{
    Q_ASSERT(oldPos != newPos);

	GLC_StructReference* pRef= this->structReference();
    QList<GLC_StructOccurrence*> occurrences= pRef->listOfStructOccurrence();
	const int count= occurrences.count();
	for (int i= 0; i < count; ++i)
	{
        GLC_StructOccurrence* pOcc= occurrences.at(i);
        Q_ASSERT(oldPos <= pOcc->m_Childs.count());
        Q_ASSERT(newPos <= pOcc->m_Childs.count());
        pOcc->m_Childs.swap(oldPos, newPos);
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
		m_pWorldHandle->removeOccurrence(this);
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
	// Update the number of occurrences
	if (m_pStructInstance->hasStructOccurrence())
	{
        GLC_StructOccurrence* pFirstOccurrence= m_pStructInstance->firstOccurrenceHandle();
		m_pNumberOfOccurrence= pFirstOccurrence->m_pNumberOfOccurrence;
		++(*m_pNumberOfOccurrence);
        QList<GLC_StructOccurrence*> childs= pFirstOccurrence->m_Childs;
		const int size= childs.size();
		for (int i= 0; i < size; ++i)
		{
            GLC_StructOccurrence* pChild= childs.at(i)->clone(m_pWorldHandle, true);
			addChild(pChild);
		}
	}
	else
	{
		m_pNumberOfOccurrence= new int(1);
	}

	// Inform the world Handle
	if (NULL != m_pWorldHandle)
	{
		m_pWorldHandle->addOccurrence(this, shaderId);
	}

	// Update Absolute matrix
	updateAbsoluteMatrix();

	// Update instance
	m_pStructInstance->structOccurrenceCreated(this);
}
