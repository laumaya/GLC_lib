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

//! \file glc_structoccurrence.h interface for the GLC_StructOccurrence class.

#ifndef GLC_STRUCTOCCURRENCE_H_
#define GLC_STRUCTOCCURRENCE_H_

#include "../maths/glc_matrix4x4.h"
#include "../glc_boundingbox.h"
#include "glc_structinstance.h"
#include <QSet>

#include "../glc_config.h"

class GLC_WorldHandle;
class GLC_Material;
class GLC_RenderProperties;

typedef QList<QPair<QString, uint> > GLC_OccurencePath;

//////////////////////////////////////////////////////////////////////
//! \class GLC_StructOccurrence
/*! \brief GLC_StructOccurrence : A scene graph occurrence node */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_StructOccurrence
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
	//! Default constructor
    GLC_StructOccurrence();

	//! Create Occurrence of the specified instance
    GLC_StructOccurrence(GLC_StructInstance* pStructInstance, GLC_WorldHandle* pWorldHandle= NULL, GLuint shaderId=0);

	//! Create Occurrence of the specified instance and Uid
    GLC_StructOccurrence(GLC_StructInstance* pStructInstance, GLC_uint id, GLC_WorldHandle* pWorldHandle= NULL, GLuint shaderId=0);

	//! Construct Occurrence with the specified GLC_3DRep
    GLC_StructOccurrence(GLC_3DRep* pRep);

	//! Construct Occurrence from the given GLC_3DRep and Uid
    GLC_StructOccurrence(GLC_3DRep* pRep, GLC_uint id);

	//! Copy constructor
    GLC_StructOccurrence(GLC_WorldHandle*, const GLC_StructOccurrence&, bool shareInstance);

	//! Destructor
    virtual ~GLC_StructOccurrence();
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return this Occurrence id
	inline GLC_uint id() const
	{return m_Uid;}

	//! Return the instance name of this occurrence
	inline const QString name() const
	{return m_pStructInstance->name();}

	//! Return the absolute matrix of this occurrence
	inline GLC_Matrix4x4 absoluteMatrix() const
	{ return m_AbsoluteMatrix;}

	//! Return the surcharged relative matrix
	GLC_Matrix4x4 occurrenceRelativeMatrix() const;

	//! Return true if this occurrence is orphan
	inline bool isOrphan() const
	{ return NULL == m_pParent;}

	//! Return true if this occurrence has parent
	inline bool hasParent() const
	{ return NULL != m_pParent;}

	//! Return true if this occurrence has a representation
	bool hasRepresentation() const;

	//! Return true if this occurrence has 3DViewInstance
	bool has3DViewInstance() const;

	//! Return the instance of this occurrence
	inline GLC_StructInstance* structInstance() const
	{ return m_pStructInstance;}

	//! Return the reference of this occurrence
	inline GLC_StructReference* structReference() const
	{
		Q_ASSERT(NULL != m_pStructInstance);
		return m_pStructInstance->structReference();
	}

	//! Return the number of childs
	inline int childCount() const
	{ return m_Childs.size();}

	//! Return true if the occurrence has child
	inline bool hasChild() const
	{return childCount() > 0;}

	//! Return true if the given occurrence can be added to this occurrence children
    bool canBeAddedToChildren(GLC_StructOccurrence* pOccurrence) const;

	//! Return The parent of this occurrence
    inline GLC_StructOccurrence* parent() const
	{return m_pParent;}

    //! Return return all ancestor of this occurrence
    QList<GLC_StructOccurrence*> ancestorList() const;

	//! Return a child of this occurrence
	/*! The index must exist*/
    inline GLC_StructOccurrence* child(const int index) const
	{return m_Childs.at(index);}

	//! Return the list of children of this occurrence
    inline QList<GLC_StructOccurrence*> children() const
	{return m_Childs;}

	//! Return the list of all accurence under this occurrence
    QList<GLC_StructOccurrence*> subOccurrenceList() const;

	//! Return the number of faces of the representation of this occurrence
	unsigned int numberOfFaces() const;

	//! Return the number of vertex of the representation of this occurrence
	unsigned int numberOfVertex() const;

	//! Return the number of materials of the representation of this occurrence
	unsigned int numberOfMaterials() const;

	//! Return the materials List of the representation of this occurrence
	QSet<GLC_Material*> materialSet() const;

	//! Return a clone this occurrence
    GLC_StructOccurrence* clone(GLC_WorldHandle*, bool shareInstance) const;

	//! Return true if this occurrence is visible
	bool isVisible() const;

	//! Return the occurrence Bounding Box
	GLC_BoundingBox boundingBox() const;

	//! Return the occurrence number of this occurrence
	inline unsigned int occurrenceNumber() const
	{return m_OccurrenceNumber;}

	//! Return an handle of the renderProperties of this occurrence
	GLC_RenderProperties* renderPropertiesHandle() const
	{return m_pRenderProperties;}

	//! Return the number of node of this branch
	unsigned int nodeCount() const;

	//! Return the world handle of this occurrence
	inline GLC_WorldHandle* worldHandle() const
	{return m_pWorldHandle;}

	//! Return the Set of children references of this occurrence
	QSet<GLC_StructReference*> childrenReferences() const;

	//! Return the set of parents references of the given occurrence
    static QSet<GLC_StructReference*> parentsReferences(const GLC_StructOccurrence* pOccurrence);

	//! Return true if the automatic creation of 3DViewInstance is used
	inline bool useAutomatic3DViewInstanceCreation() const
	{return m_AutomaticCreationOf3DViewInstance;}

	//! Return true if this occurrence is flexible
	inline bool isFlexible() const
	{return (m_pRelativeMatrix != NULL);}

	//! Return the index position of the given occurrence
	/*! Return -1 if the given occurrence is not a child of this occurrence*/
    int indexOf(const GLC_StructOccurrence* pOcc) const;

	//! Return true if this occurrence contains the given occurrence has child
    bool containsChild(const GLC_StructOccurrence* pOcc) const;

    //! Return the occurrence path (Instance Name, Instance index) from root exluded
    GLC_OccurencePath path() const;

    //! Return the occurence of the given path
    GLC_StructOccurrence* occurrenceFromPath(GLC_OccurencePath path) const;

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Set Occurrence instance Name
	inline void setName(const QString name) {m_pStructInstance->setName(name);}

	//! Update the absolute matrix
    GLC_StructOccurrence* updateAbsoluteMatrix();

	//! Update children obsolute Matrix
    GLC_StructOccurrence* updateChildrenAbsoluteMatrix();

	//! Add Child
	/*! The new child must be orphan*/
    void addChild(GLC_StructOccurrence*);

	//! insert Child at the given position
	/*! The new child must be orphan and index >= childcount*/
    void insertChild(int index, GLC_StructOccurrence* pChild);

	//! Add Child instance and returns the newly created occurrence
    GLC_StructOccurrence* addChild(GLC_StructInstance*);

	//! Insert Child instance and returns the newly created occurrence
    GLC_StructOccurrence* insertChild(int index, GLC_StructInstance* pInstance);

	//! make the occurrence orphan
	void makeOrphan();

	//! Remove the specified child
	/*! The removed child will not be deleted*/
    bool removeChild(GLC_StructOccurrence* pChild);

	//! Reverse Normals of this Occurrence and childs
	void reverseNormals();

	//! Create the 3DViewInstance of this occurrence if there is a valid 3DRep
	bool create3DViewInstance(GLuint shaderId= 0);

	//! Remove the 3DViewInstance of this occurrence
	bool remove3DViewInstance();

	//! Set this occurrence world Handle
	void setWorldHandle(GLC_WorldHandle*);

	//! Load the representation and return true if success
	bool loadRepresentation();

	//! UnLoad the representation and return true if success
	bool unloadRepresentation();

	//! Set the occurrence number of this occurrence
	inline void setOccurrenceNumber(unsigned int n)
	{m_OccurrenceNumber= n;}

	//! Update the occurrence number of this occurrence branch
	unsigned int updateOccurrenceNumber(unsigned int n);

    //! Set this occurrence and children visibility
	void setVisibility(bool visibility);

	//! set the renderProperties of this occurrence
    void setRenderProperties(const GLC_RenderProperties& renderProperties, bool propagate= true);

    //! Remove renderProperties of this occurrence
    void removeRenderProperties();

	//! Remove empty children
	void removeEmptyChildren();

	//! Set the given reference to this occurrence
	void setReference(GLC_StructReference* pRef);

	//! Set the automatic creation of 3DViewInstance usage
	inline void setAutomatic3DViewInstanceCreationUsage(bool usage)
	{m_AutomaticCreationOf3DViewInstance= usage;}

	//! Make this occurrence a flexible occurrence
    void makeFlexible(const GLC_Matrix4x4& relativeMatrix, bool update= true);

	//! Make this occurrence rigid
    void makeRigid(bool update= true);

	//! Exchange the occurrence at index position i with the occurrence at index position j
    /*! This function assumes that both i and j are at least 0 but less than childCount().*/
    void swap(int oldPos, int newPos);

//@}

//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////
private:
	//! Detach the occurrence from the GLC_World
	void detach();

	//! Create occurrence from instance and given shader id
	void doCreateOccurrenceFromInstance(GLuint shaderId);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Occurrence Unique ID
	GLC_uint m_Uid;

	//! the occurrence's World Handle
	GLC_WorldHandle* m_pWorldHandle;

	//! Number of this Occurrence
	int* m_pNumberOfOccurrence;

	//! The struct instance of this occurrence
	GLC_StructInstance* m_pStructInstance;

	//! The parent of this occurrence
    GLC_StructOccurrence* m_pParent;

	//! The Child of this occurrence
    QList<GLC_StructOccurrence*> m_Childs;

	//! The absolute matrix of the occurrence
	GLC_Matrix4x4 m_AbsoluteMatrix;

	//! The occurrence number
	unsigned int m_OccurrenceNumber;

	//! Flag to know if a occurrence without instance is visible
	bool m_IsVisible;

	//! The occurrence rendering properties
	GLC_RenderProperties* m_pRenderProperties;

	//! Automatic creation of 3DViewInstance
	bool m_AutomaticCreationOf3DViewInstance;

	//! The relative matrix of this occurrence if this occurrence is flexible
	GLC_Matrix4x4* m_pRelativeMatrix;

   Q_DISABLE_COPY(GLC_StructOccurrence)
};

#endif /* GLC_STRUCTOCCURRENCE_H_ */
