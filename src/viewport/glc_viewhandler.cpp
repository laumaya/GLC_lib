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

#include <QtDebug>

#include "glc_viewhandler.h"

#include "glc_viewport.h"
#include "glc_movercontroller.h"

#include "../glc_factory.h"
#include "../sceneGraph/glc_octree.h"
#include "../glc_exception.h"

#include "glc_inputeventinterpreter.h"
#include "glc_defaulteventinterpreter.h"

GLC_ViewHandler::GLC_ViewHandler(QObject *pParent)
    : QObject(pParent)
    , m_World()
    , m_pLight(new GLC_Light())
    , m_pViewport(new GLC_Viewport())
    , m_pMoverController(NULL)
    , m_Samples(16)
    , m_pSpacePartitioning(NULL)
    , m_pInputEventInterpreter(new GLC_DefaultEventInterpreter(this))
    , m_RenderInSelectionMode(false)
    , m_SelectionPoint()
    , m_SelectionModes(GLC_SelectionEvent::ModeReplace | GLC_SelectionEvent::ModeInstance)
{
    m_pLight->setTwoSided(true);
    m_pLight->setPosition(1.0, 1.0, 1.0);

    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
    m_pMoverController= new GLC_MoverController(GLC_Factory::instance()->createDefaultMoverController(repColor, m_pViewport));
}

GLC_ViewHandler::~GLC_ViewHandler()
{
    delete m_pLight;
    delete m_pViewport;
    delete m_pMoverController;
    delete m_pInputEventInterpreter;
}

void GLC_ViewHandler::updateGL()
{
    emit isDirty();
}

void GLC_ViewHandler::setInputEventInterpreter(GLC_InputEventInterpreter *pEventInterpreter)
{
    delete m_pInputEventInterpreter;
    m_pInputEventInterpreter= pEventInterpreter;
}

void GLC_ViewHandler::setWorld(const GLC_World &world)
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

   updateGL();
}

void GLC_ViewHandler::setSamples(int samples)
{
    if (m_Samples != samples)
    {
        m_Samples= samples;
        updateGL();
    }
}

void GLC_ViewHandler::setSpacePartitioning(GLC_SpacePartitioning *pSpacePartitioning)
{
    m_pSpacePartitioning= pSpacePartitioning;
    m_World.collection()->bindSpacePartitioning(m_pSpacePartitioning);
    m_World.collection()->setSpacePartitionningUsage(true);
}

void GLC_ViewHandler::setNextSelection(int x, int y, GLC_SelectionEvent::Modes modes)
{
    m_RenderInSelectionMode= true;
    m_SelectionPoint.setX(x);
    m_SelectionPoint.setY(y);
    m_SelectionModes= modes;
    updateGL();
}

void GLC_ViewHandler::unsetSelection()
{
    m_RenderInSelectionMode= false;
    m_World.unselectAll();
    updateGL();
}

void GLC_ViewHandler::updateSelection(GLC_uint id)
{
    m_RenderInSelectionMode= false;

    GLC_SelectionSet selectionSet(m_World);
    selectionSet.insert(id);

    GLC_SelectionEvent selectionEvent(m_SelectionModes, selectionSet);

    m_World.updateSelection(selectionEvent);

    updateGL();
}

void GLC_ViewHandler::processMousePressEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processMousePressEvent(pMouseEvent);
}

void GLC_ViewHandler::processMouseMoveEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    if (m_pInputEventInterpreter->processMouseMoveEvent(pMouseEvent))
    {
        emit invalidateSelectionBuffer();
    }
}

void GLC_ViewHandler::processMouseReleaseEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processMouseReleaseEvent(pMouseEvent);
}

void GLC_ViewHandler::processMouseDblClickEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processMouseDblClickEvent(pMouseEvent);
}

void GLC_ViewHandler::processWheelEvent(QWheelEvent *pWWheelEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processWheelEvent(pWWheelEvent);
}

void GLC_ViewHandler::processTouchEvent(QTouchEvent *pTouchEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processTouchEvent(pTouchEvent);
}

void GLC_ViewHandler::updateBackGround()
{
    //if (m_pViewport->clearBackground();
}

void GLC_ViewHandler::render()
{
    try
    {
        QOpenGLContext::currentContext()->functions()->glUseProgram(0);

        // Calculate camera depth of view
        m_pViewport->setDistMinAndMax(m_World.boundingBox());
        m_World.collection()->updateInstanceViewableState();

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Load identity matrix
        GLC_Context::current()->glcLoadIdentity();

        m_pLight->glExecute();
        m_pViewport->glExecuteCam();

        m_World.render(0, glc::WireRenderFlag);
        m_World.render(0, glc::TransparentRenderFlag);
        m_World.render(1, glc::WireRenderFlag);
        m_pLight->disable();

        m_pMoverController->drawActiveMoverRep();
    }
    catch (GLC_Exception &e)
    {
        qDebug() << e.what();
    }
}

void GLC_ViewHandler::setDefaultUpVector(const GLC_Vector3d &vect)
{
    GLC_Camera* pCamera= m_pViewport->cameraHandle();

    pCamera->setUpCam(vect);
    pCamera->setDefaultUpVector(vect);
    pCamera->setIsoView();
}
