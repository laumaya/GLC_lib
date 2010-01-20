/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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
//! \file glc_plane.h Interface for the GLC_Plane class.

#ifndef GLC_PLANE_H_
#define GLC_PLANE_H_

#include "glc_vector3d.h"
#include "glc_vector4d.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Plane
/*! \brief GLC_Plane : Math plane representation */

/*! GLC_Plane is definined by its equation : Ax + By + CZ + D= 0 */
//////////////////////////////////////////////////////////////////////
class GLC_Plane
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_Plane();

	//! Contruct a plan with specified parameter
	GLC_Plane(double, double, double, double);

	//! Copy constructor
	GLC_Plane(const GLC_Plane&);

	//! Assignement operator
	GLC_Plane &operator=(const GLC_Plane&);

	//! Destructor
	virtual ~GLC_Plane();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Return A coef
	inline double coefA() const
	{return m_A;}

	//! Return B coef
	inline double coefB() const
	{return m_B;}

	//! Return C coef
	inline double coefC() const
	{return m_C;}

	//! Return D coef
	inline double coefD() const
	{return m_D;}

	//! Return the signed distance to a point
	inline double distanceToPoint(const GLC_Point3d& p) const
	{return m_A * p.X() + m_B * p.Y() + m_C * p.Z() + m_D;}

	//! Return the signed distance to a point
	inline double distanceToPoint(const GLC_Point4d& p) const
	{return m_A * p.X() + m_B * p.Y() + m_C * p.Z() + m_D;}

	//! Equality operator
	bool operator==(GLC_Plane) const;

	//! diff operator
	inline bool operator!=(const GLC_Plane& p) const
	{return !operator==(p);}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set A coef
	inline void setA(double a)
	{m_A= a;}

	//! Set B coef
	inline void setB(double b)
	{m_B= b;}

	//! Set C coef
	inline void setC(double c)
	{m_C= c;}

	//! Set D coef
	inline void setD(double d)
	{m_D= d;}

	//! Normalize the plane
	void normalize();


//@}

//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////
private:
	//! Plane is define by equation : Ax + By + Cz + D= 0
	double m_A, m_B, m_C, m_D;
};

#endif /* GLC_PLANE_H_ */
