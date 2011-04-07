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

//! \file glc_tsrmover.cpp Implementation of the GLC_TsrMover class.

#include "glc_tsrmover.h"
#include "glc_viewport.h"

// Default constructor
GLC_TsrMover::GLC_TsrMover(GLC_Viewport* pViewport, const QList<GLC_RepMover*>& repsList)
: GLC_Mover(pViewport, repsList)
{

}

// Copy constructor
GLC_TsrMover::GLC_TsrMover(const GLC_TsrMover& tsrMover)
: GLC_Mover(tsrMover)
{

}


GLC_TsrMover::~GLC_TsrMover()
{

}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Return a clone of the mover
GLC_Mover* GLC_TsrMover::clone() const
{
	return new GLC_TsrMover(*this);
}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Initialized the mover
void GLC_TsrMover::init(const GLC_UserInput& userInput)
{
	m_PreviousVector= m_pViewport->mapPosMouse(static_cast<double>(userInput.x()), static_cast<double>(userInput.y()));
}

// Move the camera
bool GLC_TsrMover::move(const GLC_UserInput& userInput)
{
	m_PreviousVector= m_pViewport->mapPosMouse(static_cast<double>(userInput.x()), static_cast<double>(userInput.y()));

	if (!qFuzzyCompare(userInput.rotationAngle(), 0))
	{
		GLC_Matrix4x4 rotationMatrix(m_pViewport->cameraHandle()->forward(), userInput.rotationAngle());
		m_pViewport->cameraHandle()->move(rotationMatrix);
	}

	if (!qFuzzyCompare(userInput.scaleFactor(), 0))
	{
		m_pViewport->cameraHandle()->zoom(userInput.scaleFactor());
	}

	if (!userInput.translation().isNull())
	{
		GLC_Vector3d mappedTranslation= m_pViewport->mapPosMouse(userInput.translation().getX(), userInput.translation().getY());
		m_pViewport->cameraHandle()->pan(mappedTranslation);
	}

	return true;
}

