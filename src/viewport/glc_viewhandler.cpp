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
    , m_pLight(new GLC_Light(GLC_Light::LightPosition, GL_LIGHT0))
    , m_pViewport(new GLC_Viewport())
    , m_pMoverController(NULL)
    , m_Samples(16)
    , m_pSpacePartitioning(NULL)
    , m_pInputEventInterpreter(new GLC_DefaultEventInterpreter(this))
    , m_RenderingMode(normalRenderMode)
    , m_PointerPosition()
    , m_SelectionModes(GLC_SelectionEvent::ModeReplace | GLC_SelectionEvent::ModeInstance)
    , m_CurrentSelectionSet()
    , m_UnprojectedPoint()
    , m_3DWidgetManager(m_pViewport)
    , m_isRendering()

    , m_ScreenShotMode(false)
    , m_ScreenshotSettings()
    , m_ScreenShotImage()

    , m_RenderFlag(glc::ShadingFlag)

    , m_BlockUpdate(false)

{
    m_pLight->setTwoSided(true);
    m_pLight->setPosition(1.0, 1.0, 1.0);

    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
    m_pMoverController= new GLC_MoverController(GLC_Factory::instance()->createDefaultMoverController(repColor, m_pViewport));

    connect(m_pMoverController, SIGNAL(repaintNeeded()), this, SLOT(updateGL()));
}

GLC_ViewHandler::~GLC_ViewHandler()
{
    delete m_pLight;
    delete m_pViewport;
    delete m_pMoverController;
    delete m_pInputEventInterpreter;
}

void GLC_ViewHandler::updateGL(bool synchrone)
{
    if (!m_BlockUpdate)
    {
        m_isRendering= synchrone;
        emit isDirty();
        while (m_isRendering)
        {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
}

void GLC_ViewHandler::updateSynchronized()
{
    updateGL(true);
}

void GLC_ViewHandler::clearSelectionBuffer()
{
    emit invalidateSelectionBuffer();
}

void GLC_ViewHandler::setInputEventInterpreter(GLC_InputEventInterpreter *pEventInterpreter)
{
    delete m_pInputEventInterpreter;
    m_pInputEventInterpreter= pEventInterpreter;

    connect(this, SIGNAL(userInputUpdated(GLC_UserInput)), m_pInputEventInterpreter, SLOT(userInputChanged(GLC_UserInput)));
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

QPair<GLC_SelectionSet, GLC_Point3d> GLC_ViewHandler::selectAndUnproject(int x, int y, GLC_SelectionEvent::Modes modes)
{
    m_RenderingMode= GLC_ViewHandler::selectRenderMode;
    m_PointerPosition.setX(x);
    m_PointerPosition.setY(y);
    m_CurrentSelectionSet.clear();
    m_UnprojectedPoint.setVect(0.0, 0.0, 0.0);
    m_SelectionModes= modes;
    updateGL(true); // Execute OpenGL synchronously to get selection Set
    QPair<GLC_SelectionSet, GLC_Point3d> subject(m_CurrentSelectionSet, m_UnprojectedPoint);
    return subject;
}

void GLC_ViewHandler::unsetSelection()
{
    m_RenderingMode= GLC_ViewHandler::normalRenderMode;
    m_CurrentSelectionSet.clear();
    m_UnprojectedPoint.setVect(0.0, 0.0, 0.0);
    m_World.unselectAll();
    updateGL();
}

QImage GLC_ViewHandler::takeScreenshot(const GLC_ScreenShotSettings &screenShotSettings)
{
    m_ScreenshotSettings= screenShotSettings;
    m_ScreenShotMode= true;
    updateGL(true);  // Execute OpenGL synchronously to get screenshot image
    return m_ScreenShotImage;
}

void GLC_ViewHandler::updateSelection(const GLC_SelectionSet& selectionSet, const GLC_Point3d &point)
{
    m_RenderingMode= GLC_ViewHandler::normalRenderMode;
    m_CurrentSelectionSet= selectionSet;
    m_UnprojectedPoint= point;
}

void GLC_ViewHandler::setLight(GLC_Light *pLight)
{
    Q_ASSERT(NULL != pLight);
    delete m_pLight;
    m_pLight= pLight;
}

void GLC_ViewHandler::setScreenShotImage(const QImage &image)
{
    m_ScreenShotMode= false;
    m_ScreenShotImage= image;
}

void GLC_ViewHandler::processMousePressEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processMousePressEvent(pMouseEvent);
}

void GLC_ViewHandler::processMouseMoveEvent(QMouseEvent *pMouseEvent)
{
    Q_ASSERT(NULL != m_pInputEventInterpreter);
    m_pInputEventInterpreter->processMouseMoveEvent(pMouseEvent);
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

void GLC_ViewHandler::renderBackGround()
{
    if (m_ScreenShotMode && (m_ScreenshotSettings.mode() == GLC_ScreenShotSettings::Color))
    {
        m_pViewport->clearBackground(m_ScreenshotSettings.backgroundColor());
    }
    else
    {
        m_pViewport->clearBackground();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GLC_ViewHandler::render()
{
    try
    {
        QOpenGLContext::currentContext()->functions()->glUseProgram(0);

        // Calculate camera depth of view
        m_pViewport->setDistMinAndMax(m_World.boundingBox());
        m_World.collection()->updateInstanceViewableState();

        renderBackGround();

        // Load identity matrix
        GLC_Context::current()->glcLoadIdentity();

        m_pLight->glExecute();
        if (m_ScreenShotMode && (m_ScreenshotSettings.mode() == GLC_ScreenShotSettings::Image))
        {
            m_pViewport->glExecuteCam(m_ScreenshotSettings.backgroundImage());
        }
        else
        {
            m_pViewport->glExecuteCam();
        }

        m_World.render(0, m_RenderFlag);
        m_World.render(0, glc::TransparentRenderFlag);
        m_World.render(1, m_RenderFlag);

        if (!GLC_State::isInSelectionMode())
        {
            m_pMoverController->drawActiveMoverRep();
        }

        m_3DWidgetManager.render();
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
