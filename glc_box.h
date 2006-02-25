/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.5, packaged on February, 2006.

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


#include "glc_geometry.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Box
/*! \brief GLC_Box : OpenGL Box*/

/*! An GLC_Box is a polygonal geometry\n
 *  It's a rectangular parallelepiped box centred at (0, 0, 0)*/
 

//////////////////////////////////////////////////////////////////////

class GLC_Box : public GLC_Geometry  
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor */
//@{
//////////////////////////////////////////////////////////////////////
public:	
	//! Construct an GLC_Box
	/*! By default, discretion is set to #GLC_POLYDISCRET*/
	GLC_Box(double dLongueur, double dLargeur, double dHauteur
		, const char *pName= "Cuboid", const GLfloat *pColor= 0);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
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

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

// Display List modifying functions
	//! Set X length
	/*! This Function invalid OpenGL display list*/
	void SetLgX(double LgX)
	{
		 m_dLgX= LgX;
		 m_ListIsValid= false;
	}

	//! Set Y length
	/*! This Function invalid OpenGL display list*/
	void SetLgY(double LgY)
	{
		 m_dLgY= LgY;
		 m_ListIsValid= false;
	}

	//! Set Z length
	/*! This Function invalid OpenGL display list*/
	void SetLgZ(double LgZ)
	{
		 m_dLgZ= LgZ;
		 m_ListIsValid= false;
	}
// End of Display List modifying functions

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n*/
	virtual void GlDraw(void);

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function is implemented here.*/
	virtual void GlPropGeom(void);
//@}

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

	
};
#endif //GLC_BOX_H_
