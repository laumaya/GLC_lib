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

//! \file glc_world.h interface for the GLC_World class.

#ifndef GLC_WORLD_H_
#define GLC_WORLD_H_

#include "glc_collection.h"
#include "glc_product.h"
#include "glc_part.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_World
/*! \brief GLC_World : The Root of GLC_Lib Scene Graph*/
/*!
 *  GLC_World contain :
 * 		- The Scene root GLC_Product.
 * 		- a GLC_Collection which manage all scene shapes (GLC_Instance)
 *
 */
//////////////////////////////////////////////////////////////////////
class GLC_World
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_World();

	//! Copy constructor
	GLC_World(const GLC_World&);

	//! Destructor
	virtual ~GLC_World();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the root product of the world
	inline GLC_Product* rootProduct() const {return m_pRoot;}

	//! Return the world collection
	inline GLC_Collection* collection() {return m_pCollection;}

	//! Return true if the world is empty
	inline bool isEmpty() const {return  m_pCollection->isEmpty();}

	//! Return number of faces
	inline int numberOfFaces() const {return m_pRoot->numberOfFaces();}

	//! Return number of vertex
	inline int numberOfVertex() const{return m_pRoot->numberOfVertex();}

	//! Return the number of materials
	inline int numberOfMaterials() const {return m_pRoot->numberOfMaterials();}

	//! Return the list of material
	inline QList<GLC_Material*> listOfMaterials() const {return m_pRoot->materialSet().toList();}

	//! Return list of invisible instance name
	inline QList<QString> getInvisibleInstanceName() const
	{return m_pCollection->getInvisibleInstanceName();}

	//! Return instances name from the specified shading group
	inline QList<QString> getInstanceNameFromShadingGroup(GLuint id) const
	{return m_pCollection->getInstanceNameFromShadingGroup(id);}

	//! Return the number of used shading group
	inline int numberOfUsedShadingGroup() const
	{return m_pCollection->numberOfUsedShadingGroup();}


//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Merge this world with another world
	void mergeWithAnotherWorld(GLC_World &);

	//! Reverse worlds part normal
	inline void reversePartNormal() {m_pRoot->reverseChildPartNormal();}

	//! Clear this world
	GLC_World& clear() {return *this= GLC_World();}
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Operator Overload */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Assignement operator
	GLC_World& operator=(const GLC_World&);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Display the world
	inline void glExecute(GLuint groupId) {m_pCollection->glExecute(groupId);}

	//! Display the world's shader group
	inline void glExecuteShaderGroup() {m_pCollection->glExecuteShaderGroup();}

//@}
//////////////////////////////////////////////////////////////////////
// private members
//////////////////////////////////////////////////////////////////////
private:
	//! The instance Collection
	GLC_Collection* m_pCollection;

	//! The root of the product structure
	GLC_Product* m_pRoot;

	//! Number of this world
	int* m_pNumberOfWorld;
};

#endif /*GLC_WORLD_H_*/
