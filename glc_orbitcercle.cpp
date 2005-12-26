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

//! \file glc_orbitCercle.cpp implementation of the GLC_OrbitCercle class.

#include "glc_cercle.h"
#include "glc_orbitcercle.h"

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////

GLC_OrbitCercle::GLC_OrbitCercle(const double &dRayon, const char *pName,
					   const GLfloat *pColor)
:GLC_Cercle(dRayon, pName, pColor),
m_Arc1(dRayon, "Arc1", pColor, ARCANGLE),
m_Arc2(dRayon, "Arc2", pColor, ARCANGLE)
{
	// 2 circle arcs position
	GLC_Matrix4x4 MatRot(AxeZ, -ARCANGLE / 2);
	GLC_Matrix4x4 MatInt(AxeY, -PI / 2);
	MatRot= MatInt * MatRot;

	m_MatArc1= MatRot;

	MatInt.SetMatRot(AxeZ, PI/2);
	MatRot= MatInt * MatRot;

	m_MatArc2= MatRot;
	
	// Set arc discretion
	m_Arc1.SetDiscretion(ARCDISCRET);
	m_Arc2.SetDiscretion(ARCDISCRET);
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Arcs orientation and position in concordance with mouse position
void GLC_OrbitCercle::SetOrientArcs(GLC_Vector4d VectAngle, const GLC_Matrix4x4 &Matrice)
{	
	VectAngle.SetZ(0);
	VectAngle.SetNormal(1);
	
	GLC_Matrix4x4 MatRot;
	double Angle;
	
	// Compute the 2 arcs orientation
	if (VectAngle.GetY() > 0)
	{	// Angle entre 0 et PI
		Angle= acos(VectAngle.GetX());
		MatRot.SetMatRot(AxeZ, Angle);		
	}
	else
	{	// Angle between 0 et -PI
		Angle= -acos(VectAngle.GetX());
		MatRot.SetMatRot(AxeZ, Angle);		
	}

	// Composition of orientation matrix and mapping matrix
	MatRot= Matrice * MatRot;

	
	m_Arc1.SetMatrice(MatRot * m_MatArc1);
	m_Arc2.SetMatrice(MatRot * m_MatArc2);


}

// Set Arcs position in concordance with mouse position
void GLC_OrbitCercle::MapArcs(const GLC_Matrix4x4 &Matrice)
{
	m_Arc1.MultMatrice(Matrice);
	m_Arc2.MultMatrice(Matrice);
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Orbit circle OpenGL Execution
bool GLC_OrbitCercle::GlExecute(double Profondeur)

{
	if (GetVisible())
	{
		bool result;

		// Orbit circle must be shown
		glDisable(GL_DEPTH_TEST);

		glPushMatrix();
		
		glLoadIdentity();

		// Put cicle at the middle of camera range of depth
		glTranslated(0, 0, - (Profondeur) );

		// Save Positionning matrix of arcs
		const GLC_Matrix4x4 MatSavArc1(m_Arc1.GetMatrice());
		const GLC_Matrix4x4 MatSavArc2(m_Arc2.GetMatrice());

		// Scale Z to 0. Preject arcs in the main circle plane
		// Avoid perspective problems
		GLC_Matrix4x4 MatScaling;
		MatScaling.SetMatScaling(1,1,0);
		m_Arc1.MultMatrice(MatScaling);
		m_Arc2.MultMatrice(MatScaling);


		// Display arcs
		result= m_Arc1.GlExecute() && m_Arc2.GlExecute();

		// Restore positionning matrix of arcs
		m_Arc1.SetMatrice(MatSavArc1);
		m_Arc2.SetMatrice(MatSavArc2);
				
		// Display base class (Main circle)
		result= result && GLC_Cercle::GlExecute();
		
		glPopMatrix();

		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		return result;
	}
	else
		return true;
}
