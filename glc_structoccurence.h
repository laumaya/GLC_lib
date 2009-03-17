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

//! \file glc_structoccurence.h interface for the GLC_StructOccurence class.

#ifndef GLC_STRUCTOCCURENCE_H_
#define GLC_STRUCTOCCURENCE_H_

#include "glc_object.h"
#include "glc_matrix4x4.h"
#include <QSet>

class GLC_Collection;
class GLC_StructInstance;
class GLC_Material;

class GLC_StructOccurence : public GLC_Object
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
	//! Default constructor
	GLC_StructOccurence(GLC_Collection*, GLC_StructInstance*, GLuint shaderId=0);

	//! Copy constructor
	GLC_StructOccurence(GLC_Collection*, const GLC_StructOccurence&);

	//! Destructor
	virtual ~GLC_StructOccurence();
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the absolute matrix
	inline GLC_Matrix4x4 absoluteMatrix() const
	{ return m_AbsoluteMatrix;}

	//! Return true if this occurence is orphan
	inline bool isOrphan() const
	{ return NULL == m_pParent;}

	//! Return true if this occurence have a representation
	inline bool haveRepresentation() const
	{ return m_HaveRepresentation;}

	//! Return the instance of this occurence
	inline GLC_StructInstance* structInstance() const
	{ return m_pStructInstance;}

	//! Return the number of childs
	inline int childCount() const
	{ return m_Childs.size();}

	//! Return a child
	/*! The index must exist*/
	inline GLC_StructOccurence* child(const int index) const
	{return m_Childs.at(index);}

	//! Return the list of children
	inline QList<GLC_StructOccurence*> children() const
	{ return m_Childs;}

	//! Get number of faces
	unsigned int numberOfFaces() const;

	//! Get number of vertex
	unsigned int numberOfVertex() const;

	//! Get number of materials
	unsigned int numberOfMaterials() const;

	//! Get materials List
	QSet<GLC_Material*> materialSet() const;

	//! Clone the occurence
	GLC_StructOccurence* clone(GLC_Collection*) const;

	//! Return true if the occurence is visible
	bool isVisible() const;


//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Update the absolute matrix
	void updateAbsoluteMatrix();

	//! Add Child
	/*! The new child must be orphan*/
	void addChild(GLC_StructOccurence*);

	//! Add Child instance (the occurence is created)
	void addChild(GLC_StructInstance*);

	//! Add Children
	/*! the new children must be orphan*/
	void addChildren(const QList<GLC_StructOccurence*>&);

	//! make the occurence orphan
	void makeOrphan();

	//! Remove the specified child
	/*! The removed child will not be deleted*/
	inline bool removeChild(GLC_StructOccurence* pChild)
	{
		pChild->m_pParent= NULL;
		return m_Childs.removeOne(pChild);
	}

	//! Reverse Normals of this Occurence and childs
	void reverseNormals();

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! the occurence's collection
	GLC_Collection* m_pCollection;

	//! Number of this Occurence
	int* m_pNumberOfOccurence;

	//! The struct instance of this occurence
	GLC_StructInstance* m_pStructInstance;

	//! The parent of this occurence
	GLC_StructOccurence* m_pParent;

	//! The Child of this occurence
	QList<GLC_StructOccurence*> m_Childs;

	//! The absolute matrix of the occurence
	GLC_Matrix4x4 m_AbsoluteMatrix;

	//! true if occurence have a representation
	bool m_HaveRepresentation;

};

#endif /* GLC_STRUCTOCCURENCE_H_ */
