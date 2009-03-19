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

//! \file glc_3dxmltoworld.cpp implementation of the GLC_3dxmlToWorld class.

#include "glc_3dxmltoworld.h"
#include "glc_world.h"
#include "glc_fileformatexception.h"
#include "glc_box.h"
// Quazip library
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QString>
#include <QGLContext>
#include <QFileInfo>

GLC_3dxmlToWorld::GLC_3dxmlToWorld(const QGLContext* pContext)
: QObject()
, m_pQGLContext(pContext)
, m_pStreamReader(NULL)
, m_FileName()
, m_p3dxmlArchive(NULL)
, m_p3dxmlFile(NULL)
, m_RootName()
, m_pWorld(NULL)
, m_ReferenceHash()
, m_AssyLinkList()
, m_InstanceOf()
, m_SetOfExtRef()
, m_InstanceOfExtRefHash()
, m_ExternalReferenceHash()
{

}

GLC_3dxmlToWorld::~GLC_3dxmlToWorld()
{
	delete m_pStreamReader;
	m_pStreamReader= NULL;

	// Clear the 3dxml file
	if (NULL != m_p3dxmlFile)
	{
		m_p3dxmlFile->close();
		delete m_p3dxmlFile;
		m_p3dxmlFile= NULL;
	}

	// Clear the 3dxml archive
	if (NULL != m_p3dxmlArchive)
	{
		m_p3dxmlArchive->close();
		delete m_p3dxmlArchive;
		m_p3dxmlArchive= NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Create an GLC_World from an input 3DXML File
GLC_World* GLC_3dxmlToWorld::CreateWorldFrom3dxml(QFile &file)
{
	m_FileName= file.fileName();

	// Create the 3dxml Zip archive
	m_p3dxmlArchive= new QuaZip(m_FileName);
	// Trying to load archive
	if(not m_p3dxmlArchive->open(QuaZip::mdUnzip))
	{
	  qDebug() << "The 3dxml file is not a valid zip archive";
	  // In this case, the 3dxml is not compressed
	  // TODO Load uncompressed 3dxml
	  return false;
	}
	// Set the file Name Codec
	m_p3dxmlArchive->setFileNameCodec("IBM866");
	// Create QuaZip File
	m_p3dxmlFile= new QuaZipFile(m_p3dxmlArchive);

	// Load the manifest
	loadManifest();

	// Load the product structure
	loadProductStructure();


	emit currentQuantum(100);
	return m_pWorld;
}

//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////
// Load the 3dxml's manifest
void GLC_3dxmlToWorld::loadManifest()
{
	// Get the manifest of the 3dxml
	if (not m_p3dxmlArchive->setCurrentFile("Manifest.xml", QuaZip::csInsensitive))
	{
		QString message(QString("GLC_3dxmlToWorld::loadManifest File ") + m_FileName + " doesn't contains Manifest.3dxml");
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}

	// Open the manifest of the 3dxml
	if(not m_p3dxmlFile->open(QIODevice::ReadOnly))
    {
		QString message(QString("GLC_3dxmlToWorld::loadManifest Unable to Open ") + m_FileName);
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::FileNotSupported);
		clear();
		throw(fileFormatException);
    }

	m_pStreamReader= new QXmlStreamReader(m_p3dxmlFile);

	m_RootName= getContent("Root");

	if (m_pStreamReader->atEnd() or m_pStreamReader->hasError())
	{
		QString message(QString("GLC_3dxmlToWorld::loadManifest Manifest file ") + m_FileName + " doesn't contains Root Element");
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}

	delete m_pStreamReader;
	m_pStreamReader= NULL;

	m_p3dxmlFile->close();
}

//! Close all files and clear memmory
void GLC_3dxmlToWorld::clear()
{
	delete m_pWorld;
	m_pWorld= NULL;

	delete m_pStreamReader;
	m_pStreamReader= NULL;

	// Clear the 3dxml file
	if (NULL != m_p3dxmlFile)
	{
		m_p3dxmlFile->close();
		delete m_p3dxmlFile;
		m_p3dxmlFile= NULL;
	}

	// Clear the 3dxml archive
	if (NULL != m_p3dxmlArchive)
	{
		m_p3dxmlArchive->close();
		delete m_p3dxmlArchive;
		m_p3dxmlArchive= NULL;
	}
}

// Go to Element
void GLC_3dxmlToWorld::goToElement(const QString& elementName)
{
	while(not m_pStreamReader->atEnd() and not ((QXmlStreamReader::StartElement == m_pStreamReader->tokenType()) and (m_pStreamReader->name() == elementName)))
	{
		m_pStreamReader->readNext();
	}
}

// Return the content of an element
QString GLC_3dxmlToWorld::getContent(const QString& element)
{
	QString Content;
	while(not m_pStreamReader->atEnd() and not ((QXmlStreamReader::EndElement == m_pStreamReader->tokenType()) and (m_pStreamReader->name() == element)))
	{
		m_pStreamReader->readNext();
		if (m_pStreamReader->isCharacters() and not m_pStreamReader->text().isEmpty())
		{
			Content+= m_pStreamReader->text().toString();
		}
	}

	return Content.trimmed();
}

// Load the product structure
void GLC_3dxmlToWorld::loadProductStructure()
{
	// Get the root of the 3dxml
	if (not m_p3dxmlArchive->setCurrentFile(m_RootName, QuaZip::csInsensitive))
	{
		QString message(QString("GLC_3dxmlToWorld::loadProductStructure Root Structure ") + m_RootName + " Not found");
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}

	// Open the root of the 3dxml
	if(not m_p3dxmlFile->open(QIODevice::ReadOnly))
    {
		QString message(QString("GLC_3dxmlToWorld::loadProductStructure Unable to Open ") + m_RootName);
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::FileNotSupported);
		clear();
		throw(fileFormatException);
    }

	// Bind the reader to the root
	m_pStreamReader= new QXmlStreamReader(m_p3dxmlFile);

	goToElement("ProductStructure");
	if (m_pStreamReader->atEnd() or m_pStreamReader->hasError())
	{
		QString message(QString("GLC_3dxmlToWorld::loadProductStructure Element ProctStructure Not found in ") + m_FileName);
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}
	// Load the structure
	while(not m_pStreamReader->atEnd() and not m_pStreamReader->hasError())
	{
		if ((QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
				and ((m_pStreamReader->name() == "Reference3D") or (m_pStreamReader->name() == "Instance3D")))
		{
			if (m_pStreamReader->name() == "Reference3D") loadReference3D();
			else loadInstance3D();
		}
		m_pStreamReader->readNext();
	}

	// Check if an error Occur
	if (m_pStreamReader->hasError())
	{
		QString message(QString("GLC_3dxmlToWorld::loadProductStructure An error occur in ") + m_FileName);
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}

	delete m_pStreamReader;
	m_pStreamReader= NULL;

	m_p3dxmlFile->close();

	// Load external ref
	loadExternalRef3D();

	{ // Link locals instance with reference
		InstanceOfHash::iterator iInstance= m_InstanceOf.begin();
		while (iInstance != m_InstanceOf.constEnd())
		{
			GLC_StructInstance* pInstance= iInstance.key();
			GLC_StructReference* pRef= m_ReferenceHash.value(iInstance.value());
			pInstance->setReference(pRef);

			++iInstance;
		}
		m_InstanceOf.clear();

	}

	{ // Link external instance with reference

		InstanceOfExtRefHash::iterator iInstance= m_InstanceOfExtRefHash.begin();
		while (iInstance != m_InstanceOfExtRefHash.constEnd())
		{
			GLC_StructInstance* pInstance= iInstance.key();
			GLC_StructReference* pRef= m_ExternalReferenceHash.value(iInstance.value());
			pInstance->setReference(pRef);

			++iInstance;
		}
		m_InstanceOfExtRefHash.clear();
		m_ExternalReferenceHash.clear();

	}

	// Create the unfolded tree
	createUnfoldedTree();

}

// Load a Reference3D
void GLC_3dxmlToWorld::loadReference3D()
{
	const unsigned int id= m_pStreamReader->attributes().value("id").toString().toUInt();
	const QString refName(m_pStreamReader->attributes().value("name").toString());
	GLC_StructReference* pStructReference;

	if (id == 1) // This is the root reference.
	{
		m_pWorld= new GLC_World();
		m_pWorld->setRootName(refName);
		pStructReference= m_pWorld->rootOccurence()->structInstance()->structReference();
		pStructReference->setName(refName);
	}
	else
	{
		pStructReference= new GLC_StructReference(refName);
	}

	m_ReferenceHash.insert(id, pStructReference);
}

// Load a Instance3D
void GLC_3dxmlToWorld::loadInstance3D()
{
	const QString local= "urn:3DXML:Reference:loc:";
	const QString externRef= "urn:3DXML:Reference:ext:";

	const QString instName(m_pStreamReader->attributes().value("name").toString());
	const unsigned int aggregatedById= getContent("IsAggregatedBy").toUInt();
	QString instanceOf= getContent("IsInstanceOf");
	const QString matrixString= getContent("RelativeMatrix");
	GLC_Matrix4x4 instanceMatrix= loadMatrix(matrixString);


	GLC_StructInstance* pStructInstance;

	if (instanceOf.contains(local))
	{
		const unsigned int refId= instanceOf.remove(local).toUInt();
		pStructInstance= new GLC_StructInstance(instName);
		pStructInstance->move(instanceMatrix);
		m_InstanceOf.insert(pStructInstance, refId);
	}
	else
	{
		const QString extRefId= instanceOf.remove(externRef).remove("#1");
		m_SetOfExtRef << extRefId;
		pStructInstance= new GLC_StructInstance(instName);
		pStructInstance->move(instanceMatrix);
		m_InstanceOfExtRefHash.insert(pStructInstance, extRefId);
	}

	AssyLink assyLink;
	assyLink.m_ParentRefId= aggregatedById;
	assyLink.m_pChildInstance= pStructInstance;

	m_AssyLinkList.append(assyLink);
}

// Load External Ref
void GLC_3dxmlToWorld::loadExternalRef3D()
{
	const int size= m_SetOfExtRef.size();
	int previousQuantumValue= 0;
	int currentQuantumValue= 0;
	int currentFileIndex= 0;
	emit currentQuantum(currentQuantumValue);

	SetOfExtRef::iterator iExtRef= m_SetOfExtRef.begin();
	while (iExtRef != m_SetOfExtRef.constEnd())
	{
		const QString currentRefFileName= (*iExtRef);

		// Get the refFile of the 3dxml
		if (not m_p3dxmlArchive->setCurrentFile(currentRefFileName, QuaZip::csInsensitive))
		{
			QString message(QString("GLC_3dxmlToWorld::loadExternalRef3D File ") + m_FileName + " doesn't contains " + currentRefFileName);
			GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
			clear();
			throw(fileFormatException);
		}

		// Open the current external file of the 3dxml
		if(not m_p3dxmlFile->open(QIODevice::ReadOnly))
	    {
			QString message(QString("GLC_3dxmlToWorld::loadExternalRef3D Unable to Open ") + currentRefFileName);
			GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::FileNotSupported);
			clear();
			throw(fileFormatException);
	    }

		m_pStreamReader= new QXmlStreamReader(m_p3dxmlFile);

		m_ExternalReferenceHash.insert(currentRefFileName, addExtenalRef());
		++currentFileIndex;
		// Progrees bar indicator
		currentQuantumValue = static_cast<int>((static_cast<double>(currentFileIndex) / size) * 100);
		if (currentQuantumValue > previousQuantumValue)
		{
			emit currentQuantum(currentQuantumValue);
		}
		previousQuantumValue= currentQuantumValue;

		delete m_pStreamReader;
		m_pStreamReader= NULL;
		m_p3dxmlFile->close();

		++iExtRef;
	}
	m_SetOfExtRef.clear();

}

// Create Instance from 3DXML Rep
GLC_StructReference* GLC_3dxmlToWorld::addExtenalRef()
{
	GLC_Instance loadInstance(new GLC_Box(20.0, 20.0, 20.0));

	GLC_StructReference* pStructReference= new GLC_StructReference(loadInstance);

	return pStructReference;
}

// Load Matrix
GLC_Matrix4x4 GLC_3dxmlToWorld::loadMatrix(const QString& stringMatrix)
{

	QStringList stringValues(stringMatrix.split(" "));

	QVector<double> values(16);
	// Rotation
	values[0]= stringValues[0].toDouble();
	values[1]= stringValues[1].toDouble();
	values[2]= stringValues[2].toDouble();
	values[3]= 0.0;
	values[4]= stringValues[3].toDouble();
	values[5]= stringValues[4].toDouble();
	values[6]= stringValues[5].toDouble();
	values[7]= 0.0;
	values[8]= stringValues[6].toDouble();
	values[9]= stringValues[7].toDouble();
	values[10]= stringValues[8].toDouble();
	values[11]= 0.0;
	// Translation
	values[12]= stringValues[9].toDouble();
	values[13]= stringValues[10].toDouble();
	values[14]= stringValues[11].toDouble();
	values[15]= 1.0;

	return GLC_Matrix4x4(values.data());
}

// Create the unfolded  tree
void GLC_3dxmlToWorld::createUnfoldedTree()
{
	// Run throw all link in the list of link
	AssyLinkList::iterator iLink= m_AssyLinkList.begin();
	while(iLink != m_AssyLinkList.constEnd())
	{
		GLC_StructInstance* pChildInstance= (*iLink).m_pChildInstance;
		if (pChildInstance->structReference() == NULL)
		{
			pChildInstance->setReference(new GLC_StructReference("Part"));
		}
		GLC_StructReference* pRef= m_ReferenceHash.value((*iLink).m_ParentRefId);
		// Attach pInstance at all Occurence of pRef
		QList<GLC_StructInstance*> instanceList= pRef->listOfStructInstances();
		const int instanceNumber= instanceList.size();
		for (int i= 0; i < instanceNumber; ++i)
		{
			if (instanceList.at(i)->haveStructOccurence())
			{
				QList<GLC_StructOccurence*> occurenceList= instanceList.at(i)->listOfStructOccurences();
				const int occurenceNumber= occurenceList.size();
				for (int j= 0; j < occurenceNumber; ++j)
				{
					if (pChildInstance->haveStructOccurence() and pChildInstance->firstOccurenceHandle()->isOrphan())
					{
						Q_ASSERT(pChildInstance->listOfStructOccurences().size() == 1);
						occurenceList.at(j)->addChild(pChildInstance->firstOccurenceHandle());
					}
					else
					{
						occurenceList.at(j)->addChild(pChildInstance);
					}
				}
			}
			else
			{
				GLC_StructOccurence* pOccurence= new GLC_StructOccurence(m_pWorld->collection(), instanceList.at(i));

				if (pChildInstance->haveStructOccurence() and pChildInstance->firstOccurenceHandle()->isOrphan())
				{
					Q_ASSERT(pChildInstance->listOfStructOccurences().size() == 1);
					pOccurence->addChild(pChildInstance->firstOccurenceHandle());
				}
				else
				{
					pOccurence->addChild(pChildInstance);
				}
			}
		}

		++iLink;
	}
	m_AssyLinkList.clear();
	m_ReferenceHash.clear();

}
