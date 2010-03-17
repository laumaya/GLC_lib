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
//! \file glc_flymover.h Interface for the GLC_FlyMover class.

#ifndef GLC_FLYMOVER_H_
#define GLC_FLYMOVER_H_
#include "glc_mover.h"

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_FlyMover
/*! \brief GLC_FlyMover : Fly Mode interactive manipulation */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_FlyMover : public GLC_Mover
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_FlyMover(GLC_Viewport*, const QList<GLC_RepMover*>& repsList= QList<GLC_RepMover*>());

	//! Copy constructor
	GLC_FlyMover(const GLC_FlyMover& flyMover);

	//! Destructor
	virtual ~GLC_FlyMover();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return a clone of the mover
	virtual GLC_Mover* clone() const;
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Initialized the mover
	virtual void init(int, int);

	//! Move the camera
	virtual void move(int, int);

//@}

/////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
private:
	//! Map the position of mouse for the fly mode
	GLC_Vector3d mapForFlying(double x, double y);

//////////////////////////////////////////////////////////////////////
// Private Members
//////////////////////////////////////////////////////////////////////
private:
	//! Maximum angle
	double m_MaxAngle;
};

#endif /* GLC_FLYMOVER_H_ */
