/*
 *
 *
 *  Created on: 24/01/2016
 *      Author: Laurent Ribon
 */

#include <QColor>

#include <GLC_Exception>
#include <GLC_Factory>

#include "OpenGLView.h"

OpenGLView::OpenGLView(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_GlView()
    , m_MoverController()
    , m_Light()
    , m_World()
{
    connect(&m_GlView, SIGNAL(updateOpenGL()), this, SLOT(update()));

    // Set up mover controller
    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
    m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_GlView);
    m_GlView.cameraHandle()->setIsoView();
    m_Light.setPosition(1.0, 1.0, 1.0);

    qDebug() << "OpenGLView format " << this->format().majorVersion();
}

void OpenGLView::setWorld(const GLC_World &world)
{
    m_World= world;
    m_GlView.reframe(m_World.boundingBox());
    update();
}

void OpenGLView::initializeGL()
{
    // OpenGL initialisation
    GLC_State::init();
    m_GlView.initGl();
    qDebug() << GLC_State::version();
}

void OpenGLView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLC_Context::current()->glcLoadIdentity();
    try
    {
        // Enable and execute lighting
        m_Light.glExecute();
        m_GlView.setDistMinAndMax(m_World.boundingBox());
        m_GlView.glExecuteCam();

        m_GlView.useClipPlane(true);

        // Display non transparent normal object
        m_World.render(0, glc::ShadingFlag);

        // Display transparent normal object
        if (!GLC_State::isInSelectionMode())
        {
            m_World.render(0, glc::TransparentRenderFlag);
            // Display transparent instance of the shaders group
        }

        // Display Selected Objects
        m_World.render(1, glc::ShadingFlag);

        m_GlView.useClipPlane(false);

        if (GLC_State::isInSelectionMode()) // Don't display orbit circle in snapshootmode
        {
            m_MoverController.drawActiveMoverRep();
        }
    }
    catch (GLC_Exception &e)
    {
        qDebug() << e.what();
    }

}

void OpenGLView::resizeGL(int width, int height)
{
    m_GlView.setWinGLSize(width, height);
}

