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

//! \file glc_node.h interface for the GLC_Node class.

#ifndef GLC_NODE_H_
#define GLC_NODE_H_

#include "glc_matrix4x4.h"
#include "glc_enum.h"

#include <QString>

class GLC_World;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Node
/*! \brief GLC_Node : Base abstarct class of Product and part of the Product Structure*/

/*! An GLC_Node is a node from the scene graph
 * */
//////////////////////////////////////////////////////////////////////
class GLC_Node
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Constructor of the node
	GLC_Node(GLC_World *);
	//! Destructor of the node
	virtual ~GLC_Node();	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return node ID
	inline GLC_uint getID() const {return m_Uid;}
	
	//! Return the relative matrix of the node
	inline GLC_Matrix4x4 relativeMatrix() const {return m_RelativeMatrix;}
	
	//! Return the absolute matrix of the node
	inline GLC_Matrix4x4 absoluteMatrix() const {return m_AbsoluteMatrix;}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set the father of this node
	void setFather(GLC_Node* );
	
	//! Update Node absolute matrix
	virtual void updateAbsoluteMatrix()= 0;	
//@}
//////////////////////////////////////////////////////////////////////
// private members
//////////////////////////////////////////////////////////////////////
private:
	GLC_uint m_Uid;
	
//////////////////////////////////////////////////////////////////////
// protected members
//////////////////////////////////////////////////////////////////////
protected:
	//! the node's world
	GLC_World* m_pWorld;
	//! The father of this node
	GLC_Node* m_pFather;	
	//! Node reference
	QString m_Ref;
	//! Node relative Matrix
	GLC_Matrix4x4 m_RelativeMatrix;
	//! Node Absolute Matrix
	GLC_Matrix4x4 m_AbsoluteMatrix;
};

#endif /*GLC_NODE_H_*/
