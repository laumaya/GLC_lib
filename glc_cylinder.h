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
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Construct an GLC_Cylinder
	/*! By default, discretion is set to #GLC_POLYDISCRET \n
	 *  By default, Axis of Cylinder is Z Axis*/
	GLC_Cylinder(double dRadius, double dLength, const char *pName= "Cylinder", const GLfloat *pColor= 0);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Get Lenght of the Cylinder
	double GetLength(void) const
	{
		return m_dLength;
	}

	//! Get Radius of cylinder
	double GetRadius(void) const
	{
		return m_Radius;
	}

	//! Get Cylinder discretion
	int GetDiscretion(void) const
	{
		return m_nDiscret;
	}
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

// Functions impacting Display List

	//! Set Cylinder length
	void SetLength(double Length)
	{
		 m_dLength= Length;

		 m_ListIsValid= false;
	}

	//! Set Cylinder radius
	void SetRadius(double Radius)
	{
		m_Radius= Radius;
		
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

	//! Cylinder's radius
	double m_Radius;

	//! Cylinder length (Z Axis direction)
	double m_dLength;

	//! Cylinder polygon discretisation
	int m_nDiscret;

	//! Cylinder is capped
	bool m_bCapEnded;

	
};
#endif //GLC_CYLINDER_H_
