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
	 *  By default, Axis of Cylinder is Z Axis
	 *  dRadius must be > 0
	 *  dLength must be > 0*/
	GLC_Cylinder(double dRadius, double dLength, const char *pName= "Cylinder", const GLfloat *pColor= 0);
	
	//! Copy contructor
	GLC_Cylinder(const GLC_Cylinder& sourceCylinder);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Get Lenght of the Cylinder
	double GetLength(void) const;

	//! Get Radius of cylinder
	double GetRadius(void) const;

	//! Get Cylinder discretion
	int GetDiscretion(void) const;
	
	//! return the cylinder bounding box
	virtual GLC_BoundingBox* getBoundingBox(void) const;
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
// Functions impacting Display List

	//! Set Cylinder length
	/*! Length must be > 0*/
	void SetLength(double Length);

	//! Set Cylinder radius
	/*! Radius must be > 0*/
	void SetRadius(double Radius);

	//! Set Discretion
	/*! Discretion must be > 0*/
	void SetDiscretion(int TargetDiscret);

	//! End Caps
	void SetEndedCaps(bool CapsEnded);

// End of functions impacting display list
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n
	 *  Throw GLC_OpenGlException*/
	virtual void GlDraw(void);

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function is implemented here.\n
	 *  Throw GLC_OpenGlException*/
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
