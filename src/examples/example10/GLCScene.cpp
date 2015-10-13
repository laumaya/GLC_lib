/****************************************************************************
 This file is part of the documentation of GLC-lib library.
 Copyright (C) 2012 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This file is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 http://www.fsf.org/licensing/licenses/info/GPLv2.html and
 http://www.gnu.org/copyleft/gpl.html.
*****************************************************************************/

#include <QLabel>
#include <QOpenGLFunctions>
#include <QStringList>
#include <QFileDialog>
#include <QFileInfo>

#include <GLC_Context>
#include <GLC_Factory>
#include <GLC_Exception>
#include <GLC_Octree>


#include "GLCScene.h"

// For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
#include <OpenGL.h>
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtConcurrent/QtConcurrent>
#endif

static int labelCount= 4;

GLCScene::GLCScene()
    : QGraphicsScene()
    , m_Light()
    , m_World()
    , m_CurrentPath(QDir::homePath())
    , m_CurrentFilePath()
    , m_WorldLoader()
    , m_Labels()
    , m_pLoadWorldButton(NULL)
    , m_Viewport()
    , m_MoverController()
    , m_SavedMousePos()
    , m_MouseTimer()
    , m_Axis(m_Viewport.cameraHandle()->defaultUpVector())
    , m_Angle(0.05)
{
    // Set up mover controller
    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
    m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_Viewport);

    createSceneWidgets();

    // Set some GLC_lib state in order to render heavy scene
    GLC_State::setDefaultOctreeDepth(4);
    GLC_State::setPixelCullingUsage(true);
    GLC_State::setSpacePartionningUsage(true);
    m_Viewport.setMinimumPixelCullingSize(6);

    m_Light.setTwoSided(true);
    m_Light.setPosition(1.0, 1.0, 1.0);

    // Load GLC_lib logo
    connect(&m_WorldLoader, SIGNAL(finished()), this, SLOT(worldLoaded()));
    loadModel(":glc_lib.obj");
    QTimer::singleShot(20, this, SLOT(updateCameraPos()));
}

GLCScene::~GLCScene()
{

}

void GLCScene::drawBackground(QPainter *painter, const QRectF &)
{
    bool useOpenGL= (painter->paintEngine()->type() == QPaintEngine::OpenGL);
    useOpenGL= useOpenGL || (painter->paintEngine()->type() == QPaintEngine::OpenGL2);

    if (!useOpenGL)
    {
        qWarning("OpenGLScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view");
        return;
    }

    // GLC_lib initialisation
    static bool firstRender= true;
    if (firstRender)
    {
        initGl();
        firstRender= false;
    }

    // Save OpenGL matrix
    GLC_Context* pCurrentContext= GLC_Context::current();
    pCurrentContext->glcMatrixMode(GL_PROJECTION);
    pCurrentContext->glcPushMatrix();
    pCurrentContext->glcMatrixMode(GL_MODELVIEW);
    pCurrentContext->glcPushMatrix();

    renderWorld();

    // Restore OpenGL matrix
    pCurrentContext->glcMatrixMode(GL_MODELVIEW);
    pCurrentContext->glcPopMatrix();
    pCurrentContext->glcMatrixMode(GL_PROJECTION);
    pCurrentContext->glcPopMatrix();

    QTimer::singleShot(20, this, SLOT(update()));
}

void GLCScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if (!event->isAccepted())
    {
        if (!m_MoverController.hasActiveMover())
        {
            m_World.collection()->setLodUsage(true, &m_Viewport);
            switch (event->button())
            {
            case (Qt::RightButton):
                m_MouseTimer.start();
                m_SavedMousePos.setX(event->scenePos().x()); m_SavedMousePos.setY(event->scenePos().y());
                m_MoverController.setActiveMover(GLC_MoverController::TrackBall, GLC_UserInput(event->scenePos().x(), event->scenePos().y()));
                break;
            case (Qt::LeftButton):
                m_MoverController.setActiveMover(GLC_MoverController::Pan, GLC_UserInput(event->scenePos().x(), event->scenePos().y()));
                break;
            case (Qt::MidButton):
                m_MoverController.setActiveMover(GLC_MoverController::Zoom, GLC_UserInput(event->scenePos().x(), event->scenePos().y()));
                break;

            default:
                break;
            }
            event->accept();
        }
    }
}

void GLCScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    if (!event->isAccepted())
    {
        if (m_MoverController.hasActiveMover())
        {
            if (m_MoverController.activeMoverId() == GLC_MoverController::TrackBall)
            {
                m_MouseTimer.restart();
                m_SavedMousePos.setX(event->scenePos().x()); m_SavedMousePos.setY(event->scenePos().y());
                m_SavedCamForwadVector= m_Viewport.cameraHandle()->forward();
            }
            m_MoverController.move(GLC_UserInput(event->scenePos().x(), event->scenePos().y()));
        }
        event->accept();
    }
}

void GLCScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (!event->isAccepted())
    {
        if (m_MoverController.hasActiveMover())
        {
            if (m_MoverController.activeMoverId() == GLC_MoverController::TrackBall)
            {
                const int elapsed= m_MouseTimer.restart();
                if ((elapsed < 100))
                {
                    GLC_Vector3d newCamForwardVector= m_Viewport.cameraHandle()->forward();
                    m_Axis= m_SavedCamForwadVector ^ newCamForwardVector;
                    m_Angle= m_SavedCamForwadVector.angleWithVect(newCamForwardVector);
                    if (m_Angle > 0.0)
                    {
                        QTimer::singleShot(20, this, SLOT(updateCameraPos()));
                    }
                }
            }
            m_World.collection()->setLodUsage(false, &m_Viewport);
            m_MoverController.setNoMover();
            event->accept();
        }
    }
}

QDialog* GLCScene::createDialog() const
{
    QDialog* pSubject = new QDialog();

    pSubject->setWindowOpacity(0.8);
    pSubject->setLayout(new QVBoxLayout);

    return pSubject;
}

void GLCScene::createSceneWidgets()
{
    // Model info widget
    QWidget* pModelInfo = createDialog();
    pModelInfo->layout()->setMargin(20);
    for (int i= 0; i < labelCount; ++i)
    {
        m_Labels.append(new QLabel());
        pModelInfo->layout()->addWidget(m_Labels.last());
    }

    // Control widget
    QWidget* pControls = createDialog();
    m_pLoadWorldButton = new QPushButton(tr("Load model"));
    pControls->layout()->addWidget(m_pLoadWorldButton);
    connect(m_pLoadWorldButton, SIGNAL(clicked()), this, SLOT(loadModel()));

    QWidget* pReframe = new QPushButton(tr("Reframe"));
    pControls->layout()->addWidget(pReframe);
    connect(pReframe, SIGNAL(clicked()), this, SLOT(reframe()));

    // Instruction widget
    QWidget* pInstructions = createDialog();
    pInstructions->layout()->addWidget(new QLabel(tr("Use mouse right button and drag to rotate")));
    pInstructions->layout()->addWidget(new QLabel(tr("Use mouse left button and drag to pan")));
    pInstructions->layout()->addWidget(new QLabel(tr("Use mouse middle button and drag to zoom")));

    addWidget(pInstructions);
    addWidget(pControls);
    addWidget(pModelInfo);

    QPointF pos(10, 10);
    foreach (QGraphicsItem* pItem, items())
    {
        pItem->setFlag(QGraphicsItem::ItemIsMovable);
        pItem->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

        const QRectF rect = pItem->boundingRect();
        pItem->setPos(pos.x() - rect.x(), pos.y() - rect.y());
        pos += QPointF(0, 10 + rect.height());
    }
}

static GLC_World* loadWorld(const QString& filePath)
{
    GLC_World* pSubject= NULL;
    try
    {
        QFile file(filePath);
        pSubject= new GLC_World(GLC_Factory::instance()->createWorldFromFile(file));
    }
    catch (GLC_Exception &e)
    {
        qDebug() << e.what();
        delete pSubject;
        return NULL;
    }

    return pSubject;
}

void GLCScene::loadModel(const QString &filePath)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    m_pLoadWorldButton->setEnabled(false);
    m_WorldLoader.setFuture(QtConcurrent::run(::loadWorld, filePath));
}

void GLCScene::updateLabels(const QString& filePath)
{
    QString fileName= QFileInfo(filePath).fileName();
    if (fileName.isEmpty() && !m_World.isEmpty()) fileName= tr("Logo of GLC_lib");
    const QString vertexCount= QString::number(m_World.numberOfVertex());
    const QString triangleCount= QString::number(m_World.numberOfFaces());
    const QString materialCount= QString::number(m_World.numberOfMaterials());

    m_Labels[0]->setText(tr("File name : ") + fileName);
    m_Labels[1]->setText(tr("Vertex count : ") + vertexCount);
    m_Labels[2]->setText(tr("Triangle count : ") + triangleCount);
    m_Labels[3]->setText(tr("Material count : ") + materialCount);
}

void GLCScene::initGl()
{
    // For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
    const GLint swapInterval = 1;
    CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &swapInterval);
#endif

    m_Viewport.initGl();

    QOpenGLContext::currentContext()->functions()->glEnable(GL_NORMALIZE);
    try
    {
        m_Viewport.loadBackGroundImage(":background.png");
    }
    catch (GLC_Exception &e)
    {
        qDebug() << e.what();
    }
}

void GLCScene::renderWorld()
{
    try
    {
        m_Viewport.setWinGLSize(static_cast<int>(width()), static_cast<int>(height()));

        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        // Clear screen
        f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Load identity matrix
        GLC_Context::current()->glcLoadIdentity();

        // Calculate camera depth of view
        m_Viewport.setDistMinAndMax(m_World.boundingBox());

        m_World.collection()->updateInstanceViewableState();

        m_Light.glExecute();
        m_Viewport.glExecuteCam();

        f->glEnable(GL_MULTISAMPLE);
        m_World.render(0, glc::ShadingFlag);
        m_World.render(0, glc::TransparentRenderFlag);
        f->glDisable(GL_MULTISAMPLE);

    }
    catch (GLC_Exception &e)
    {
        qDebug() << e.what();
    }
}

void GLCScene::loadModel()
{
    // Define File Format filter
    QStringList filters;
    filters.append(tr("All Known format(*.obj *.OBJ *.3ds *.3DS *.stl *.STL *.off *.OFF *.3DXML *.3dxml *.DAE *.dae *.BSRep)"));
    filters.append(tr("Alias File Format OBJ (*.obj *.OBJ)"));
    filters.append(tr("3D Studio File Format 3DS (*.3ds *.3DS)"));
    filters.append(tr("STL File Format STL (*.stl *.STL)"));
    filters.append(tr("Object File Format OFF (*.off *.OFF)"));
    filters.append(tr("Dassault Systemes 3DXML(*.3dxml *.3DXML)"));
    filters.append(tr("Sony Collada(*.dae *.DAE)"));
    filters.append(tr("GLC_lib Binary Serialized Representation(*.BSRep)"));

    QString filePath = QFileDialog::getOpenFileName(views().first(), tr("Open model file"), m_CurrentPath, filters.join("\n"));
    if (!filePath.isEmpty())
    {
        m_CurrentFilePath= filePath;
        m_CurrentPath= QFileInfo(filePath).path();
        loadModel(filePath);
    }
}

void GLCScene::worldLoaded()
{
    GLC_World* pWorld= m_WorldLoader.result();
    if (NULL != pWorld)
    {
        // Set the world to the newly loaded
        m_World= *pWorld;
        delete pWorld;

        // Set octree space partitionning
        GLC_Octree* pOctree= new GLC_Octree(m_World.collection());
        pOctree->updateSpacePartitioning();
        m_World.collection()->bindSpacePartitioning(pOctree);

        // Set Level of detail usage
        m_World.collection()->setLodUsage(false, &m_Viewport);

        // Set default camera
        m_Viewport.cameraHandle()->setIsoView();
        m_Viewport.reframe(m_World.boundingBox());

        updateLabels(m_CurrentFilePath);
    }

    QApplication::restoreOverrideCursor();
    m_pLoadWorldButton->setEnabled(true);
}

void GLCScene::reframe()
{
    if (!m_World.isEmpty())
    {
        m_Viewport.reframe(m_World.boundingBox());
    }
}

void GLCScene::updateCameraPos()
{
    bool update= (m_Angle > 0);
    if (update && m_MoverController.hasActiveMover())
    {
        update= (m_MoverController.activeMoverId() != GLC_MoverController::TrackBall);
        update= update && (m_MoverController.activeMoverId() != GLC_MoverController::Pan);
    }

    if (update)
    {
        m_Viewport.cameraHandle()->rotateAroundTarget(m_Axis, m_Angle);
        m_World.collection()->setLodUsage(true, &m_Viewport);
        QTimer::singleShot(20, this, SLOT(updateCameraPos()));
    }
    else
    {
        m_World.collection()->setLodUsage(true, &m_Viewport);
    }
}
