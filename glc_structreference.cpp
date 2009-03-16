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

//! \file glc_structreference.cpp implementation of the GLC_StructReference class.

#include "glc_structreference.h"

// Default constructor
GLC_StructReference::GLC_StructReference()
: m_ListOfInstance()
, m_pRepresentation(NULL)
{


}

// Create reference with representation
GLC_StructReference::GLC_StructReference(const GLC_Instance& rep)
: m_ListOfInstance()
, m_pRepresentation(new GLC_Instance(rep))
{

}

GLC_StructReference::~GLC_StructReference()
{
	delete m_pRepresentation;
}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Create a Struct instance of this reference
GLC_StructInstance* GLC_StructReference::createStructInstance()
{
	GLC_StructInstance* pInstance= new GLC_StructInstance(this);
	m_ListOfInstance.append(pInstance);

	return pInstance;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

