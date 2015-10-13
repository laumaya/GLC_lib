/*
 * glc_declarativeitem.cpp
 *
 *  Created on: 22/04/2013
 *      Author: laumaya
 */

#include <QGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_1_0>
#include <GLC_Context>
#include <GLC_Exception>
#include <GLC_Factory>

#include "glc_declarativeitem.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLC_DeclarativeItem::GLC_DeclarativeItem(QDeclarativeItem *pParent)
    : QDeclarativeItem(pParent)
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
    setFlag(QGraphicsItem::ItemHasNoContents, false);

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

GLC_DeclarativeItem::~GLC_DeclarativeItem()
{
    delete m_pSourceFbo;
    delete m_pTargetFbo;
    delete m_pSelectionFbo;
}

QVariant GLC_DeclarativeItem::world() const
{
    QVariant subject;
    subject.setValue(m_World);

    return subject;
}

void GLC_DeclarativeItem::setWorld(QVariant worldVariant)
{
    GLC_World world= worldVariant.value<GLC_World>();
    if (!world.isEmpty())
    {
        m_World= world;
        m_Viewport.reframe(world.boundingBox());
    }
}

void GLC_DeclarativeItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    delete m_pSourceFbo;
    m_pSourceFbo= NULL;

    delete m_pTargetFbo;
    m_pTargetFbo= NULL;

    delete m_pSelectionFbo;
    m_pSelectionFbo= NULL;
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void GLC_DeclarativeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    bool useOpenGL= (painter->paintEngine()->type() == QPaintEngine::OpenGL);
    useOpenGL= useOpenGL || (painter->paintEngine()->type() == QPaintEngine::OpenGL2);

    if (!useOpenGL)
    {
        qWarning("GLC_DeclarativeItem: needs OpenGL paint engine");
        return;
    }

    if (!m_World.isEmpty() && widthValid() && heightValid() && isComponentComplete())
    {
        if (m_IsinSelectionMode)
        {
            renderForSelection(painter);
        }
        else
        {
            render(painter);
        }
    }
}

void GLC_DeclarativeItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
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

void GLC_DeclarativeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
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

void GLC_DeclarativeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);

    if (m_MoverController.hasActiveMover())
    {
        m_MoverController.setNoMover();
        update();
    }
}

void GLC_DeclarativeItem::initGl()
{
    m_Viewport.initGl();
    QOpenGLContext::currentContext()->functions()->glEnable(GL_NORMALIZE);

    m_FirstRender= false;
}

void GLC_DeclarativeItem::render(QPainter *painter)
{
    painter->beginNativePainting();

    setupFbo(this->width(), this->height());
    if (m_pTargetFbo && m_pTargetFbo->isValid() && m_pSourceFbo && m_pSourceFbo->isValid())
    {
        initGl();

        pushMatrix();
        m_pSourceFbo->bind();

        m_Viewport.setWinGLSize(width(), height());
        // Calculate camera depth of view
        m_Viewport.setDistMinAndMax(m_World.boundingBox());

        renderWorld();

        m_pSourceFbo->release();
        m_Viewport.setWinGLSize(painter->device()->width(), painter->device()->height());
        popMatrix();

        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glDisable(GL_DEPTH_TEST);
        f->glEnable(GL_TEXTURE_2D);

        QOpenGLFunctions_1_0 *glFuncs_1_0 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_1_0 >();
        if (!glFuncs_1_0)
        {
            qWarning() << "Could not obtain required OpenGL context version";
            exit(1);
        }
        else
        {
            glFuncs_1_0->glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

            QRect rect(0, 0, width(), height());
            QGLFramebufferObject::blitFramebuffer(m_pTargetFbo, rect, m_pSourceFbo, rect);

            m_pTargetFbo->drawTexture(QRectF(0.0, 0.0, width(), height()), m_pTargetFbo->texture());
            glFuncs_1_0->glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }
    }
    else
    {
        delete m_pSourceFbo;
        m_pSourceFbo= NULL;

        delete m_pTargetFbo;
        m_pTargetFbo= NULL;
    }
    painter->endNativePainting();
}

void GLC_DeclarativeItem::renderForSelection(QPainter *painter)
{
    painter->beginNativePainting();

    setupFbo(this->width(), this->height());
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
    m_Viewport.setWinGLSize(painter->device()->width(), painter->device()->height());
    popMatrix();

    QOpenGLContext::currentContext()->functions()->glDisable(GL_DEPTH_TEST);

    painter->endNativePainting();

    m_World.unselectAll();
    if (m_World.containsOccurence(selectionId))
    {
        m_World.select(m_World.occurence(selectionId));
    }

    render(painter);
}

void GLC_DeclarativeItem::renderWorld()
{
    try
    {
        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        // Clear screen
        f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void GLC_DeclarativeItem::setupFbo(int width, int height)
{
    if (NULL == m_pSourceFbo)
    {
        Q_ASSERT(NULL == m_pTargetFbo);
        QGLFramebufferObjectFormat sourceFormat;
        sourceFormat.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
        sourceFormat.setSamples(8);

        m_pSourceFbo= new QGLFramebufferObject(width, height, sourceFormat);
        m_pTargetFbo= new QGLFramebufferObject(width, height, QGLFramebufferObject::CombinedDepthStencil);
    }
}

void GLC_DeclarativeItem::setupSelectionFbo(int width, int height)
{
    if (NULL == m_pSelectionFbo)
    {
        m_pSelectionFbo= new QGLFramebufferObject(width, height, QGLFramebufferObject::CombinedDepthStencil);
    }
}

void GLC_DeclarativeItem::pushMatrix()
{
    GLC_Context* pCurrentContext= GLC_Context::current();
    pCurrentContext->glcMatrixMode(GL_PROJECTION);
    pCurrentContext->glcPushMatrix();
    pCurrentContext->glcMatrixMode(GL_MODELVIEW);
    pCurrentContext->glcPushMatrix();
}

void GLC_DeclarativeItem::popMatrix()
{
    GLC_Context* pCurrentContext= GLC_Context::current();
    pCurrentContext->glcMatrixMode(GL_PROJECTION);
    pCurrentContext->glcPopMatrix();
    pCurrentContext->glcMatrixMode(GL_MODELVIEW);
    pCurrentContext->glcPopMatrix();
}

void GLC_DeclarativeItem::select(qreal x, qreal y)
{
    m_IsinSelectionMode= true;
    m_CurrentPos.setVect(x, y);
    update();
}
