/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.1.0, packaged on March, 2009.

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

#ifndef GLC_REPTRACKBALLMOVER_H_
#define GLC_REPTRACKBALLMOVER_H_

#include "glc_repmover.h"
#include "../geometry/glc_circle.h"
#include "../sceneGraph/glc_instance.h"

class GLC_Factory;


class GLC_RepTrackBallMover : public GLC_RepMover
{
public:
	//! Default constructor
	GLC_RepTrackBallMover(GLC_Viewport*);

	// Destructor
	virtual ~GLC_RepTrackBallMover();
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Set Arcs orientation and position in concordance with mouse position
	virtual void init(const GLC_Vector4d& VectAngle, const GLC_Matrix4x4 &Matrice);

	//! Set Arcs position in concordance with mouse position
	virtual void update(const GLC_Matrix4x4 &Matrice);

	//! Set representation main color
	virtual void setMainColor(const QColor& color);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Virtual interface for OpenGL Geometry set up.
	virtual void glDraw();

//@}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
private:
	//! Compute trackball radius
	void computeRadius();

//////////////////////////////////////////////////////////////////////
// Private Members
//////////////////////////////////////////////////////////////////////
private:
	//! trackball radius
	double m_Radius;
	//! Factory
	GLC_Factory* m_pFactory;
	//! Main Circle
	GLC_Circle m_MainCircle;
	//! Arc 1 showing orbit sphere orientation
	GLC_Instance m_Arc1;
	//! Arc 1 positionning Matrix
	GLC_Matrix4x4 m_MatArc1;
	//! Arc 2 showing orbit sphere orientation
	GLC_Instance m_Arc2;
	//! Arc 2 positionning Matrix
	GLC_Matrix4x4 m_MatArc2;
	//! The ratio of the trackball size
	double m_Ratio;

};

#endif /* GLC_REPTRACKBALLMOVER_H_ */
