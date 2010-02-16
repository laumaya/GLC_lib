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
//! \file glc_cuttingplane.h Interface for the GLC_CuttingPlane class.

#ifndef GLC_CUTTINGPLANE_H_
#define GLC_CUTTINGPLANE_H_

#include "glc_3dwidget.h"
#include "../glc_config.h"
#include "../maths/glc_vector3d.h"
#include "../maths/glc_plane.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_CuttingPlane
/*! \brief GLC_CuttingPlane :  3d cutting plane widget*/

/*! GLC_CuttingPlane */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_CuttingPlane : public GLC_3DWidget
{
	Q_OBJECT
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Construct a 3d cutting plane widget with the given viewport
	GLC_CuttingPlane(const GLC_Point3d& center, const GLC_Vector3d& normal, double l1, double l2, GLC_3DWidgetManagerHandle*  pWidgetManagerHandle= NULL);

	//! Destructor
	virtual ~GLC_CuttingPlane();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return this cutting plane center
	inline GLC_Point3d center() const
	{return m_Center;}

	//! Return this cutting plane normal
	inline GLC_Vector3d normal() const
	{return m_Normal;}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Update the lenght of this cutting plane
	void updateLenght(double l1, double l2);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Interaction Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! This widget as been selected
	virtual glc::WidgetEventFlag select(const GLC_Point3d&);

	//! This widget as been unselected
	virtual glc::WidgetEventFlag unselect(const GLC_Point3d&);

	//! The mouse is over this widget and a mousse button is pressed
	virtual glc::WidgetEventFlag mousePressed(const GLC_Point3d&, Qt::MouseButton);

	//! The mouse is over this widget
	virtual glc::WidgetEventFlag mouseOver(const GLC_Point3d&);

	//! This widget is selected and the mousse move with a pressed buttons
	virtual glc::WidgetEventFlag mouseMove(const GLC_Point3d&, Qt::MouseButtons);

//@}

//////////////////////////////////////////////////////////////////////
// Protected services function
//////////////////////////////////////////////////////////////////////
protected:
	//! Create the 3DView instance of this 3d widget
	virtual void create3DviewInstance();


//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////
private:
	//! Create sliding plane
	void prepareToSlide();


//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////
private:
	//! The cutting plane center
	GLC_Point3d m_Center;

	//! The cutting plane Normal
	GLC_Vector3d m_Normal;

	//! The cutting plane size
	double m_L1, m_L2;

	//! The previous selected point
	GLC_Point3d m_Previous;

	//! The current sliding plane
	GLC_Plane m_SlidingPlane;

};

#endif /* GLC_CUTTINGPLANE_H_ */
