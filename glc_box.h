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

//! \file glc_box.h interface for the GLC_Box class.

#ifndef GLC_BOX_H_
#define GLC_BOX_H_


#include "glc_geometrie.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Box
/*! \brief GLC_Box : OpenGL Box*/

/*! An GLC_Box is a polygonal geometry\n
 *  It's a rectangular parallelepiped box centred at (0, 0, 0)*/
 

//////////////////////////////////////////////////////////////////////

class GLC_Box : public GLC_Geometrie  
{
//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
public:	
	//! Construct an GLC_Box
	/*! By default, discretion is set to #GLC_POLYDISCRET*/
	GLC_Box(double dLongueur, double dLargeur, double dHauteur
		, const char *pName= "Cuboid", const GLfloat *pColor= 0);

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Get X length
	double GetLgX(void) const
	{
		return m_dLgX;
	}

	//! Get Y length
	double GetLgY(void) const
	{
		return m_dLgY;
	}

	//! Get Z length
	double GetLgZ(void) const
	{
		return m_dLgZ;
	}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
public:

// Fonction modifiant la liste d'affichage
	//! Set X length
	void SetLgX(double LgX)
	{
		 m_dLgX= LgX;
		 m_bListeIsValid= false;
	}

	//! Set Y length
	void SetLgY(double LgY)
	{
		 m_dLgY= LgY;
		 m_bListeIsValid= false;
	}

	//! Set Z length
	void SetLgZ(double LgZ)
	{
		 m_dLgZ= LgZ;
		 m_bListeIsValid= false;
	}
// Fin des fonctions modifiant la liste d'affichage

// Fonction modifiant les proprièté géométrique
	
	//! Set Drawing style of polygons
	void SetPolygonMode(GLenum Face, GLenum Mode)
	{
		m_PolyFace= Face;
		m_PolyMode= Mode;
		m_bValidite = false;
	}

//////////////////////////////////////////////////////////////////////
//  OpenGL Functions
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

	//! X Length
	double m_dLgX;

	//! Y Length
	double m_dLgY;

	//! Z Length
	double m_dLgZ;

	//! Drawing style of polygons
	GLenum m_PolyFace;
	//! Drawing style of polygons
	GLenum m_PolyMode;

	
};
#endif //GLC_BOX_H_
