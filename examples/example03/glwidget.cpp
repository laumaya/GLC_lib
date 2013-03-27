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
#include "glwidget.h"

#include <GLC_UserInput>
#include <GLC_Context>

// GLC_Lib Exception
#include <GLC_Exception>

// For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
#include <OpenGL.h>
#endif

GLWidget::GLWidget(QWidget *p_parent)
: QGLWidget(new GLC_Context(QGLFormat(QGL::SampleBuffers)),p_parent)
, m_Cylinder(GLC_Factory::instance()->createCylinder(1.0, 2.0))	// Cylinder definition.
, m_Collection()
, m_light()
, m_GlView()
, m_MoverController()
{
	connect(&m_GlView, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));
	// Set cylinder material
	QColor matBlue;
	matBlue.setRgbF(0.5, 0.8, 1.0, 1.0);
	m_Cylinder.geomAt(0)->addMaterial(new GLC_Material(matBlue));

    m_Collection.add(m_Cylinder);

	// Set up mover controller
	QColor repColor;
	repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
	m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_GlView);

	// Set iso view
	m_GlView.cameraHandle()->setIsoView();
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
	// Reframe to the cylinder bounding Box
    m_GlView.reframe(m_Collection.boundingBox());
}

void GLWidget::paintGL()
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load identity matrix
	GLC_Context::current()->glcLoadIdentity();
	try
	{
		// Set the opengl clipping plane
		m_GlView.setDistMinAndMax(m_Cylinder.boundingBox());

		// define the light
		m_light.glExecute();

		// define view matrix
		m_GlView.glExecuteCam();

		// Display the cylinder
        m_Collection.render(0, glc::ShadingFlag);

		// Display UI Info (orbit circle)
		m_MoverController.drawActiveMoverRep();
	}
	catch (GLC_Exception &e)
	{
		qDebug() << e.what();
	}
}

void GLWidget::resizeGL(int width, int height)
{
	m_GlView.setWinGLSize(width, height);	// Compute window aspect ratio

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
		// Update opengl clipping plane
		m_GlView.setDistMinAndMax(m_Cylinder.boundingBox());
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
