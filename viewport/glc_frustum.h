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
//! \file glc_frustum.h Interface for the GLC_Frustum class.

#ifndef GLC_FRUSTUM_H_
#define GLC_FRUSTUM_H_

#include "glc_viewport.h"
#include "../maths/glc_plane.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Frustum
/*! \brief GLC_Frustum : OpenGL Frustum */

/*! GLC_Frustum by 6 planes */
//////////////////////////////////////////////////////////////////////
class GLC_Frustum
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_Frustum(GLC_Viewport*);

	//! Copy constructor
	GLC_Frustum(const GLC_Frustum&);

	//! Destructor
	virtual ~GLC_Frustum();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Return the left clipping plane
	inline GLC_Plane leftClippingPlane() const
	{return m_LeftPlane;}

	//! Return the Right clipping plane
	inline GLC_Plane rightClippingPlane() const
	{return m_RightPlane;}

	//! Return the top clipping plane
	inline GLC_Plane topClippingPlane() const
	{return m_TopPlane;}

	//! Return the bottom clipping plane
	inline GLC_Plane bottomClippingPlane() const
	{return m_BottomPlane;}

	//! Return the near clipping plane
	inline GLC_Plane nearClippingPlane() const
	{return m_NearPlane;}

	//! Return the far clipping plane
	inline GLC_Plane farClippingPlane() const
	{return m_FarPlane;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Set the left clipping plane
	inline void setLeftClippingPlane(const GLC_Plane& plane)
	{m_LeftPlane= plane;}

	//! Set the right clipping plane
	inline void setRightClippingPlane(const GLC_Plane& plane)
	{m_RightPlane= plane;}

	//! Set the top clipping plane
	inline void setTopClippingPlane(const GLC_Plane& plane)
	{m_TopPlane= plane;}

	//! Set the bottom clipping plane
	inline void setBottomClippingPlane(const GLC_Plane& plane)
	{m_BottomPlane= plane;}

	//! Set the near clipping plane
	inline void setNearClippingPlane(const GLC_Plane& plane)
	{m_NearPlane= plane;}

	//! Set the far clipping plane
	inline void setFarClippingPlane(const GLC_Plane& plane)
	{m_FarPlane= plane;}

	//! Update the frustum
	void update(GLC_Viewport*);

//@}

//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////
private:
	//! The left clip plane
	GLC_Plane m_LeftPlane;

	//! The right clip plane
	GLC_Plane m_RightPlane;

	//! The right clip plane
	GLC_Plane m_TopPlane;

	//! The right clip plane
	GLC_Plane m_BottomPlane;

	//! The right clip plane
	GLC_Plane m_NearPlane;

	//! The right clip plane
	GLC_Plane m_FarPlane;
};

#endif /* GLC_FRUSTUM_H_ */
