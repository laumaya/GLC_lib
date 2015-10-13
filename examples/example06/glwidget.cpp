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
#include <QOpenGLFunctions>

#include <GLC_UserInput>
#include <GLC_Context>

#include "glwidget.h"

// For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
#include <OpenGL.h>
#endif

GLWidget::GLWidget(QWidget *p_parent)
: QGLWidget(new GLC_Context(QGLFormat(QGL::SampleBuffers)), p_parent)
, m_Light()
, m_GlView()
, m_MoverController()
{
//////////////////////////// GLC specific///////////////////////////////////////
    connect(&m_GlView, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));
	m_Light.setPosition(1.0, 1.0, 1.0);

	QColor repColor;
	repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
	m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_GlView);

	// Create objects to display
	CreateScene();

//////////////////////////End GLC specific/////////////////////////////////////
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

	// OpenGL initialisation
	m_GlView.initGl();

	// Reframe the scene on the loaded mesh
	m_GlView.reframe(m_World.boundingBox());


}

void GLWidget::paintGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // Clear screen
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load identity matrix
	GLC_Context::current()->glcLoadIdentity();

	// Calculate camera depth of view
	m_GlView.setDistMinAndMax(m_World.boundingBox());

	// define the light
	m_Light.glExecute();

	// define view matrix
	m_GlView.glExecuteCam();

	// Render the scene
	m_World.render(0, glc::ShadingFlag);
	m_World.render(0, glc::TransparentRenderFlag);

	// Display UI Info (orbit circle)
	m_MoverController.drawActiveMoverRep();
}

void GLWidget::resizeGL(int width, int height)
{
//////////////////////////// GLC specific///////////////////////////////////////
	m_GlView.setWinGLSize(width, height);	// Compute window aspect ratio
//////////////////////////End GLC specific/////////////////////////////////////

}

// Create GLC_Object to display
void GLWidget::CreateScene()
{
	// Load mesh from a file
	QFile manFile(":man.obj");
	m_World= GLC_Factory::instance()->createWorldFromFile(manFile);

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
		updateGL();
	}
}
