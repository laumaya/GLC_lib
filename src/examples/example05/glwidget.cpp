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
#include <GLC_Polylines>

#include "glwidget.h"


GLWidget::GLWidget(QWidget *p_parent)
: QGLWidget(new QGLContext(QGLFormat()), p_parent)
, m_Light()
, m_DefaultColor()
, m_World()
, m_GlView()
, m_MoverController()
{
    connect(&m_GlView, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));
	m_Light.setPosition(1.0, 1.0, 1.0);

	m_DefaultColor.setRgbF(0.5, 0.8, 1.0, 1.0);

	m_GlView.cameraHandle()->setIsoView();

	QColor repColor;
	repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
	m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_GlView);

	// Create objects to display
	CreateScene();
}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
	// OpenGL initialisation from NEHE production
	m_GlView.initGl();

	m_GlView.reframe(m_World.boundingBox());
}

void GLWidget::paintGL()
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load identity matrix
    GLC_Context::current()->glcLoadIdentity();

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
    GLC_StructOccurrence* pRoot= m_World.rootOccurrence();
    GLC_StructInstance* pInstance= NULL;

    // Create a box at the center of the scene
    GLC_3DRep box(GLC_Factory::instance()->createBox(0.1, 0.1, 0.1));
    box.geomAt(0)->replaceMasterMaterial(new GLC_Material(m_DefaultColor));
    pRoot->addChild(new GLC_StructInstance(box.clone()));

    // create Z axis representation
    GLC_3DRep cylinder(GLC_Factory::instance()->createCylinder(0.02, 2.0));
    cylinder.geomAt(0)->addMaterial(new GLC_Material(Qt::blue));
    pRoot->addChild(new GLC_StructInstance(cylinder.clone()));

    // create X axis representation
    cylinder= GLC_Factory::instance()->createCylinder(0.02, 2.0);
    cylinder.geomAt(0)->replaceMasterMaterial(new GLC_Material(Qt::red));
    GLC_Matrix4x4 matRot(glc::Y_AXIS, glc::PI/2);			//Create a rotation matrix
    pInstance= new GLC_StructInstance(cylinder.clone());
    pInstance->move(matRot);								// move the cylinder
    pRoot->addChild(pInstance);

    // create Y axis representation
    cylinder= GLC_Factory::instance()->createCylinder(0.02, 2.0);
    cylinder.geomAt(0)->replaceMasterMaterial(new GLC_Material(Qt::green));
    matRot.setMatRot(glc::X_AXIS, -glc::PI/2);								// Set rotation matrix
    pInstance= new GLC_StructInstance(cylinder.clone());
    pInstance->move(matRot);								// move the cylinder
    pRoot->addChild(pInstance);

    // Create 3 circles around axis
    GLC_3DRep circle(GLC_Factory::instance()->createCircle(2.5));
    circle.geomAt(0)->setWireColor(Qt::white);
    pRoot->addChild(new GLC_StructInstance(circle.clone()));

    circle= GLC_Factory::instance()->createCircle(2.5);
    circle.geomAt(0)->setWireColor(Qt::white);
    matRot.setMatRot(glc::Y_AXIS, glc::PI/2);
    pInstance= new GLC_StructInstance(circle.clone());
    pInstance->move(matRot);								// move the circle
    pRoot->addChild(pInstance);

    circle= GLC_Factory::instance()->createCircle(2.5);
    circle.geomAt(0)->setWireColor(Qt::white);
    matRot.setMatRot(glc::X_AXIS, glc::PI/2);
    pInstance= new GLC_StructInstance(circle.clone());
    pInstance->move(matRot);								// move the circle
    pRoot->addChild(pInstance);
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

void GLWidget::mouseReleaseEvent(QMouseEvent *)
{
	if (m_MoverController.hasActiveMover())
	{
		m_MoverController.setNoMover();
		updateGL();
	}
}
