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

//! \file glc_node.h interface for the GLC_Node class.

#ifndef GLC_NODE_H_
#define GLC_NODE_H_

#include "glc_matrix4x4.h"
#include "glc_enum.h"

#include <QString>

class GLC_Collection;

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
	GLC_Node(GLC_Collection *, GLC_uint);

	//! Copy Constructor
	GLC_Node(const GLC_Node&);

	//! Destructor of the node
	virtual ~GLC_Node();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return node ID
	inline GLC_uint id() const {return m_Uid;}

	//! Return the relative matrix of the node
	inline GLC_Matrix4x4 relativeMatrix() const {return m_RelativeMatrix;}

	//! Return the absolute matrix of the node
	inline GLC_Matrix4x4 absoluteMatrix() const {return m_AbsoluteMatrix;}

	//! Return the parent
	inline GLC_Node* parent() {return m_pParent;}

	//! Return node name
	inline QString name() const {return m_Ref;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set the parent of this node
	void setParent(GLC_Node* );

	//! Update Node absolute matrix
	virtual void updateAbsoluteMatrix()= 0;

	//! Set the reference of this node
	inline void setReference(const QString& ref) {m_Ref= ref;}
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Operator Overload */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Assignement operator
	GLC_Node& operator=(const GLC_Node&);
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
	//! the node's collection
	GLC_Collection* m_pCollection;

	//! The parent of this node
	GLC_Node* m_pParent;

	//! Node reference
	QString m_Ref;

	//! Node relative Matrix
	GLC_Matrix4x4 m_RelativeMatrix;

	//! Node Absolute Matrix
	GLC_Matrix4x4 m_AbsoluteMatrix;
};

#endif /*GLC_NODE_H_*/
