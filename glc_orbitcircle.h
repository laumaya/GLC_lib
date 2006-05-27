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

//! \file glc_orbitCircle.h interface for the GLC_OrbitCircle class.

#ifndef GLC_ORBITCIRCLE_H_
#define GLC_ORBITCIRCLE_H_


#include "glc_circle.h"

//! The angle of arcs
#define ARCANGLE (30 * PI / 180)	

// Arc discretiisation
#define ARCDISCRET 6	

//////////////////////////////////////////////////////////////////////
//! \class GLC_OrbitCircle
/*! \brief GLC_OrbitCircle : OpenGL 3D Orbit Circle*/

/*! An GLC_OrbitCircle is a wire geometry composed of 3d lines \n
 * It's an circle (GLC_Circle) with 2 arcs (GLC_Circle) of an angle of #ARCANGLE\n
 * The orbit Circle represent camera orientation.
 * */
//////////////////////////////////////////////////////////////////////

class GLC_OrbitCircle : public GLC_Circle  
{

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an orbit Circle with a given radius, name and color.
	GLC_OrbitCircle(const double &dRayon, const char *pName,
		const GLfloat *pColor);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Reimplented SetRadius Function
	bool SetRadius(double R)
	{
		bool Result;
		
		Result= GLC_Circle::SetRadius(R);	// Rayon de la classe de base		

		m_Arc1.SetRadius(R);
		m_Arc2.SetRadius(R);
		return Result;
	}

	//! Set Arcs orientation and position in concordance with mouse position
	void SetOrientArcs(GLC_Vector4d VectAngle, const GLC_Matrix4x4 &Matrice);

	//! Set Arcs position in concordance with mouse position
	void MapArcs(const GLC_Matrix4x4 &Matrice);
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:	
	//! Orbit circle OpenGL Execution
	bool GlExecute(double Profondeur);
	
//@}

//////////////////////////////////////////////////////////////////////
// Private Members
//////////////////////////////////////////////////////////////////////
private:
	//! Arc 1 showing orbit sphere orientation	
	GLC_Circle m_Arc1;
	//! Arc 1 positionning Matrix
	GLC_Matrix4x4 m_MatArc1;
	//! Arc 2 showing orbit sphere orientation
	GLC_Circle m_Arc2;
	//! Arc 2 positionning Matrix
	GLC_Matrix4x4 m_MatArc2;


};

#endif //GLC_ORBITCIRCLE_H_
