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

//! \file glc_world.cpp implementation of the GLC_World class.

#include "glc_world.h"
#include "glc_structinstance.h"
#include "glc_structreference.h"

#include "../glc_selectionevent.h"
#include "../geometry/glc_mesh.h"

GLC_World::GLC_World()
: m_pWorldHandle(new GLC_WorldHandle())
{

}

GLC_World::GLC_World(GLC_StructOccurrence* pOcc)
: m_pWorldHandle(new GLC_WorldHandle(pOcc))
{

}

GLC_World::GLC_World(const GLC_World& world)
: m_pWorldHandle(world.m_pWorldHandle)
{
	//qDebug() << "GLC_World::GLC_World() : " << (*m_pNumberOfWorld) << " " << this;
	// Increment the number of world
	m_pWorldHandle->ref();

}

GLC_World::~GLC_World()
{
    if (!m_pWorldHandle->deref())
	{
		delete m_pWorldHandle;
    }
}

QList<GLC_StructOccurrence *> GLC_World::minimumSelectedOccurrenceList() const
{
    QSet<GLC_StructOccurrence *> selectedOccSet= QSet<GLC_StructOccurrence *>::fromList(m_pWorldHandle->selectionSetHandle()->occurrencesList());
    QList<GLC_StructOccurrence *> subject;
    QSet<GLC_StructOccurrence *>::ConstIterator iOcc= selectedOccSet.begin();
    while (iOcc != selectedOccSet.constEnd())
    {
        QList<GLC_StructOccurrence*> ancestorList= (*iOcc)->ancestorList();
        const int count= ancestorList.count();
        bool addOcc= true;
        for (int i= 0; i < count; ++i)
        {
            if (selectedOccSet.contains(ancestorList.at(i)))
            {
                addOcc= false;
                break;
            }
        }
        if (addOcc) subject.append(*iOcc);

        ++iOcc;
    }

    return subject;
}

GLC_StructOccurrence* GLC_World::takeRootOccurrence()
{
    return m_pWorldHandle->takeRootOccurrence();
}

void GLC_World::replaceRootOccurrence(GLC_StructOccurrence* pOcc)
{
    m_pWorldHandle->replaceRootOccurrence(pOcc);
}

void GLC_World::mergeWithAnotherWorld(GLC_World& anotherWorld)
{
	GLC_StructOccurrence* pAnotherRoot= anotherWorld.rootOccurrence();
    GLC_StructOccurrence* pRoot= rootOccurrence();
	if (pAnotherRoot->childCount() > 0)
	{
		QList<GLC_StructOccurrence*> childs= pAnotherRoot->children();
		const int size= childs.size();
		for (int i= 0; i < size; ++i)
		{
            pRoot->addChild(childs.at(i)->clone(m_pWorldHandle, false));
		}
        pRoot->updateChildrenAbsoluteMatrix();
	}
	else
	{
        pRoot->addChild(anotherWorld.rootOccurrence()->clone(m_pWorldHandle, false));
    }
}

void GLC_World::updateSelection(const GLC_SelectionEvent &selectionEvent)
{
    GLC_SelectionEvent selectionCopy(selectionEvent);
    selectionCopy.setAttachedWorld(*this);
    m_pWorldHandle->updateSelection(selectionCopy);
}

void GLC_World::createSharpEdges(double precision, double angleThreshold)
{
    angleThreshold= glc::toRadian(angleThreshold);
    QList<GLC_StructReference*> referenceList= references();
    const int count= referenceList.count();
    for (int i= 0; i < count; ++i)
    {
        GLC_StructReference* pRef= referenceList.at(i);
        if (pRef->hasRepresentation())
        {
            GLC_3DRep* pRep= dynamic_cast<GLC_3DRep*>(pRef->representationHandle());
            if (NULL != pRep)
            {
                for (int j= 0; j < pRep->numberOfBody(); ++j)
                {
                    GLC_Mesh* pMesh= dynamic_cast<GLC_Mesh*>(pRep->geomAt(j));
                    if (NULL != pMesh)
                    {
                        pMesh->createSharpEdges(precision, angleThreshold);
                    }
                }
            }
        }
    }
}

GLC_World& GLC_World::operator=(const GLC_World& world)
{
    if ((this != &world) && (this->m_pWorldHandle != world.m_pWorldHandle))
	{
        // Decrement the number of world
        if (!m_pWorldHandle->deref())
        {
            delete m_pWorldHandle;
        }
        m_pWorldHandle= world.m_pWorldHandle;
        m_pWorldHandle->ref();
    }
    return *this;
}

bool GLC_World::operator ==(const GLC_World &other) const
{
    bool subject= this == &other;
    subject= (subject || (this->m_pWorldHandle == other.m_pWorldHandle));

    return subject;
}
