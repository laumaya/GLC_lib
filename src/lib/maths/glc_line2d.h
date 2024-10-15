/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/
//! \file GLC_Line2d.h Interface for the GLC_Line2d class.

#ifndef GLC_Line2d_H_
#define GLC_Line2d_H_

#include <QMetaType>

#include "glc_vector2d.h"

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Line2d
/*! \brief GLC_Line2d : Math 3d line representation */

/*! GLC_Line2d is definined by a 3d point and a vector*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_Line2d
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
    GLC_Line2d();

	//! Construct a 3d line with the given 3d point and vector
    GLC_Line2d(const GLC_Point2d& point, const GLC_Vector2d& vector);

	//! Construct a 3d line with the given 3d line
    GLC_Line2d(const GLC_Line2d& line);

	//! Destructor
    ~GLC_Line2d();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the starting 3d point of this line
    inline GLC_Point2d startingPoint() const
	{return m_Point;}

	//! Return the direction vector of this line
    inline GLC_Vector2d direction() const
	{return m_Vector;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set the starting point of this  3d line
    inline void setStartingPoint(const GLC_Point2d& point)
	{m_Point= point;}

	//! Set the direction vector of this line
    inline void setDirection(const GLC_Vector2d& direction)
	{m_Vector= direction;}
//@}

//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////
private:
	//! Starting point of the 3d line
    GLC_Point2d m_Point;

	//! Vector of the line
    GLC_Vector2d m_Vector;

};

Q_DECLARE_METATYPE(GLC_Line2d)

#endif /* GLC_Line2d_H_ */
