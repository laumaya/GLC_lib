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

//! \file glc_structinstance.cpp implementation of the GLC_StructInstance class.

#include "glc_structinstance.h"
#include "glc_structreference.h"
#include "glc_structoccurence.h"

// Default constructor
GLC_StructInstance::GLC_StructInstance(GLC_StructReference* pStructReference)
: m_pNumberOfInstance(NULL)
, m_pStructReference(pStructReference)
, m_ListOfOccurences()
, m_RelativeMatrix()
, m_Name()
{
	if (pStructReference->haveStructInstance())
	{
		m_pNumberOfInstance= pStructReference->firstInstanceHandle()->m_pNumberOfInstance;
		++(*m_pNumberOfInstance);
	}
	else
	{
		m_pNumberOfInstance= new int(1);
	}
	// Inform reference that an instance has been created
	m_pStructReference->structInstanceCreated(this);
	//qDebug() << "GLC_StructInstance::GLC_StructInstance : " << (*m_pNumberOfInstance) << " " << m_pNumberOfInstance;
}

// Create empty instance
GLC_StructInstance::GLC_StructInstance(const QString& name)
: m_pNumberOfInstance(NULL)
, m_pStructReference(NULL)
, m_ListOfOccurences()
, m_RelativeMatrix()
, m_Name(name)
{
}

// Set the reference of an empty instance
void GLC_StructInstance::setReference(GLC_StructReference* pStructReference)
{
	Q_ASSERT(NULL == m_pStructReference);
	m_pStructReference= pStructReference;
	if (m_pStructReference->haveStructInstance())
	{
		m_pNumberOfInstance= m_pStructReference->firstInstanceHandle()->m_pNumberOfInstance;
		++(*m_pNumberOfInstance);
	}
	else
	{
		m_pNumberOfInstance= new int(1);
	}
	// Inform reference that an instance has been created
	m_pStructReference->structInstanceCreated(this);
	//qDebug() << "GLC_StructInstance::GLC_StructInstance : " << (*m_pNumberOfInstance) << " " << m_pNumberOfInstance;
}

// Destructor
GLC_StructInstance::~GLC_StructInstance()
{
	Q_ASSERT(m_pNumberOfInstance != NULL);

	// Inform reference that an insatnce has been deleted
	m_pStructReference->structInstanceDeleted(this);
	//qDebug() << "GLC_StructInstance::~GLC_StructInstance : " << (*m_pNumberOfInstance) - 1 << " " << m_pNumberOfInstance;
	if ((--(*m_pNumberOfInstance)) == 0)
	{
		delete m_pStructReference;
		delete m_pNumberOfInstance;
	}
}
