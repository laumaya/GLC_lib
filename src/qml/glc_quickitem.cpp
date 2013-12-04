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

#include <QSGSimpleTextureNode>
#include <QQuickWindow>

#include "../glc_context.h"
#include "../glc_exception.h"
#include "../glc_factory.h"

#include "glc_quickitem.h"
#include "../viewport/glc_viewhandler.h"

GLC_QuickItem::GLC_QuickItem(GLC_QuickItem *pParent)
    : QQuickItem(pParent)
    , m_Anchor()
    , m_pSourceFbo(NULL)
    , m_pTargetFbo(NULL)
    , m_pSelectionFbo(NULL)
    , m_CurrentPos()
    , m_IsInSelectionMode(false)
    , m_SelectionBufferIsDirty(true)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
    setFlag(QQuickItem::ItemHasContents);

    connect(m_Anchor.data(), SIGNAL(isDirty()), this, SLOT(update()));
}

GLC_QuickItem::~GLC_QuickItem()
{
    delete m_pSourceFbo;
    delete m_pTargetFbo;
    delete m_pSelectionFbo;
}

QVariant GLC_QuickItem::viewHandler() const
{
    QVariant subject;
    subject.setValue(m_Anchor);

    return subject;
}

void GLC_QuickItem::setViewhandler(QVariant viewHandler)
{
    disconnect(m_Anchor.data(), SIGNAL(isDirty()), this, SLOT(update()));
    m_Anchor= viewHandler.value<GLC_ViewHandler>();
    connect(m_Anchor.data(), SIGNAL(isDirty()), this, SLOT(update()));
}

void GLC_QuickItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    delete m_pSourceFbo;
    m_pSourceFbo= NULL;

    delete m_pTargetFbo;
    m_pTargetFbo= NULL;

    delete m_pSelectionFbo;
    m_pSelectionFbo= NULL;
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode* GLC_QuickItem::updatePaintNode(QSGNode* pNode, UpdatePaintNodeData* pData)
{
    QSGSimpleTextureNode* pTextureNode = static_cast<QSGSimpleTextureNode*>(pNode);

    if (!pTextureNode)
        pTextureNode = new QSGSimpleTextureNode();

    GLC_World world= m_Anchor.world();

    if (!world.isEmpty() && widthValid() && heightValid() && isComponentComplete())
    {
        if (m_IsInSelectionMode)
        {
            renderForSelection(pTextureNode, pData);
        }
        else
        {
            render(pTextureNode, pData);
        }
    }

    return pTextureNode;
}

void GLC_QuickItem::mousePressEvent(QMouseEvent *e)
{
    if (!m_Anchor.moverControllerHandle()->hasActiveMover())
    {
        switch (e->button())
        {
        case (Qt::RightButton):
            m_Anchor.viewportHandle()->setWinGLSize(width(), height(), false);
            m_Anchor.moverControllerHandle()->setActiveMover(GLC_MoverController::TrackBall, GLC_UserInput(e->pos().x(), e->pos().y()));
            m_Anchor.world().collection()->setLodUsage(true, m_Anchor.viewportHandle());
            update();
            break;
        case (Qt::LeftButton):
            select(e->pos().x(), e->pos().y());
            break;
        case (Qt::MidButton):
            m_Anchor.viewportHandle()->setWinGLSize(width(), height(), false);
            m_Anchor.moverControllerHandle()->setActiveMover(GLC_MoverController::Zoom, GLC_UserInput(e->pos().x(), e->pos().y()));
            m_Anchor.world().collection()->setLodUsage(true, m_Anchor.viewportHandle());
            update();
            break;

        default:
            break;
        }
    }
}

void GLC_QuickItem::mouseMoveEvent(QMouseEvent *e)
{
    if (m_Anchor.moverControllerHandle()->hasActiveMover())
    {
        m_Anchor.viewportHandle()->setWinGLSize(width(), height(), false);
        m_Anchor.moverControllerHandle()->move(GLC_UserInput(e->pos().x(), e->pos().y()));
        update();
    }
}

void GLC_QuickItem::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    if (m_Anchor.moverControllerHandle()->hasActiveMover())
    {
        m_Anchor.moverControllerHandle()->setNoMover();
        m_Anchor.world().collection()->setLodUsage(false, m_Anchor.viewportHandle());
        m_SelectionBufferIsDirty= true;
        update();
    }
}

void GLC_QuickItem::setOpenGLState()
{
    m_Anchor.viewportHandle()->initGl();
}

void GLC_QuickItem::render(QSGSimpleTextureNode *pTextureNode, UpdatePaintNodeData *pData)
{
    qDebug() << "GLC_QuickItem::render";
    QTime time;
    time.start();

    int x = this->x();
    int y = this->y();

    const int width= this->width();
    const int height= this->height();

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

        m_pSourceFbo->bind();

        m_Anchor.viewportHandle()->setWinGLSize(width, height);

        QTime time2;
        time2.start();
        doRender();
        qDebug() << "Render time " << time2.elapsed();

        m_pSourceFbo->release();

        QRect rect(0, 0, width, height);
        QTime time3;
        time3.start();
        QOpenGLFramebufferObject::blitFramebuffer(m_pTargetFbo, rect, m_pSourceFbo, rect);
        qDebug() << "blitFramebuffer time " << time3.elapsed();
        popOpenGLMatrix();

    }
    else
    {
        delete m_pSourceFbo;
        m_pSourceFbo= NULL;

        delete m_pTargetFbo;
        m_pTargetFbo= NULL;
    }
    qDebug() << "GLC_QuickItem::render" << time.elapsed();
}

void GLC_QuickItem::renderForSelection(QSGSimpleTextureNode *pTextureNode, UpdatePaintNodeData* pData)
{

    pushOpenGLMatrix();
    setupSelectionFbo(this->width(), this->height());

    setOpenGLState();

    m_pSelectionFbo->bind();


    if (m_SelectionBufferIsDirty)
    {
        qDebug() << "m_SelectionBufferIsDirty";
        m_Anchor.viewportHandle()->setWinGLSize(width(), height());
        GLC_State::setSelectionMode(true);
        doRender();
        GLC_State::setSelectionMode(false);
        m_SelectionBufferIsDirty= false;
    }
    GLC_uint selectionId= m_Anchor.viewportHandle()->selectOnPreviousRender(m_CurrentPos.x(), m_CurrentPos.y(), GL_COLOR_ATTACHMENT0);

    m_IsInSelectionMode= false;

    m_pSelectionFbo->release();
    popOpenGLMatrix();

    GLC_World world= m_Anchor.world();
    world.unselectAll();
    if (world.containsOccurence(selectionId))
    {
        world.select(world.occurence(selectionId));
    }
    render(pTextureNode, pData);
}

void GLC_QuickItem::doRender()
{
    defaultRenderWorld();
}

void GLC_QuickItem::defaultRenderWorld()
{
    try
    {
        QOpenGLContext::currentContext()->functions()->glUseProgram(0);

        GLC_World world= m_Anchor.world();
        GLC_Viewport* pViewport= m_Anchor.viewportHandle();

        // Calculate camera depth of view
        pViewport->setDistMinAndMax(world.boundingBox());
        world.collection()->updateInstanceViewableState();

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Load identity matrix
        GLC_Context::current()->glcLoadIdentity();

        m_Anchor.lightHandle()->glExecute();
        pViewport->glExecuteCam();

        world.render(0, glc::WireRenderFlag);
        world.render(0, glc::TransparentRenderFlag);
        world.render(1, glc::WireRenderFlag);
        m_Anchor.lightHandle()->disable();

        m_Anchor.moverControllerHandle()->drawActiveMoverRep();
    }
    catch (GLC_Exception &e)
    {
        qDebug() << e.what();
    }
}

void GLC_QuickItem::setupFbo(int width, int height, QSGSimpleTextureNode *pTextureNode)
{
    if (width && height)
    {
        if (NULL == m_pSourceFbo)
        {
            Q_ASSERT(NULL == m_pTargetFbo);
            qDebug() << "About to create render Fbo";
            QTime time;
            time.start();

            QOpenGLFramebufferObjectFormat sourceFormat;
            sourceFormat.setAttachment(QOpenGLFramebufferObject::Depth);
            sourceFormat.setSamples(m_Anchor.samples());

            m_pSourceFbo= new QOpenGLFramebufferObject(width, height, sourceFormat);
            m_pTargetFbo= new QOpenGLFramebufferObject(width, height);
            qDebug() << "render fbo created in " << time.elapsed();
        }

        pTextureNode->setTexture(this->window()->createTextureFromId(m_pTargetFbo->texture(), m_pTargetFbo->size()));
    }
    else
    {
        pTextureNode->setTexture(this->window()->createTextureFromId(0, QSize(0,0)));
    }

    pTextureNode->setRect(this->boundingRect());
}

void GLC_QuickItem::setupSelectionFbo(int width, int height)
{
    if (NULL == m_pSelectionFbo)
    {
        qDebug() << "About to create m_pSelectionFbo";
        QTime time;
        time.start();
        m_pSelectionFbo= new QOpenGLFramebufferObject(width, height, QOpenGLFramebufferObject::Depth);
        qDebug() << "m_pSelectionFbo created in " << time.elapsed();
        m_SelectionBufferIsDirty= true;
    }
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

void GLC_QuickItem::select(qreal x, qreal y)
{
    m_IsInSelectionMode= true;
    m_CurrentPos.setVect(x, y);
    update();
}
