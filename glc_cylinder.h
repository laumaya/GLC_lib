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

//! \file glc_cylinder.h interface for the GLC_Cylinder class.

#ifndef GLC_CYLINDER_H_
#define GLC_CYLINDER_H_


#include "glc_geometry.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Cylinder
/*! \brief GLC_Cylinder : OpenGL 3D Cylinder*/

/*! An GLC_Cylinder is a polygonnal geometry \n
 * It can be capped or not
 * */
//////////////////////////////////////////////////////////////////////

class GLC_Cylinder : public GLC_Geometry  
{
//////////////////////////////////////////////////////////////////////
// constructor destructor
//////////////////////////////////////////////////////////////////////
public:

	//! Construct an GLC_Cylinder
	/*! By default, discretion is set to #GLC_POLYDISCRET \n
	 *  By default, Axis of Cylinder is Z Axis*/
	GLC_Cylinder(double dRayon, double dLongeur, const char *pName= "Cylinder", const GLfloat *pColor= 0);

//////////////////////////////////////////////////////////////////////
// Fonctions Get
//////////////////////////////////////////////////////////////////////
public:

	//! Get Lenght of the Cylinder
	double GetLg(void) const
	{
		return m_dLg;
	}

	//! Get Radius of cylinder
	double GetRayon(void) const
	{
		return m_dRayon;
	}

	//! Get Cylinder discretion
	int GetDiscretion(void) const
	{
		return m_nDiscret;
	}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
public:

// Functions impacting Display List

	//! Set Cylinder length
	void SetLgAxe(double Lg)
	{
		 m_dLg= Lg;

		 m_ListIsValid= false;
	}

	//! Set Cylinder radius
	void SetRayon(double Rayon)
	{
		m_dRayon= Rayon;
		
		m_ListIsValid= false;
	}

	//! Set Discretion
	void SetDiscretion(int TargetDiscret)
	{
		TargetDiscret= abs(TargetDiscret);
		if (TargetDiscret != m_nDiscret)
		{
			m_nDiscret= TargetDiscret;
			if (m_nDiscret < 6) m_nDiscret= 6;

			m_ListIsValid= false;
		}
	}

	//! End Caps
	void SetEndedCaps(bool CapsEnded)
	{
		m_bCapEnded= CapsEnded;

		m_ListIsValid= false;
	}

// End of functions impacting display list

// Fonction modifiant les proprièté géométrique
	
	//! Polygon's display style
	void SetPolygonMode(GLenum Face, GLenum Mode)
	{
		m_PolyFace= Face;
		m_PolyMode= Mode;
		m_GeometryIsValid = false;
	}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
private:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n*/
	virtual void GlDraw(void);

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function is implemented here.*/
	virtual void GlPropGeom(void);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////

private:

	//! Cylinder's radius
	double m_dRayon;

	//! Cylinder length (Z Axis direction)
	double m_dLg;

	//! Cylinder polygon discretisation
	int m_nDiscret;

	//! Cylinder is capped
	bool m_bCapEnded;

	//! polygons display style
	GLenum m_PolyFace;
	GLenum m_PolyMode;

	
};
#endif //GLC_CYLINDER_H_
