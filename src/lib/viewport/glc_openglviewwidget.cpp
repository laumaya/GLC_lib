/*
 *  glc_openglviewwidget.cpp
 *
 *  Created on: 07/02/2016
 *      Author: Laurent Ribon
 */
#include "glc_openglviewwidget.h"

GLC_OpenGLViewWidget::GLC_OpenGLViewWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_pAuxFbo(NULL)
    , m_pScreenShotFbo(NULL)
    , m_Viewhandler(NULL)
    , m_UnprojectedPoint()
{
    // Set the initial viewHandler
    QSharedPointer<GLC_OpenGLViewHandler> viewHandlerPointer(new GLC_OpenGLViewHandler);
    QVariant viewHandler;
    viewHandler.setValue(viewHandlerPointer);
    setViewhandler(viewHandler);

}

GLC_OpenGLViewWidget::~GLC_OpenGLViewWidget()
{
    delete m_pAuxFbo;
    delete m_pScreenShotFbo;
}

QVariant GLC_OpenGLViewWidget::viewHandler() const
{
    QVariant subject;
    subject.setValue(m_Viewhandler);

    return subject;
}

void GLC_OpenGLViewWidget::setViewhandler(QVariant viewHandler)
{
    if (NULL != m_Viewhandler)
    {
        GLC_OpenGLViewHandler* pViewHandler= m_Viewhandler.data();
        disconnect(pViewHandler, SIGNAL(acceptHoverEvent(bool)), this, SLOT(setMouseTracking(bool)));
        disconnect(this, SIGNAL(frameBufferCreationFailed()), pViewHandler, SIGNAL(frameBufferCreationFailed()));
        disconnect(this, SIGNAL(frameBufferBindingFailed()), pViewHandler, SIGNAL(frameBufferBindingFailed()));
    }

    m_Viewhandler= viewHandler.value<QSharedPointer<GLC_OpenGLViewHandler> >();
    Q_ASSERT(!m_Viewhandler.isNull());
    GLC_OpenGLViewHandler* pViewHandler= m_Viewhandler.data();

    connect(pViewHandler, SIGNAL(acceptHoverEvent(bool)), this, SLOT(setMouseTracking(bool)));
    connect(this, SIGNAL(frameBufferCreationFailed()), pViewHandler, SIGNAL(frameBufferCreationFailed()));
    connect(this, SIGNAL(frameBufferBindingFailed()), pViewHandler, SIGNAL(frameBufferBindingFailed()));

    pViewHandler->setOpenGLViewWidget(this);
}

void GLC_OpenGLViewWidget::setMouseTracking(bool track)
{

}

void GLC_OpenGLViewWidget::updateSelection()
{
    qDebug() << "GLC_OpenGLViewWidget::updateSelection()";

    makeCurrent();
    Q_ASSERT(NULL != m_Viewhandler);

    if (m_pAuxFbo && m_pAuxFbo->isValid())
    {
        if (!m_pAuxFbo->bind()) emit frameBufferBindingFailed();

        // Get selection coordinate
        const int x= m_Viewhandler->pointerPosition().x();
        const int y= m_Viewhandler->pointerPosition().y();

        GLC_World world= m_Viewhandler->world();
        GLC_SelectionSet selectionSet;

        const GLC_uint instanceId= m_Viewhandler->viewportHandle()->selectOnPreviousRender(x, y, GL_COLOR_ATTACHMENT0);
        qDebug() << "instanceId " << instanceId;
        m_UnprojectedPoint= m_Viewhandler->viewportHandle()->unproject(x, y, GL_COLOR_ATTACHMENT0);

        GLC_3DViewCollection* pCollection= world.collection();
        const bool contains= pCollection->contains(instanceId);

        if (instanceId && (m_Viewhandler->selectionModes() & GLC_SelectionEvent::ModeInstance))
        {
            selectionSet.insert(instanceId);
        }
        else if (contains && (m_Viewhandler->selectionModes() & GLC_SelectionEvent::ModeBody))
        {
            selectionSet.setAttachedWorld(world);
            GLC_uint bodyId= selectBody(instanceId, x, y);
            if (bodyId)
            {
                selectionSet.insert(instanceId, bodyId);
            }
        }
        else if (contains && (m_Viewhandler->selectionModes() & GLC_SelectionEvent::ModePrimitive))
        {
            selectionSet.setAttachedWorld(world);
            QPair<GLC_uint, GLC_uint> primitive= selectPrimitive(instanceId, x, y);
            if (primitive.first && primitive.second)
            {
                selectionSet.insert(instanceId, primitive.first, primitive.second);
            }
        }

        m_Viewhandler->updateCurrentSelectionSet(selectionSet, m_UnprojectedPoint);
        m_pAuxFbo->release();
    }

 }

void GLC_OpenGLViewWidget::select(uint id)
{
    if (!m_Viewhandler.isNull())
    {
        GLC_World world= m_Viewhandler->world();
        world.unselectAll();
        world.select(id);
    }
}

QImage GLC_OpenGLViewWidget::takeScreenShot()
{
    QImage subject;
    Q_ASSERT(NULL != m_Viewhandler);
    GLC_ScreenShotSettings screenShotSettings= m_Viewhandler->screenShotSettings();
    const int width= screenShotSettings.size().width();
    const int height= screenShotSettings.size().height();

    setupScreenShotFbo(width, height);

    if (m_pScreenShotFbo && m_pScreenShotFbo->isValid())
    {
        if (!m_pScreenShotFbo->bind()) emit frameBufferBindingFailed();

        doRender();

        m_pScreenShotFbo->release();
    }

    subject= m_pScreenShotFbo->toImage();

    delete m_pScreenShotFbo;
    m_pScreenShotFbo= NULL;

    return subject;
}

GLC_uint GLC_OpenGLViewWidget::selectBody(GLC_uint instanceId, int x, int y)
{
    Q_ASSERT(NULL != m_Viewhandler);
    GLC_uint subject= 0;
    GLC_3DViewCollection* pCollection= m_Viewhandler->world().collection();
    if (pCollection->contains(instanceId))
    {
        GLC_Viewport* pView= m_Viewhandler->viewportHandle();
        GLC_3DViewInstance* pInstance= pCollection->instanceHandle(instanceId);
        subject= pView->selectBody(pInstance, x, y, GL_COLOR_ATTACHMENT0);
    }
    return subject;
}

void GLC_OpenGLViewWidget::initializeGL()
{
    GLC_State::init();
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->viewportHandle()->initGl();
    }
}

void GLC_OpenGLViewWidget::paintGL()
{
    Q_ASSERT(NULL != m_Viewhandler);
    renderForSelection();
    doRender();
}

void GLC_OpenGLViewWidget::resizeGL(int width, int height)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->setSize(width, height);
        setupAuxFbo(width, height);
    }
}

void GLC_OpenGLViewWidget::mousePressEvent(QMouseEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processMousePressEvent(e);
    }
}

void GLC_OpenGLViewWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processMouseMoveEvent(e);
    }
}

void GLC_OpenGLViewWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processMouseReleaseEvent(e);
    }
}

void GLC_OpenGLViewWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processMouseDblClickEvent(e);
    }
}

void GLC_OpenGLViewWidget::wheelEvent(QWheelEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processWheelEvent(e);
    }
}

void GLC_OpenGLViewWidget::touchEvent(QTouchEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processTouchEvent(e);
    }
}

void GLC_OpenGLViewWidget::hoverMoveEvent(QHoverEvent *event)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processHoverMoveEvent(event);
    }
}

void GLC_OpenGLViewWidget::renderForSelection()
{
    Q_ASSERT(NULL != m_Viewhandler);

    if ((NULL != m_pAuxFbo) && m_pAuxFbo->isValid())
    {
        if (!m_pAuxFbo->bind()) emit frameBufferBindingFailed();

        GLC_State::setSelectionMode(true);
        doRender();
        GLC_State::setSelectionMode(false);
    }

    m_pAuxFbo->release();
}

QPair<GLC_uint, GLC_uint> GLC_OpenGLViewWidget::selectPrimitive(GLC_uint instanceId, int x, int y)
{
    Q_ASSERT(NULL != m_Viewhandler);
    QPair<GLC_uint, GLC_uint> subject;
    GLC_3DViewCollection* pCollection= m_Viewhandler->world().collection();
    if (pCollection->contains(instanceId))
    {
        GLC_Viewport* pView= m_Viewhandler->viewportHandle();
        GLC_3DViewInstance* pInstance= pCollection->instanceHandle(instanceId);
        QPair<int, GLC_uint> primitive= pView->selectPrimitive(pInstance, x, y, GL_COLOR_ATTACHMENT0);
        if (primitive.first > -1)
        {
            GLC_uint bodyId= pInstance->geomAt(primitive.first)->id();
            subject.first= bodyId;
            subject.second= primitive.second;
        }
    }
    return subject;
}

void GLC_OpenGLViewWidget::doRender()
{
    Q_ASSERT(NULL != m_Viewhandler);
    m_Viewhandler->render();
}

void GLC_OpenGLViewWidget::setupAuxFbo(int width, int height)
{
    Q_ASSERT(NULL != m_Viewhandler);

    if ((width > 0) && (height > 0))
    {
        delete m_pAuxFbo;
        m_pAuxFbo= new QOpenGLFramebufferObject(width, height, QOpenGLFramebufferObject::Depth);
    }
    else
    {
        delete m_pAuxFbo;
        m_pAuxFbo= NULL;
    }
}

void GLC_OpenGLViewWidget::setupScreenShotFbo(int width, int height)
{
    Q_ASSERT(NULL != m_Viewhandler);
    Q_ASSERT(NULL == m_pScreenShotFbo);

    QOpenGLFramebufferObjectFormat sourceFormat;
    sourceFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    sourceFormat.setSamples(m_Viewhandler->samples());

    m_pScreenShotFbo= new QOpenGLFramebufferObject(width, height, sourceFormat);

    // Test frame buffer validity
    const bool isValid= m_pScreenShotFbo->isValid();
    if (!isValid) emit frameBufferCreationFailed();
}
