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

//! \file glc_world.cpp implementation of the GLC_World class.

#include "glc_world.h"
#include "glc_product.h"
#include "glc_structinstance.h"
#include "glc_structreference.h"

// Default constructor
GLC_World::GLC_World()
: m_pCollection(new GLC_Collection())
, m_pRoot(new GLC_StructOccurence(m_pCollection, (new GLC_StructReference())->createStructInstance()))
, m_pNumberOfWorld(new int(1))
{
	//qDebug() << "GLC_World::GLC_World() : " << (*m_pNumberOfWorld) << " " << this;
}

// Copy constructor
GLC_World::GLC_World(const GLC_World& world)
: m_pCollection(world.m_pCollection)
, m_pRoot(world.m_pRoot)
, m_pNumberOfWorld(world.m_pNumberOfWorld)
{
	//qDebug() << "GLC_World::GLC_World() : " << (*m_pNumberOfWorld) << " " << this;
	// Increment the number of world
	++(*m_pNumberOfWorld);
}

GLC_World::~GLC_World()
{

	// Decrement the number of world
	--(*m_pNumberOfWorld);
	//qDebug() << "GLC_World::GLC_World() : " << (*m_pNumberOfWorld) << " " << this;
	if ((*m_pNumberOfWorld) == 0)
	{
		// this is the last World, delete the root product and collection
		m_pCollection->clear(); // Clear collection first (performance)
		delete m_pRoot;
		delete m_pCollection;
		delete m_pNumberOfWorld;
	}
}

// Merge this world with another world
void GLC_World::mergeWithAnotherWorld(GLC_World& anotherWorld)
{
	GLC_StructOccurence* pAnotherRoot= anotherWorld.rootOccurence();
	if (pAnotherRoot->childCount() > 0)
	{
		QList<GLC_StructOccurence*> childs= pAnotherRoot->children();
		const int size= childs.size();
		for (int i= 0; i < size; ++i)
		{
			m_pRoot->addChild(childs.at(i)->clone(m_pCollection));
		}
		m_pRoot->updateChildsAbsoluteMatrix();
	}
}

// Assignment operator
GLC_World& GLC_World::operator=(const GLC_World& world)
{
	// Decrement the number of world
	--(*m_pNumberOfWorld);
	if ((*m_pNumberOfWorld) == 0)
	{
		// this is the last World, delete the root product and collection
		m_pCollection->clear(); // Clear collection first (performance)
		delete m_pRoot;
		delete m_pCollection;
		delete m_pNumberOfWorld;
	}
	m_pRoot= world.m_pRoot;
	m_pCollection= world.m_pCollection;
	m_pNumberOfWorld= world.m_pNumberOfWorld;
	++(*m_pNumberOfWorld);
	return *this;
}
