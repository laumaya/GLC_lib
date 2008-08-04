/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.9, packaged on May, 2008.

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

//! \file glc_node.cpp implementation of the GLC_Node class.

#include "glc_node.h"
#include "glc_world.h"

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////

GLC_Node::GLC_Node(GLC_Collection *pCollection)
: m_Uid(glc::GLC_GenID())
, m_pCollection(pCollection)
, m_pParent(NULL)
, m_Ref()
, m_RelativeMatrix()
, m_AbsoluteMatrix()
{
}
// Copy constructor
GLC_Node::GLC_Node(const GLC_Node& node)
: m_Uid(node.m_Uid)
, m_pCollection(node.m_pCollection)
, m_pParent(node.m_pParent)
, m_Ref(node.m_Ref)
, m_RelativeMatrix(node.m_RelativeMatrix)
, m_AbsoluteMatrix(node.m_AbsoluteMatrix)
{
}

GLC_Node::~GLC_Node()
{
}

// Set the parent of this node
void GLC_Node::setParent(GLC_Node* pNode)
{
	m_pParent= pNode;
	updateAbsoluteMatrix();
}

// Assignement operator
GLC_Node& GLC_Node::operator=(const GLC_Node& node)
{
	m_Uid= node.m_Uid;
	m_pCollection= node.m_pCollection;
	m_pParent= node.m_pParent;
	m_Ref= node.m_Ref;
	m_RelativeMatrix= node.m_RelativeMatrix;
	m_AbsoluteMatrix= node.m_AbsoluteMatrix;
	return *this;	
}







