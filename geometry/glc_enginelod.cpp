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

//! \file glc_enginelod.cpp implementation of the GLC_EngineLod class.


#include "glc_enginelod.h"

// Default constructor
GLC_EngineLod::GLC_EngineLod(double accuracy)
: m_Accuracy(accuracy)
, m_IboId(0)
, m_IboVector()
{


}

// Copy Constructor
GLC_EngineLod::GLC_EngineLod(const GLC_EngineLod& lod)
: m_Accuracy(lod.m_Accuracy)
, m_IboId(0)
, m_IboVector(lod.m_IboVector)
{


}

GLC_EngineLod::~GLC_EngineLod()
{
	// Delete IBO
	if (0 != m_IboId)
	{
		glDeleteBuffers(1, &m_IboId);
	}

}
