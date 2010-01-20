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
//! \file glc_octreenode.cpp implementation for the GLC_OctreeNode class.

#include "glc_octreenode.h"

// Default constructor
GLC_OctreeNode::GLC_OctreeNode(const GLC_BoundingBox& boundingBox, GLC_OctreeNode* pParent)
: m_BoundingBox(boundingBox)
, m_pParent(pParent)
, m_Children()
, m_3DViewInstanceSet()
, m_Empty(true)
{


}

// Copy constructor
GLC_OctreeNode::GLC_OctreeNode(const GLC_OctreeNode& octreeNode, GLC_OctreeNode* pParent)
: m_BoundingBox(octreeNode.m_BoundingBox)
, m_pParent(pParent)
, m_Children()
, m_3DViewInstanceSet(octreeNode.m_3DViewInstanceSet)
, m_Empty(octreeNode.m_Empty)
{
	if (!octreeNode.m_Children.isEmpty())
	{
		const int size= octreeNode.m_Children.size();
		for (int i= 0; i < size; ++i)
		{
			m_Children.append(new GLC_OctreeNode(*(octreeNode.m_Children.at(i)), this));
		}
	}
}

// Destructor
GLC_OctreeNode::~GLC_OctreeNode()
{
	const int size= m_Children.size();
	for (int i= 0; i < size; ++i)
	{
		delete m_Children.at(i);
	}
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Add children to the node with the specified level
void GLC_OctreeNode::addChildren()
{
	Q_ASSERT(m_Children.isEmpty());
	Q_ASSERT(!m_BoundingBox.isEmpty());

	const double xLower=  m_BoundingBox.lowerCorner().X();
	const double yLower=  m_BoundingBox.lowerCorner().Y();
	const double zLower=  m_BoundingBox.lowerCorner().Z();

	const double xUpper=  m_BoundingBox.upperCorner().X();
	const double dX= (xUpper - xLower) / 2.0;
	const double yUpper=  m_BoundingBox.upperCorner().Y();
	const double dY= (yUpper - yLower) / 2.0;
	const double zUpper=  m_BoundingBox.upperCorner().Z();
	const double dZ= (zUpper - zLower) / 2.0;


	// Add 8 Children
	GLC_Point4d lower;
	GLC_Point4d upper;
	GLC_OctreeNode* pOctreeNode= NULL;

	{ // Child 1
		lower.setVect(xLower, yLower, zLower);
		upper.setVect(xLower + dX, yLower + dY, zLower + dZ);
		GLC_BoundingBox box(lower, upper);
		pOctreeNode= new GLC_OctreeNode(box, this);
		m_Children.append(pOctreeNode);
	}
	{ // Child 2
		lower.setVect(xLower + dX, yLower, zLower);
		upper.setVect(xUpper, yLower + dY, zLower + dZ);
		GLC_BoundingBox box(lower, upper);
		pOctreeNode= new GLC_OctreeNode(box, this);
		m_Children.append(pOctreeNode);
	}
	{ // Child 3
		lower.setVect(xLower + dX, yLower + dY, zLower);
		upper.setVect(xUpper, yUpper, zLower + dZ);
		GLC_BoundingBox box(lower, upper);
		pOctreeNode= new GLC_OctreeNode(box, this);
		m_Children.append(pOctreeNode);
	}
	{ // Child 4
		lower.setVect(xLower, yLower + dY, zLower);
		upper.setVect(xLower + dX, yUpper, zLower + dZ);
		GLC_BoundingBox box(lower, upper);
		pOctreeNode= new GLC_OctreeNode(box, this);
		m_Children.append(pOctreeNode);
	}
	{ // Child 5
		lower.setVect(xLower, yLower, zLower + dZ);
		upper.setVect(xLower + dX, yLower + dY, zUpper);
		GLC_BoundingBox box(lower, upper);
		pOctreeNode= new GLC_OctreeNode(box, this);
		m_Children.append(pOctreeNode);
	}
	{ // Child 6
		lower.setVect(xLower + dX, yLower, zLower + dZ);
		upper.setVect(xUpper, yLower + dY, zUpper);
		GLC_BoundingBox box(lower, upper);
		pOctreeNode= new GLC_OctreeNode(box, this);
		m_Children.append(pOctreeNode);
	}
	{ // Child 7
		lower.setVect(xLower + dX, yLower + dY, zLower + dZ);
		upper.setVect(xUpper, yUpper, zUpper);
		GLC_BoundingBox box(lower, upper);
		pOctreeNode= new GLC_OctreeNode(box, this);
		m_Children.append(pOctreeNode);
	}
	{ // Child 8
		lower.setVect(xLower, yLower + dY, zLower + dZ);
		upper.setVect(xLower + dX, yUpper, zUpper);
		GLC_BoundingBox box(lower, upper);
		pOctreeNode= new GLC_OctreeNode(box, this);
		m_Children.append(pOctreeNode);
	}
}

// Add instance
void GLC_OctreeNode::addInstance(GLC_3DViewInstance* pInstance, int depth)
{
	m_Empty= false;
	const GLC_BoundingBox instanceBox= pInstance->boundingBox();
	// Check if the instance's bounding box intersect this node bounding box
	if (intersect(instanceBox))
	{
		if (0 == depth)
		{
			m_3DViewInstanceSet.insert(pInstance);
		}
		else
		{
			if (m_Children.isEmpty())
			{
				// Create children
				addChildren();
			}
			QVector<bool> childIntersect(8);
			bool allIntersect= true;
			bool currentIntersect= false;
			for (int i= 0; i < 8; ++i)
			{
				currentIntersect= m_Children.at(i)->intersect(instanceBox);
				allIntersect= allIntersect && currentIntersect;
				childIntersect[i]= currentIntersect;
			}
			if (allIntersect)
			{
				m_3DViewInstanceSet.insert(pInstance);
			}
			else
			{
				for (int i= 0; i < 8; ++i)
				{
					if (childIntersect[i])
					{
						m_Children[i]->addInstance(pInstance, depth - 1);
					}
				}
			}
		}

	}
}

// Remove empty node
void GLC_OctreeNode::removeEmptyChildren()
{
	NodeList::iterator iList= m_Children.begin();
	while(m_Children.constEnd() != iList)
	{
		GLC_OctreeNode* pCurrentChild= *iList;
		if (pCurrentChild->isEmpty())
		{
			delete pCurrentChild;
			iList= m_Children.erase(iList);
		}
		else
		{
			pCurrentChild->removeEmptyChildren();
			++iList;
		}

	}

}

