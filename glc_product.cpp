/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

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

GLC_Product::GLC_Product(GLC_Collection *pCollection)
: GLC_Node(pCollection, glc::GLC_GenID())
, m_ChildProducts()
, m_ChildParts()
{
}

// Copy constructor
GLC_Product::GLC_Product(const GLC_Product& product)
: GLC_Node(product)
, m_ChildProducts()
, m_ChildParts()
{
	addChildProducts(product.childProducts(), m_pCollection);
	addChildParts(product.childParts(), m_pCollection);
}

GLC_Product::~GLC_Product()
{
	removeChilds();
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// return true if product is parent of node
bool GLC_Product::isParentOf(const GLC_uint key) const
{
	return m_ChildProducts.contains(key) || m_ChildParts.contains(key);
}

// Clone the product
GLC_Product* GLC_Product::clone(GLC_Collection * pCollection) const
{
	GLC_Product* pResult= new GLC_Product(pCollection);
	pResult->setReference(this->m_Ref);
	pResult->m_RelativeMatrix= m_RelativeMatrix;
	pResult->m_AbsoluteMatrix= m_AbsoluteMatrix;
	pResult->addChildProducts(this->childProducts(), pCollection);
	pResult->addChildParts(this->childParts(), pCollection);
	return pResult;
}

// Get number of faces
unsigned int GLC_Product::numberOfFaces() const
{
	unsigned int number= 0;
	// Get the number of faces of child product
	QHashIterator<GLC_uint, GLC_Product*> productIterator(m_ChildProducts);
 	while (productIterator.hasNext())
 	{
     	productIterator.next();
     	number+= productIterator.value()->numberOfFaces();
 	}
	// Get the number of faces of child parts
 	QHashIterator<GLC_uint, GLC_Part*> partIterator(m_ChildParts);
 	while (partIterator.hasNext())
 	{
     	partIterator.next();
     	number+= partIterator.value()->numberOfFaces();
 	}

 	return number;
}
// Get number of vertex
unsigned int GLC_Product::numberOfVertex() const
{
	unsigned int number= 0;
	// Get the number of vertex of child product
	QHashIterator<GLC_uint, GLC_Product*> productIterator(m_ChildProducts);
 	while (productIterator.hasNext())
 	{
     	productIterator.next();
     	number+= productIterator.value()->numberOfVertex();
 	}
	// Get the number of vertex of child parts
 	QHashIterator<GLC_uint, GLC_Part*> partIterator(m_ChildParts);
 	while (partIterator.hasNext())
 	{
     	partIterator.next();
     	number+= partIterator.value()->numberOfVertex();
 	}

 	return number;

}

// Get number of materials
unsigned int GLC_Product::numberOfMaterials() const
{
	return materialSet().size();
}

// Get materials List
QSet<GLC_Material*> GLC_Product::materialSet() const
{
	QSet<GLC_Material*> result;
	QHashIterator<GLC_uint, GLC_Product*> productIterator(m_ChildProducts);
 	while (productIterator.hasNext())
 	{
     	productIterator.next();
     	result.unite(productIterator.value()->materialSet());
 	}
	// Get the number of vertex of child parts
 	QHashIterator<GLC_uint, GLC_Part*> partIterator(m_ChildParts);
 	while (partIterator.hasNext())
 	{
     	partIterator.next();
     	result.unite(partIterator.value()->materialSet());
 	}

 	return result;
}

// Return true if all child of the product are hidden
bool GLC_Product::isHidden() const
{
	bool result= true;
	{
		QList<GLC_Product*> childProductsList(childProducts());
		const int size= childProductsList.size();
		int i= 0;
		while (result and (i < size))
		{
			result= result and childProductsList[i]->isHidden();
			++i;
		}
	}
	if (result)
	{
		QList<GLC_Part*> childPartsList(childParts());
		const int size= childPartsList.size();
		int i= 0;
		while (result and (i < size))
		{
			result= result and childPartsList[i]->isHidden();
			++i;
		}
	}
	return result;
}


// Return the child product associated with the key
GLC_Product* GLC_Product::childProduct(const GLC_uint key)
{
	if (m_ChildProducts.contains(key))
	{
		return m_ChildProducts[key];
	}
	else
	{
		return NULL;
	}
}
// Return the child part associated with the key
GLC_Part* GLC_Product::childPart(const GLC_uint key)
{
	if (m_ChildParts.contains(key))
	{
		return m_ChildParts[key];
	}
	else
	{
		return NULL;
	}
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

// Add a new child product
GLC_Product* GLC_Product::addNewChildProduct()
{
	GLC_Product* pChild= new GLC_Product(m_pCollection);
	m_ChildProducts.insert(pChild->id(), pChild);
	pChild->setParent(this);
	return pChild;
}

// Add child products
void GLC_Product::addChildProducts(QList<GLC_Product*> products, GLC_Collection* pCollection)
{
	const int max= products.size();
	for (int i= 0; i < max; ++i)
	{
		GLC_Product* pProduct= products[i]->clone(pCollection);
		m_ChildProducts.insert(pProduct->id(), pProduct);
		pProduct->setParent(this);
	}
}

// Add child part containing specified instance
 GLC_Part* GLC_Product::addChildPart(GLC_Instance& instance)
 {
 	GLC_Part* pChild= new GLC_Part(m_pCollection, instance);
	m_ChildParts.insert(pChild->id(), pChild);
	pChild->setParent(this);
	return pChild;
 }

 // Add child parts
void GLC_Product::addChildParts(QList<GLC_Part*> parts, GLC_Collection* pCollection)
{
	const int max= parts.size();
	for (int i= 0; i < max; ++i)
	{
		GLC_Part* pPart= parts[i]->clone(pCollection);
		m_ChildParts.insert(pPart->id(), pPart);
		pPart->setParent(this);
	}
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
	if (NULL != m_pParent)
	{
		m_AbsoluteMatrix= m_pParent->absoluteMatrix() * m_RelativeMatrix;
	}
}

// Clear child part and child product
void GLC_Product::removeChilds()
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

// Reverse child part normal
void GLC_Product::reverseChildPartNormal()
{
	// Reverse child part normal of child product
	QHashIterator<GLC_uint, GLC_Product*> productIterator(m_ChildProducts);
 	while (productIterator.hasNext())
 	{
     	productIterator.next();
     	productIterator.value()->reverseChildPartNormal();
 	}
	// Reverse child part normal
 	QHashIterator<GLC_uint, GLC_Part*> partIterator(m_ChildParts);
 	while (partIterator.hasNext())
 	{
     	partIterator.next();
     	partIterator.value()->reverseMeshNormal();
 	}

}

// Assignement operator
GLC_Product& GLC_Product::operator=(const GLC_Product& product)
{
	removeChilds();
	GLC_Node::operator=(product);
	addChildProducts(product.childProducts(), m_pCollection);
	addChildParts(product.childParts(), m_pCollection);
	return *this;
}
