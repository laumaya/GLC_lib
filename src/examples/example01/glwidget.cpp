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

#include <GLC_Context>

#include "glwidget.h"

GLWidget::GLWidget(QWidget *pParent)
: QGLWidget(new QGLContext(QGLFormat(QGL::SampleBuffers)), pParent)
, m_Circle(GLC_Factory::instance()->createCircle(0.2))  // Circle radius
, m_GlView()
{
	m_Circle.geomAt(0)->setWireColor(Qt::white);
}

GLWidget::~GLWidget()
{

}
void GLWidget::initializeGL()
{

    m_GlView.initGl();
}

void GLWidget::paintGL()
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load identity matrix
	glLoadIdentity();

//////////////////////////// GLC specific///////////////////////////////////////
	// define view matrix
	m_GlView.glExecuteCam();

	// Display the circle
    //m_Circle.render();
//////////////////////////End GLC specific/////////////////////////////////////

}

void GLWidget::resizeGL(int width, int height)
{
//////////////////////////// GLC specific///////////////////////////////////////
	m_GlView.setWinGLSize(width, height);	// Compute window aspect ratio
//////////////////////////End GLC specific/////////////////////////////////////
}
