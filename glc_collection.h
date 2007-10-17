/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.7, packaged on September, 2007.

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

//! \file glc_collection.h interface for the GLC_Collection class.

#ifndef GLC_COLLECTION_H_
#define GLC_COLLECTION_H_


#include <QHash>
#include "glc_instance.h"
#include "glc_enum.h"

class GLC_Material;

//! Geometry hash table
typedef QHash< GLC_uint, GLC_Instance> CNodeMap;
typedef QHash< GLC_uint, GLC_Instance*> SelectedNodeHash;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Collection
/*! \brief GLC_Collection : GLC_Instance flat collection */

/*! An GLC_Collection contain  :
 * 		- A hash table containing GLC_Instance Class
 */
//////////////////////////////////////////////////////////////////////

class GLC_Collection  
{

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_Collection();
	
	//! Destructor
	/*! Delete all Node in the Hash Table and clear the Hash Table*/
	virtual ~GLC_Collection();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Return the number of Node in the collection
	int getNumber(void) const
	{
		return m_NodeMap.size();
	}
	//! Return a GLC_Instance from collection
	/*! If the element is not found in collection a empty node is return*/
	GLC_Instance* getInstanceHandle(GLC_uint Key);
	
	//! Return the collection Bounding Box
	GLC_BoundingBox getBoundingBox(void);
	
	//! Return the number of Node in the selection Hash
	int getNumberOfSelectedNode(void) const {return m_SelectedNodes.size();}
	
	//! Get the Hash table of Selected Nodes
	SelectedNodeHash* getSelections() {return &m_SelectedNodes;}
	
	//! Return true if the Node is in the collection
	bool isInCollection(GLC_uint key) const {return m_NodeMap.contains(key);}

	//! Return true if the element is selected
	bool isSelected(GLC_uint key) const {return m_SelectedNodes.contains(key);}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Add a GLC_Instance in the collection
	/*! return true if success false otherwise*/
	bool add(GLC_Instance& node);

	//! Remove a GLC_Geometry from the collection and delete it
	/*! 	- Find the GLC_Geometry in the collection
	 * 		- Delete the GLC_Geometry
	 * 		- Remove the Geometry container from collection
	 * 		- Delete the associated OpenGL Display list
	 * 		- Remove the Display list container from collection
	 * 		- Invalidate the collection OpenGL Display list
	 * return true if success false otherwise*/
	bool remove(GLC_uint Key);

	//! Remove and delete all GLC_Geometry from the collection
	void clear(void);
	
	//! Select a Node
	bool select(GLC_uint);
	
	//! unselect a node
	bool unselect(GLC_uint);
	
	//! unselect all node
	void unselectAll();

//@}
	
//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Display the collection
	void glExecute(void);

	//! Create member's OpenGL display list
	void createMemberLists(void);


//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////

private:
	//! Display collection's member
	void glDraw(void);

//@}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////
private:

	//! Check if Members are up to date (List + Geometry properties)
	bool memberIsUpToDate(void);

	//! Create Collection's OpenGL display list
	bool createList(void);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////

private:
	//! GLC_Instance Hash Table
	CNodeMap m_NodeMap;

	//! Collection's OpenGL list ID
	GLuint m_ListID;

	//! Validity of collection's OpenGL list
	bool m_ListIsValid;
	
	//! BoundingBox of the collection
	GLC_BoundingBox* m_pBoundingBox;
		
	//! Selected Node Hash Table
	SelectedNodeHash m_SelectedNodes;
		
};
#endif //GLC_COLLECTION_H_
