/*
 * glc_declarativeitem.cpp
 *
 *  Created on: 22/04/2013
 *      Author: laumaya
 */

#include <QSGSimpleTextureNode>
#include <QQuickWindow>

#include <GLC_Context>
#include <GLC_Exception>
#include <GLC_Factory>

#include "glc_quickitem.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLC_QuickItem::GLC_QuickItem(GLC_QuickItem *pParent)
    : QQuickItem(pParent)
    , m_Viewport()
    , m_World()
    , m_Light()
    , m_MoverController()
    , m_FirstRender(true)
    , m_pSourceFbo(NULL)
    , m_pTargetFbo(NULL)
    , m_pSelectionFbo(NULL)
    , m_CurrentPos()
    , m_IsinSelectionMode(false)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
    setFlag(QQuickItem::ItemHasContents);

    // Set some GLC_lib state in order to render heavy scene
    GLC_State::setDefaultOctreeDepth(4);
    GLC_State::setPixelCullingUsage(true);
    GLC_State::setSpacePartionningUsage(true);
    m_Viewport.setMinimumPixelCullingSize(6);

    m_Light.setTwoSided(true);
    m_Light.setPosition(10.0, 10.0, 10.0);

    m_Viewport.cameraHandle()->setEyeCam(GLC_Point3d(1.0, 1.0, 1.0));

    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
    m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_Viewport);
}

GLC_QuickItem::~GLC_QuickItem()
{
    delete m_pSourceFbo;
    delete m_pTargetFbo;
    delete m_pSelectionFbo;
}

QVariant GLC_QuickItem::world() const
{
    QVariant subject;
    subject.setValue(m_World);

    return subject;
}

void GLC_QuickItem::setWorld(QVariant worldVariant)
{
    GLC_World world= worldVariant.value<GLC_World>();
    if (!world.isEmpty())
    {
        m_World= world;
        m_Viewport.reframe(world.boundingBox());
    }
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


    if (!m_World.isEmpty() && widthValid() && heightValid() && isComponentComplete())
    {
        if (m_IsinSelectionMode)
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
    if (!m_MoverController.hasActiveMover())
    {
        const QSize oldSize= m_Viewport.size();

        switch (e->button())
        {
        case (Qt::RightButton):
            m_Viewport.setWinGLSize(width(), height(), false);
            m_MoverController.setActiveMover(GLC_MoverController::TrackBall, GLC_UserInput(e->pos().x(), e->pos().y()));
            m_Viewport.setWinGLSize(oldSize, false);
            update();
            break;
        case (Qt::LeftButton):
            select(e->pos().x(), e->pos().y());
            break;
        case (Qt::MidButton):
            m_Viewport.setWinGLSize(width(), height(), false);
            m_MoverController.setActiveMover(GLC_MoverController::Zoom, GLC_UserInput(e->pos().x(), e->pos().y()));
            m_Viewport.setWinGLSize(oldSize, false);
            update();
            break;

        default:
            break;
        }
    }
}

void GLC_QuickItem::mouseMoveEvent(QMouseEvent *e)
{
    if (m_MoverController.hasActiveMover())
    {
        QSize oldSize= m_Viewport.size();
        m_Viewport.setWinGLSize(width(), height(), false);
        m_MoverController.move(GLC_UserInput(e->pos().x(), e->pos().y()));
        m_Viewport.setWinGLSize(oldSize, false);
        update();
    }
}

void GLC_QuickItem::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    if (m_MoverController.hasActiveMover())
    {
        m_MoverController.setNoMover();
        update();
    }
}

void GLC_QuickItem::initGl()
{
    m_Viewport.initGl();
    glEnable(GL_NORMALIZE);

    m_FirstRender= false;
}

void GLC_QuickItem::render(QSGSimpleTextureNode *pTextureNode, UpdatePaintNodeData *pData)
{
    const int width= this->width();
    const int height= this->height();

    setupFbo(width, height, pTextureNode);
//    // Flip Y-axis
//    QMatrix4x4 flipY;
//    flipY.translate(width*0.5, height*0.5);
//    flipY.scale(1.0, -1.0);
//    flipY.translate(-width*0.5, -height*0.5);
//    pData->transformNode->setMatrix(flipY * pData->transformNode->matrix());

    if (m_pTargetFbo && m_pTargetFbo->isValid() && m_pSourceFbo && m_pSourceFbo->isValid())
    {
        initGl();

        pushMatrix();
        m_pSourceFbo->bind();

        m_Viewport.setWinGLSize(width, height);
        // Calculate camera depth of view
        m_Viewport.setDistMinAndMax(m_World.boundingBox());

        renderWorld();

        m_pSourceFbo->release();
        popMatrix();

        QRect rect(0, 0, width, height);
        QOpenGLFramebufferObject::blitFramebuffer(m_pTargetFbo, rect, m_pSourceFbo, rect);
    }
    else
    {
        delete m_pSourceFbo;
        m_pSourceFbo= NULL;

        delete m_pTargetFbo;
        m_pTargetFbo= NULL;
    }
}

void GLC_QuickItem::renderForSelection(QSGSimpleTextureNode *pTextureNode, UpdatePaintNodeData* pData)
{

    setupFbo(this->width(), this->height(), pTextureNode);
    setupSelectionFbo(this->width(), this->height());

    initGl();

    pushMatrix();
    m_pSelectionFbo->bind();

    m_Viewport.setWinGLSize(width(), height());
    // Calculate camera depth of view
    m_Viewport.setDistMinAndMax(m_World.boundingBox());

    GLC_State::setSelectionMode(true);
    renderWorld();
    GLC_uint selectionId= m_Viewport.selectOnPreviousRender(m_CurrentPos.x(), m_CurrentPos.y(), GL_COLOR_ATTACHMENT0);
    GLC_State::setSelectionMode(false);
    m_IsinSelectionMode= false;

    m_pSelectionFbo->release();
    popMatrix();

    m_World.unselectAll();
    if (m_World.containsOccurence(selectionId))
    {
        m_World.select(m_World.occurence(selectionId));
    }

    render(pTextureNode, pData);
}

void GLC_QuickItem::renderWorld()
{
    try
    {
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Load identity matrix
        GLC_Context::current()->glcLoadIdentity();

        m_World.collection()->updateInstanceViewableState();

        m_Light.glExecute();
        m_Viewport.glExecuteCam();

        m_World.render(0, glc::ShadingFlag);
        m_World.render(0, glc::TransparentRenderFlag);

        m_World.render(1, glc::ShadingFlag);

        m_MoverController.drawActiveMoverRep();
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
            QOpenGLFramebufferObjectFormat sourceFormat;
            sourceFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            sourceFormat.setSamples(8);

            m_pSourceFbo= new QOpenGLFramebufferObject(width, height, sourceFormat);
            m_pTargetFbo= new QOpenGLFramebufferObject(width, height);
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
        m_pSelectionFbo= new QOpenGLFramebufferObject(width, height, QGLFramebufferObject::CombinedDepthStencil);
    }
}

void GLC_QuickItem::pushMatrix()
{
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    GLC_Context* pCurrentContext= GLC_Context::current();
    pCurrentContext->glcMatrixMode(GL_PROJECTION);
    pCurrentContext->glcPushMatrix();
    pCurrentContext->glcMatrixMode(GL_MODELVIEW);
    pCurrentContext->glcPushMatrix();
}

void GLC_QuickItem::popMatrix()
{
    glPopAttrib();
    glPopClientAttrib();

    GLC_Context* pCurrentContext= GLC_Context::current();
    pCurrentContext->glcMatrixMode(GL_PROJECTION);
    pCurrentContext->glcPopMatrix();
    pCurrentContext->glcMatrixMode(GL_MODELVIEW);
    pCurrentContext->glcPopMatrix();
}

void GLC_QuickItem::select(qreal x, qreal y)
{
    m_IsinSelectionMode= true;
    m_CurrentPos.setVect(x, y);
    update();
}
