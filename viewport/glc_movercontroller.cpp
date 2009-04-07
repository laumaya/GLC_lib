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

//! \file glc_movercontroller.cpp Implementation of the GLC_MoverController class.

#include "glc_movercontroller.h"

GLC_MoverController::GLC_MoverController()
: m_ActiveMoverId(0)
, m_MoverHash()
{


}

GLC_MoverController::~GLC_MoverController()
{
	MoverHash::iterator iMover= m_MoverHash.begin();
	while (iMover != m_MoverHash.constEnd())
	{
		delete iMover.value();

		++iMover;
	}
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Add a mover to the controller
void GLC_MoverController::addMover(GLC_Mover* pMover, const int id)
{
	Q_ASSERT(not m_MoverHash.contains(id));
	m_MoverHash.insert(id, pMover);
}

// Remove mover from the controller
void GLC_MoverController::removeMover(const int id)
{
	Q_ASSERT(m_MoverHash.contains(id));
	m_MoverHash.remove(id);
	if (id == m_ActiveMoverId)
	{
		m_ActiveMoverId= 0;
	}
}
