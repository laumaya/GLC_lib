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

#include <QSet>

#include "glc_worldhandle.h"
#include "glc_structreference.h"
#include "../glc_selectionevent.h"

GLC_WorldHandle::GLC_WorldHandle()
: m_Collection()
, m_pRoot(new GLC_StructOccurrence())
, m_Ref(1)
, m_OccurrenceHash()
, m_UpVector(glc::Z_AXIS)
, m_SelectionSet(this)
{
    m_pRoot->setWorldHandle(this);
}

GLC_WorldHandle::GLC_WorldHandle(GLC_StructOccurrence *pOcc)
    : m_Collection()
    , m_pRoot(pOcc)
    , m_Ref(1)
    , m_OccurrenceHash()
    , m_UpVector(glc::Z_AXIS)
    , m_SelectionSet(this)
{
    Q_ASSERT(pOcc->isOrphan());
    pOcc->setWorldHandle(this);
}

GLC_WorldHandle::~GLC_WorldHandle()
{
    delete m_pRoot;
}

// Return the list of instance
QList<GLC_StructInstance*> GLC_WorldHandle::instances() const
{
	QSet<GLC_StructInstance*> instancesSet;
    QHash<GLC_uint, GLC_StructOccurrence*>::const_iterator iOccurrence= m_OccurrenceHash.constBegin();
    while (iOccurrence != m_OccurrenceHash.constEnd())
	{
        instancesSet.insert(iOccurrence.value()->structInstance());
        ++iOccurrence;
	}
	return instancesSet.toList();
}

// Return the list of Reference
QList<GLC_StructReference*> GLC_WorldHandle::references() const
{
	QSet<GLC_StructReference*> referencesSet;
    QHash<GLC_uint, GLC_StructOccurrence*>::const_iterator iOccurrence= m_OccurrenceHash.constBegin();
    while (iOccurrence != m_OccurrenceHash.constEnd())
	{
        referencesSet.insert(iOccurrence.value()->structReference());
        ++iOccurrence;
	}
	return referencesSet.toList();
}

// Return the number of body
int GLC_WorldHandle::numberOfBody() const
{
	QList<GLC_StructReference*> referenceList(references());
	const int size= referenceList.size();
	int numberOfBody= 0;
	for (int i= 0; i < size; ++i)
	{
		numberOfBody+= referenceList.at(i)->numberOfBody();
	}
	return numberOfBody;
}

int GLC_WorldHandle::representationCount() const
{
	QList<GLC_StructReference*> referenceList(references());
	const int size= referenceList.size();
	int count= 0;
	for (int i= 0; i < size; ++i)
	{
		if (referenceList.at(i)->hasRepresentation()) ++count;
	}
	return count;

}

GLC_StructOccurrence *GLC_WorldHandle::occurrenceFromPath(GLC_OccurencePath path) const
{
    return m_pRoot->occurrenceFromPath(path);
}

void GLC_WorldHandle::replaceRootOccurrence(GLC_StructOccurrence *pOcc)
{
    Q_ASSERT(pOcc->isOrphan());
    delete m_pRoot;
    m_pRoot= pOcc;
    m_pRoot->setWorldHandle(this);
}

GLC_StructOccurrence *GLC_WorldHandle::takeRootOccurrence()
{
    GLC_StructOccurrence* pSubject= m_pRoot;
    pSubject->makeOrphan();

    m_pRoot= new GLC_StructOccurrence();
    m_pRoot->setWorldHandle(this);

    return pSubject;
}

// An Occurrence has been added
void GLC_WorldHandle::addOccurrence(GLC_StructOccurrence* pOccurrence, bool isSelected, GLuint shaderId)
{
    Q_ASSERT(!m_OccurrenceHash.contains(pOccurrence->id()));
    m_OccurrenceHash.insert(pOccurrence->id(), pOccurrence);
    GLC_StructReference* pRef= pOccurrence->structReference();
	Q_ASSERT(NULL != pRef);

	// Add instance representation in the collection
    if (pOccurrence->useAutomatic3DViewInstanceCreation() && pRef->representationIsLoaded())
	{
        pOccurrence->create3DViewInstance(shaderId);
        if (isSelected) select(pOccurrence->id());
	}
}

// An Occurrence has been removed
void GLC_WorldHandle::removeOccurrence(GLC_StructOccurrence* pOccurrence)
{
    Q_ASSERT(m_OccurrenceHash.contains(pOccurrence->id()));
    // Remove the occurrence from the selection set
    m_SelectionSet.remove(pOccurrence);
    // Remove the occurrence from the main occurrence hash table
    m_OccurrenceHash.remove(pOccurrence->id());
	// Remove instance representation from the collection
    m_Collection.remove(pOccurrence->id());

    if (pOccurrence == m_pRoot)
    {
        m_pRoot= new GLC_StructOccurrence;
        m_pRoot->setWorldHandle(this);
    }
}

void GLC_WorldHandle::select(GLC_uint occurrenceId)
{
    Q_ASSERT(m_OccurrenceHash.contains(occurrenceId));
    m_SelectionSet.insert(occurrenceId);
    m_Collection.select(occurrenceId);

    const GLC_StructOccurrence* pSelectedOccurrence= m_OccurrenceHash.value(occurrenceId);
    if (pSelectedOccurrence->hasChild())
	{
        QList<GLC_StructOccurrence*> subOccurrenceList= pSelectedOccurrence->subOccurrenceList();
        const int subOccurrenceCount= subOccurrenceList.size();
        for (int i= 0; i < subOccurrenceCount; ++i)
		{
            const GLC_uint currentOccurrenceId= subOccurrenceList.at(i)->id();
            m_SelectionSet.insert(currentOccurrenceId);
            m_Collection.select(currentOccurrenceId);
		}
    }
}

void GLC_WorldHandle::updateSelection(const GLC_SelectionEvent &selectionEvent)
{
    const GLC_SelectionEvent::Modes selectionModes= selectionEvent.modes();

    if (selectionModes & GLC_SelectionEvent::ModeReplace)
    {
        m_SelectionSet= selectionEvent.selectionSet();
    }
    else if (selectionModes & GLC_SelectionEvent::ModeSubstract)
    {
        m_SelectionSet.substract(selectionEvent.selectionSet());
    }
    else if (selectionModes & GLC_SelectionEvent::ModeExclusiveUnit)
    {
        m_SelectionSet.exclusiveUnite(selectionEvent.selectionSet());
    }

    updateSelectedInstanceFromSelectionSet();
}

void GLC_WorldHandle::unselect(GLC_uint occurrenceId, bool propagate)
{
    Q_ASSERT(m_OccurrenceHash.contains(occurrenceId));
    m_SelectionSet.remove(occurrenceId);
    m_Collection.unselect(occurrenceId);

    const GLC_StructOccurrence* pSelectedOccurrence= m_OccurrenceHash.value(occurrenceId);
    if (propagate && pSelectedOccurrence->hasChild())
	{
        QList<GLC_StructOccurrence*> subOccurrenceList= pSelectedOccurrence->subOccurrenceList();
        const int subOccurrenceCount= subOccurrenceList.size();
        for (int i= 0; i < subOccurrenceCount; ++i)
		{
            const GLC_uint currentOccurrenceId= subOccurrenceList.at(i)->id();
            m_Collection.unselect(currentOccurrenceId);
            m_SelectionSet.remove(currentOccurrenceId);
		}
	}
}

void GLC_WorldHandle::selectAllWith3DViewInstance(bool allShowState)
{
	m_Collection.selectAll(allShowState);
	QList<GLC_uint> selectedId= m_Collection.selection()->keys();
	m_SelectionSet.clear();
	const int selectionCount= selectedId.count();
	for (int i= 0; i < selectionCount; ++i)
	{
		m_SelectionSet.insert(selectedId.at(i));
	}
}

void GLC_WorldHandle::unselectAll()
{
	m_SelectionSet.clear();
	m_Collection.unselectAll();
}

void GLC_WorldHandle::showHideSelected3DViewInstance()
{
	QList<GLC_3DViewInstance*> selected3dviewInstance= m_Collection.selection()->values();
	const int instanceCount= selected3dviewInstance.count();
	for(int i= 0; i < instanceCount; ++i)
	{
		GLC_3DViewInstance* pCurrentInstance= selected3dviewInstance.at(i);
		pCurrentInstance->setVisibility(!pCurrentInstance->isVisible());
	}
}

void GLC_WorldHandle::setSelected3DViewInstanceVisibility(bool isVisible)
{
	QList<GLC_3DViewInstance*> selected3dviewInstance= m_Collection.selection()->values();
	const int instanceCount= selected3dviewInstance.count();
	for(int i= 0; i < instanceCount; ++i)
	{
		GLC_3DViewInstance* pCurrentInstance= selected3dviewInstance.at(i);
		pCurrentInstance->setVisibility(isVisible);
    }
}

void GLC_WorldHandle::updateSelectedInstanceFromSelectionSet()
{
    m_Collection.unselectAll();
    OccurrenceSelection occurrenceSelection= m_SelectionSet.occurrenceSelection();
    OccurrenceSelection::const_iterator iOccSel= occurrenceSelection.constBegin();
    while (iOccSel != occurrenceSelection.constEnd())
    {
        const GLC_uint occId= iOccSel.key();
        Q_ASSERT(m_OccurrenceHash.contains(occId));
        m_Collection.select(occId);

        const GLC_StructOccurrence* pSelectedOccurrence= m_OccurrenceHash.value(occId);
        QList<GLC_StructOccurrence*> subOccurrenceList= pSelectedOccurrence->subOccurrenceList();
        const int subOccurrenceCount= subOccurrenceList.size();
        for (int i= 0; i < subOccurrenceCount; ++i)
        {
            const GLC_uint currentOccurrenceId= subOccurrenceList.at(i)->id();
            if (m_Collection.contains(currentOccurrenceId))
            {
                m_Collection.select(currentOccurrenceId);
            }
        }
        ++iOccSel;
    }
}

