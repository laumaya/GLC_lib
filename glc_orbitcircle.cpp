/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

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

//! \file glc_orbitCircle.cpp implementation of the GLC_OrbitCircle class.
#include "glc_factory.h"
#include "glc_circle.h"
#include "glc_orbitcircle.h"

#include <QGLContext>

using namespace glc;
//! The angle of arcs
#define ARCANGLE (30 * PI / 180)	

// Arc discretisation
#define ARCDISCRET 12	

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////

GLC_OrbitCircle::GLC_OrbitCircle(const double &dRayon, const QGLContext *pContext)
: m_pFactory(GLC_Factory::instance(pContext))
, m_MainCircle(dRayon)
, m_Arc1(m_pFactory->createCircle(dRayon, ARCANGLE))
, m_MatArc1()
, m_Arc2(m_pFactory->createCircle(dRayon, ARCANGLE))
, m_MatArc2()
{
	// 2 circle arcs position
	GLC_Matrix4x4 MatRot(Z_AXIS, -ARCANGLE / 2);
	GLC_Matrix4x4 MatInt(Y_AXIS, -PI / 2);
	MatRot= MatInt * MatRot;

	m_MatArc1= MatRot;

	MatInt.setMatRot(Z_AXIS, PI/2);
	MatRot= MatInt * MatRot;

	m_MatArc2= MatRot;
	
	// Set arc discretion
	GLC_Circle* pCircle;
	pCircle= static_cast<GLC_Circle*>(m_Arc1.getGeometry());
	pCircle= static_cast<GLC_Circle*>(m_Arc2.getGeometry());
}
// Change the radius of the orbit circle
void GLC_OrbitCircle::setRadius(double R)
{
	
	// Main circle radius
	m_MainCircle.setRadius(R);
	
	GLC_Circle* pCircle;	
	// Arc 1 radius
	pCircle= static_cast<GLC_Circle*>(m_Arc1.getGeometry());
	pCircle->setRadius(R); 		
	// Arc 2 radius
	pCircle= static_cast<GLC_Circle*>(m_Arc2.getGeometry());
	pCircle->setRadius(R); 		
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Arcs orientation and position in concordance with mouse position
void GLC_OrbitCircle::setOrientArcs(GLC_Vector4d VectAngle, const GLC_Matrix4x4 &Matrice)
{	
	VectAngle.setZ(0);
	VectAngle.setNormal(1);
	
	GLC_Matrix4x4 MatRot;
	double Angle;
	
	// Compute the 2 arcs orientation
	if (VectAngle.Y() > 0)
	{	// Angle entre 0 et PI
		Angle= acos(VectAngle.X());
		MatRot.setMatRot(Z_AXIS, Angle);		
	}
	else
	{	// Angle between 0 et -PI
		Angle= -acos(VectAngle.X());
		MatRot.setMatRot(Z_AXIS, Angle);		
	}

	// Composition of orientation matrix and mapping matrix
	MatRot= Matrice * MatRot;
	
	m_Arc1.setMatrix(MatRot * m_MatArc1);
	m_Arc2.setMatrix(MatRot * m_MatArc2);

}

// Set Arcs position in concordance with mouse position
void GLC_OrbitCircle::mapArcs(const GLC_Matrix4x4 &Matrice)
{
	m_Arc1.multMatrix(Matrice);
	m_Arc2.multMatrix(Matrice);
}

// overload function setColor(color);
void GLC_OrbitCircle::setRGBAColor(const QColor& color)
{
	m_MainCircle.setColor(color);
	m_Arc1.getGeometry()->setColor(color);
	m_Arc2.getGeometry()->setColor(color);
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// orbit circle OpenGL Execution
void GLC_OrbitCircle::glExecute(double Profondeur)

{
	// orbit circle must be shown
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
	
	glLoadIdentity();

	// Put circle at the middle of camera range of depth
	glTranslated(0, 0, - (Profondeur) );

	// Save Positionning matrix of arcs
	const GLC_Matrix4x4 MatSavArc1(m_Arc1.getMatrix());
	const GLC_Matrix4x4 MatSavArc2(m_Arc2.getMatrix());

	// Scale Z to 0. Project arcs in the main circle plane
	// Avoid perspective problems
	GLC_Matrix4x4 MatScaling;
	MatScaling.setMatScaling(1,1,0);
	m_Arc1.multMatrix(MatScaling);
	m_Arc2.multMatrix(MatScaling);

	// Display arcs
	m_Arc1.glExecute();
	m_Arc2.glExecute();

	// Restore positionning matrix of arcs
	m_Arc1.setMatrix(MatSavArc1);
	m_Arc2.setMatrix(MatSavArc2);
			
	// Display base class (Main circle)
	m_MainCircle.glExecute(false, false);
	
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
}
