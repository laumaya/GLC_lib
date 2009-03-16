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

//! \file glc_structinstance.h interface for the GLC_StructInstance class.

#ifndef GLC_STRUCTINSTANCE_H_
#define GLC_STRUCTINSTANCE_H_

#include <QList>
#include "glc_matrix4x4.h"
#include "glc_instance.h"


class GLC_StructReference;
class GLC_StructOccurence;

class GLC_StructInstance
{
public:
	//! Default constructor
	GLC_StructInstance(GLC_StructReference*);

	// Destructor
	virtual ~GLC_StructInstance();
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return true if this instance have occurence
	inline bool haveStructOccurence() const
	{ return not m_ListOfOccurences.isEmpty();}

	//! Return first occurence handle
	inline GLC_StructOccurence* firstOccurenceHandle() const
	{ return m_ListOfOccurences.first();}

	//! Return the relative matrix of this instance
	inline GLC_Matrix4x4 relativeMatrix() const
	{ return m_RelativeMatrix;}

	//! Return the reference of this instance
	inline GLC_StructReference* structReference() const
	{ return m_pStructReference;}

	//! Return the list off occurence of this instance
	inline QList<GLC_StructOccurence*> listOfStructOccurences() const
	{ return m_ListOfOccurences;}

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! An occurence of this instance have been created
	inline void structOccurenceCreated(GLC_StructOccurence* pOccurence)
	{m_ListOfOccurences.append(pOccurence);}

	//! Move the instance by specified matrix
	inline GLC_StructInstance* move(const GLC_Matrix4x4& matrix)
	{
		m_RelativeMatrix= matrix * m_RelativeMatrix;
		return this;
	}

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Number of this Instance
	int* m_pNumberOfInstance;

	//! The Struct reference of this instance
	GLC_StructReference* m_pStructReference;

	//! The list of instance's occurences
	QList<GLC_StructOccurence*> m_ListOfOccurences;

	//! The relative matrix
	GLC_Matrix4x4 m_RelativeMatrix;
};

#endif /* GLC_STRUCTINSTANCE_H_ */
