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

//! \file glc_world.cpp implementation of the GLC_World class.

#include "glc_world.h"
#include "glc_product.h"

// Default constructor
GLC_World::GLC_World()
: m_pRoot(new GLC_Product(this))
, m_pCollection( new GLC_Collection())
{
}

// Copy constructor
GLC_World::GLC_World(const GLC_World& world)
: m_pRoot(new GLC_Product(this))
, m_pCollection( new GLC_Collection())
{
	mergeWithAnotherWorld(world);
}

GLC_World::~GLC_World()
{
	delete m_pRoot;
	delete m_pCollection;
}

// Merge this world with another world
void GLC_World::mergeWithAnotherWorld(const GLC_World& anotherWorld)
{
	GLC_Product* pAnotherRoot= anotherWorld.rootProduct();
	if (pAnotherRoot->productChildCount() > 0)
	{
		m_pRoot->addChildProducts(pAnotherRoot->childProducts(), this);
	}
	if (pAnotherRoot->partChildCount() > 0)
	{
		m_pRoot->addChildParts(pAnotherRoot->childParts(), this);
	}
	
}

// Assignement operator
GLC_World& GLC_World::operator=(const GLC_World& world)
{
	// Remove Childs from the root product
	m_pRoot->removeChilds();
	mergeWithAnotherWorld(world);
	return *this;
}
