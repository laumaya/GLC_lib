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
    , m_pViewhandler(NULL)
    , m_pSourceFbo(NULL)
    , m_pTargetFbo(NULL)
    , m_pSelectionFbo(NULL)
    , m_SelectionBufferIsDirty(true)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
    setFlag(QQuickItem::ItemHasContents);
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
    subject.setValue(m_pViewhandler);

    return subject;
}

void GLC_QuickItem::setViewhandler(QVariant viewHandler)
{
    if (m_pViewhandler)
    {
        disconnect(m_pViewhandler, SIGNAL(isDirty()), this, SLOT(update()));
        disconnect(m_pViewhandler, SIGNAL(invalidateSelectionBuffer()), this, SLOT(invalidateSelectionBuffer()));
    }

    m_pViewhandler= viewHandler.value<GLC_ViewHandler*>();

    connect(m_pViewhandler, SIGNAL(isDirty()), this, SLOT(update()), Qt::DirectConnection);
    connect(m_pViewhandler, SIGNAL(invalidateSelectionBuffer()), this, SLOT(invalidateSelectionBuffer()), Qt::DirectConnection);
}

void GLC_QuickItem::invalidateSelectionBuffer()
{
    m_SelectionBufferIsDirty= true;
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
    {
        pTextureNode = new QSGSimpleTextureNode();
        pTextureNode->setTexture(this->window()->createTextureFromId(0, QSize(0,0)));
    }

    const bool widthOk= this->width() > 0.0;
    const bool heightOk= this->height() > 0.0;

    if (m_pViewhandler && widthOk && heightOk)
    {
        if (widthValid() && heightValid() && isComponentComplete())
        {
            if (m_pViewhandler->isInSelectionMode())
            {
                renderForSelection();
            }
            else
            {
                render(pTextureNode, pData);
            }
        }
        else
        {
            pTextureNode->setTexture(this->window()->createTextureFromId(0, QSize(0,0)));
        }
    }
    else
    {
        pTextureNode->setTexture(this->window()->createTextureFromId(0, QSize(0,0)));
    }

    return pTextureNode;
}

void GLC_QuickItem::mousePressEvent(QMouseEvent *e)
{
    m_pViewhandler->processMousePressEvent(e);
}

void GLC_QuickItem::mouseMoveEvent(QMouseEvent *e)
{
    m_pViewhandler->processMouseMoveEvent(e);
}

void GLC_QuickItem::mouseReleaseEvent(QMouseEvent *e)
{
    m_pViewhandler->processMouseReleaseEvent(e);
}

void GLC_QuickItem::mouseDoubleClickEvent(QMouseEvent *e)
{
    m_pViewhandler->processMouseDblClickEvent(e);
}

void GLC_QuickItem::wheelEvent(QWheelEvent *e)
{
    m_pViewhandler->processWheelEvent(e);
}

void GLC_QuickItem::touchEvent(QTouchEvent *e)
{
    m_pViewhandler->processTouchEvent(e);
}

void GLC_QuickItem::setOpenGLState()
{
    m_pViewhandler->viewportHandle()->initGl();
}

void GLC_QuickItem::render(QSGSimpleTextureNode *pTextureNode, UpdatePaintNodeData *pData)
{
    const int x = this->x();
    const int y = this->y();

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

        m_pViewhandler->viewportHandle()->setWinGLSize(width, height);

        doRender();

        m_pSourceFbo->release();

        QRect rect(0, 0, width, height);
        QOpenGLFramebufferObject::blitFramebuffer(m_pTargetFbo, rect, m_pSourceFbo, rect);
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
    setupSelectionFbo(this->width(), this->height());

    if (m_pSelectionFbo && m_pSelectionFbo->isValid())
    {
        pushOpenGLMatrix();
        setOpenGLState();

        m_pSelectionFbo->bind();

        if (m_SelectionBufferIsDirty)
        {
            qDebug() << "m_SelectionBufferIsDirty";
            m_pViewhandler->viewportHandle()->setWinGLSize(width(), height());
            GLC_State::setSelectionMode(true);
            doRender();
            GLC_State::setSelectionMode(false);
            m_SelectionBufferIsDirty= false;
        }

        // Get selection coordinate
        const int x= m_pViewhandler->selectionPoint().x();
        const int y= m_pViewhandler->selectionPoint().y();

        GLC_uint selectionId= m_pViewhandler->viewportHandle()->selectOnPreviousRender(x, y, GL_COLOR_ATTACHMENT0);

        m_pSelectionFbo->release();
        popOpenGLMatrix();

        m_pViewhandler->updateSelection(selectionId);
    }
}

void GLC_QuickItem::doRender()
{
    m_pViewhandler->render();
}

void GLC_QuickItem::setupFbo(int width, int height, QSGSimpleTextureNode *pTextureNode)
{
    if ((width > 0) && (height > 0))
    {
        if (NULL == m_pSourceFbo)
        {
            Q_ASSERT(NULL == m_pTargetFbo);

            QOpenGLFramebufferObjectFormat sourceFormat;
            sourceFormat.setAttachment(QOpenGLFramebufferObject::Depth);
            sourceFormat.setSamples(m_pViewhandler->samples());

            m_pSourceFbo= new QOpenGLFramebufferObject(width, height, sourceFormat);
            m_pTargetFbo= new QOpenGLFramebufferObject(width, height);
        }
        pTextureNode->setTexture(this->window()->createTextureFromId(m_pTargetFbo->texture(), m_pTargetFbo->size()));
    }
    else
    {
        delete m_pSourceFbo;
        m_pSourceFbo= NULL;

        delete m_pTargetFbo;
        m_pTargetFbo= NULL;

        pTextureNode->setTexture(this->window()->createTextureFromId(0, QSize(0,0)));
    }

    pTextureNode->setRect(this->boundingRect());
}

void GLC_QuickItem::setupSelectionFbo(int width, int height)
{
    if ((width > 0) && (height > 0))
    {
        if (NULL == m_pSelectionFbo)
        {
            m_pSelectionFbo= new QOpenGLFramebufferObject(width, height, QOpenGLFramebufferObject::Depth);
            m_SelectionBufferIsDirty= true;
        }
    }
    else
    {
        delete m_pSelectionFbo;
        m_pSelectionFbo= NULL;
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
