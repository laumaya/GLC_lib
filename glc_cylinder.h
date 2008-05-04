/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.9, packaged on May, 2008.

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
	GLC_Cylinder(double dRadius, double dLength);
	
	//! Copy contructor
	GLC_Cylinder(const GLC_Cylinder& sourceCylinder);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Get Lenght of the Cylinder
	double getLength(void) const;

	//! Get Radius of cylinder
	double getRadius(void) const;

	//! Get Cylinder discretion
	int getDiscretion(void) const;
	
	//! return the cylinder bounding box
	virtual GLC_BoundingBox* getBoundingBox(void) const;
	
	//! Return a copy of the geometry
	virtual GLC_Geometry* clone() const;	
	
	//! return true if cylinder's ended are capped
	bool EndedIsCaped() const {return m_EndedIsCaped;}
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
// Functions impacting Display List

	//! Set Cylinder length
	/*! Length must be > 0*/
	void setLength(double Length);

	//! Set Cylinder radius
	/*! Radius must be > 0*/
	void setRadius(double Radius);

	//! Set Discretion
	/*! Discretion must be > 0*/
	void setDiscretion(int TargetDiscret);

	//! End Caps
	void setEndedCaps(bool CapsEnded);

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
	virtual void glDraw(void);
//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! Cylinder's radius
	double m_Radius;

	//! Cylinder length (Z Axis direction)
	double m_Length;

	//! Cylinder polygon discretisation
	int m_Discret;

	//! Cylinder is capped
	bool m_EndedIsCaped;
	
};
#endif //GLC_CYLINDER_H_
