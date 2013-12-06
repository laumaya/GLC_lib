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

#include "glc_viewhandlerdata.h"

#include "glc_viewport.h"
#include "glc_movercontroller.h"

#include "../glc_factory.h"
#include "../sceneGraph/glc_octree.h"
#include "glc_inputeventinterpreter.h"
#include "glc_defaulteventinterpreter.h"

GLC_ViewHandlerData::GLC_ViewHandlerData()
    : QObject()
    , m_World()
    , m_pLight(new GLC_Light())
    , m_pViewport(new GLC_Viewport())
    , m_pMoverController(NULL)
    , m_Samples(16)
    , m_pSpacePartitioning(NULL)
    , m_pInputEventInterpreter(new GLC_DefaultEventInterpreter(this))
    , m_RenderInSelectionMode(false)
    , m_SelectionPoint()
    , m_SelectionMode(GLC_SelectionEvent::Replace)
{
    m_pLight->setTwoSided(true);
    m_pLight->setPosition(10.0, 10.0, 10.0);

    m_pViewport->cameraHandle()->setEyeCam(GLC_Point3d(1.0, 1.0, 1.0));

    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
    m_pMoverController= new GLC_MoverController(GLC_Factory::instance()->createDefaultMoverController(repColor, m_pViewport));
}

GLC_ViewHandlerData::~GLC_ViewHandlerData()
{
    delete m_pLight;
    delete m_pViewport;
    delete m_pMoverController;
    delete m_pInputEventInterpreter;
}

void GLC_ViewHandlerData::updateView()
{
    emit isDirty();
}

void GLC_ViewHandlerData::setWorld(const GLC_World &world)
{
    if (NULL != m_pSpacePartitioning)
    {
        m_pSpacePartitioning= m_pSpacePartitioning->clone();
    }

    m_World= world;

    if (NULL != m_pSpacePartitioning)
    {
        m_World.collection()->bindSpacePartitioning(m_pSpacePartitioning);
        m_World.collection()->setSpacePartitionningUsage(true);
    }
    else
    {
        m_World.collection()->setSpacePartitionningUsage(false);
    }

    m_pViewport->reframe(m_World.boundingBox());

    emit isDirty();
}

void GLC_ViewHandlerData::setSamples(int samples)
{
    if (m_Samples != samples)
    {
        m_Samples= samples;
        emit isDirty();
    }
}

void GLC_ViewHandlerData::setSpacePartitioning(GLC_SpacePartitioning *pSpacePartitioning)
{
    m_pSpacePartitioning= pSpacePartitioning;
    m_World.collection()->bindSpacePartitioning(m_pSpacePartitioning);
    m_World.collection()->setSpacePartitionningUsage(true);
}

void GLC_ViewHandlerData::setNextSelection(int x, int y, GLC_SelectionEvent::Mode mode)
{
    m_RenderInSelectionMode= true;
    m_SelectionPoint.setX(x);
    m_SelectionPoint.setY(y);
    m_SelectionMode= mode;
    emit isDirty();
}

void GLC_ViewHandlerData::unsetSelection()
{
    m_RenderInSelectionMode= false;
    m_World.unselectAll();
    emit isDirty();
}

void GLC_ViewHandlerData::updateSelection(GLC_uint id)
{
    m_RenderInSelectionMode= false;
    const bool contains= m_World.containsOccurence(id);
    bool selectionChange= false;

    switch (m_SelectionMode) {
    case GLC_SelectionEvent::Replace:
        m_World.unselectAll();
        if (contains)
        {
            m_World.select(id);
        }
        selectionChange= true;
        break;
    case GLC_SelectionEvent::Add:
        if (contains)
        {
            m_World.select(id);
        }
        selectionChange= true;
        break;
    case GLC_SelectionEvent::Remove:
        if (contains && (m_World.isSelected(id)))
        {
            m_World.unselect(id);
        }
        selectionChange= true;
        break;

    default:
        break;
    }
    if (selectionChange) emit isDirty();
}

void GLC_ViewHandlerData::processMousePressEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processMousePressEvent(pMouseEvent);
}

void GLC_ViewHandlerData::processMouseMoveEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    if (m_pInputEventInterpreter->processMouseMoveEvent(pMouseEvent))
    {
        emit invalidateSelectionBuffer();
    }
}

void GLC_ViewHandlerData::processMouseReleaseEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processMouseReleaseEvent(pMouseEvent);
}

void GLC_ViewHandlerData::processMouseDblClickEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processMouseDblClickEvent(pMouseEvent);
}

void GLC_ViewHandlerData::processWheelEvent(QWheelEvent *pWWheelEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processWheelEvent(pWWheelEvent);
}

void GLC_ViewHandlerData::processTouchEvent(QTouchEvent *pTouchEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processTouchEvent(pTouchEvent);
}

void GLC_ViewHandlerData::updateBackGround()
{
    //if (m_pViewport->clearBackground();
}
