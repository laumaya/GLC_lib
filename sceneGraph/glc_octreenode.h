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

#include "../glc_config.h"
#include "../maths/glc_vector4d.h"

#ifndef GLC_OCTREENODE_H_
#define GLC_OCTREENODE_H_

class GLC_OctreeNode;

//////////////////////////////////////////////////////////////////////
//! \class GLC_OctreeNode
/*! \brief GLC_OctreeNode : A node of Space partioning implementated with octree */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_OctreeNode
{
	typedef QList<GLC_OctreeNode*> NodeList;
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_OctreeNode(GLC_OctreeNode* pParent= NULL);

	//! Destructor
	virtual ~GLC_OctreeNode();
//@}
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Octree node center position
	GLC_Vector4d m_Center;

	//! Parent Octree node
	GLC_OctreeNode* m_Parent;

	//! Child Octree node
	NodeList m_Childs;

	//! Set of 3DViewInstance
	QSet<GLC_3DViewInstance*> m_3DViewInstanceSet;

};

#endif /* GLC_OCTREENODE_H_ */
