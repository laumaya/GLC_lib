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

//! \file glc_circle.h interface for the GLC_Circle class.

#ifndef GLC_CIRCLE_H_
#define GLC_CIRCLE_H_

#include "glc_geometry.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Circle
/*! \brief GLC_Circle : OpenGL 3D Circle*/

/*! An GLC_Circle is a wire geometry composed of 3d lines \n
 * It can be an entire circle or an arc.
 * */
//////////////////////////////////////////////////////////////////////

class GLC_Circle : public GLC_Geometry  
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:	

	//! Construct an GLC_Circle
	/*! By default, discretion is set to #GLC_DISCRET*/
	GLC_Circle(const double &dRadius, const char *pName= "Circle",
		const GLfloat *pColor= 0, double Angle= 2 * PI);

	//! Destructor
	/*! \todo Check if this destructor is necessary*/
	virtual ~GLC_Circle();	// Delete display list

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	
	//! Return Circle Discretion
	int GetDiscretion(void) const
	{
		return m_nDiscret;
	}
	//! Return Circle radius
	double GetRadius(void) const
	{
		return m_Radius;
	}
	//! return Circle diameter
	double GetDiameter(void) const
	{
		return 2 * m_Radius;
	}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set Circle diameter
	bool SetDiameter(double D)
	{
		return !!(SetRadius(D / 2));
	}
	//! Set Circle Radius
	bool SetRadius(double R);
	
	//! Set Circle discret
	void SetDiscretion(int TargetDiscret);
	
	//! Set Circle Angle
	bool SetAngle(double AngleRadians);	// Angle in Radians
	
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
// private members
//////////////////////////////////////////////////////////////////////
private:

	//! Circle Radius
	double m_Radius;
	//! Circle Discretion
	int m_nDiscret;

	//! Angle of circle in radians
	double m_dAngle;
		
};
#endif //GLC_CIRCLE_H_
