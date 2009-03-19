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

//! \file glc_structreference.h interface for the GLC_StructReference class.

#ifndef GLC_STRUCTREFERENCE_H_
#define GLC_STRUCTREFERENCE_H_

#include <QString>
#include <QList>

#include "glc_structinstance.h"
#include "glc_instance.h"

class GLC_StructReference
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default Constructor
	GLC_StructReference(const QString& name= QString());

	//! Create reference with representation
	GLC_StructReference(const GLC_Instance&);

	//! Destructor
	virtual ~GLC_StructReference();
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return true if this reference have instance
	inline bool haveStructInstance() const
	{ return not m_ListOfInstance.isEmpty();}

	//! Return first instance handle
	inline GLC_StructInstance* firstInstanceHandle() const
	{ return m_ListOfInstance.first();}

	//! Create a Struct instance of this reference
	GLC_StructInstance* createStructInstance();

	//! Return the list of instance of this reference
	inline QList<GLC_StructInstance*> listOfStructInstances() const
	{ return m_ListOfInstance;}

	//! Return true if this reference have a representation
	inline bool haveRepresentation() const
	{ return NULL != m_pRepresentation;}

	//! Return an instance of the representation
	/*! representation must exists*/
	inline GLC_Instance instanceRepresentation() const
	{return m_pRepresentation->instanciate();}

	//! Return the name
	inline QString name() const
	{return m_Name;}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! An Instance of this reference have been created
	inline void structInstanceCreated(GLC_StructInstance* pInstance)
	{m_ListOfInstance.append(pInstance);}

	//! An Instance of this reference have been deleted
	inline void structInstanceDeleted(GLC_StructInstance* pInstance)
	{m_ListOfInstance.removeOne(pInstance);}

	//! Set the reference name
	inline void setName(const QString& name)
	{m_Name= name;}

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The list of reference's instances
	QList<GLC_StructInstance*> m_ListOfInstance;

	//! The representation of reference
	GLC_Instance* m_pRepresentation;

	//! The Reference Name
	QString m_Name;

};

#endif /* GLC_STRUCTREFERENCE_H_ */
