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
#include <QFile>
#include <QGLContext>
#include <QOpenGLFunctions>

#include <GLC_UserInput>
#include <GLC_Context>

#include "glwidget.h"

// For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
#include <OpenGL.h>
#endif

GLWidget::GLWidget(QWidget *p_parent)
: QGLWidget(new QGLContext(QGLFormat(QGL::SampleBuffers)), p_parent)
, m_Light()
, m_GlView()
, m_MoverController()
{
    connect(&m_GlView, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));

    m_Light.setPosition(1.0, 1.0, 1.0);

	QColor repColor;
	repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
	m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_GlView);

	m_GlView.cameraHandle()->setDefaultUpVector(glc::Z_AXIS);
	m_GlView.cameraHandle()->setIsoView();

	// Create objects to display
	CreateScene();


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

	// OpenGL initialization
	m_GlView.initGl();
	// to create texture a rendering context must be present.

	m_GlView.reframe(m_World.boundingBox());
}

void GLWidget::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // Clear screen
   // f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load identity matrix
	GLC_Context::current()->glcLoadIdentity();

    // Calculate camera depth of view
	m_GlView.setDistMinAndMax(m_World.boundingBox());

	// define the light
	m_Light.glExecute();

	// define view matrix
	m_GlView.glExecuteCam();

	// Display the collection of GLC_Object
	m_World.render(0, glc::ShadingFlag);

	// Display UI Info (orbit circle)
	m_MoverController.drawActiveMoverRep();
}

void GLWidget::resizeGL(int width, int height)
{
	m_GlView.setWinGLSize(width, height);	// Compute window aspect ratio
}

// Create GLC_Object to display
void GLWidget::CreateScene()
{
    // Load Backfround image.
    m_GlView.loadBackGroundImage(QLatin1String(":fond_exemple07.png"));

    // create a material with the specified texture
    GLC_Texture* pTexture= GLC_Factory::instance()->createTexture(QString::fromLatin1(":exemple07.png"));

    // Load "example07.obj"
    QFile fileEx07(QLatin1String(":example07.obj"));
    m_World= GLC_Factory::instance()->createWorldFromFile(fileEx07);
    m_World.collection()->instancesHandle().first()->geomAt(0)->firstMaterial()->setTexture(pTexture);

}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
	if (m_MoverController.hasActiveMover()) return;
	switch (e->button())
	{
	case (Qt::RightButton):
		m_MoverController.setActiveMover(GLC_MoverController::TrackBall, GLC_UserInput(e->x(), e->y()));
		updateGL();
		break;
	case (Qt::LeftButton):
		m_MoverController.setActiveMover(GLC_MoverController::Pan, GLC_UserInput(e->x(), e->y()));
		updateGL();
		break;
	case (Qt::MidButton):
		m_MoverController.setActiveMover(GLC_MoverController::Zoom, GLC_UserInput(e->x(), e->y()));
		updateGL();
		break;

	default:
		break;
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent * e)
{
	if (!m_MoverController.hasActiveMover()) return;
	m_MoverController.move(GLC_UserInput(e->x(), e->y()));
	m_GlView.setDistMinAndMax(m_World.boundingBox());
	updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent*)
{
	if (!m_MoverController.hasActiveMover()) return;
	m_MoverController.setNoMover();
	updateGL();
}
