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

//! \file glc_part.h interface for the GLC_Part class.

#ifndef GLC_PART_H_
#define GLC_PART_H_

#include "glc_node.h"
#include "glc_instance.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Part
/*! \brief GLC_Part : Part of the Product Structure*/

/*! An GLC_Part is a part Node in the product structure \n
 * An GLC_Part agregate a GLC_Instance.
 * */
class GLC_Part : public GLC_Node
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Constructor
	GLC_Part(GLC_Collection *, GLC_Instance&);
	//! Destructor
	virtual ~GLC_Part();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! return the part's representation ID
	inline GLC_uint getRepID() const {return m_RepID;}
	//! Clone the part
	GLC_Part* clone(GLC_Collection *) const;
	//! Get number of Faces
	int getNumberOfFaces() const;
	//! Get number of vertex
	int getNumberOfVertex() const;
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Move the node with specified matrix
	void move(const GLC_Matrix4x4 &);
	
	//! Update Part absolute matrix
	void updateAbsoluteMatrix();
	
	//! Reverse representation normal if representation is a mesh
	void reverseMeshNormal();
	
//@}

//////////////////////////////////////////////////////////////////////
// private members
//////////////////////////////////////////////////////////////////////
private:
	//! Représentation id of the part
	GLC_uint m_RepID;
	
};

#endif /*GLC_PART_H_*/
