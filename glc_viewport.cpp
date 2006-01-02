/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9, packaged on Novemeber, 2005.

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

// Used by selection function member
#include <QGLWidget>
#include "glc_viewport.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

// Default constructor
GLC_Viewport::GLC_Viewport()
// Camera definition
: m_pViewCam(NULL)				// Camera
, m_dCamDistMax(500)			// Camera Maximum distance
, m_dCamDistMin(0.01)			// Camera Minimum distance
, m_dFov(35)					// Camera angle of view
, m_pImagePlane(NULL)			// Background image
, m_bAfficheTargetCam(false)	// Show state of camera's target
// OpenGL Window size
, m_nWinHSize(0)				// Horizontal OpenGL viewport size
, m_nWinVSize(0)				// Vertical OpenGL viewport size
// Zoom
, m_dPrevZoom(0)				// Previous Zoom Factor
// Orbit circle definition
, m_pOrbitCircle(NULL)			// Orbit circle
, m_dRatWinSph(0.90)			// Circle ratio size / window size
, m_bAfficheOrbitCircle(false)	// Show state of orbit Circle
{
	// create a camera
	m_pViewCam= new GLC_Camera;

	// Color of orbit circle
	const GLfloat color[4]= {1.0f, 0.11372f, 0.11372f, 0.0f};
	// Create Orbit circle of specified color
	m_pOrbitCircle= new GLC_OrbitCircle(1,"Cercle_Sph", color);
	
	// Compute Orbit Circle
	//UpdateOrbitCircle();
}

// Delete Camera, Image Plane and Orbit circle
GLC_Viewport::~GLC_Viewport()
{
	// Delete the camera
	if (m_pViewCam != NULL)
	{
		delete m_pViewCam;	
		m_pViewCam= NULL;
	}

	// Delete Orbit circle
	if (m_pOrbitCircle != NULL)
	{
		delete m_pOrbitCircle;	
		m_pOrbitCircle= NULL;
	}

	// delete image plane
	if (m_pImagePlane != NULL)
	{
		delete m_pImagePlane;
		m_pImagePlane= NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Convert mouse View coordinate to orbit coordinate (Centred and between (-1,-1) and (1,1))
GLC_Vector4d GLC_Viewport::MapPosMouse( GLdouble Posx, GLdouble Posy) const
{
	// Change the window origin (Up Left -> centred)
	Posx= Posx - (double)m_nWinHSize  / 2;
	Posy= (double)m_nWinVSize / 2 - Posy;

	GLC_Vector4d VectMouse(Posx, Posy,0);
	
	// Compute the length of camera's field of view
	const double ChampsVision = 2 * m_pViewCam->GetDistEyeTarget() *  tan((m_dFov * PI / 180) / 2);

	// the side of camera's square is mapped on Vertical length of window
	// Ratio OpenGL/Pixel = dimend GL / dimens Pixel
	const double Ratio= ChampsVision / (double)m_nWinVSize;

	VectMouse= VectMouse * Ratio;

	return VectMouse;
}

//////////////////////////////////////////////////////////////////////
// Public OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Define camera's target position
void GLC_Viewport::GlPointing(GLint x, GLint y)
{
	// Z Buffer component of selected point between 0 and 1
	GLfloat Depth;
	// read selected point
	glReadPixels(x, GetWinVSize() - y , 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Depth);
	
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
	gluUnProject((GLdouble) x, (GLdouble) (GetWinVSize() - y) , Depth
		, ViewMatrix, ProjMatrix, Viewport, &pX, &pY, &pZ);

	qDebug("Return depth: %e, z= %e\n", Depth, pZ);

	// Test if there is geometry under picking point
	if (fabs(Depth - 1.0) > EPSILON)
	{	// Geometry find -> Update camera's target position
		const GLC_Vector4d VectTarget(pX, pY, pZ);
		const double Distance= (m_pViewCam->GetVectEye() - VectTarget).GetNorme();
		qDebug("Return Distance: %e\n", Distance);
		m_pViewCam->SetTargetCam(VectTarget);
	}
	else
	{	// Geometrie not find -> panning
		
		const GLC_Vector4d VectCur(MapPosMouse(x, y));
		const GLC_Vector4d VectPrev(MapPosMouse(GetWinHSize() / 2, GetWinVSize() / 2));
		const GLC_Vector4d VectPan(VectCur - VectPrev);	// panning vector
		// Pan camera
		m_pViewCam->Pan(VectPan);		
	}
}

//////////////////////////////////////////////////////////////////////
// Private OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Display Orbit Circle
void GLC_Viewport::GlExecuteOrbitCircle()
{
	if (m_bAfficheOrbitCircle)
	{

		m_pOrbitCircle->GlExecute(m_dCamDistMax - m_dCamDistMin);
	}
}
// Display Camera's target
void GLC_Viewport::GlExecuteTargetCam()	//! \todo Create a display list
{
	if (m_bAfficheTargetCam)
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
		const double ChampsVision = 2 * (m_pViewCam->GetDistEyeTarget()) *  tan((m_dFov * PI / 180) / 2);
	
		// the side of camera's square is mapped on Vertical length of window
		// Axis length in OpenGL unit = length(Pixel) * (dimend GL / dimens Pixel)
		const double dLgAxe= ((double)nLgAxe * ChampsVision / (double)m_nWinVSize) / 7;
		const double dDecAxe= dLgAxe / 3;
		glPushMatrix();

		glTranslated(m_pViewCam->GetVectTarget().GetX(), m_pViewCam->GetVectTarget().GetY(),
			m_pViewCam->GetVectTarget().GetZ() );

		// Graphic propertys
		glDisable(GL_LIGHTING);
		glColor4fv(m_pOrbitCircle->GetfRGBA());
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
	}
}

// Display background image
void GLC_Viewport::GLExecuteImagePlane()
{
	if (m_pImagePlane != NULL)
	{
		m_pImagePlane->GlExecute();
	}
}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Inform the viewport that the OpenGL Viewport has been modified
void GLC_Viewport::SetWinGLSize(int HSize, int VSize)
{
	m_nWinHSize= HSize;
	m_nWinVSize= VSize;

	// Update image plane size
	if (m_pImagePlane != NULL)
		m_pImagePlane->UpdatePlaneSize();

	// Update Orbit circle size
	UpdateOrbitCircle();

	// from NeHe's Tutorial 3
	if (m_nWinVSize == 0)								// Prevent A Divide By Zero By
	{
		m_nWinVSize= 1;									// Making Height Equal One
	}

	glViewport(0,0,m_nWinHSize,m_nWinVSize);			// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	double AspectRatio;
	AspectRatio= static_cast<double>(m_nWinHSize)/static_cast<double>(m_nWinVSize);
	gluPerspective(m_dFov, AspectRatio, m_dCamDistMin, m_dCamDistMax);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

//! Select an object and return is UID
GLC_uint GLC_Viewport::Select(QGLWidget *pGLWidget, int x, int y)
{
	const int BUFSIZE= 1024;
	GLuint SelectBuf[BUFSIZE];

	// Change to selection mode
	BeginSelection(x, y);
	
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
	EndSelection();

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
bool GLC_Viewport::ChargeImagePlane(QGLWidget *GLWidget, const QString Image)
{
	if (m_pImagePlane != NULL)
	{
		delete m_pImagePlane;
	}
	m_pImagePlane= new GLC_ImagePlane(this);
	return m_pImagePlane->ChargeImage(GLWidget, Image);
}

// delete background image
void GLC_Viewport::RemoveImagePlane()
{
	if (m_pImagePlane != NULL)
	{
		delete m_pImagePlane;
		m_pImagePlane= NULL;
	}

}


// Pan to the position (Cx, Cy)
void GLC_Viewport::Pan(double Cx, double Cy)
{
	const GLC_Vector4d VectCur(MapPosMouse(Cx,Cy));
	const GLC_Vector4d VectPan= VectCur - m_VectPrevPan;	// Vecteur de déplacement
	
	// Pan the camera
	m_pViewCam->Pan(-VectPan);
	m_VectPrevPan= VectCur;
}

// Prepare orbiting operation
void GLC_Viewport::PrepareOrbiting(double Cx, double Cy)
{
	m_VectPrevOrbit.SetVect(MapForOrbit(Cx,Cy));
	m_bAfficheOrbitCircle= true;
	m_bAfficheTargetCam= true;

	const double Angle= acos(AxeZ * m_VectPrevOrbit);
	const GLC_Vector4d AxeRot(AxeZ ^ m_VectPrevOrbit);

	GLC_Matrix4x4 Matrice(AxeRot, Angle);

	// Update orbit circle's arcs
	m_pOrbitCircle->SetOrientArcs(m_VectPrevOrbit, Matrice);
}


// Orbit in accordance of mouse position
void GLC_Viewport::Orbit(double Cx, double Cy)
{
	const GLC_Vector4d VectCurOrbit(MapForOrbit(Cx, Cy));

	// Update camera position (orbit)
	m_pViewCam->Orbit(m_VectPrevOrbit, VectCurOrbit);

	// Update arcs of circle's positionning matrix
	const GLC_Matrix4x4 MatRot(m_VectPrevOrbit, VectCurOrbit);
	m_pOrbitCircle->MapArcs(MatRot);

	// Previous vector become current vector
	m_VectPrevOrbit = VectCurOrbit;

}
// Prepare Zooming operation
void GLC_Viewport::PrepareZooming(int Cy)
{
	m_bAfficheTargetCam= true;
	// Change origine (view center) and cover between -1 and 1
	m_dPrevZoom= ((double)m_nWinVSize / 2 - Cy) / ( (double)m_nWinVSize / 2);;
}

// Zoom in accordance of Y mouse position
void GLC_Viewport::Zoom(int Cy)
{
	// Change origine (View Center) and cover (from -1 to 1)
	double Posy= ((double)m_nWinVSize / 2 - Cy) / ( (double)m_nWinVSize / 2);

	// Compute Zoom factor between (1 / MAXZOOMFACTOR) and (MAXZOOMFACTOR)
	double ZoomFactor= Posy - m_dPrevZoom;
	
	if (ZoomFactor > 0)
	{
		ZoomFactor= (MAXZOOMFACTOR - 1.0) * ZoomFactor + 1.0;
	}
	else
	{
		ZoomFactor= 1.0 / ( (MAXZOOMFACTOR - 1.0) * fabs(ZoomFactor) + 1.0 );
	}

	m_pViewCam->Zoom(ZoomFactor);

	m_dPrevZoom = Posy;
}

// Set near clipping distance
bool GLC_Viewport::SetDistMin(double DistMin)
{
	DistMin= fabs(DistMin);
	if (DistMin < m_dCamDistMax)
	{
		m_dCamDistMin= DistMin;
		
		UpdateProjectionMat();	// Update OpenGL projection matrix
		
		UpdateOrbitCircle();	// Update Orbit circle
		
		if (m_pImagePlane != NULL)
		{
			m_pImagePlane->UpdateZPosition();	// Update image plane Z Position
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
bool GLC_Viewport::SetDistMax(double DistMax)
{
	DistMax= fabs(DistMax);
	if (DistMax > m_dCamDistMin)
	{
		m_dCamDistMax= DistMax;
		
		UpdateProjectionMat();	// Update OpenGL projection matrix
		
		UpdateOrbitCircle();	// Update Orbit circle
		
		if (m_pImagePlane != NULL)
		{
			m_pImagePlane->UpdateZPosition();	// Update image plane Z Position
		}
		
		return true;
	}
	else
	{
		qDebug("GLC_Viewport::SetDistMax : KO");
		return false;
	}
}


//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////

// Convert mouse View coordinate to orbit coordinate (Centred and betwen (-1,-1) and (1,1))
GLC_Vector4d GLC_Viewport::MapForOrbit( double Posx, double Posy) const
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
	if (VectMouse.GetNorme() > 1)
	{
		VectMouse.SetNormal(1);
	}
	else
	{
		VectMouse.SetZ(sqrt(1 - VectMouse.GetX() *  VectMouse.GetX() - VectMouse.GetY() * VectMouse.GetY()));
	}
	
	return VectMouse;
}

// Update OpenGL Projection Matrix
void GLC_Viewport::UpdateProjectionMat(void) const
{
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	double AspectRatio;
	AspectRatio= static_cast<double>(m_nWinHSize)/static_cast<double>(m_nWinVSize);
	gluPerspective(m_dFov, AspectRatio, m_dCamDistMin, m_dCamDistMax);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

// Update orbit circle dimensions
void GLC_Viewport::UpdateOrbitCircle()
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
	const double ChampsVision = 2 * (m_dCamDistMax - m_dCamDistMin) *  tan((m_dFov * PI / 180) / 2);
	
	// the side of camera's square is mapped on Vertical length of window
	// Circle radius in OpenGL unit = Radius(Pixel) * (dimend GL / dimens Pixel)
	const double RayonSph= ((double)nRayonSph * ChampsVision / (double)m_nWinVSize);

	m_pOrbitCircle->SetRayon(RayonSph);

}

// Change to selection mode, save Visualisation state
void GLC_Viewport::BeginSelection(GLdouble x, GLdouble y)
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
void GLC_Viewport::EndSelection(void)
{
	// Change to projection mode
	glMatrixMode(GL_PROJECTION);
	// retore projection matrix
	glPopMatrix();
	// Back to visualisation mode
	glMatrixMode(GL_MODELVIEW);

}
