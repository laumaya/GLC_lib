/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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
//! \file glc_octree.cpp implementation for the GLC_Octree class.

#include "glc_octree.h"
#include "glc_octreenode.h"
#include "glc_3dviewcollection.h"

// Default constructor
GLC_Octree::GLC_Octree(GLC_3DViewCollection* pCollection)
: GLC_SpacePartitioning(pCollection)
, m_pRootNode(NULL)
, m_OctreeDepth(4)
{


}

// Copy constructor
GLC_Octree::GLC_Octree(const GLC_Octree& octree)
: GLC_SpacePartitioning(octree)
, m_pRootNode(NULL)
, m_OctreeDepth(octree.m_OctreeDepth)
{
	if (NULL != octree.m_pRootNode)
	{
		m_pRootNode= new GLC_OctreeNode(*(octree.m_pRootNode));
	}
}

// Destructor
GLC_Octree::~GLC_Octree()
{
	delete m_pRootNode;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Update QHash table of visible GLC_3DViewInstance
void GLC_Octree::updateViewableInstances()
{

}

// Update the space partionning
void GLC_Octree::updateSpacePartitioning()
{
	qDebug() << "Create octree";
	delete m_pRootNode;
	m_pRootNode= new GLC_OctreeNode(m_pCollection->boundingBox());
	m_pRootNode->addChildren(m_OctreeDepth);
	qDebug() << "Octree created";

	qDebug() << "fill Octree";
	// fill the octree
	QList<GLC_3DViewInstance*> instanceList(m_pCollection->instancesHandle());
	const int size= instanceList.size();
	qDebug() << "Number of instances :" << size;
	for (int i= 0; i < size; ++i)
	{
		m_pRootNode->addInstance(instanceList.at(i));
	}
	qDebug() << "Octree filled";
}

