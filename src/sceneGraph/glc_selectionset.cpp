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
//! \file glc_selectionset.cpp implementation for the GLC_SelectionSet class.

#include "glc_selectionset.h"
#include "glc_worldhandle.h"
#include "glc_world.h"

GLC_SelectionSet::GLC_SelectionSet()
    : m_pWorldHandle(NULL)
    , m_OccurrenceSelection()
{

}

GLC_SelectionSet::GLC_SelectionSet(GLC_WorldHandle* pWorldHandle)
    : m_pWorldHandle(pWorldHandle)
    , m_OccurrenceSelection()
{
    Q_ASSERT(0 == m_pWorldHandle->collection()->selectionSize());

}

GLC_SelectionSet::GLC_SelectionSet(GLC_World &world)
    : m_pWorldHandle(world.worldHandle())
    , m_OccurrenceSelection()
{

}

GLC_SelectionSet::GLC_SelectionSet(const GLC_SelectionSet &other)
    : m_pWorldHandle(other.m_pWorldHandle)
    , m_OccurrenceSelection(other.m_OccurrenceSelection)
{

}

GLC_SelectionSet::~GLC_SelectionSet()
{

}

bool GLC_SelectionSet::belongsToSameWorldHandle(const GLC_SelectionSet &selectionSet) const
{
    bool subject= m_pWorldHandle == selectionSet.m_pWorldHandle;

    return subject;
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

bool GLC_SelectionSet::isEmpty() const
{
    return m_OccurrenceSelection.isEmpty();
}


int GLC_SelectionSet::count() const
{
    return m_OccurrenceSelection.size();
}

long GLC_SelectionSet::bodyCount() const
{
    long subject= 0;
    OccurrenceSelection::const_iterator iOcc= m_OccurrenceSelection.constBegin();
    while (iOcc != m_OccurrenceSelection.constEnd())
    {
        if (!iOcc.value().isEmpty())
        {
            subject+= iOcc.value().count();
        }
        else
        {
            const GLC_uint currentId= iOcc.key();
            Q_ASSERT(m_pWorldHandle->collection()->contains(currentId));
            GLC_3DViewInstance* pInstance= m_pWorldHandle->collection()->instanceHandle(currentId);
            subject+= pInstance->numberOfBody();
        }
        ++iOcc;
    }

    return subject;
}

long GLC_SelectionSet::primitiveCount() const
{
    long subject= 0;
    OccurrenceSelection::const_iterator iOcc= m_OccurrenceSelection.constBegin();
    while (iOcc != m_OccurrenceSelection.constEnd())
    {
        if (!iOcc.value().isEmpty())
        {
            BodySelection bodySelection= iOcc.value();
            BodySelection::const_iterator iBody= bodySelection.constBegin();
            while (iBody != bodySelection.constEnd())
            {
                if (!iBody.value().isEmpty())
                {
                    subject+= iBody.value().count();
                }
                else
                {
                    const GLC_uint currentOccId= iOcc.key();
                    Q_ASSERT(m_pWorldHandle->collection()->contains(currentOccId));
                    GLC_3DViewInstance* pInstance= m_pWorldHandle->collection()->instanceHandle(currentOccId);
                    GLC_3DRep rep= pInstance->representation();
                    GLC_Geometry* pGeom= rep.geomOfId(iBody.key());
                    Q_ASSERT(NULL != pGeom);
                    subject+= pGeom->primitiveCount();
                }
                ++iBody;
            }
        }
        else
        {
            const GLC_uint currentId= iOcc.key();
            Q_ASSERT(m_pWorldHandle->collection()->contains(currentId));
            GLC_3DViewInstance* pInstance= m_pWorldHandle->collection()->instanceHandle(currentId);
            const int bodyCount= pInstance->numberOfBody();
            for (int i= 0; i < bodyCount; ++i)
            {
                subject+= pInstance->geomAt(i)->primitiveCount();
            }
        }

        ++iOcc;
    }

    return subject;
}

QList<GLC_StructOccurrence*> GLC_SelectionSet::occurrencesList() const
{
    QList<GLC_StructOccurrence*> subject;
    if (m_pWorldHandle)
    {
        OccurrenceSelection::const_iterator iOcc= m_OccurrenceSelection.constBegin();
        while (iOcc != m_OccurrenceSelection.constEnd())
        {
            const GLC_uint id= iOcc.key();
            Q_ASSERT(m_pWorldHandle->containsOccurence(id));
            subject.append(m_pWorldHandle->getOccurence(id));
        }
    }

    return subject;
}

bool GLC_SelectionSet::contains(GLC_uint occId, GLC_uint bodyId)
{
    bool subject= m_OccurrenceSelection.contains(occId);
    subject= subject && (m_OccurrenceSelection.value(occId).contains(bodyId));

    return subject;
}

bool GLC_SelectionSet::contains(GLC_uint occId, GLC_uint bodyId, GLC_uint primitiveId)
{
    bool subject= m_OccurrenceSelection.contains(occId);
    subject= subject && (m_OccurrenceSelection.value(occId).contains(bodyId));
    subject= subject && (m_OccurrenceSelection.value(occId).value(bodyId).contains(primitiveId));

    return subject;
}

bool GLC_SelectionSet::operator==(const GLC_SelectionSet &other) const
{
    bool subject= this == &other;
    subject= subject && (m_pWorldHandle == other.m_pWorldHandle);
    subject= subject && (m_OccurrenceSelection == other.m_OccurrenceSelection);

    return subject;
}

void GLC_SelectionSet::setAttachedWorld(GLC_World &world)
{
    clear();
    m_pWorldHandle= world.worldHandle();
}

GLC_SelectionSet &GLC_SelectionSet::operator=(const GLC_SelectionSet &other)
{
    if (this->operator!=(other))
    {
        m_pWorldHandle= other.m_pWorldHandle;
        m_OccurrenceSelection= other.m_OccurrenceSelection;
    }

    return *this;
}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
bool GLC_SelectionSet::insert(GLC_StructOccurrence* pOccurence)
{
    return insert(pOccurence->id());
}

bool GLC_SelectionSet::insert(GLC_uint occurenceId)
{
    Q_ASSERT(m_pWorldHandle->containsOccurence(occurenceId));
    if (!contains(occurenceId))
    {
        Q_ASSERT(!m_OccurrenceSelection.contains(occurenceId));
        m_OccurrenceSelection.insert(occurenceId, BodySelection());
        return true;
    }
    else return false;
}

bool GLC_SelectionSet::insert(GLC_uint occurenceId, GLC_uint bodyId)
{
    bool subject= false;
    Q_ASSERT(m_pWorldHandle->containsOccurence(occurenceId));
    if (!contains(occurenceId))
    {
        Q_ASSERT(!m_OccurrenceSelection.contains(occurenceId));
        BodySelection bodySelection;
        bodySelection.insert(bodyId, PrimitiveSelection());
        m_OccurrenceSelection.insert(occurenceId, bodySelection);
        subject= true;
    }
    else
    {
        Q_ASSERT(m_OccurrenceSelection.contains(occurenceId));
        if (!m_OccurrenceSelection.value(occurenceId).contains(bodyId))
        {
            m_OccurrenceSelection[occurenceId].insert(bodyId, PrimitiveSelection());
            subject= true;
        }
    }

    return subject;
}

bool GLC_SelectionSet::insert(GLC_uint occurenceId, GLC_uint bodyId, GLC_uint primitiveId)
{
    bool subject= false;
    Q_ASSERT(m_pWorldHandle->containsOccurence(occurenceId));
    if (!contains(occurenceId))
    {
        Q_ASSERT(!m_OccurrenceSelection.contains(occurenceId));
        PrimitiveSelection primitiveSelection;
        primitiveSelection.insert(primitiveId);
        BodySelection bodySelection;
        bodySelection.insert(bodyId, primitiveSelection);
        m_OccurrenceSelection.insert(occurenceId, bodySelection);
        subject= true;
    }
    else
    {
        Q_ASSERT(m_OccurrenceSelection.contains(occurenceId));
        if (!m_OccurrenceSelection.value(occurenceId).contains(bodyId))
        {
            PrimitiveSelection primitiveSelection;
            primitiveSelection.insert(primitiveId);
            m_OccurrenceSelection[occurenceId].insert(bodyId, primitiveSelection);
            subject= true;
        }
        else
        {
            if (!m_OccurrenceSelection.value(occurenceId).value(bodyId).contains(primitiveId))
            {
                (m_OccurrenceSelection[occurenceId])[bodyId].insert(primitiveId);
                subject= true;
            }
        }
    }

    return subject;
}

bool GLC_SelectionSet::remove(GLC_StructOccurrence* pOccurence)
{
    return remove(pOccurence->id());
}

bool GLC_SelectionSet::remove(GLC_uint occurenceId)
{
    bool subject= false;
    Q_ASSERT(m_pWorldHandle->containsOccurence(occurenceId));
    if (contains(occurenceId))
    {
        m_OccurrenceSelection.remove(occurenceId);
        subject= true;
    }

    return subject;
}

bool GLC_SelectionSet::remove(GLC_uint occurenceId, GLC_uint bodyId)
{
    bool subject= false;
    Q_ASSERT(m_pWorldHandle->containsOccurence(occurenceId));
    if (contains(occurenceId, bodyId))
    {
        m_OccurrenceSelection[occurenceId].remove(bodyId);
        subject= true;
    }
    return subject;
}

bool GLC_SelectionSet::remove(GLC_uint occurenceId, GLC_uint bodyId, GLC_uint primitiveId)
{
    bool subject= false;
    if (contains(occurenceId, bodyId, primitiveId))
    {
        (m_OccurrenceSelection[occurenceId])[bodyId].remove(primitiveId);
        subject= true;
    }
    return subject;
}

void GLC_SelectionSet::clear()
{
    m_OccurrenceSelection.clear();
}

GLC_SelectionSet &GLC_SelectionSet::unite(const GLC_SelectionSet &other)
{
    Q_ASSERT(m_pWorldHandle == other.m_pWorldHandle);
    OccurrenceSelection::const_iterator iOcc= other.m_OccurrenceSelection.constBegin();
    while (iOcc != other.m_OccurrenceSelection.constEnd())
    {
        const GLC_uint occId= iOcc.key();
        if (!contains(occId))
        {
            m_OccurrenceSelection.insert(occId, other.m_OccurrenceSelection.value(occId));
        }
        else
        {
            const BodySelection bodySelection= iOcc.value();
            BodySelection::const_iterator iBody= bodySelection.constBegin();
            while (iBody != bodySelection.constEnd())
            {
                const GLC_uint bodyId= iBody.key();
                if (!contains(occId, bodyId))
                {
                    m_OccurrenceSelection[occId].insert(bodyId, other.m_OccurrenceSelection.value(occId).value(bodyId));
                }
                else
                {
                    const PrimitiveSelection primitiveSelection= iBody.value();
                    PrimitiveSelection::const_iterator iPrim= primitiveSelection.constBegin();
                    while (iPrim != primitiveSelection.constEnd())
                    {
                        const GLC_uint primId= *iPrim;
                        if (!contains(occId, bodyId, primId))
                        {
                            (m_OccurrenceSelection[occId])[bodyId].insert(primId);
                        }
                        ++iPrim;
                    }
                }
                ++iBody;
            }
        }
        ++iOcc;
    }

    return *this;
}

GLC_SelectionSet &GLC_SelectionSet::substract(const GLC_SelectionSet &other)
{
    Q_ASSERT(m_pWorldHandle == other.m_pWorldHandle);
    OccurrenceSelection::const_iterator iOcc= other.m_OccurrenceSelection.constBegin();
    while (iOcc != other.m_OccurrenceSelection.constEnd())
    {
        const GLC_uint occId= iOcc.key();
        if (contains(occId))
        {
            const BodySelection bodySelection= iOcc.value();
            if (bodySelection.isEmpty())
            {
                m_OccurrenceSelection.remove(occId);
            }
            else
            {
                BodySelection::const_iterator iBody= bodySelection.constBegin();
                while (iBody != bodySelection.constEnd())
                {
                    const GLC_uint bodyId= iBody.key();
                    if (contains(occId, bodyId))
                    {
                        const PrimitiveSelection primitiveSelection= iBody.value();
                        if (primitiveSelection.isEmpty())
                        {
                            m_OccurrenceSelection[occId].remove(bodyId);
                        }
                        else
                        {
                            PrimitiveSelection::const_iterator iPrim= primitiveSelection.constBegin();
                            while (iPrim != primitiveSelection.constEnd())
                            {
                                const GLC_uint primId= *iPrim;
                                if (contains(occId, bodyId, primId))
                                {
                                    (m_OccurrenceSelection[occId])[bodyId].remove(primId);
                                }
                                ++iPrim;
                            }
                        }
                    }
                    ++iBody;
                }
            }
        }
        ++iOcc;
    }

    return *this;
}
