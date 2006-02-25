/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.5, packaged on February, 2006.

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

//! \file GLC_Object.cpp Implementation of the GLC_Object class.

#include "glc_object.h"
using namespace std;


//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

GLC_Object::GLC_Object(const char *pName)
: m_Uid(GLC_GenID())	// Object ID
, m_Name(pName)			// Object Name
{

}

GLC_Object::~GLC_Object()
{

}


//////////////////////////////////////////////////////////////////////
// Get function
//////////////////////////////////////////////////////////////////////

// Identifiant de l'objet
GLC_uint GLC_Object::GetID() const
{
	return m_Uid;
}

// Nom de l'objet
const char* GLC_Object::GetName() const
{
	return m_Name.c_str();
}

//////////////////////////////////////////////////////////////////////
// SET function
//////////////////////////////////////////////////////////////////////

// Nom de l'objet
void GLC_Object::SetName(const char *SetName)
{
	m_Name.assign(SetName);
}

