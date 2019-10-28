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

//! \file glc_structreference.cpp implementation of the GLC_StructReference class.

#include "glc_structreference.h"
#include "glc_structoccurrence.h"

// Default constructor
GLC_StructReference::GLC_StructReference(const QString& name)
    : m_SetOfInstance()
    , m_pRepresentation(nullptr)
    , m_Name(name)
    , m_pAttributes(nullptr)
{


}

// Create reference with representation
GLC_StructReference::GLC_StructReference(GLC_Rep* pRep)
    : m_SetOfInstance()
    , m_pRepresentation(pRep)
    , m_Name(m_pRepresentation->name())
    , m_pAttributes(nullptr)
{

}

// Copy constructor
GLC_StructReference::GLC_StructReference(const GLC_StructReference& other)
    : m_SetOfInstance()
    , m_pRepresentation(nullptr)
    , m_Name(other.m_Name)
    , m_pAttributes(nullptr)
{
    if (nullptr != other.m_pAttributes)
    {
        m_pAttributes= new GLC_Attributes(*(other.m_pAttributes));
    }
    if (nullptr != other.m_pRepresentation)
	{
        m_pRepresentation= other.m_pRepresentation->clone();
	}
}

//! Overload "=" operator
GLC_StructReference& GLC_StructReference::operator=(const GLC_StructReference& other)
{
    if (this != &other)
	{
		m_SetOfInstance.clear();
        m_Name= other.m_Name;

        delete m_pAttributes;
        if (nullptr != other.m_pAttributes)
        {
            m_pAttributes= new GLC_Attributes(*(other.m_pAttributes));
        }
        else m_pAttributes= nullptr;

        delete m_pRepresentation;
        if (nullptr != other.m_pRepresentation)
		{
            m_pRepresentation= other.m_pRepresentation->clone();
		}
        else m_pRepresentation= nullptr;
	}
	return *this;
}

GLC_StructReference::~GLC_StructReference()
{
	delete m_pRepresentation;
	delete m_pAttributes;
}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

QSet<GLC_StructOccurrence*> GLC_StructReference::setOfStructOccurrence() const
{
	QList<GLC_StructInstance*> instanceList= listOfStructInstances();
	QSet<GLC_StructOccurrence*> occurrenceSet;
	const int size= instanceList.size();
	for (int i= 0; i < size; ++i)
	{
		QList<GLC_StructOccurrence*> occurrenceList= instanceList.at(i)->listOfStructOccurrences();
		const int occurrenceSize= occurrenceList.size();
		for (int occIndex= 0; occIndex < occurrenceSize; ++occIndex)
		{
			occurrenceSet.insert(occurrenceList.at(occIndex));
		}
	}
	return occurrenceSet;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Set the reference representation
void GLC_StructReference::setRepresentation(const GLC_3DRep& rep)
{
	// Unload occurrence representation
	{
		QSet<GLC_StructOccurrence*> structOccurrenceSet= this->setOfStructOccurrence();
		QSet<GLC_StructOccurrence*>::iterator iOcc= structOccurrenceSet.begin();
		while (structOccurrenceSet.constEnd() != iOcc)
		{
			(*iOcc)->remove3DViewInstance();
			++iOcc;
		}
	}

    if(nullptr == m_pRepresentation)
	{
		m_pRepresentation= new GLC_3DRep(rep);
	}
	else
	{
		*(m_pRepresentation)= rep;
	}

	if (m_pRepresentation->isLoaded())
	{
		QSet<GLC_StructOccurrence*> structOccurrenceSet= this->setOfStructOccurrence();
		QSet<GLC_StructOccurrence*>::iterator iOcc= structOccurrenceSet.begin();
		while (structOccurrenceSet.constEnd() != iOcc)
		{
			GLC_StructOccurrence* pOccurrence= *iOcc;
			Q_ASSERT(!pOccurrence->has3DViewInstance());
			if (pOccurrence->useAutomatic3DViewInstanceCreation())
			{
				pOccurrence->create3DViewInstance();
			}
			++iOcc;
		}
	}
}

GLC_Rep* GLC_StructReference::representationHandle() const
{
    Q_ASSERT(nullptr != m_pRepresentation);
	return m_pRepresentation;
}

QString GLC_StructReference::representationName() const
{
    if (nullptr != m_pRepresentation)
	{
		return m_pRepresentation->name();
	}
	else return QString();
}

bool GLC_StructReference::representationIsLoaded() const
{
    if (nullptr != m_pRepresentation)
	{
		return m_pRepresentation->isLoaded();
	}
	else return false;

}

QString GLC_StructReference::representationFileName() const
{
    if (nullptr != m_pRepresentation)
	{
		return m_pRepresentation->fileName();
	}
	else return QString();
}

bool GLC_StructReference::representationIsEmpty() const
{
    if (nullptr != m_pRepresentation)
	{
		return m_pRepresentation->isEmpty();
	}
	else return true;

}

void GLC_StructReference::setRepresentationName(const QString& representationName)
{
    if (nullptr != m_pRepresentation)
	{
		m_pRepresentation->setName(representationName);
	}
}

bool GLC_StructReference::loadRepresentation()
{
    Q_ASSERT(nullptr != m_pRepresentation);
	if (m_pRepresentation->load())
	{
		QSet<GLC_StructOccurrence*> structOccurrenceSet= this->setOfStructOccurrence();
		QSet<GLC_StructOccurrence*>::iterator iOcc= structOccurrenceSet.begin();
		while (structOccurrenceSet.constEnd() != iOcc)
		{
			GLC_StructOccurrence* pOccurrence= *iOcc;
			Q_ASSERT(!pOccurrence->has3DViewInstance());
			if (pOccurrence->useAutomatic3DViewInstanceCreation())
			{
				pOccurrence->create3DViewInstance();
			}
			++iOcc;
		}
		return true;
	}
	else return false;
}

bool GLC_StructReference::unloadRepresentation()
{
    Q_ASSERT(nullptr != m_pRepresentation);
	if (m_pRepresentation->unload())
	{
		QSet<GLC_StructOccurrence*> structOccurrenceSet= this->setOfStructOccurrence();
		QSet<GLC_StructOccurrence*>::iterator iOcc= structOccurrenceSet.begin();
		while (structOccurrenceSet.constEnd() != iOcc)
		{
			(*iOcc)->remove3DViewInstance();
			++iOcc;
		}
		return true;
	}
	else return false;
}

QList<GLC_StructOccurrence*> GLC_StructReference::addChild(GLC_StructOccurrence* pOccurrence)
{
	QList<GLC_StructOccurrence*> subject;
	if (hasStructInstance() && firstInstanceHandle()->hasStructOccurrence())
	{
		QList<GLC_StructOccurrence*> parentOccurrences= listOfStructOccurrence();
		const int parentCount= parentOccurrences.count();
        GLC_StructInstance* pNewInstance= nullptr;
		for (int i= 0; i < parentCount; ++i)
		{
			GLC_StructOccurrence* pCurrentParent= parentOccurrences.at(i);
            GLC_StructOccurrence* pNewChild= nullptr;
            if (nullptr == pNewInstance)
			{
				pNewChild= pOccurrence;
				pNewInstance= pNewChild->structInstance();
				pCurrentParent->addChild(pNewChild);
			}
			else
			{
				pNewChild= pCurrentParent->addChild(pNewInstance);
			}
			subject.append(pNewChild);
		}
	}

	return subject;
}

