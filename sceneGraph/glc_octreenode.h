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
//! \file glc_octreenode.h interface for the GLC_OctreeNode class.


#ifndef GLC_OCTREENODE_H_
#define GLC_OCTREENODE_H_

#include "glc_3dviewinstance.h"
#include "../glc_boundingbox.h"
#include "../glc_config.h"
#include "../viewport/glc_frustum.h"
#include <QList>
#include <QSet>

class GLC_OctreeNode;

//////////////////////////////////////////////////////////////////////
//! \class GLC_OctreeNode
/*! \brief GLC_OctreeNode : A node of Space partioning implementated with octree */
//////////////////////////////////////////////////////////////////////
class GLC_OctreeNode
{
	typedef QList<GLC_OctreeNode*> NodeList;
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_OctreeNode(const GLC_BoundingBox&, GLC_OctreeNode* pParent= NULL);

	//! Copy constructor
	GLC_OctreeNode(const GLC_OctreeNode&, GLC_OctreeNode* pParent= NULL);

	//! Destructor
	virtual ~GLC_OctreeNode();
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	// Return the node bounding box
	inline GLC_BoundingBox& boundingBox()
	{return m_BoundingBox;}

	//! Return True if the node intersect the bounding box
	inline bool intersect(const GLC_BoundingBox& boundingSphere)
	{return m_BoundingBox.intersectBoundingSphere(boundingSphere);}

	//! Return true if the node has child
	inline bool hasChild() const
	{return !m_Children.isEmpty();}

	//! Return the child at the index
	inline GLC_OctreeNode* childAt(int i) const
	{
		Q_ASSERT(i < m_Children.size());
		return m_Children.at(i);
	}

	//! Return the none child count
	inline int childCount() const
	{return m_Children.size();}

	//! Return true if the node has geometry
	inline bool hasGeometry() const
	{return !m_3DViewInstanceSet.isEmpty();}

	//! Return true if the node is empty
	inline bool isEmpty() const
	{return m_Empty;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Add children to the node with the specified level
	void addChildren();

	//! Add instance
	void addInstance(GLC_3DViewInstance*, int);

	//! Update instances visibility
	void updateViewableInstances(const GLC_Frustum&);

	//! Remove empty node
	void removeEmptyChildren();
//@}

//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////
private:
	//! Set the viewable flag
	void setViewFlag(bool);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Octree node bounding box
	GLC_BoundingBox m_BoundingBox;

	//! Parent Octree node
	GLC_OctreeNode* m_pParent;

	//! Children Octree node
	NodeList m_Children;

	//! Set of 3DViewInstance
	QSet<GLC_3DViewInstance*> m_3DViewInstanceSet;

	//! Flag to know if the node is empty
	bool m_Empty;

};

#endif /* GLC_OCTREENODE_H_ */
