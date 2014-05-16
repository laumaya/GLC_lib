/*
 *  glc_quickselection.cpp
 *
 *  Created on: 13/05/2014
 *      Author: Laurent Ribon
 */

#include "glc_quickselection.h"
#include "../sceneGraph/glc_structoccurrence.h"

GLC_QuickSelection::GLC_QuickSelection(QObject *parent)
    : QObject(parent)
    , m_Occurrences()
    , m_World()
{
}

GLC_QuickSelection::~GLC_QuickSelection()
{
    clear();
}

void GLC_QuickSelection::setWorld(const GLC_World &world)
{
    m_World= world;
    update();
}

void GLC_QuickSelection::update()
{
    clear();
    QList<GLC_StructOccurrence*> occurrences= m_World.selectedOccurrenceList();
    const int count= occurrences.count();
    for (int i= 0; i < count; ++i)
    {
        GLC_QuickOccurrence* pQuickOcc= new GLC_QuickOccurrence(occurrences.at(i));
        m_Occurrences.append(pQuickOcc);
    }
    emit selectionChanged();
}

void GLC_QuickSelection::clear()
{
    qDeleteAll(m_Occurrences);
    m_Occurrences.clear();
}
