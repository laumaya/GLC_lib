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

//! \file glc_world.h interface for the GLC_World class.

#ifndef GLC_WORLD_H_
#define GLC_WORLD_H_

#include "glc_3dviewcollection.h"
#include "glc_structoccurrence.h"
#include "glc_structreference.h"
#include "glc_structinstance.h"
#include "glc_worldhandle.h"
#include "glc_selectionset.h"

#include "../glc_config.h"

class GLC_SelectionEvent;

//////////////////////////////////////////////////////////////////////
//! \class GLC_World
/*! \brief GLC_World : The Root of GLC_Lib Scene Graph*/
/*!
 *  GLC_World contain :
 * 		- The Scene root GLC_Product.
 * 		- a GLC_3DViewCollection which manage all scene shapes (GLC_3DViewInstance)
 *
 */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_World
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_World();

	//! Create a world and set the root occurrence to the given occurrence
	explicit GLC_World(GLC_StructOccurrence* pOcc);

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
	//! Return the entire world Bounding Box
	inline GLC_BoundingBox boundingBox()
	{ return m_pWorldHandle->collection()->boundingBox();}

	//! Return the root of the world
	inline GLC_StructOccurrence* rootOccurrence() const
    {return m_pWorldHandle->rootOccurrence();}

	//! Return the world collection
	inline GLC_3DViewCollection* collection()
	{return m_pWorldHandle->collection();}

	//! Return the size of the world
	inline int size() const
	{return m_pWorldHandle->collection()->size();}

	//! Return true if the world is empty
	inline bool isEmpty() const
    {return  m_pWorldHandle->collection()->isEmpty() && !rootOccurrence()->hasChild();}

	//! Return number of faces
	inline int numberOfFaces() const
    {return rootOccurrence()->numberOfFaces();}

	//! Return number of vertex
	inline int numberOfVertex() const
    {return rootOccurrence()->numberOfVertex();}

	//! Return the number of materials
	inline int numberOfMaterials() const
    {return rootOccurrence()->numberOfMaterials();}

	//! Return the list of material
	inline QList<GLC_Material*> listOfMaterials() const
    {return rootOccurrence()->materialSet().toList();}

	//! Return list of world's instances
	inline QList<GLC_3DViewInstance*> instancesHandle() const
	{return m_pWorldHandle->collection()->instancesHandle();}

	//! Return all visible GLC_3DViewInstance from the world
	inline QList<GLC_3DViewInstance*> visibleInstancesHandle() const
	{return m_pWorldHandle->collection()->visibleInstancesHandle();}

	//! Return instances name from the specified shading group
	inline QList<QString> instanceNamesFromShadingGroup(GLuint id) const
	{return m_pWorldHandle->collection()->instanceNamesFromShadingGroup(id);}

	//! Return the number of used shading group
	inline int numberOfUsedShadingGroup() const
	{return m_pWorldHandle->collection()->numberOfUsedShadingGroup();}

	//! Return the worldHandle of this world
	inline GLC_WorldHandle* worldHandle()
	{return m_pWorldHandle;}

    //! Return the worldHandle of this world
    inline const GLC_WorldHandle* worldHandle() const
    {return m_pWorldHandle;}

	//! Return the occurrence specified by an id
	/*! Id must be a valid identifier*/
	inline GLC_StructOccurrence* occurrence(GLC_uint id) const
	{return m_pWorldHandle->getOccurrence(id);}

	//! Return the list off occurrences
	inline QList<GLC_StructOccurrence*> listOfOccurrence() const
	{return m_pWorldHandle->occurrences();}

    //! Return the list of occurrences id
    inline QList<GLC_uint> occurrencesId() const
    {return m_pWorldHandle->occurrencesId();}

	//! Return the number of occurrence
	inline int numberOfOccurrence() const
	{return m_pWorldHandle->numberOfOccurrence();}

	//! Return true if the world contians specified id
	inline int containsOccurrence(GLC_uint id) const
	{return m_pWorldHandle->containsOccurrence(id);}

	//! Return the list of instance
	inline QList<GLC_StructInstance*> instances() const
	{return m_pWorldHandle->instances();}

	//! Return the list of Reference
	inline QList<GLC_StructReference*> references() const
	{return m_pWorldHandle->references();}

	//! Return the number of body
	inline int numberOfBody() const
	{return m_pWorldHandle->numberOfBody();}

	//! Return the number of representation
	inline int representationCount() const
	{return m_pWorldHandle->representationCount();}

	//! Return the world Up vector
	inline GLC_Vector3d upVector() const
	{return m_pWorldHandle->upVector();}

	//! Return the number of selected occurrence
	int selectionSize() const
    {return m_pWorldHandle->selectionSetHandle()->size();}

	//! Return true if the given occurrence is selected
	inline bool isSelected(const GLC_StructOccurrence* pOccurrence) const
	{return m_pWorldHandle->selectionSetHandle()->contains(pOccurrence);}

	//! Return true if the given occurrence id is selected
	inline bool isSelected(GLC_uint selectionId) const
	{return m_pWorldHandle->selectionSetHandle()->contains(selectionId);}

	//! Return the list of selected occurrences
	inline QList<GLC_StructOccurrence*> selectedOccurrenceList() const
    {return m_pWorldHandle->selectionSetHandle()->occurrencesList();}

    //! Return the minimum selected occurrences list
    QList<GLC_StructOccurrence*> minimumSelectedOccurrenceList() const;

	//! Take the root occurrence of this world
	GLC_StructOccurrence* takeRootOccurrence();

    //! Return a copy of the selection set
    inline GLC_SelectionSet selectionSet()
    {return m_pWorldHandle->selectionSet();}

    //! Return the occurence of the given path
    inline GLC_StructOccurrence* occurrenceFromPath(GLC_OccurencePath path) const
    {return m_pWorldHandle->occurrenceFromPath(path);}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Replace the root occurrence of this world by the given occurrence
	void replaceRootOccurrence(GLC_StructOccurrence* pOcc);

	//! Merge this world with another world
	void mergeWithAnotherWorld(GLC_World &);

	//! Reverse worlds part normal
    inline void reversePartNormal() {rootOccurrence()->reverseNormals();}

	//! Clear this world
	GLC_World& clear() {return *this= GLC_World();}

	//! Set the World root Name
	inline void setRootName(const QString& name)
    {rootOccurrence()->setName(name);}

	//! Set the world Up Vector
	inline void setUpVector(const GLC_Vector3d& vect)
	{m_pWorldHandle->setUpVector(vect);}

	//! Set the attached viewport of this world
	inline void setAttachedViewport(GLC_Viewport* pViewport)
	{m_pWorldHandle->setAttachedViewport(pViewport);}

	//! Select the given occurrence
	/*! The given occurrence must belong to the world handle of this world*/
	inline void select(const GLC_StructOccurrence* pOccurrence)
	{m_pWorldHandle->select(pOccurrence->id());}

	//! Select the given occurrence id
	/*! The given occurrence id must belong to the world handle of this world*/
	inline void select(GLC_uint occurrenceId)
	{m_pWorldHandle->select(occurrenceId);}

    //! Update the current selection from the given selection event
    void updateSelection(const GLC_SelectionEvent& selectionEvent);

	//! Unselect the given occurrence id
	/*! The given occurrence id must belong to the world handle of this world*/
	inline void unselect(GLC_uint occurrenceId)
	{m_pWorldHandle->unselect(occurrenceId);}

	//! Select all occurrence of this world with a 3DViewInstance
	inline void selectAllWith3DViewInstance()
	{m_pWorldHandle->selectAllWith3DViewInstance(true);}

	//! Select all occurrence of this world with a 3DViewInstance in the current show state
	inline void selectAllWith3DViewInstanceInCurrentShowState()
	{m_pWorldHandle->selectAllWith3DViewInstance(false);}

	//! Unselect all occurrence of this world
	inline void unselectAll()
	{m_pWorldHandle->unselectAll();}

	//! Show / Hide selected 3DViewInstance
	inline void showHideSelected3DViewInstance()
	{m_pWorldHandle->showHideSelected3DViewInstance();}

	//! Show selected 3DViewInstance
	inline void showSelected3DViewInstance()
	{m_pWorldHandle->setSelected3DViewInstanceVisibility(true);}

	//! Hide selected 3DViewInstance
	inline void hideSelected3DViewInstance()
	{m_pWorldHandle->setSelected3DViewInstanceVisibility(false);}
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Operator Overload */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Assignement operator
	GLC_World& operator=(const GLC_World&);

    //! Return true if this world is the same than the given world
    bool operator==(const GLC_World& other) const;

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Display the world
	inline void render(GLuint groupId, glc::RenderFlag renderFlag= glc::ShadingFlag)
	{m_pWorldHandle->collection()->render(groupId, renderFlag);}

	//! Display the world's shader group
	inline void renderShaderGroup(glc::RenderFlag renderFlag= glc::ShadingFlag)
	{m_pWorldHandle->collection()->renderShaderGroup(renderFlag);}

//@}
//////////////////////////////////////////////////////////////////////
// private members
//////////////////////////////////////////////////////////////////////
private:
	//! The World Handle
	GLC_WorldHandle* m_pWorldHandle;
};

Q_DECLARE_METATYPE(GLC_World)

#endif /*GLC_WORLD_H_*/
