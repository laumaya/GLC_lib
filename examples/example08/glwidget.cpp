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
#include <GLC_Context>

#include "glwidget.h"
#include "GLC_UserInput"

// For VSYNC problem under Mac OS X
#if defined(Q_OS_MAC)
#include <OpenGL.h>
#endif

GLWidget::GLWidget(QWidget *p_parent)
: QGLWidget(new GLC_Context(QGLFormat(QGL::SampleBuffers)), p_parent)
, m_Light()
, m_GlView()
, m_TreeId()
, m_MoverController()
{
    connect(&m_GlView, SIGNAL(updateOpenGL()), this, SLOT(updateGL()));
    m_Light.setPosition(15.0, 15.0, 0.0);

	m_GlView.cameraHandle()->setDefaultUpVector(glc::Z_AXIS);
	m_GlView.cameraHandle()->setIsoView();

	QColor repColor;
	repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
	m_MoverController= GLC_Factory::instance()->createDefaultMoverController(repColor, &m_GlView);

	CreateScene();

}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
	// OpenGL initialisation
	m_GlView.initGl();

	// For VSYNC problem under Mac OS X
	#if defined(Q_OS_MAC)
	const GLint swapInterval = 1;
	CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &swapInterval);
	#endif

		// Load background image
	m_GlView.loadBackGroundImage(":fond_exemple08.png");

	// Reframe the scene
	m_GlView.reframe(m_World.boundingBox());

}

void GLWidget::paintGL()
{

	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	// Load the christmas tree from the obj file
	QFile treeObjFile(":tree.obj");
	m_World= GLC_Factory::instance()->createWorldFromFile(treeObjFile);

	GLC_Material* pCurrentMat= NULL;
	// Set material properties to the tree
	QList<GLC_StructOccurence*> occurenceList= m_World.listOfOccurence();
	for (int i= 0; i < occurenceList.size(); ++i)
	{
		if (occurenceList.at(i)->hasRepresentation())
		{
			GLC_StructReference* pRef= occurenceList.at(i)->structReference();
			GLC_3DRep* pRep= dynamic_cast<GLC_3DRep*>(pRef->representationHandle());
			if (NULL != pRep)
			{
				pCurrentMat= pRep->geomAt(0)->firstMaterial();
				pCurrentMat->setAmbientColor(QColor::fromRgb(22, 134, 33, 255));
				pCurrentMat->setDiffuseColor(QColor::fromRgb(85, 170, 127, 255));
			}
			m_TreeId << occurenceList.at(i)->id();
		}
	}
	// spheres creation

	// Create the sphere struct occurence
	GLC_StructOccurence* pSphereOccurence= new GLC_StructOccurence(new GLC_3DRep(GLC_Factory::instance()->createSphere(3.0)));

	// Get the sphere geometry
	GLC_Geometry* pSphereGeometry= dynamic_cast<GLC_3DRep*>(pSphereOccurence->structReference()->representationHandle())->geomAt(0);
	// Get the sphere material
	pCurrentMat= new GLC_Material();
	pCurrentMat->setAmbientColor(Qt::red);
	pCurrentMat->setDiffuseColor(QColor::fromRgbF(0.8, 0.2, 0.2, 1.0));

    pSphereGeometry->replaceMasterMaterial(pCurrentMat);

	// translate the first sphere
	pSphereOccurence->structInstance()->translate(6.0, 0.0, 71.0);
	// Add the first sphere to the world
	m_World.rootOccurence()->addChild(pSphereOccurence);

	// Create the list of sphere position
	QList<GLC_Point3d> positionList;
	positionList << GLC_Point3d(16.0, -8.0, 7.0);
	positionList << GLC_Point3d(6.0, -20.0, 4.0);
	positionList << GLC_Point3d(23.0, 13.0, 6.0);
	positionList << GLC_Point3d(-18.0, 15.0, 9.0);
	positionList << GLC_Point3d(-16.0, -20.0, -8.0);
	positionList << GLC_Point3d(19.0, 38.0, -18.0);
	positionList << GLC_Point3d(26.0, -24.0, -22.0);
	positionList << GLC_Point3d(-19.0, 0.0, -25.0);
	positionList << GLC_Point3d(35.0, -14.0, -44.0);

	// Copy the sphere at position from the list
	for (int i= 0; i < positionList.size(); ++i)
	{
		// Copy the sphere geometry
		pSphereGeometry= pSphereGeometry->clone();
		// Copy the sphere material
		pSphereGeometry->replaceMasterMaterial(new GLC_Material(*pCurrentMat));
		// Create new occurence of this sphere geometry
		pSphereOccurence= new GLC_StructOccurence(new GLC_3DRep(pSphereGeometry));
		// translate the sphere
		pSphereOccurence->structInstance()->translate(positionList.at(i));
		// Add the sphere to the world
		m_World.rootOccurence()->addChild(pSphereOccurence);
	}
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
		select(e->x(), e->y());
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

void GLWidget::select(const int x, const int y)
{

	setAutoBufferSwap(false);
	GLC_uint SelectionID= m_GlView.renderAndSelect(x, y);
	setAutoBufferSwap(true);

	if (SelectionID != 0)
	{
		GLC_3DViewInstance instance(*m_World.collection()->instanceHandle(SelectionID));
		if ((!instance.isEmpty()) && (!m_TreeId.contains(instance.id())))
		{	// Instance is in the collection and is not the tree
			if (instance.geomAt(0)->firstMaterial()->ambientColor() == Qt::blue)
			{
				instance.geomAt(0)->firstMaterial()->setAmbientColor(Qt::red);
				instance.geomAt(0)->firstMaterial()->setDiffuseColor(QColor::fromRgbF(0.8, 0.2, 0.2, 1.0));
			}
			else
			{
				instance.geomAt(0)->firstMaterial()->setAmbientColor(Qt::blue);
				instance.geomAt(0)->firstMaterial()->setDiffuseColor(QColor::fromRgbF(0.2, 0.2, 0.8, 1.0));
			}
		}
	}
	updateGL();
}

