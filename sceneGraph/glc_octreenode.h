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

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Add children to the node with the specified level
	void addChildren(int);

	//! Clear instances set
	void clearInstanceOfNode();

	//! Add instance
	void addInstance(GLC_3DViewInstance*);

	//! Update instances visibility
	void updateInstancesVisibility();
//@}

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

};

#endif /* GLC_OCTREENODE_H_ */
