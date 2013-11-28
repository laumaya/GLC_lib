/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

#include <QtDebug>
#include <QGLContext>

#include <GLC_UserInput>
#include <io/glc_colladatoworld.h>
#include <GLC_Context>

#include "glwidget.h"

// For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
#include <OpenGL.h>
#endif

GLWidget::GLWidget(QWidget *p_parent)
    : QGLWidget(new QGLContext(QGLFormat(QGL::SampleBuffers)), p_parent)
    , m_Light()
    , m_World()
    , m_GlView()
    , m_MoverController()
    , m_ShuttleBoundingBox()
    , m_MotionTimer()
{
    connect(&m_GlView, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));

    m_Light.setPosition(4000.0, 40000.0, 80000.0);
    //m_GlView.setBackgroundColor(Qt::white);
    m_Light.setAmbientColor(Qt::lightGray);

    m_GlView.cameraHandle()->setDefaultUpVector(glc::Z_AXIS);
    m_GlView.cameraHandle()->setIsoView();

    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
    m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_GlView);

    createScene();
    // Signal and slot connection
    connect(&m_MotionTimer, SIGNAL(timeout()), this, SLOT(rotateView()));

    qDebug() << glc::X_AXIS.signedAngleWithVect(-glc::Y_AXIS, glc::Z_AXIS);
    qDebug() << fmod(glc::X_AXIS.signedAngleWithVect(-glc::Y_AXIS, glc::Z_AXIS), 2.0 * glc::PI);
}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
    // For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
    const GLint swapInterval = 1;
    CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &swapInterval);
#endif

    // OpenGL initialisation from NEHE production
    m_GlView.initGl();
    // Reframe the scene
    m_GlView.reframe(m_ShuttleBoundingBox);

    glEnable(GL_NORMALIZE);

    m_MotionTimer.start(60);


}

void GLWidget::paintGL()
{
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Load identity matrix
    GLC_Context::current()->glcLoadIdentity();

    // Calculate camera depth of view
    m_GlView.setDistMinAndMax(m_World.boundingBox());


    // define view matrix
    m_GlView.glExecuteCam();
    m_Light.glExecute();

    m_World.render(0 ,glc::ShadingFlag);

    // Display UI Info (orbit circle)
    m_MoverController.drawActiveMoverRep();

}

void GLWidget::resizeGL(int width, int height)
{
    m_GlView.setWinGLSize(width, height);	// Compute window aspect ratio

}

void GLWidget::createScene()
{

    QFile democles(":Democles.dae");
    m_World= GLC_Factory::instance()->createWorldFromFile(democles);

    m_ShuttleBoundingBox= m_World.boundingBox();

    GLC_StructOccurence* pRoot= m_World.rootOccurence();

    QImage texture(QString(":particle.png"));
    GLC_3DRep pointSprite;
    const float min= -20000.0f;
    const float max= 20000.0f;
    for (int i= 0; i < 300; ++i)
    {
        QColor currentColor;
        currentColor.setRedF(getRandomMinMax(0.4, 1.0));
        currentColor.setGreenF(getRandomMinMax(0.4, 0.7));
        currentColor.setBlueF(getRandomMinMax(0.4, 1.0));

        GLC_Material* pMaterial= GLC_Factory::instance()->createMaterial(texture);
        pMaterial->setDiffuseColor(currentColor);

        pointSprite= GLC_Factory::instance()->createPointSprite(getRandomMinMax(5.0f, 10.0f), pMaterial);

        GLC_StructReference* pStructReference= new GLC_StructReference(new GLC_3DRep(pointSprite));
        GLC_StructInstance* pStructInstance= new GLC_StructInstance(pStructReference);

        GLC_Point3d position(getRandomMinMax(min, max), getRandomMinMax(min, max), getRandomMinMax(min, max));
        const double norm= position.length();
        if ((norm > max) || (norm < (max / 2))) position.setLength(max);
        pStructInstance->translate(position);

        pRoot->addChild(pStructInstance);
    }

}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    if (m_MoverController.hasActiveMover()) return;

    switch (e->button())
    {
    case (Qt::LeftButton):
        m_MotionTimer.stop();
        m_MoverController.setActiveMover(GLC_MoverController::TurnTable, GLC_UserInput(e->x(), e->y()));
        updateGL();
        break;
    default:
        break;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent * e)
{
    if (m_MoverController.hasActiveMover())
    {
        m_MoverController.move(GLC_UserInput(e->x(), e->y()));
        m_GlView.setDistMinAndMax(m_World.boundingBox());
        updateGL();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent*)
{
    if (m_MoverController.hasActiveMover())
    {
        m_MoverController.setNoMover();
        m_MotionTimer.start();
        updateGL();
    }
}

//////////////////////////////////////////////////////////////////////
// Private slots Functions
//////////////////////////////////////////////////////////////////////
// Rotate the view
void GLWidget::rotateView()
{
    m_GlView.cameraHandle()->rotateAroundTarget(glc::Z_AXIS, 2.0 * glc::PI / static_cast<double>(200));
    updateGL();
}

