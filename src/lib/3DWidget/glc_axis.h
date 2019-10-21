/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/
//! \file glc_axis.h Interface for the GLC_Axis class.

#ifndef GLC_AXIS_H_
#define GLC_AXIS_H_

#include "glc_3dwidget.h"
#include "../glc_config.h"

class GLC_AbstractManipulator;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Axis
/*! \brief GLC_Axis :  3d Widget axis use to translate objects*/

/*! GLC_Axis */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_Axis : public GLC_3DWidget
{
	enum Manipulator
	{
		NoneManipulator,
		X_AxisManipulator,
		Y_AxisManipulator,
		Z_AxisManipulator
	};

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Construct a axis widget
    GLC_Axis(const GLC_Point3d& center, GLC_3DWidgetManagerHandle*  pWidgetManagerHandle= nullptr);

	//! Copy constructor
    GLC_Axis(const GLC_Axis& other);

	//! Destructor
    ~GLC_Axis() override;
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return this axis center
    GLC_Point3d center() const
	{return m_Center;}

	//! Return this axis length
    double axisLength() const
	{return m_AxisLength;}

    //! Return the axis radius/length ratio
    double axisRadiusRatio() const
    {return m_AxisRadiusRatio;}

    //! Return the move step
    double moveStep() const
    {return m_MoveStep;}

    GLC_Vector3d offset() const
    {return m_Offset;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Copy the given cutting plane in this cutting plane and return a reference on this cutting plane
	virtual GLC_Axis& operator=(const GLC_Axis& axis);

	//! Update widget representation
    void updateWidgetRep() override;

	//! Set the axis length
	void setAxisLength(double length);

    //! Set the axis radius/length ratio
    void setAxisRadiusLengthRatio(double value);

	//! Set Axis center
	void setCenter(const GLC_Point3d& newCenter);

    //! Set Axis orientation
    void setOrientation(const GLC_Matrix4x4& matrix);

    //! Set the move step
    void setMoveStep(double value)
    {m_MoveStep= value;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Interaction Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! This widget as been selected
    glc::WidgetEventFlag select(const GLC_Point3d&, GLC_uint id) override;

	//! This widget as been unselected
    glc::WidgetEventFlag unselect(const GLC_Point3d&, GLC_uint id) override;

	//! The mouse is over this widget and a mousse button is pressed
    glc::WidgetEventFlag pressed(const GLC_Point3d&, GLC_uint id) override;

	//! The mouse is over this widget and a mousse button is released
    glc::WidgetEventFlag released() override;

	//! This widget is selected and the mousse move with a pressed buttons
    glc::WidgetEventFlag move(const GLC_Point3d&, GLC_uint id) override;

//@}

//////////////////////////////////////////////////////////////////////
// Protected services function
//////////////////////////////////////////////////////////////////////
protected:
	//! Create the 3DView instance of this 3d widget
    void create3DviewInstance() override;

	//! Reset the view state of this 3DWidget
    void resetViewState() override;

//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////
private:
	//! Move the manipulator 3D representation
	void moveManipulatorRep(const GLC_Point3d& pos);

//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////
private:
	//! The axis center
	GLC_Point3d m_Center;

    //! The axis orientation matrix
    GLC_Matrix4x4 m_OrientationMatrix;

	//! The manipulator scale factor
	double m_ScaleFactor;

	//! Current manipulator enum
	Manipulator m_CurrentManipulator;

	//! The current manipulator of this cutting plane
	GLC_AbstractManipulator* m_pCurrentManipulator;

	//! The axis length
	double m_AxisLength;

	//! The axis radius Ratio : Radius / Length
	double m_AxisRadiusRatio;

    //! The move step
    double m_MoveStep;

    GLC_Vector3d m_Offset;

};

#endif /* GLC_AXIS_H_ */
