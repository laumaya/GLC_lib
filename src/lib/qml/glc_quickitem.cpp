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
//! \file glc_quickitem.cpp implementation of the GLC_QuickItem class.

#include <QSGSimpleTextureNode>
#include <QQuickWindow>

#include "../glc_context.h"
#include "../glc_exception.h"
#include "../glc_factory.h"

#include "glc_quickitem.h"


#include "../sceneGraph/glc_octree.h"

GLC_QuickItem::GLC_QuickItem(GLC_QuickItem *pParent)
    : QQuickItem(pParent)
    , m_Viewhandler(NULL)
    , m_pSourceFbo(NULL)
    , m_pTargetFbo(NULL)
    , m_pAuxFbo(NULL)
    , m_pScreenShotFbo(NULL)
    , m_SelectionBufferIsDirty(true)
    , m_UnprojectedPoint()
    , m_pCamera(new GLC_QuickCamera(this))
    , m_Source()
    , m_pQuickSelection(new GLC_QuickSelection(this))
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
    setFlag(QQuickItem::ItemHasContents);

    // Set the initial viewHandler
    QSharedPointer<GLC_QuickViewHandler> viewHandlerPointer(new GLC_QuickViewHandler);
    QVariant viewHandler;
    viewHandler.setValue(viewHandlerPointer);
    setViewhandler(viewHandler);
    initConnections();
}

GLC_QuickItem::~GLC_QuickItem()
{
    qDebug() << "GLC_QuickItem::~GLC_QuickItem() " << m_Source;
    delete m_pSourceFbo;
    delete m_pTargetFbo;
    delete m_pAuxFbo;
}

QVariant GLC_QuickItem::viewHandler() const
{
    QVariant subject;
    subject.setValue(m_Viewhandler);

    return subject;
}

bool GLC_QuickItem::spacePartitionningEnabled() const
{
    bool subject= false;
    if (NULL != m_Viewhandler)
    {
        subject= m_Viewhandler->spacePartitionningEnabled();
    }

    return subject;
}

QVector3D GLC_QuickItem::defaultUpVector() const
{
    QVector3D subject;
    if (!m_Viewhandler.isNull())
    {
        subject= m_Viewhandler->viewportHandle()->cameraHandle()->defaultUpVector().toQVector3D();
    }

    return subject;
}

void GLC_QuickItem::setViewhandler(QVariant viewHandler)
{
    if (NULL != m_Viewhandler)
    {
        GLC_QuickViewHandler* pViewHandler= m_Viewhandler.data();
        disconnect(pViewHandler, SIGNAL(isDirty()), this, SLOT(update()));
        disconnect(pViewHandler, SIGNAL(invalidateSelectionBuffer()), this, SLOT(invalidateSelectionBuffer()));
        disconnect(pViewHandler, SIGNAL(acceptHoverEvent(bool)), this, SLOT(setMouseTracking(bool)));
        disconnect(pViewHandler, SIGNAL(selectionChanged()), m_pQuickSelection, SLOT(update()));
        disconnect(m_pQuickSelection, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
        disconnect(this, SIGNAL(frameBufferCreationFailed()), pViewHandler, SIGNAL(frameBufferCreationFailed()));
        disconnect(this, SIGNAL(frameBufferBindingFailed()), pViewHandler, SIGNAL(frameBufferBindingFailed()));
    }

    m_Viewhandler= viewHandler.value<QSharedPointer<GLC_QuickViewHandler> >();
    Q_ASSERT(!m_Viewhandler.isNull());
    GLC_QuickViewHandler* pViewHandler= m_Viewhandler.data();

    connect(pViewHandler, SIGNAL(isDirty()), this, SLOT(update()), Qt::DirectConnection);
    connect(pViewHandler, SIGNAL(invalidateSelectionBuffer()), this, SLOT(invalidateSelectionBuffer()), Qt::DirectConnection);
    connect(pViewHandler, SIGNAL(acceptHoverEvent(bool)), this, SLOT(setMouseTracking(bool)));
    connect(pViewHandler, SIGNAL(selectionChanged()), m_pQuickSelection, SLOT(update()));
    connect(m_pQuickSelection, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
    connect(this, SIGNAL(frameBufferCreationFailed()), pViewHandler, SIGNAL(frameBufferCreationFailed()));
    connect(this, SIGNAL(frameBufferBindingFailed()), pViewHandler, SIGNAL(frameBufferBindingFailed()));

    m_pCamera->setCamera(m_Viewhandler->viewportHandle()->cameraHandle());
    m_pQuickSelection->setWorld(m_Viewhandler->world());
}

void GLC_QuickItem::invalidateSelectionBuffer()
{
    m_SelectionBufferIsDirty= true;
}

void GLC_QuickItem::setMouseTracking(bool track)
{
    setAcceptHoverEvents(track);
}

void GLC_QuickItem::setSource(QString arg)
{
    QUrl url(arg);
    if (url.isLocalFile())
    {
        arg= QUrl(arg).toLocalFile();
    }

    if (m_Source != arg)
    {
        try
        {
            QFile file(arg);
            GLC_World world= GLC_Factory::instance()->createWorldFromFile(file);
            m_Viewhandler->setWorld(world);
            m_pQuickSelection->setWorld(world);
            emit sourceChanged(arg);
            m_Source = arg;
        }
        catch (GLC_Exception& e)
        {
            qWarning() << e.what();
        }

    }
}

void GLC_QuickItem::setSpacePartitionningEnabled(bool enabled)
{
    if (!m_Viewhandler.isNull() && (enabled != m_Viewhandler->spacePartitionningEnabled()))
    {
        m_Viewhandler->setSpacePartitionningEnabled(enabled);
        emit spacePartitionningEnabledChanged(enabled);
    }
}

void GLC_QuickItem::setDefaultUpVector(const QVector3D &vect)
{
    if (!m_Viewhandler.isNull() && (defaultUpVector() != vect))
    {
        m_Viewhandler->setDefaultUpVector(GLC_Vector3d(vect));
        update();
    }
}

void GLC_QuickItem::select(uint id)
{
    if (!m_Viewhandler.isNull())
    {
        GLC_World world= m_Viewhandler->world();
        world.unselectAll();
        world.select(id);
        m_pQuickSelection->update();
    }
}

void GLC_QuickItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    deleteViewBuffers();

    delete m_pAuxFbo;
    m_pAuxFbo= NULL;
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode* GLC_QuickItem::updatePaintNode(QSGNode* pNode, UpdatePaintNodeData* pData)
{
    glFinish();
    QSGSimpleTextureNode* pTextureNode = static_cast<QSGSimpleTextureNode*>(pNode);

    if (pTextureNode == NULL)
    {
        pTextureNode = new QSGSimpleTextureNode();
        pTextureNode->setTexture(this->window()->createTextureFromId(0, QSize(0,0)));
    }

    if ((NULL != m_Viewhandler) && m_Viewhandler->isEnable())
    {
        bool widthOk= this->width() > 0.0;
        bool heightOk= this->height() > 0.0;

        if (m_Viewhandler->screenShotModeIsOn())
        {
            GLC_ScreenShotSettings screenShotSettings= m_Viewhandler->screenShotSettings();
            QSize size= screenShotSettings.size();

            widthOk= size.width() > 0.0;
            heightOk= size.height() > 0.0;
        }

        if (m_Viewhandler && widthOk && heightOk)
        {
            if (widthValid() && heightValid() && isComponentComplete())
            {
                if (m_Viewhandler->screenShotModeIsOn())
                {
                    renderForScreenShot();
                }
                else if (m_Viewhandler->renderingMode() == GLC_ViewHandler::normalRenderMode)
                {
                    render(pTextureNode, pData);
                }
                else
                {
                    renderForSelection();
                }
            }
        }
        else
        {
            deleteViewBuffers();
            delete m_pAuxFbo;
            m_pAuxFbo= NULL;
        }

        m_Viewhandler->renderingFinished();

        glFinish();
    }

    return pTextureNode;
}

void GLC_QuickItem::mousePressEvent(QMouseEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processMousePressEvent(e);
    }
}

void GLC_QuickItem::mouseMoveEvent(QMouseEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processMouseMoveEvent(e);
    }
}

void GLC_QuickItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processMouseReleaseEvent(e);
    }

    forceActiveFocus();
}

void GLC_QuickItem::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processMouseDblClickEvent(e);
    }
}

void GLC_QuickItem::wheelEvent(QWheelEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processWheelEvent(e);
    }
}

void GLC_QuickItem::touchEvent(QTouchEvent *e)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processTouchEvent(e);
    }
}

void GLC_QuickItem::hoverMoveEvent(QHoverEvent *event)
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->processHoverMoveEvent(event);
    }
}

void GLC_QuickItem::setOpenGLState()
{
    if (NULL != m_Viewhandler)
    {
        m_Viewhandler->viewportHandle()->initGl();
    }
}

void GLC_QuickItem::initConnections()
{
    connect(m_pCamera, SIGNAL(updateView()), this, SLOT(update()));
}

void GLC_QuickItem::render(QSGSimpleTextureNode *pTextureNode, UpdatePaintNodeData *pData)
{
    Q_ASSERT(NULL != m_Viewhandler);

    const int x = this->x();
    const int y = this->y();

    int width= this->width();
    int height= this->height();

    setupFbo(width, height, pTextureNode);

    // Flip Y-axis
    QMatrix4x4 flipY;
    flipY.translate(width*0.5, height*0.5);
    flipY.scale(1.0, -1.0);
    flipY.translate(-width*0.5 + x, -height*0.5 - y);
    pData->transformNode->setMatrix(flipY);

    if (m_pTargetFbo && m_pTargetFbo->isValid() && m_pSourceFbo && m_pSourceFbo->isValid())
    {
        pushOpenGLMatrix();
        setOpenGLState();

        if (!m_pSourceFbo->bind()) emit frameBufferBindingFailed();

        m_Viewhandler->setSize(width, height);

        doRender();

        m_pSourceFbo->release();

        QRect rect(0, 0, width, height);
        QOpenGLFramebufferObject::blitFramebuffer(m_pTargetFbo, rect, m_pSourceFbo, rect);

        glFinish();
        delete m_pSourceFbo;
        m_pSourceFbo= NULL;
        popOpenGLMatrix();
    }
    else
    {
        delete m_pSourceFbo;
        m_pSourceFbo= NULL;

        delete m_pTargetFbo;
        m_pTargetFbo= NULL;
    }
}

void GLC_QuickItem::renderForSelection()
{
    Q_ASSERT(NULL != m_Viewhandler);
    setupAuxFbo(this->width(), this->height());

    if (m_pAuxFbo && m_pAuxFbo->isValid())
    {
        pushOpenGLMatrix();
        setOpenGLState();

        if (!m_pAuxFbo->bind()) emit frameBufferBindingFailed();

        if (m_SelectionBufferIsDirty)
        {
            m_Viewhandler->setSize(width(), height());
            GLC_State::setSelectionMode(true);
            doRender();
            GLC_State::setSelectionMode(false);
            m_SelectionBufferIsDirty= false;
        }

        // Get selection coordinate
        const int x= m_Viewhandler->pointerPosition().x();
        const int y= m_Viewhandler->pointerPosition().y();

        GLC_World world= m_Viewhandler->world();
        GLC_SelectionSet selectionSet;

        const GLC_uint instanceId= m_Viewhandler->viewportHandle()->selectOnPreviousRender(x, y, GL_COLOR_ATTACHMENT0);
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
    }

    m_pAuxFbo->release();
    popOpenGLMatrix();
}

void GLC_QuickItem::renderForScreenShot()
{
    Q_ASSERT(NULL != m_Viewhandler);
    GLC_ScreenShotSettings screenShotSettings= m_Viewhandler->screenShotSettings();
    const int width= screenShotSettings.size().width();
    const int height= screenShotSettings.size().height();

    setupScreenShotFbo(width, height);

    if (m_pScreenShotFbo && m_pScreenShotFbo->isValid())
    {
        pushOpenGLMatrix();
        setOpenGLState();

        if (!m_pScreenShotFbo->bind()) emit frameBufferBindingFailed();

        m_Viewhandler->setSize(width, height);
        doRender();

        m_pScreenShotFbo->release();
        popOpenGLMatrix();
    }

    QImage screenShot= m_pScreenShotFbo->toImage();

    delete m_pScreenShotFbo;
    m_pScreenShotFbo= NULL;

    m_Viewhandler->setScreenShotImage(screenShot);
}

GLC_uint GLC_QuickItem::selectBody(GLC_uint instanceId, int x, int y)
{
    Q_ASSERT(NULL != m_Viewhandler);
    GLC_uint subject= 0;
    GLC_3DViewCollection* pCollection= m_Viewhandler->world().collection();
    if (pCollection->contains(instanceId))
    {
        GLC_Viewport* pView= m_Viewhandler->viewportHandle();
        GLC_3DViewInstance* pInstance= pCollection->instanceHandle(instanceId);
        subject= pView->selectBody(pInstance, x, y, GL_COLOR_ATTACHMENT0);
        m_SelectionBufferIsDirty= true;
    }
    return subject;
}

QPair<GLC_uint, GLC_uint> GLC_QuickItem::selectPrimitive(GLC_uint instanceId, int x, int y)
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
        m_SelectionBufferIsDirty= true;
    }
    return subject;
}

void GLC_QuickItem::doRender()
{
    glFinish();
    m_Viewhandler->render();
    glFinish();
}

void GLC_QuickItem::setupFbo(int width, int height, QSGSimpleTextureNode *pTextureNode)
{
    Q_ASSERT(NULL != m_Viewhandler);
    delete m_pSourceFbo;
    m_pSourceFbo= NULL;

    delete m_pTargetFbo;
    m_pTargetFbo= NULL;

    if ((width > 0) && (height > 0))
    {
        QOpenGLFramebufferObjectFormat sourceFormat;
        sourceFormat.setAttachment(QOpenGLFramebufferObject::Depth);
        sourceFormat.setSamples(m_Viewhandler->samples());

        m_pSourceFbo= new QOpenGLFramebufferObject(width, height, sourceFormat);
        m_pTargetFbo= new QOpenGLFramebufferObject(width, height);
        pTextureNode->setTexture(this->window()->createTextureFromId(m_pTargetFbo->texture(), m_pTargetFbo->size()));
        pTextureNode->setRect(this->boundingRect());

        // Test frame buffer validity
        const bool isValid= (m_pSourceFbo->isValid() && m_pTargetFbo->isValid());
        if (!isValid) emit frameBufferCreationFailed();
    }
    else
    {
        pTextureNode->setTexture(this->window()->createTextureFromId(0, QSize(0,0)));
        pTextureNode->setRect(this->boundingRect());
    }
}

void GLC_QuickItem::setupAuxFbo(int width, int height)
{
    Q_ASSERT(NULL != m_Viewhandler);

    if ((width > 0) && (height > 0))
    {
        if (NULL == m_pAuxFbo)
        {
            m_pAuxFbo= new QOpenGLFramebufferObject(width, height, QOpenGLFramebufferObject::Depth);
            m_SelectionBufferIsDirty= true;

            // Test frame buffer validity
            const bool isValid= m_pAuxFbo->isValid();
            if (!isValid) emit frameBufferCreationFailed();
        }
    }
    else
    {
        delete m_pAuxFbo;
        m_pAuxFbo= NULL;
    }
}

void GLC_QuickItem::setupScreenShotFbo(int width, int height)
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

void GLC_QuickItem::pushOpenGLMatrix()
{
    GLC_Context* pCurrentContext= GLC_Context::current();

    pCurrentContext->glcMatrixMode(GL_TEXTURE);
    pCurrentContext->glcPushMatrix();
    pCurrentContext->glcLoadIdentity();

    pCurrentContext->glcMatrixMode(GL_PROJECTION);
    pCurrentContext->glcPushMatrix();

    pCurrentContext->glcMatrixMode(GL_MODELVIEW);
    pCurrentContext->glcPushMatrix();
}

void GLC_QuickItem::popOpenGLMatrix()
{
    GLC_Context* pCurrentContext= GLC_Context::current();
    pCurrentContext->glcMatrixMode(GL_TEXTURE);
    pCurrentContext->glcPopMatrix();

    pCurrentContext->glcMatrixMode(GL_PROJECTION);
    pCurrentContext->glcPopMatrix();

    pCurrentContext->glcMatrixMode(GL_MODELVIEW);
    pCurrentContext->glcPopMatrix();
}

void GLC_QuickItem::deleteViewBuffers()
{
    delete m_pSourceFbo;
    m_pSourceFbo= NULL;

    delete m_pTargetFbo;
    m_pTargetFbo= NULL;
}
