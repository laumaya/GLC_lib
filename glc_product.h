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

//! \file glc_product.h interface for the GLC_Product class.

#ifndef GLC_PRODUCT_H_
#define GLC_PRODUCT_H_

#include "glc_node.h"
#include <QHash>
#include <QList>
#include "glc_instance.h"

class GLC_Part;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Product
/*! \brief GLC_Product : Product of the Product Structure*/

/*! An GLC_Product is a product Node in the product structure \n
 * An GLC_Product agregate another product and/or GLC_Part.
 * */
//////////////////////////////////////////////////////////////////////
class GLC_Product : public GLC_Node
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Constructor
	GLC_Product(GLC_World *);
	//! Destructor
	virtual ~GLC_Product();
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! return true if product is parent of node
	bool isParentOf(const GLC_uint) const;
	//! Return the number of child
	inline int childCount() const {return m_ChildProducts.size() + m_ChildParts.size();}
	//! Return the number of product child
	inline int productChildCount() const {return m_ChildProducts.size();}
	//! Return the number of part child
	inline int partChildCount() const {return m_ChildParts.size();}
	//! Return the child product associated with the key
	GLC_Product* childProduct(const GLC_uint);
	//! Return the productChildList
	inline QList<GLC_Product*> childProducts() const {return m_ChildProducts.values();}
	//! Return the child part associated with the key
	GLC_Part* childPart(const GLC_uint);
	//! Return the partChildList
	inline QList<GLC_Part*> childParts() const {return m_ChildParts.values();}
	//! Clone the product
	GLC_Product* clone(GLC_World *) const;
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Move the Product with specified matrix
	void move(const GLC_Matrix4x4 &);
	
	//! Add a new child product
	/*! Return the added product Uid
	 */
	GLC_Product* addNewChildProduct();

	//! Add child products
	void addChildProducts(QList<GLC_Product*>, GLC_World *);
		
	//! Add child part containing specified instance
	/*! Return the added part Uid
	 */
	 GLC_Part* addChildPart(GLC_Instance&);

	//! Add child parts
	void addChildParts(QList<GLC_Part*>, GLC_World *);
	 
	 //! Remove a child with the specified UID
	 /*! Return true if the child as been succesfully removed
	  */
	 bool removeChild(const GLC_uint);
	 
	//! Update Product absolute matrix
	void updateAbsoluteMatrix();	
	 
//@}

//////////////////////////////////////////////////////////////////////
// private members
//////////////////////////////////////////////////////////////////////
private:
	//! Child products collection
	QHash<GLC_uint, GLC_Product*> m_ChildProducts;
	//! Child parts collection
	QHash<GLC_uint, GLC_Part*> m_ChildParts;
};

#endif /*GLC_PRODUCT_H_*/
