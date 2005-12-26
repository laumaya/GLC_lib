/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9, packaged on Novemeber, 2005.

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
#include "glc_geometrie.h"
#include "glc_enum.h"

//! Geometry hash table
typedef QHash< GLC_uint, GLC_Geometrie*> CGeomMap;

//! OpenGL Dsiplay list Hash Table
typedef QHash< GLC_uint, GLuint> CListeMap;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Collection
/*! \brief GLC_Collection : GLC_Geometrie flat collection */

/*! An GLC_Collection contain  :
 * 		- A hash table containing GLC_Geometrie pointer
 * 		- A Hash table containing OpenGL sub list ID
 */
//////////////////////////////////////////////////////////////////////

class GLC_Collection  
{

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_Collection();
	
	//! Destructor
	/*! Delete all geometry in the Hash Table and clear the Hash Table*/
	virtual ~GLC_Collection();

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////
public:

	//! Return the number of elements in the collection
	int GetNumber(void) const
	{
		return m_TheMap.size();
	}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
public:

	//! Add a GLC_Geometrie in the collection
	/*! return true if success false otherwise*/
	bool AddGLC_Geom(GLC_Geometrie* pGeom);

	//! Remove a GLC_Geometrie from the collection and delete it
	/*! 	- Find the GLC_Geometrie in the collection
	 * 		- Delete the GLC_Geometrie
	 * 		- Remove the Geometry container from collection
	 * 		- Delete the associated OpenGL Display list
	 * 		- Remove the Display list container from collection
	 * 		- Invalidate the collection OpenGL Display list
	 * return true if success false otherwise*/
	bool DelGLC_Geom(GLC_uint Key);

	//! Remove a GLC_Geometrie from the collection
	/*! 	- Find the GLC_Geometrie in the collection
	 * 		- Remove the Geometry container from collection
	 * 		- Delete the associated OpenGL Display list
	 * 		- Remove the Display list container from collection
	 * 		- Invalidate the collection OpenGL Display list
	 * return true if success false otherwise*/	
	bool RemGLC_Geom(GLC_uint Key);

	//! Remove and delete all GLC_Geometrie from the collection
	void Erase(void);

	//! return a pointer to an GLC_Geometrie from collection
	/*! If the element is not found in collection a NULL pointer is returned*/
	GLC_Geometrie* GetElement(GLC_uint Key);

	
//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Display the collection
	void GlExecute(void);

	//! Create member's OpenGL display list
	void CreateMemberList(void);

	//! Create Collection sub OpenGL list
	void CreateSousList(void);

private:
	//! Display collection's member
	void GlDraw(void);

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////
private:
	//! Check if members'list are up to date
	bool MemberListIsUpToDate(void);

	//! Check if Members are up to date (List + Geometry properties)
	bool MemberIsUpToDate(void);

	//! Create Collection's OpenGL display list
	bool CreationListe(void);

	//! Delete Collection's OpenGL display list
	void DeleteListe(void)
	{
		//! if the display list is valid it's deleted
		if (glIsList(m_ListeID))
		{
			glDeleteLists(m_ListeID, 1);
			//qDebug() << "GLC_Collection::DeleteListe : Display list " << m_ListeID << " Deleted";			
		}
	}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////

private:
	//! GLC_Geometrie Hash Table
	CGeomMap m_TheMap;

	//! Sub list Hash Table
	CListeMap m_ListeMap;

	//! Collection's OpenGL list ID
	GLuint m_ListeID;

	//! Validity of collection's OpenGL list
	bool m_bListeIsValid;
	
};
#endif //GLC_COLLECTION_H_
