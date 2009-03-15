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

//! \file glc_structoccurence.cpp implementation of the GLC_StructOccurence class.

#include "glc_structoccurence.h"
#include "glc_collection.h"
#include "glc_structinstance.h"

// Default constructor
GLC_StructOccurence::GLC_StructOccurence(GLC_Collection* pCollection, GLC_StructInstance* pStructInstance)
: GLC_Object()
, m_pCollection(pCollection)
, m_pNumberOfOccurence(NULL)
, m_pStructInstance(pStructInstance)
, m_pParent(NULL)
, m_Childs()
, m_AbsoluteMatrix()
, m_HaveRepresentation(false)
{
	if (pStructInstance->haveStructOccurence())
	{
		m_pNumberOfOccurence= pStructInstance->firstOccurenceHandle()->m_pNumberOfOccurence;
		++(*m_pNumberOfOccurence);
	}
	else
	{
		m_pNumberOfOccurence= new int(1);
	}
}

// Destructor
GLC_StructOccurence::~GLC_StructOccurence()
{
	Q_ASSERT(m_pNumberOfOccurence != NULL);

	if ((--(*m_pNumberOfOccurence)) == 0)
	{
		delete m_pStructInstance;
		delete m_pNumberOfOccurence;
	}
}

//////////////////////////////////////////////////////////////////////
// name Set Functions
//////////////////////////////////////////////////////////////////////

// Update the absolute matrix
void GLC_StructOccurence::updateAbsoluteMatrix()
{
	if (NULL != m_pParent)
	{
		m_AbsoluteMatrix= m_pParent->absoluteMatrix() * m_pStructInstance->relativeMatrix();
	}
	else
	{
		m_AbsoluteMatrix= m_pStructInstance->relativeMatrix();
	}
	// If the occurence have a representation, update it.
	if (m_HaveRepresentation)
	{
		m_pCollection->getInstanceHandle(id())->setMatrix(m_AbsoluteMatrix);
	}
}

// Add Child
void GLC_StructOccurence::addChild(GLC_StructOccurence* pChild)
{
	Q_ASSERT(pChild->isOrphan());

	// Add the child to the list of child
	m_Childs.append(pChild);

}
// make the occurence orphan
void GLC_StructOccurence::makeOrphan()
{
	Q_ASSERT(not isOrphan());
	m_pParent->removeChild(this);
	m_pParent= NULL;
}
