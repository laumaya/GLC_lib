/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.1.0, packaged on March, 2009.

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

//! \file glc_viewport.cpp implementation of the GLC_Viewport class.


#include "glc_viewport.h"
#include "../glc_openglexception.h"
#include "../glc_ext.h"
#include "../shading/glc_selectionmaterial.h"
#include "../glc_state.h"

#include <QtDebug>

using namespace glc;
//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

// Default constructor
GLC_Viewport::GLC_Viewport(QGLWidget *GLWidget)
// Camera definition
: m_pViewCam(NULL)				// Camera
, m_dCamDistMax(500)			// Camera Maximum distance
, m_dCamDistMin(0.01)			// Camera Minimum distance
, m_dFov(35)					// Camera angle of view
, m_pImagePlane(NULL)			// Background image
, m_CameraTargetIsVisible(false)	// Show state of camera's target
// OpenGL Window size
, m_nWinHSize(0)				// Horizontal OpenGL viewport size
, m_nWinVSize(0)				// Vertical OpenGL viewport size
// zoom
, m_dPrevZoom(0)				// Previous zoom Factor
// orbit circle definition
, m_pOrbitCircle(NULL)			// orbit circle
, m_dRatWinSph(0.95)			// Circle ratio size / window size
, m_OrbitCircleIsVisible(false)	// Show state of orbit Circle
, m_pQGLWidget(GLWidget)		// Attached QGLWidget
// the default backgroundColor
, m_BackgroundColor(Qt::black)
, m_ImagePlaneListID(0)
{
	// create a camera
	m_pViewCam= new GLC_Camera;

	// Color of orbit circle
	QColor color;
	color.setRgbF(1.0, 0.11372, 0.11372, 0.0);
	// Create orbit circle of specified color
	m_pOrbitCircle= new GLC_OrbitCircle(1.0, m_pQGLWidget->context());
	m_pOrbitCircle->setRGBAColor(color);

	// Compute orbit Circle
	//updateOrbitCircle();
}

// Delete Camera, Image Plane and orbit circle
GLC_Viewport::~GLC_Viewport()
{
	// Delete the camera
	if (m_pViewCam != NULL)
	{
		delete m_pViewCam;
		m_pViewCam= NULL;
	}

	// Delete orbit circle
	if (m_pOrbitCircle != NULL)
	{
		delete m_pOrbitCircle;
		m_pOrbitCircle= NULL;
	}

	// delete background image
	deleteBackGroundImage();
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Convert mouse View coordinate to orbit coordinate (Centred and between (-1,-1) and (1,1))
GLC_Vector4d GLC_Viewport::mapPosMouse( GLdouble Posx, GLdouble Posy) const
{
	// Change the window origin (Up Left -> centred)
	Posx= Posx - (double)m_nWinHSize  / 2;
	Posy= (double)m_nWinVSize / 2 - Posy;

	GLC_Vector4d VectMouse(Posx, Posy,0);

	// Compute the length of camera's field of view
	const double ChampsVision = 2 * m_pViewCam->getDistEyeTarget() *  tan((m_dFov * PI / 180) / 2);

	// the side of camera's square is mapped on Vertical length of window
	// Ratio OpenGL/Pixel = dimend GL / dimens Pixel
	const double Ratio= ChampsVision / (double)m_nWinVSize;

	VectMouse= VectMouse * Ratio;

	return VectMouse;
}

//////////////////////////////////////////////////////////////////////
// Public OpenGL Functions
//////////////////////////////////////////////////////////////////////
// Initialize OpenGL with default values
void GLC_Viewport::initGl()
{
	// OpenGL initialisation from NEHE production
	m_pQGLWidget->qglClearColor(m_BackgroundColor);       // Background
	glClearDepth(1.0f);                                   // Depth Buffer Setup
	glShadeModel(GL_SMOOTH);                              // Enable Smooth Shading
	glEnable(GL_DEPTH_TEST);                              // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                               // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    // Really Nice Perspective Calculation
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Init GLC_State
	GLC_State::init();
}

// Define camera's target position
void GLC_Viewport::glPointing(GLint x, GLint y)
{
	// Z Buffer component of selected point between 0 and 1
	GLfloat Depth;
	// read selected point
	glReadPixels(x, getWinVSize() - y , 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Depth);

	// Current visualisation matrix
	GLdouble ViewMatrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, ViewMatrix);
	// Current projection matrix
	GLdouble ProjMatrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX, ProjMatrix);
	// definition of the current viewport
	GLint Viewport[4];
	glGetIntegerv(GL_VIEWPORT, Viewport);

	// OpenGL ccordinate of selected point
	GLdouble pX, pY, pZ;
	gluUnProject((GLdouble) x, (GLdouble) (getWinVSize() - y) , Depth
		, ViewMatrix, ProjMatrix, Viewport, &pX, &pY, &pZ);

	// OpenGL error handler
	GLenum error= glGetError();
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Viewport::GlPointing ", error);
		throw(OpenGlException);
	}

	//qDebug("Return depth: %e, z= %e\n", Depth, pZ);

	// Test if there is geometry under picking point
	if (fabs(Depth - 1.0) > EPSILON)
	{	// Geometry find -> Update camera's target position
		const GLC_Point4d target(pX, pY, pZ);
		m_pViewCam->setTargetCam(target);
	}
	else
	{	// Geometrie not find -> panning

		const GLC_Point4d curPos(mapPosMouse(x, y));
		const GLC_Point4d prevPos(mapPosMouse(getWinHSize() / 2, getWinVSize() / 2));
		const GLC_Vector4d VectPan(curPos - prevPos);	// panning vector
		// pan camera
		m_pViewCam->pan(VectPan);
	}
}

// Update OpenGL Projection Matrix
void GLC_Viewport::updateProjectionMat(void) const
{
	// Make opengl context attached the current One
	m_pQGLWidget->makeCurrent();

	glMatrixMode(GL_PROJECTION);						// select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	double AspectRatio;
	AspectRatio= static_cast<double>(m_nWinHSize)/static_cast<double>(m_nWinVSize);
	gluPerspective(m_dFov, AspectRatio, m_dCamDistMin, m_dCamDistMax);

	glMatrixMode(GL_MODELVIEW);							// select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

//! Force the aspect ratio of the window
void GLC_Viewport::forceAspectRatio(double ratio)
{
	// Make opengl context attached the current One
	m_pQGLWidget->makeCurrent();

	glMatrixMode(GL_PROJECTION);						// select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	gluPerspective(m_dFov, ratio, m_dCamDistMin, m_dCamDistMax);

	glMatrixMode(GL_MODELVIEW);							// select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	// Calculate The Aspect Ratio Of The Window
	double AspectRatio= static_cast<double>(m_nWinHSize)/static_cast<double>(m_nWinVSize);

	if (ratio > AspectRatio)
	{
		// Save window size
		const int width= m_nWinHSize;

		// Calculate new width
		m_nWinHSize= static_cast<int>(static_cast<double>(m_nWinVSize) * ratio);

		// Update image plane size
		if (m_pImagePlane != NULL)
			m_pImagePlane->updatePlaneSize();

		// Restore width
		m_nWinHSize= width;
	}

}

//////////////////////////////////////////////////////////////////////
// Private OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Display orbit Circle
void GLC_Viewport::glExecuteOrbitCircle()
{
	if (m_OrbitCircleIsVisible)
	{

		m_pOrbitCircle->glExecute(m_dCamDistMin + (m_dCamDistMax - m_dCamDistMin) / 2);
	}
}
// Display Camera's target
void GLC_Viewport::glExecuteTargetCam()	//! \todo Create a display list
{
	if (m_CameraTargetIsVisible)
	{

		int nLgAxe;
		if (m_nWinHSize > m_nWinVSize)
		{
			nLgAxe = static_cast<int>(m_dRatWinSph * m_nWinVSize / 2);
		}
		else
		{
			nLgAxe = static_cast<int>(m_dRatWinSph * m_nWinHSize / 2);
		}

		// Compute the length of camera's field of view
		const double ChampsVision = 2 * (m_pViewCam->getDistEyeTarget()) *  tan((m_dFov * PI / 180) / 2);

		// the side of camera's square is mapped on Vertical length of window
		// Axis length in OpenGL unit = length(Pixel) * (dimend GL / dimens Pixel)
		const double dLgAxe= ((double)nLgAxe * ChampsVision / (double)m_nWinVSize) / 7;
		const double dDecAxe= dLgAxe / 3;
		glPushMatrix();

		glTranslated(m_pViewCam->getTarget().X(), m_pViewCam->getTarget().Y(),
			m_pViewCam->getTarget().Z() );

		// Graphic propertys
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor4d(m_pOrbitCircle->getdRed(), m_pOrbitCircle->getdGreen(), m_pOrbitCircle->getdBlue(), m_pOrbitCircle->getdAlpha());
		glLineWidth(1.0);

		// Display camera's target lines
		glBegin(GL_LINES);
			//X axis
			glVertex3d(-dLgAxe, 0, 0);
			glVertex3d(-dDecAxe, 0, 0);
			glVertex3d(dDecAxe, 0, 0);
			glVertex3d(dLgAxe, 0, 0);

			//Y axis
			glVertex3d(0, -dLgAxe, 0);
			glVertex3d(0, -dDecAxe, 0);
			glVertex3d(0, dDecAxe, 0);
			glVertex3d(0, dLgAxe, 0);

			//Z axis
			glVertex3d(0, 0, -dLgAxe);
			glVertex3d(0, 0, -dDecAxe);
			glVertex3d(0, 0, dDecAxe);
			glVertex3d(0, 0, dLgAxe);

		glEnd();

		glPopMatrix();

		// OpenGL error handler
		GLenum error= glGetError();
		if (error != GL_NO_ERROR)
		{
			GLC_OpenGlException OpenGlException("GLC_Viewport::GlExecuteTargetCam ", error);
			throw(OpenGlException);
		}

	}
}

// Display background image
void GLC_Viewport::glExecuteImagePlane()
{
	if(not GLC_State::isInSelectionMode())
	{
		if (m_pImagePlane != NULL)
		{
			// Geometry validity
			if (!m_pImagePlane->isValid())
			{
				m_pImagePlane->glLoadTexture();
			}

			// Geometry invalid or collection node list ID == 0
			if ((!m_pImagePlane->isValid()) || (m_ImagePlaneListID == 0))
			{
				//qDebug() << "GLC_CollectionNode::GlExecute: geometry validity : " << m_pImagePlane->isValid();
				//qDebug() << "GLC_CollectionNode::GlExecute: list ID : " << m_ImagePlaneListID;

				if (m_ImagePlaneListID == 0)
				{
					//qDebug() << "GLC_CollectionNode::GlExecute: List not found";
					m_ImagePlaneListID= glGenLists(1);
				}
				glNewList(m_ImagePlaneListID, GL_COMPILE_AND_EXECUTE);
					m_pImagePlane->glExecute(false, false);
				glEndList();
				//qDebug() << "GLC_CollectionNode::GlExecute : Display list " << m_ImagePlaneListID << " created";
			}
			else
			{
				glCallList(m_ImagePlaneListID);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Inform the viewport that the OpenGL Viewport has been modified
void GLC_Viewport::setWinGLSize(int HSize, int VSize)
{
	m_nWinHSize= HSize;
	m_nWinVSize= VSize;

	// from NeHe's Tutorial 3
	if (m_nWinVSize == 0)								// Prevent A Divide By Zero By
	{
		m_nWinVSize= 1;									// Making Height Equal One
	}

	glViewport(0,0,m_nWinHSize,m_nWinVSize);			// Reset The Current Viewport

	updateProjectionMat();

	// Update image plane size
	if (m_pImagePlane != NULL)
		m_pImagePlane->updatePlaneSize();

	// Update orbit circle size
	updateOrbitCircle();

}

//! select an object and return is UID
GLC_uint GLC_Viewport::select(QGLWidget *pGLWidget, int x, int y)
{
	m_pQGLWidget->qglClearColor(Qt::black);
	GLC_State::setSelectionMode(true);
	// Draw the scene
	pGLWidget->updateGL();
	GLC_State::setSelectionMode(false);

	// Read the back buffer
	glReadBuffer(GL_BACK);
	// Use 5 pixels by 5 pixels Square
	GLubyte colorId[100]; // 5 * 5= 25 pixels 25 * 4 = 100 bytes
	// Lower left corner of the square
	int newX= x - 2;
	int newY= (pGLWidget->size().height() - y) - 2;
	if (newX < 0) newX= 0;
	if (newY < 0) newY= 0;

	// Get the array of pixels
	glReadPixels(newX, newY, 5, 5, GL_RGBA, GL_UNSIGNED_BYTE, colorId);

	// Restore Background color
	m_pQGLWidget->qglClearColor(m_BackgroundColor);

	QList<GLC_uint> idList;
	QList<int> idWeight;

	// Find the most meaningful color
	for (int i= 0; i < 25; ++i)
	{
		GLC_uint id= GLC_Instance::decodeRgbId(&colorId[i * 4]);
		if (idList.contains(id))
		{
			++(idWeight[idList.indexOf(id)]);
		}
		else if (id != 0)
		{
			idList.append(id);
			idWeight.append(1);
		}
	}
	GLC_uint returnId= 0;
	// If the list is empty return 0
	if (not idList.isEmpty())
	{
		int maxWeight= 0;
		int maxIndex= 0;
		const int listSize= idWeight.size();
		for (int i= 0; i < listSize; ++i)
		{
			if (maxWeight < idWeight[i])
			{
				maxWeight= idWeight[i];
				maxIndex= i;
			}
		}
		returnId= idList[maxIndex];
	}

	return returnId;
}

// load background image
void GLC_Viewport::loadBackGroundImage(const QString Image)
{
	deleteBackGroundImage();
	m_pImagePlane= new GLC_ImagePlane(this);
	m_pImagePlane->loadImageFile(m_pQGLWidget->context(), Image);
}

// delete background image
void GLC_Viewport::deleteBackGroundImage()
{
	if (m_ImagePlaneListID != 0)
	{
		glDeleteLists(m_ImagePlaneListID, 1);
		m_ImagePlaneListID= 0;

	}
	if (m_pImagePlane != NULL)
	{
		delete m_pImagePlane;
		m_pImagePlane= NULL;
	}

}


// pan to the position (Cx, Cy)
void GLC_Viewport::pan(double Cx, double Cy)
{
	const GLC_Vector4d VectCur(mapPosMouse(Cx,Cy));
	const GLC_Vector4d VectPan= VectCur - m_VectPrevPan;	// Vecteur de déplacement

	// pan the camera
	m_pViewCam->pan(-VectPan);
	m_VectPrevPan= VectCur;
}

// Prepare orbiting operation
void GLC_Viewport::prepareOrbiting(double Cx, double Cy, bool circleVisibility, bool targetVisibility)
{
	m_VectPrevOrbit.setVect(mapForOrbit(Cx,Cy));
	m_OrbitCircleIsVisible= circleVisibility;
	m_CameraTargetIsVisible= targetVisibility;

	const double Angle= acos(Z_AXIS * m_VectPrevOrbit);
	const GLC_Vector4d AxeRot(Z_AXIS ^ m_VectPrevOrbit);

	GLC_Matrix4x4 Matrice(AxeRot, Angle);

	// Update orbit circle's arcs
	m_pOrbitCircle->setOrientArcs(m_VectPrevOrbit, Matrice);
}


// orbit in accordance of mouse position
void GLC_Viewport::orbit(double Cx, double Cy)
{
	const GLC_Vector4d VectCurOrbit(mapForOrbit(Cx, Cy));

	// Update camera position (orbit)
	m_pViewCam->orbit(m_VectPrevOrbit, VectCurOrbit);

	// Update arcs of circle's positionning matrix
	const GLC_Matrix4x4 MatRot(m_VectPrevOrbit, VectCurOrbit);
	m_pOrbitCircle->mapArcs(MatRot);

	// Previous vector become current vector
	m_VectPrevOrbit = VectCurOrbit;

}
// Prepare Zooming operation
void GLC_Viewport::prepareZooming(int Cy, bool tragetVisibility)
{
	m_CameraTargetIsVisible= tragetVisibility;
	// Change origine (view center) and cover between -1 and 1
	m_dPrevZoom= ((double)m_nWinVSize / 2 - Cy) / ( (double)m_nWinVSize / 2);;
}

// zoom in accordance of Y mouse position
void GLC_Viewport::zoom(int Cy)
{
	// Change origine (View Center) and cover (from -1 to 1)
	double Posy= ((double)m_nWinVSize / 2 - Cy) / ( (double)m_nWinVSize / 2);

	// Compute zoom factor between (1 / MAXZOOMFACTOR) and (MAXZOOMFACTOR)
	double ZoomFactor= Posy - m_dPrevZoom;

	if (ZoomFactor > 0)
	{
		ZoomFactor= (MAXZOOMFACTOR - 1.0) * ZoomFactor + 1.0;
	}
	else
	{
		ZoomFactor= 1.0 / ( (MAXZOOMFACTOR - 1.0) * fabs(ZoomFactor) + 1.0 );
	}

	m_pViewCam->zoom(ZoomFactor);

	m_dPrevZoom = Posy;
}

// reframe the current scene
void GLC_Viewport::reframe(const GLC_BoundingBox& box)
{
	Q_ASSERT(!box.isEmpty());

	// Center view on the BoundingBox
	const GLC_Vector4d deltaVector(box.getCenter() - m_pViewCam->getTarget());
	m_pViewCam->translate(deltaVector);

	double cameraCover= box.boundingSphereRadius() * 2.0;

	// Compute Camera distance
	const double distance = cameraCover / (2.0 * tan((m_dFov * PI / 180.0) / 2.0));

	// Update Camera position
	m_pViewCam->setDistEyeTarget(distance);
}

// Set near clipping distance
bool GLC_Viewport::setDistMin(double DistMin)
{
	DistMin= fabs(DistMin);
	if (DistMin < m_dCamDistMax)
	{
		m_dCamDistMin= DistMin;

		updateProjectionMat();	// Update OpenGL projection matrix

		updateOrbitCircle();	// Update orbit circle

		if (m_pImagePlane != NULL)
		{
			m_pImagePlane->updateZPosition();	// Update image plane Z Position
		}

		return true;
	}
	else
	{
		qDebug("GLC_Viewport::SetDistMin : KO");
		return false;
	}

}

// Set far clipping distance
bool GLC_Viewport::setDistMax(double DistMax)
{
	DistMax= fabs(DistMax);
	if (DistMax > m_dCamDistMin)
	{
		m_dCamDistMax= DistMax;

		// Update OpenGL projection matrix
		updateProjectionMat();

		// Update orbit circle
		updateOrbitCircle();

		// Update image plane Z Position
		if (m_pImagePlane != NULL)
		{
			m_pImagePlane->updateZPosition();
		}
		return true;
	}
	else
	{
		qDebug("GLC_Viewport::SetDistMax : KO");
		return false;
	}
}

// Set Near and Far clipping distance
void GLC_Viewport::setDistMinAndMax(const GLC_BoundingBox& bBox)
{
	if(not bBox.isEmpty())
	{
		// The scene is not empty
		GLC_Matrix4x4 matTranslateCam(-m_pViewCam->getEye());
		GLC_Matrix4x4 matRotateCam(m_pViewCam->getMatCompOrbit());
		GLC_Matrix4x4 matComp(matRotateCam.invert() * matTranslateCam);

		// The bounding Box in Camera coordinate
		GLC_BoundingBox boundingBox(bBox);
		boundingBox.transform(matComp);
		// Increase size of the bounding box
		const double increaseFactor= 1.1;
		// Convert box distance in sphere distance
		const double center= fabs(boundingBox.getCenter().Z());
		const double radius= boundingBox.boundingSphereRadius();
		const double min= center - radius * increaseFactor;
		const double max= center + radius * increaseFactor;

		GLC_Point4d camEye(m_pViewCam->getEye());
		camEye= matComp * camEye;

		if (min > 0.0)
		{
			// Outside bounding Sphere
			m_dCamDistMin= min;
			m_dCamDistMax= max;
			//qDebug() << "distmin" << m_dCamDistMin;
			//qDebug() << "distmax" << m_dCamDistMax;
		}
		else
		{
			// Inside bounding Sphere
			m_dCamDistMin= fmin(0.01 * radius, m_pViewCam->getDistEyeTarget() / 4.0);
			m_dCamDistMax= max;
			//qDebug() << "inside distmin" << m_dCamDistMin;
			//qDebug() << "inside distmax" << m_dCamDistMax;
		}
	}
	else
	{
		// The scene is empty
		m_dCamDistMin= m_pViewCam->getDistEyeTarget() / 2.0;
		m_dCamDistMax= m_pViewCam->getDistEyeTarget();
	}

	// Update OpenGL projection matrix
	updateProjectionMat();
	// Update orbit circle
	updateOrbitCircle();
	// Update image plane Z Position
	if (m_pImagePlane != NULL)
	{
		m_pImagePlane->updateZPosition();
	}
}

void GLC_Viewport::setBackgroundColor(QColor setColor)
{
	m_BackgroundColor= setColor;
	m_pQGLWidget->qglClearColor(m_BackgroundColor);
}
//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////

// Convert mouse View coordinate to orbit coordinate (Centred and betwen (-1,-1) and (1,1))
GLC_Vector4d GLC_Viewport::mapForOrbit( double Posx, double Posy) const
{
	double AspectRatio;

	// Change origine and cover
	if (m_nWinHSize < m_nWinVSize)
	{
		AspectRatio= (double)m_nWinVSize / (double)m_nWinHSize;
		Posx= ( (Posx - (double)m_nWinHSize  / 2 ) / ( (double)m_nWinHSize / 2) ) / m_dRatWinSph;
		Posy= AspectRatio * ( ( (double)m_nWinVSize / 2 - Posy) / ( (double)m_nWinVSize / 2 ) ) / m_dRatWinSph;
	}
	else
	{
		AspectRatio= (double)m_nWinHSize / (double)m_nWinVSize;
		Posx= AspectRatio * ( (Posx - m_nWinHSize  / 2 ) / ( m_nWinHSize / 2) ) / m_dRatWinSph;
		Posy= ( (m_nWinVSize / 2 - Posy) / (m_nWinVSize / 2 ) ) / m_dRatWinSph;
	}

	// Distance between pick point and origine can't be over then 1 (1 is radius of orbit circle)
	GLC_Vector4d VectMouse(Posx, Posy,0);
	if (VectMouse.norm() > 1)
	{
		VectMouse.setNormal(1);
	}
	else
	{
		VectMouse.setZ(sqrt(1.0 - VectMouse.X() *  VectMouse.X() - VectMouse.Y() * VectMouse.Y()));
	}

	return VectMouse;
}


// Update orbit circle dimensions
void GLC_Viewport::updateOrbitCircle()
{
	int nRayonSph;
	if (m_nWinHSize > m_nWinVSize)
	{
		nRayonSph = static_cast<int>(m_dRatWinSph * m_nWinVSize / 2);
	}
	else
	{
		nRayonSph = static_cast<int>(m_dRatWinSph * m_nWinHSize / 2);
	}

	// Compute the length of camera's field of view
	const double ChampsVision = 2 * (m_dCamDistMin + (m_dCamDistMax - m_dCamDistMin) / 2) *  tan((m_dFov * PI / 180) / 2);

	// the side of camera's square is mapped on Vertical length of window
	// Circle radius in OpenGL unit = Radius(Pixel) * (dimend GL / dimens Pixel)
	const double RayonSph= ((double)nRayonSph * ChampsVision / (double)m_nWinVSize);

	if (fabs(RayonSph) > EPSILON)
	{
		m_pOrbitCircle->setRadius(RayonSph);
	}

}
