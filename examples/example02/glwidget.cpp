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

#include <GLC_UserInput>
#include <GLC_Context>

#include "glwidget.h"


GLWidget::GLWidget(QWidget *pParent)
: QGLWidget(new GLC_Context(QGLFormat(QGL::SampleBuffers)), pParent)
, m_Circle(GLC_Factory::instance()->createCircle(0.3))	// Circle radius
, m_GlView()
, m_MoverController()
{
    connect(&m_GlView, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));
	m_Circle.geomAt(0)->setWireColor(Qt::white);

	// Color of mover representation (Trackball)
	QColor repColor;
	repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
	// Contruct default mover controller
	m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_GlView);

}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
	// OpenGL initialisation from NEHE production
	m_GlView.initGl();
}

void GLWidget::paintGL()
{

	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load identity matrix
    GLC_Context::current()->glcLoadIdentity();
	m_GlView.setDistMinAndMax(m_Circle.boundingBox());
//////////////////////////// GLC specific///////////////////////////////////////
	// define view matrix
	m_GlView.glExecuteCam();

	// Display the circle
	m_Circle.render();

// Exemple02 New
	// Display the active mover representation
	m_MoverController.drawActiveMoverRep();
// End Exemple02 New

//////////////////////////End GLC specific/////////////////////////////////////

}

void GLWidget::resizeGL(int width, int height)
{
//////////////////////////// GLC specific///////////////////////////////////////
	m_GlView.setWinGLSize(width, height);	// Compute window aspect ratio
//////////////////////////End GLC specific/////////////////////////////////////

}
// Exemple02 New
void GLWidget::mousePressEvent(QMouseEvent *e)
{
	if (m_MoverController.hasActiveMover()) return;
	switch (e->button())
	{
	case (Qt::RightButton):
		// Set track ball mover
		m_MoverController.setActiveMover(GLC_MoverController::TrackBall, GLC_UserInput(e->x(), e->y()));
		updateGL();
		break;
	case (Qt::LeftButton):
		// Set Pan mover
		m_MoverController.setActiveMover(GLC_MoverController::Pan, GLC_UserInput(e->x(), e->y()));
		updateGL();
		break;
	case (Qt::MidButton):
		// Set Zoom mover
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
		// Move with the active mover
		m_MoverController.move(GLC_UserInput(e->x(), e->y()));
		updateGL();
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent*)
{
	if (m_MoverController.hasActiveMover())
	{
		// Set No mover
		m_MoverController.setNoMover();
		updateGL();
	}
}
// End Exemple02 New
