/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.8, packaged on January, 2008.

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

//! \file glc_part.cpp implementation of the GLC_Part class.

#include "glc_part.h"
#include "glc_world.h"

GLC_Part::GLC_Part(GLC_World *pWorld, GLC_Instance& instance)
: GLC_Node(pWorld)
, m_RepID(instance.getID())
{
	m_pWorld->collection()->add(instance);
}

GLC_Part::~GLC_Part()
{
	m_pWorld->collection()->remove(m_RepID);
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Move the Part with specified matrix
void GLC_Part::move(const GLC_Matrix4x4 &matrix)
{
	m_RelativeMatrix= matrix * m_RelativeMatrix;
	m_AbsoluteMatrix= matrix * m_AbsoluteMatrix;
	m_pWorld->collection()->getInstanceHandle(m_RepID)->setMatrix(m_AbsoluteMatrix);
}
// Update Part absolute matrix
void GLC_Part::updateAbsoluteMatrix()
{
	if (NULL != m_pParent)
	{
		m_AbsoluteMatrix= m_pParent->absoluteMatrix() * m_RelativeMatrix;
		m_pWorld->collection()->getInstanceHandle(m_RepID)->setMatrix(m_AbsoluteMatrix);
	}
}	

