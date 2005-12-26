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

//! \file glc_orbitCercle.h interface for the GLC_OrbitCercle class.

#ifndef GLC_ORBITCERCLE_H_
#define GLC_ORBITCERCLE_H_


#include "glc_cercle.h"

//! The angle of arcs
#define ARCANGLE (30 * PI / 180)	

// Arc discretiisation
#define ARCDISCRET 6	

//////////////////////////////////////////////////////////////////////
//! \class GLC_OrbitCercle
/*! \brief GLC_OrbitCercle : OpenGL 3D Orbit Circle*/

/*! An GLC_OrbitCercle is a wire geometry composed of 3d lines \n
 * It's an circle (GLC_Cercle) with 2 arcs (GLC_Cercle) of an angle of #ARCANGLE\n
 * The orbit Circle represent camera orientation.
 * */
//////////////////////////////////////////////////////////////////////

class GLC_OrbitCercle : public GLC_Cercle  
{

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
public:
	//! Create an orbit Circle with a given radius, name and color.
	GLC_OrbitCercle(const double &dRayon, const char *pName,
		const GLfloat *pColor);

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
public:

	//! Reimplented SetRayon Function
	bool SetRayon(double R)
	{
		bool Result;
		
		Result= GLC_Cercle::SetRayon(R);	// Rayon de la classe de base		

		m_Arc1.SetRayon(R);
		m_Arc2.SetRayon(R);
		return Result;
	}

	//! Set Arcs orientation and position in concordance with mouse position
	void SetOrientArcs(GLC_Vector4d VectAngle, const GLC_Matrix4x4 &Matrice);

	//! Set Arcs position in concordance with mouse position
	void MapArcs(const GLC_Matrix4x4 &Matrice);


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
public:	
	//! Orbit circle OpenGL Execution
	bool GlExecute(double Profondeur);

//////////////////////////////////////////////////////////////////////
// Private Members
//////////////////////////////////////////////////////////////////////
private:
	//! Arc 1 showing orbit sphere orientation	
	GLC_Cercle m_Arc1;
	//! Arc 1 positionning Matrix
	GLC_Matrix4x4 m_MatArc1;
	//! Arc 2 showing orbit sphere orientation
	GLC_Cercle m_Arc2;
	//! Arc 2 positionning Matrix
	GLC_Matrix4x4 m_MatArc2;


};

#endif //GLC_ORBITCERCLE_H_
