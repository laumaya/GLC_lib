/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/

#ifndef GLC_WORLDHANDLE_H_
#define GLC_WORLDHANDLE_H_

#include <QHash>
#include <QAtomicInt>

#include "glc_3dviewcollection.h"
#include "glc_structoccurrence.h"
#include "glc_selectionset.h"

#include "../glc_config.h"

class GLC_SelectionEvent;

//////////////////////////////////////////////////////////////////////
//! \class GLC_WorldHandle
/*! \brief GLC_WorldHandle : Handle of shared GLC_World*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_WorldHandle
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! The default constructor
	GLC_WorldHandle();

    //! Create a worldHandle and set the root occurrence to the given occurrence
    explicit GLC_WorldHandle(GLC_StructOccurrence* pOcc);

	//! The default destructor
	~GLC_WorldHandle();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the collection
	inline GLC_3DViewCollection* collection()
	{return &m_Collection;}

    //! Return the root of the world
    inline GLC_StructOccurrence* rootOccurrence() const
    {return m_pRoot;}

	//! Return the number of world associated with this handle
	inline int numberOfWorld() const
    {return m_Ref.load();}

    //! Return true if the specified occurrence id is in this world
    inline bool containsOccurrence(GLC_uint id) const
    {return m_OccurrenceHash.contains(id);}

    //! Return the occurrence specified by an id
	/*! Id must be a valid identifier*/
    inline GLC_StructOccurrence* getOccurrence(GLC_uint id) const
	{
        Q_ASSERT(m_OccurrenceHash.contains(id));
        return m_OccurrenceHash.value(id);
	}

    //! Return the list of occurrences
    inline QList<GLC_StructOccurrence*> occurrences() const
    {return m_OccurrenceHash.values();}

    //! Return the list of occurrences id
    inline QList<GLC_uint> occurrencesId() const
    {return m_OccurrenceHash.keys();}

    //! Return the number of occurrence
    inline int numberOfOccurrence() const
    {return m_OccurrenceHash.size();}

	//! Return the list of instance
	QList<GLC_StructInstance*> instances() const;

	//! Return the list of Reference
	QList<GLC_StructReference*> references() const;

	//! Return the number of body
	int numberOfBody() const;

	//! Return the number of representation
	int representationCount() const;

	//! Return the world Up vector
	inline GLC_Vector3d upVector() const
	{return m_UpVector;}

	//! Return an handle to the selection set
	inline GLC_SelectionSet* selectionSetHandle()
	{return &m_SelectionSet;}

    //! Return a copy of the selection set
    inline GLC_SelectionSet selectionSet()
    {return m_SelectionSet;}

    //! Return the occurence of the given path
    GLC_StructOccurrence* occurrenceFromPath(GLC_OccurencePath path) const;

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Replace the root occurrence of this world by the given occurrence
    void replaceRootOccurrence(GLC_StructOccurrence* pOcc);

    //! Take the root occurrence of this world
    GLC_StructOccurrence* takeRootOccurrence();

	//! Increment the number of world
    inline bool ref()
    {return m_Ref.ref();}

	//! Decrement the number of world
    inline bool deref()
    {return m_Ref.deref();}

    //! An Occurrence has been added
    void addOccurrence(GLC_StructOccurrence* pOccurrence, bool isSelected= false, GLuint shaderId= 0);

    //! An Occurrence has been removed
    void removeOccurrence(GLC_StructOccurrence* pOccurrence);

    //! All Occurrence has been removed
    inline void removeAllOccurrences()
    {m_OccurrenceHash.clear();}

	//! Set the world Up Vector
	inline void setUpVector(const GLC_Vector3d& vect)
	{m_UpVector= vect;}

	//! Set the attached viewport of this world handle
	inline void setAttachedViewport(GLC_Viewport* pViewport)
	{m_Collection.setAttachedViewport(pViewport);}

    //! Select the given occurrence id
    /*! The given occurrence id must belong to this worldhandle*/
    void select(GLC_uint occurrenceId);

    //! Update the current selection from the given selection event
    void updateSelection(const GLC_SelectionEvent& selectionEvent);

    //! Unselect the given occurrence id
    /*! The given occurrence id must belong to this worldhandle*/
    void unselect(GLC_uint occurrenceId, bool propagate= true);

    //! Select all occurrence of this world handle
	void selectAllWith3DViewInstance(bool allShowState);

    //! Unselect all occurrence of this world handle
	void unselectAll();

	//! Show / Hide selected 3DViewInstance
	void showHideSelected3DViewInstance();

	//! Set selected 3DViewInstance visibility
	void setSelected3DViewInstanceVisibility(bool isVisible);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
    void updateSelectedInstanceFromSelectionSet();

//@}

//////////////////////////////////////////////////////////////////////
// private members
//////////////////////////////////////////////////////////////////////
private:
	//! The Collection
	GLC_3DViewCollection m_Collection;

    //! The root of the structure
    GLC_StructOccurrence* m_pRoot;

    //! World reference counting
    QAtomicInt m_Ref;

    //! The hash table containing struct occurrence
    QHash<GLC_uint, GLC_StructOccurrence*> m_OccurrenceHash;

	//! This world Up Vector
	GLC_Vector3d m_UpVector;

	//! This world selectionSet
	GLC_SelectionSet m_SelectionSet;

private:
    Q_DISABLE_COPY(GLC_WorldHandle)
};

#endif /* GLC_WORLDHANDLE_H_ */
