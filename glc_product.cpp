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

//! \file glc_product.cpp implementation of the GLC_Product class.

#include "glc_product.h"
#include "glc_part.h"

GLC_Product::GLC_Product(GLC_World *pWorld)
: GLC_Node(pWorld)
{
}

GLC_Product::~GLC_Product()
{
	// Delete Child product
 	QHashIterator<GLC_uint, GLC_Product*> productIterator(m_ChildProducts);
 	while (productIterator.hasNext())
 	{
     	productIterator.next();
     	delete productIterator.value();
 	}
 	m_ChildProducts.clear();
 	
	// Delete Child part
 	QHashIterator<GLC_uint, GLC_Part*> partIterator(m_ChildParts);
 	while (partIterator.hasNext())
 	{
     	partIterator.next();
     	delete partIterator.value();
 	}
 	m_ChildParts.clear();
	
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Move the Product with specified matrix
void GLC_Product::move(const GLC_Matrix4x4& matrix)
{
	m_RelativeMatrix= matrix * m_RelativeMatrix;
	m_AbsoluteMatrix= matrix * m_AbsoluteMatrix;
	
	// Update Childe product Matrix
 	QHashIterator<GLC_uint, GLC_Product*> productIterator(m_ChildProducts);
 	while (productIterator.hasNext())
 	{
     	productIterator.next();
     	productIterator.value()->updateAbsoluteMatrix();
 	} 
	// Update Childe part Matrix
 	QHashIterator<GLC_uint, GLC_Part*> partIterator(m_ChildParts);
 	while (partIterator.hasNext())
 	{
     	partIterator.next();
     	partIterator.value()->updateAbsoluteMatrix();
 	} 
}

// Add a child product
GLC_Product* GLC_Product::addChildProduct()
{
	GLC_Product* pChild= new GLC_Product(m_pWorld);
	m_ChildProducts.insert(pChild->getID(), pChild);
	pChild->setFather(this);	
	return pChild;
}

// Add child part containing specified instance
 GLC_Part* GLC_Product::addChildPart(GLC_Instance& instance)
 {
 	GLC_Part* pChild= new GLC_Part(m_pWorld, instance);
	m_ChildParts.insert(pChild->getID(), pChild);
	pChild->setFather(this);	
	return pChild; 		
 }
 
 // Remove a child with the specified UID
 bool GLC_Product::removeChild(const GLC_uint id)
 {
 	if (m_ChildProducts.contains(id))
 	{
 		delete m_ChildProducts.take(id);
 		return true;
 	}
 	else if (m_ChildParts.contains(id))
 	{
 		delete m_ChildParts.take(id);
 		return true;
 		
 	}
 	else return false;	
 }
 
// Update Product absolute matrix
void GLC_Product::updateAbsoluteMatrix()
{
	if (NULL != m_pFather)
	{
		m_AbsoluteMatrix= m_pFather->absoluteMatrix() * m_RelativeMatrix;
	} 
}	

