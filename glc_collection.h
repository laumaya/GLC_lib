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

//! \file glc_collection.h interface for the GLC_Collection class.

#ifndef GLC_COLLECTION_H_
#define GLC_COLLECTION_H_


#include <QHash>
#include "glc_instance.h"
#include "glc_enum.h"

class GLC_Material;

//! Geometry hash table
typedef QHash< GLC_uint, GLC_Instance> CNodeMap;
typedef QHash< GLC_uint, GLC_Instance*> PointerNodeHash;

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

	//! Return true if the collection is empty
	inline bool isEmpty() const {return m_NodeMap.size() == 0;}
	
	//! Return the number of Node in the collection
	inline int getNumber(void) const{return m_NodeMap.size();}
	
	//! Return all GLC_Instance from collection
	QList<GLC_Instance*> getInstancesHandle();
	
	//! Return a GLC_Instance from collection
	/*! If the element is not found in collection a empty node is return*/
	GLC_Instance* getInstanceHandle(GLC_uint Key);
	
	//! Return the collection Bounding Box
	GLC_BoundingBox getBoundingBox(void);
	
	//! Return the number of Node in the selection Hash
	inline int getNumberOfSelectedNode(void) const {return m_SelectedNodes.size();}
	
	//! Get the Hash table of Selected Nodes
	inline PointerNodeHash* getSelections() {return &m_SelectedNodes;}
	
	//! Return true if the Node is in the collection
	inline bool isInCollection(GLC_uint key) const {return m_NodeMap.contains(key);}

	//! Return true if the element is selected
	inline bool isSelected(GLC_uint key) const {return m_SelectedNodes.contains(key);}
	
	//! Return the showing state
	inline bool getShowState() const {return m_IsInShowSate;}

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
	
	//! Select a Instance
	bool select(GLC_uint);
	
	//! unselect a Instance
	bool unselect(GLC_uint);
	
	//! unselect all Instance
	void unselectAll();
	
	//! Set the polygon mode for all Instance
	void setPolygonModeForAll(GLenum, GLenum);
	
	//! Set Instance visibility
	void setVisibility(const GLC_uint, const bool);
	
	//! Show all instances of the collection
	void showAll();
	
	//! Hide all instances of collection
	void hideAll();
	
	//! Set the Show or noShow state
	inline void swapShowState()
	{
		m_IsInShowSate= !m_IsInShowSate;
		m_ListIsValid= false;
	}

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
	PointerNodeHash m_SelectedNodes;
	
	//! Not transparent Node Hash Table
	PointerNodeHash m_NotTransparentNodes;

	//! Transparent Node Hash Table
	PointerNodeHash m_TransparentNodes;

	
	//! Show State
	bool m_IsInShowSate;
		
};
#endif //GLC_COLLECTION_H_
