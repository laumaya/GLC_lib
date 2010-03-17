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
//! \file glc_flymover.cpp Implementation of the GLC_FlyMover class.

#include "glc_flymover.h"
#include "glc_viewport.h"
#include "../maths/glc_utils_maths.h"

GLC_FlyMover::GLC_FlyMover(GLC_Viewport* pViewport, const QList<GLC_RepMover*>& repsList)
: GLC_Mover(pViewport, repsList)
, m_MaxAngle(glc::toRadian(2.0))
{

}

GLC_FlyMover::GLC_FlyMover(const GLC_FlyMover& flyMover)
: GLC_Mover(flyMover)
, m_MaxAngle(flyMover.m_MaxAngle)
{

}

GLC_FlyMover::~GLC_FlyMover()
{

}

//////////////////////////////////////////////////////////////////////
//Get Functions
//////////////////////////////////////////////////////////////////////

GLC_Mover* GLC_FlyMover::clone() const
{
	return new GLC_FlyMover(*this);
}


//////////////////////////////////////////////////////////////////////
//Set Functions
//////////////////////////////////////////////////////////////////////

void GLC_FlyMover::init(int x, int y)
{
	m_PreviousVector= mapForFlying(static_cast<double>(x), static_cast<double>(y));
}

void GLC_FlyMover::move(int x, int y)
{
	m_PreviousVector= mapForFlying(static_cast<double>(x), static_cast<double>(y));
	const GLC_Matrix4x4 viewMatrix(m_pViewport->cameraHandle()->viewMatrix());
	const GLC_Vector3d newPos= viewMatrix.inverted() * m_PreviousVector;
	const GLC_Vector3d forward(GLC_Vector3d(m_pViewport->cameraHandle()->forward()).normalize());

	// Compute rotation matrix
	const GLC_Vector3d axe(forward ^ newPos);
	if (!axe.isNull())
	{
		const double angle= acos(forward * newPos);
		const GLC_Matrix4x4 rotation(axe, angle);
		const GLC_Matrix4x4 trans1(-m_pViewport->cameraHandle()->eye());
		const GLC_Matrix4x4 trans2(m_pViewport->cameraHandle()->eye());
		const GLC_Matrix4x4 composition(trans2 * rotation * trans1);
		m_pViewport->cameraHandle()->move(composition);
	}
}

GLC_Vector3d GLC_FlyMover::mapForFlying(double x, double y)
{
	double AspectRatio;
	const double winHSize= static_cast<double>(GLC_Mover::m_pViewport->viewHSize());
	const double winVSize= static_cast<double>(GLC_Mover::m_pViewport->viewVSize());


	// Change origine and cover
	if (winHSize < winVSize)
	{
		AspectRatio= winVSize / winHSize;
		x= ( (x - winHSize  / 2.0 ) / ( winHSize / 2.0) );
		y= AspectRatio * ( ( winVSize / 2.0 - y) / ( winVSize / 2.0 ) );
	}
	else
	{
		AspectRatio= winHSize / winVSize;
		x= AspectRatio * ( (x - winHSize  / 2.0 ) / ( winHSize / 2.0) );
		y= ( (winVSize / 2.0 - y) / (winVSize / 2.0 ) );
	}

	GLC_Vector3d pos(x, y, 0.0);
	if (pos.length() > 1.0)
	{
		pos.normalize();
	}

	double z= -cos(m_MaxAngle) / sin(m_MaxAngle);
	pos.setZ(z);
	pos.normalize();

	return pos;
}

