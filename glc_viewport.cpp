/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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

#include <QtDebug>
#include <assert.h>
#include "glc_viewport.h"
#include "glc_openglexception.h"

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
, m_dRatWinSph(0.90)			// Circle ratio size / window size
, m_OrbitCircleIsVisible(false)	// Show state of orbit Circle
, m_pQGLWidget(GLWidget)		// Attached QGLWidget
// the default backgroundColor
, m_BackgroundColor((QColor::fromRgbF(0.39, 0.39, 0.39, 1.0)))
{
	// create a camera
	m_pViewCam= new GLC_Camera;

	// Color of orbit circle
	const GLfloat color[4]= {1.0f, 0.11372f, 0.11372f, 0.0f};
	// Create orbit circle of specified color
	m_pOrbitCircle= new GLC_OrbitCircle(1.0,"Cercle_Sph", color);
	
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
		const GLC_Vector4d VectTarget(pX, pY, pZ);
		const double Distance= (m_pViewCam->getVectEye() - VectTarget).getNorm();
		qDebug("Return Distance: %e\n", Distance);
		m_pViewCam->setTargetCam(VectTarget);
	}
	else
	{	// Geometrie not find -> panning
		
		const GLC_Vector4d VectCur(mapPosMouse(x, y));
		const GLC_Vector4d VectPrev(mapPosMouse(getWinHSize() / 2, getWinVSize() / 2));
		const GLC_Vector4d VectPan(VectCur - VectPrev);	// panning vector
		// pan camera
		m_pViewCam->pan(VectPan);		
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

		glTranslated(m_pViewCam->getVectTarget().getX(), m_pViewCam->getVectTarget().getY(),
			m_pViewCam->getVectTarget().getZ() );

		// Graphic propertys
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor4fv(m_pOrbitCircle->getfRGBA());
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
	if (m_pImagePlane != NULL)
	{
		m_pImagePlane->glExecute();
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

	// Update image plane size
	if (m_pImagePlane != NULL)
		m_pImagePlane->updatePlaneSize();

	// Update orbit circle size
	updateOrbitCircle();

	// from NeHe's Tutorial 3
	if (m_nWinVSize == 0)								// Prevent A Divide By Zero By
	{
		m_nWinVSize= 1;									// Making Height Equal One
	}

	glViewport(0,0,m_nWinHSize,m_nWinVSize);			// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	double AspectRatio;
	AspectRatio= static_cast<double>(m_nWinHSize)/static_cast<double>(m_nWinVSize);
	gluPerspective(m_dFov, AspectRatio, m_dCamDistMin, m_dCamDistMax);

	glMatrixMode(GL_MODELVIEW);							// select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

//! select an object and return is UID
GLC_uint GLC_Viewport::select(QGLWidget *pGLWidget, int x, int y)
{
	const int BUFSIZE= 1024;
	GLuint SelectBuf[BUFSIZE];

	// Change to selection mode
	beginSelection(x, y);
	
	glSelectBuffer(BUFSIZE, SelectBuf);
	glRenderMode(GL_SELECT);
	
	// Init selection names
	glInitNames();
	glPushName(0);
	// Draw the scene
	pGLWidget->updateGL();

	
	// Compute number of hits
	const GLint NbrHits= glRenderMode(GL_RENDER);	
	//qDebug() << "Number of hits : " << NbrHits;

	// End of selection mode, restore Visualisation state
	endSelection();

	GLC_uint ReturnID= 0;
	if (NbrHits > 0)
	{
		GLuint zMin = SelectBuf[1];	// Min object depth value
		ReturnID= SelectBuf[3];		// Object Name
		
		for (int i= 1; i < NbrHits; ++i)
		{
			if (SelectBuf[4*i+1] < zMin)
		  	{
		    	zMin = SelectBuf[4*i+1];	// Min object depth value
		    	ReturnID= SelectBuf[4*i+3];	// Object Name
		  	}
		}
	}
	//qDebug() << "ReturnID : " << ReturnID;
	return ReturnID;

}

// load background image
bool GLC_Viewport::loadBackGroundImage(QGLWidget *GLWidget, const QString Image)
{
	deleteBackGroundImage();
	m_pImagePlane= new GLC_ImagePlane(this);
	return m_pImagePlane->loadImageFile(GLWidget, Image);
}

// delete background image
void GLC_Viewport::deleteBackGroundImage()
{
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
void GLC_Viewport::prepareOrbiting(double Cx, double Cy)
{
	m_VectPrevOrbit.setVect(mapForOrbit(Cx,Cy));
	m_OrbitCircleIsVisible= true;
	m_CameraTargetIsVisible= true;

	const double Angle= acos(AxeZ * m_VectPrevOrbit);
	const GLC_Vector4d AxeRot(AxeZ ^ m_VectPrevOrbit);

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
void GLC_Viewport::prepareZooming(int Cy)
{
	m_CameraTargetIsVisible= true;
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
	assert(!box.isEmpty());
	// Ratio between screen size and bounding box size
	const double boundingBoxCover= 1.0;
	
	// Center view on the BoundingBox
	const GLC_Vector4d deltaVector(box.getCenter() - m_pViewCam->getVectTarget());
	m_pViewCam->translate(deltaVector);
	//m_pViewCam->setTargetCam(box.getCenter());
	
	// Compute the new BoundingBox
	GLC_Matrix4x4 matTranslateCam(m_pViewCam->getVectEye());
	GLC_Matrix4x4 matRotateCam(m_pViewCam->getMatCompOrbit());
	
	GLC_Matrix4x4 matComp(matRotateCam.invert() * matTranslateCam.invert());	
	
	// The bounding Box in Camera coordinate
	GLC_BoundingBox boundingBox(box);
	boundingBox.transform(matComp);
	// Compute camera's cover
	double cameraCoverX= fabs(boundingBox.getUpper().getX()
						- boundingBox.getLower().getX());

	double cameraCoverY= fabs(boundingBox.getUpper().getY()
						- boundingBox.getLower().getY());

	double cameraCover= boundingBoxCover * fmax(cameraCoverX, cameraCoverY);

	double boxProf= fabs(boundingBox.getUpper().getZ()
						- boundingBox.getLower().getZ());
	
	// Compute Camera distance
	const double distance = cameraCover / (2 * tan((m_dFov * PI / 180) / 2));
	
	// Update Camera position
	m_pViewCam->setDistEyeTarget(distance + boxProf / 2.0);
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
		qDebug("GLC_Viewport::SetDistMax : KO");
		return false;
	}
}

// Set Near and Far clipping distance
void GLC_Viewport::setDistMinAndMax(const GLC_BoundingBox& bBox)
{
	assert(!bBox.isEmpty());
	GLC_Matrix4x4 matTranslateCam(m_pViewCam->getVectEye());
	GLC_Matrix4x4 matRotateCam(m_pViewCam->getMatCompOrbit());
	
	GLC_Matrix4x4 matComp(matRotateCam.invert() * matTranslateCam.invert());	
	
	// The bounding Box in Camera coordinate
	GLC_BoundingBox boundingBox(bBox);
	boundingBox.transform(matComp);
	// Increase size of the bounding box
	const double increaseFactor= 1.1;
	// Convert box distance in sphere distance
	double d1= fabs(boundingBox.getLower().getZ());
	double d2= fabs(boundingBox.getUpper().getZ());
	double min= fmin(d1,d2)* (2.0 - increaseFactor);
	double max= fmax(d1,d2) * increaseFactor;
	GLC_Vector4d vectCamEye(m_pViewCam->getVectEye());
	
	vectCamEye= matComp * vectCamEye;
	
	if (!boundingBox.intersect(vectCamEye))
	{
	
		m_dCamDistMin= min;
		m_dCamDistMax= max;
		//qDebug() << "distmin" << m_dCamDistMin;
		//qDebug() << "distmax" << m_dCamDistMax;		
	}
	else
	{
		m_dCamDistMin= (max + min) * 0.001;
		m_dCamDistMax= max;
		//qDebug() << "inside distmin" << m_dCamDistMin;
		//qDebug() << "inside distmax" << m_dCamDistMax;		
	}
		
		updateProjectionMat();	// Update OpenGL projection matrix		
		updateOrbitCircle();	// Update orbit circle
		if (m_pImagePlane != NULL)
		{
			m_pImagePlane->updateZPosition();	// Update image plane Z Position
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
	if (VectMouse.getNorm() > 1)
	{
		VectMouse.setNormal(1);
	}
	else
	{
		VectMouse.setZ(sqrt(1 - VectMouse.getX() *  VectMouse.getX() - VectMouse.getY() * VectMouse.getY()));
	}
	
	return VectMouse;
}

// Update OpenGL Projection Matrix
void GLC_Viewport::updateProjectionMat(void) const
{
	glMatrixMode(GL_PROJECTION);						// select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	double AspectRatio;
	AspectRatio= static_cast<double>(m_nWinHSize)/static_cast<double>(m_nWinVSize);
	gluPerspective(m_dFov, AspectRatio, m_dCamDistMin, m_dCamDistMax);

	glMatrixMode(GL_MODELVIEW);							// select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
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

// Change to selection mode, save Visualisation state
void GLC_Viewport::beginSelection(GLdouble x, GLdouble y)
{
	// Change to projection mode
	glMatrixMode(GL_PROJECTION);
	// Save projection matrix
	glPushMatrix();
	glLoadIdentity();
	GLint Viewport[4];
	glGetIntegerv(GL_VIEWPORT, Viewport);
	// Load selection matrix
	gluPickMatrix(x, Viewport[3] - y, SELECT_WIDTH, SELECT_HEIGHT, Viewport);

	// Calculate The Aspect Ratio Of The Window
	double AspectRatio;
	AspectRatio= static_cast<double>(m_nWinHSize)/static_cast<double>(m_nWinVSize);
	gluPerspective(m_dFov, AspectRatio, m_dCamDistMin, m_dCamDistMax);

	// Back to visualisation mode
	glMatrixMode(GL_MODELVIEW);
}

// End of selection mode, restore Visualisation state
void GLC_Viewport::endSelection(void)
{
	// Change to projection mode
	glMatrixMode(GL_PROJECTION);
	// retore projection matrix
	glPopMatrix();
	// Back to visualisation mode
	glMatrixMode(GL_MODELVIEW);

}
