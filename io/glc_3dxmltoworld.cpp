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
#include "../sceneGraph/glc_world.h"
#include "../glc_fileformatexception.h"
#include "../geometry/glc_extendedmesh.h"

// Quazip library
#include "../quazip/quazip.h"
#include "../quazip/quazipfile.h"

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
, m_pCurrentFile(NULL)
, m_RootName()
, m_pWorld(NULL)
, m_ReferenceHash()
, m_AssyLinkList()
, m_InstanceOf()
, m_SetOfExtRef()
, m_InstanceOfExtRefHash()
, m_ExternalReferenceHash()
, m_MaterialHash()
, m_IsInArchive(false)
, m_ReferenceRepHash()
, m_LocalRepLinkList()
, m_ExternRepLinkList()
, m_SetOfExtRep()
{

}

GLC_3dxmlToWorld::~GLC_3dxmlToWorld()
{
	delete m_pStreamReader;
	m_pStreamReader= NULL;

	// Clear current file
	if (NULL != m_pCurrentFile)
	{
		m_pCurrentFile->close();
		delete m_pCurrentFile;
		m_pCurrentFile= NULL;
	}

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
	clearMaterialHash();
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
	  // In this case, the 3dxml is not compressed or is not valid
	  m_RootName= m_FileName;
	  delete m_p3dxmlArchive;
	  m_p3dxmlArchive= NULL;
	}
	else
	{
		m_IsInArchive= true;
		// Set the file Name Codec
		m_p3dxmlArchive->setFileNameCodec("IBM866");

		// Load the manifest
		loadManifest();
	}

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
	setStreamReaderToFile("Manifest.xml");
	m_RootName= getContent("Root");

	if (m_pStreamReader->atEnd() or m_pStreamReader->hasError())
	{
		QString message(QString("GLC_3dxmlToWorld::loadManifest Manifest file ") + m_FileName + " doesn't contains Root Element");
		qDebug() << message;
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

	// Clear current file
	if (NULL != m_pCurrentFile)
	{
		m_pCurrentFile->close();
		delete m_pCurrentFile;
		m_pCurrentFile= NULL;
	}

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

	clearMaterialHash();
}

// Go to Element
void GLC_3dxmlToWorld::goToElement(const QString& elementName)
{
	while(startElementNotReached(elementName))
	{
		m_pStreamReader->readNext();
	}
}

// Go to a Rep of a xml
void GLC_3dxmlToWorld::goToRepId(const QString& id)
{
	while(not m_pStreamReader->atEnd() and not ((QXmlStreamReader::StartElement == m_pStreamReader->tokenType()) and (m_pStreamReader->name() == "Representation")
			and (m_pStreamReader->attributes().value("id").toString() == id)))
	{
		m_pStreamReader->readNext();
	}

}

// Go to Polygonal Rep Type
void GLC_3dxmlToWorld::gotToPolygonalRepType()
{
	while(not m_pStreamReader->atEnd() and not ((QXmlStreamReader::StartElement == m_pStreamReader->tokenType()) and (m_pStreamReader->name() == "Rep")
			and (m_pStreamReader->attributes().value("xsi:type").toString() == "PolygonalRepType")))
	{
		//qDebug() << m_pStreamReader->name();
		//qDebug() << m_pStreamReader->attributes().value("xsi:type").toString();
		m_pStreamReader->readNext();
	}

}

// Return the content of an element
QString GLC_3dxmlToWorld::getContent(const QString& element)
{
	QString Content;
	while(endElementNotReached(element))
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
	setStreamReaderToFile(m_RootName);

	goToElement("ProductStructure");
	if (m_pStreamReader->atEnd() or m_pStreamReader->hasError())
	{
		QString message(QString("GLC_3dxmlToWorld::loadProductStructure Element ProctStructure Not found in ") + m_FileName);
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}
	// Load the structure
	while(endElementNotReached("ProductStructure"))
	{
		if ((QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
				and ((m_pStreamReader->name() == "Reference3D") or (m_pStreamReader->name() == "Instance3D")
						or (m_pStreamReader->name() == "ReferenceRep") or (m_pStreamReader->name() == "InstanceRep")))
		{
			if (m_pStreamReader->name() == "Reference3D") loadReference3D();
			else if (m_pStreamReader->name() == "Instance3D") loadInstance3D();
			else if (m_pStreamReader->name() == "ReferenceRep") loadReferenceRep();
			else loadInstanceRep();
		}
		m_pStreamReader->readNext();
	}

	// Check if an error Occur
	if (m_pStreamReader->hasError())
	{
		QString message(QString("GLC_3dxmlToWorld::loadProductStructure An error occur in ") + m_FileName);
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}

	// Load local representation ref
	loadLocalRepresentations();

	// Load external ref
	loadExternalRef3D();

	// Load extern representations
	loadExternRepresentations();

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
	qDebug() << "Local instance linked with reference";

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
	qDebug() << "external instance linked with reference";

	// Create the unfolded tree
	createUnfoldedTree();

	qDebug() << "Unfolded tree created";

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

	if (instanceOf.contains(externRef))
	{
		const QString extRefId= instanceOf.remove(externRef).remove("#1");
		m_SetOfExtRef << extRefId;
		pStructInstance= new GLC_StructInstance(instName);
		pStructInstance->move(instanceMatrix);
		m_InstanceOfExtRefHash.insert(pStructInstance, extRefId);
	}
	else if (instanceOf.contains(local))
	{
		const unsigned int refId= instanceOf.remove(local).toUInt();
		pStructInstance= new GLC_StructInstance(instName);
		pStructInstance->move(instanceMatrix);
		m_InstanceOf.insert(pStructInstance, refId);
	}
	else
	{
		// 3dvia 3dxml
		const unsigned int refId= instanceOf.toUInt();
		pStructInstance= new GLC_StructInstance(instName);
		pStructInstance->move(instanceMatrix);
		m_InstanceOf.insert(pStructInstance, refId);
	}

	AssyLink assyLink;
	assyLink.m_ParentRefId= aggregatedById;
	assyLink.m_pChildInstance= pStructInstance;

	m_AssyLinkList.append(assyLink);
}

// Load a Reference representation
void GLC_3dxmlToWorld::loadReferenceRep()
{
	const QString local= "urn:3DXML:Representation:loc:";
	const QString externName= "urn:3DXML:";

	const unsigned int id= m_pStreamReader->attributes().value("id").toString().toUInt();
	const QString refName(m_pStreamReader->attributes().value("name").toString());
	QString associatedFile(m_pStreamReader->attributes().value("associatedFile").toString());

	if (associatedFile.contains(local))
	{
		const QString repId= associatedFile.remove(local);
		m_ReferenceRepHash.insert(id, repId);
	}
	else if (associatedFile.contains(externName))
	{
		const QString repId= associatedFile.remove(externName);
		m_ReferenceRepHash.insert(id, repId);
	}
}

// Load a Instance representation
void GLC_3dxmlToWorld::loadInstanceRep()
{
	const QString local= "urn:3DXML:Reference:loc:";

	const QString instName(m_pStreamReader->attributes().value("name").toString());
	const unsigned int aggregatedById= getContent("IsAggregatedBy").toUInt();
	QString instanceOf= getContent("IsInstanceOf");

	if (instanceOf.contains(local))
	{
		// The 3dxml is a 3dxml rep from CATIA V5
		const unsigned int refId= instanceOf.remove(local).toUInt();

		RepLink repLink;
		repLink.m_ReferenceId= aggregatedById;
		repLink.m_RepId= refId;

		m_LocalRepLinkList.append(repLink);
	}
	else
	{
		// The 3dxml is a 3dvia 3dxml
		const unsigned int refId= instanceOf.toUInt();
		RepLink repLink;
		repLink.m_ReferenceId= aggregatedById;
		repLink.m_RepId= refId;

		m_ExternRepLinkList.append(repLink);
	}
}

// Load External Ref
void GLC_3dxmlToWorld::loadExternalRef3D()
{
	if (m_SetOfExtRef.isEmpty()) return;

	const int size= m_SetOfExtRef.size();
	int previousQuantumValue= 0;
	int currentQuantumValue= 0;
	int currentFileIndex= 0;
	emit currentQuantum(currentQuantumValue);

	SetOfExtRef::iterator iExtRef= m_SetOfExtRef.begin();
	while (iExtRef != m_SetOfExtRef.constEnd())
	{

		const QString currentRefFileName= (*iExtRef);
		//qDebug() << "Current File name : " << currentRefFileName;
		// Get the refFile of the 3dxml
		setStreamReaderToFile(currentRefFileName);

		m_ExternalReferenceHash.insert(currentRefFileName, createReferenceRep());
		++currentFileIndex;
		// Progrees bar indicator
		currentQuantumValue = static_cast<int>((static_cast<double>(currentFileIndex) / size) * 100);
		if (currentQuantumValue > previousQuantumValue)
		{
			emit currentQuantum(currentQuantumValue);
		}
		previousQuantumValue= currentQuantumValue;

		++iExtRef;
	}
	m_SetOfExtRef.clear();

}

// Create Instance from 3DXML Rep
GLC_StructReference* GLC_3dxmlToWorld::createReferenceRep(QString repId)
{
	QString refName;

	if (repId.isEmpty())
	{
		goToElement("ProductStructure");
		checkForXmlError("Element ProductStructure not found");

		goToElement("Reference3D");
		checkForXmlError("Element Reference3D not found");
		refName= m_pStreamReader->attributes().value("name").toString();

		goToElement("ReferenceRep");
		checkForXmlError("Element ReferenceRep not found");

		const QString format(m_pStreamReader->attributes().value("format").toString());
		checkForXmlError("attributes format not found");
		if (format != "TESSELLATED")
		{
			QString message(QString("GLC_3dxmlToWorld::addExtenalRef 3D rep format ") + format + " Not Supported");
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::FileNotSupported);
			clear();
			throw(fileFormatException);
		}
		const QString local= "urn:3DXML:Representation:loc:";
		repId= m_pStreamReader->attributes().value("associatedFile").toString().remove(local);
		checkForXmlError("attribute associatedFile not found");
		goToRepId(repId);
		checkForXmlError("repId not found");
	}

	GLC_ExtendedMesh* pMesh= new GLC_ExtendedMesh();
	pMesh->setName(refName);
	GLC_Instance currentMeshInstance(pMesh);

	int numberOfMesh= 1;
	while (endElementNotReached("Representation"))
	{
		gotToPolygonalRepType();
		if (m_pStreamReader->atEnd() or m_pStreamReader->hasError())
		{
			if (m_pStreamReader->hasError())
			{
				QString message(QString("An element have not been found in file ") + m_FileName);
				qDebug() << m_pStreamReader->errorString();
				GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
				clear();
				throw(fileFormatException);
			}
			else
			{
				if (numberOfMesh > 1)
				{
					pMesh->finished();
					return new GLC_StructReference(currentMeshInstance);
				}
				else
				{
					return new GLC_StructReference(refName);
				}
			}
		}
		if (numberOfMesh > 1)
		{
			pMesh->finished();
			pMesh = new GLC_ExtendedMesh();
			pMesh->setName(refName);
			currentMeshInstance.setGeometry(pMesh);
		}

		goToMasterLOD();
		if (m_pStreamReader->atEnd() or m_pStreamReader->hasError())
		{
			qDebug() << " Master LOD not found";
			return new GLC_StructReference("Empty Rep");
		}

		// Load Faces index data
		while (endElementNotReached("Faces"))
		{
			m_pStreamReader->readNext();
			if ( m_pStreamReader->name() == "Face")
			{
				loadFace(pMesh);
			}
		}
		checkForXmlError("End of Faces not found");

		goToElement("VertexBuffer");
		checkForXmlError("Element VertexBuffer not found");

		{
			QString verticePosition= getContent("Positions").replace(',', ' ');
			//qDebug() << "Position " << verticePosition;
			checkForXmlError("Error while retrieving Position ContentVertexBuffer");
			// Load Vertice position
			QTextStream verticeStream(&verticePosition);
			QList<GLfloat> verticeValues;
			QString buff;
			while ((!verticeStream.atEnd()))
			{
				verticeStream >> buff;
				verticeValues.append(buff.toFloat());
			}
			pMesh->addVertices(verticeValues.toVector());

		}

		{
			QString normals= getContent("Normals").replace(',', ' ');
			//qDebug() << "Normals " << normals;
			checkForXmlError("Error while retrieving Normals values");
			// Load Vertice Normals
			QTextStream normalsStream(&normals);
			QList<GLfloat> normalValues;
			QString buff;
			while ((!normalsStream.atEnd()))
			{
				normalsStream >> buff;
				normalValues.append(buff.toFloat());
			}
			pMesh->addNormals(normalValues.toVector());
		}
		++numberOfMesh;
	}

	pMesh->finished();

	return new GLC_StructReference(currentMeshInstance);
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
	qDebug() << "createUnfoldedTree";
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

	// Update position
	m_pWorld->rootOccurence()->updateChildsAbsoluteMatrix();

}
// Check for XML error
void GLC_3dxmlToWorld::checkForXmlError(const QString& info)
{
	if (m_pStreamReader->atEnd() or m_pStreamReader->hasError())
	{
		QString message(QString("An element have not been found in file ") + m_FileName);
		qDebug() << info << " " << m_pStreamReader->errorString();
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}
}
// Go to the master LOD
void GLC_3dxmlToWorld::goToMasterLOD()
{
	while(not m_pStreamReader->atEnd() and not ((QXmlStreamReader::StartElement == m_pStreamReader->tokenType()) and (m_pStreamReader->name() == "Faces")))
	{
		m_pStreamReader->readNext();
		if ((QXmlStreamReader::StartElement == m_pStreamReader->tokenType()) and (m_pStreamReader->name() == "PolygonalLOD"))
		{
			// Skip the load
			while(endElementNotReached("PolygonalLOD"))
			{
				m_pStreamReader->readNext();
			}
		}
	}
}
// Load a face
void GLC_3dxmlToWorld::loadFace(GLC_ExtendedMesh* pMesh)
{
	//qDebug() << "GLC_3dxmlToWorld::loadFace" << m_pStreamReader->name();
	// List of index declaration

	QString triangles= m_pStreamReader->attributes().value("triangles").toString().trimmed();
	QString strips= m_pStreamReader->attributes().value("strips").toString().trimmed();
	QString fans= m_pStreamReader->attributes().value("fans").toString().trimmed();

	GLC_Material* pCurrentMaterial= NULL;

	while(endElementNotReached("Face"))
	{
		if ((QXmlStreamReader::StartElement == m_pStreamReader->tokenType()) and (m_pStreamReader->name() == "Color"))
		{
			pCurrentMaterial= getMaterial();
		}
		m_pStreamReader->readNext();
	}

	// Trying to find triangles
	if (not triangles.isEmpty())
	{
		// For 3dvia mesh
		if (triangles.contains(','))
		{
			triangles.remove(',');
		}
		QTextStream trianglesStream(&triangles);
		IndexList trianglesIndex;
		QString buff;
		while ((!trianglesStream.atEnd()))
		{
			trianglesStream >> buff;
			trianglesIndex.append(buff.toUInt());
		}
		pMesh->addTriangles(pCurrentMaterial, trianglesIndex);
	}
	// Trying to find trips
	if (not strips.isEmpty())
	{

		QStringList stripsList(strips.split(","));
		const int size= stripsList.size();
		for (int i= 0; i < size; ++i)
		{
			//qDebug() << "Strip " << stripsList.at(i);
			QTextStream stripsStream(&stripsList[i]);
			IndexList stripsIndex;
			QString buff;
			while ((!stripsStream.atEnd()))
			{
				stripsStream >> buff;
				stripsIndex.append(buff.toUInt());
			}
			pMesh->addTrianglesStrip(pCurrentMaterial, stripsIndex);
		}
	}
	// Trying to find fans
	if (not fans.isEmpty())
	{
		QStringList fansList(fans.split(","));
		const int size= fansList.size();
		for (int i= 0; i < size; ++i)
		{
			QTextStream fansStream(&fansList[i]);
			IndexList fansIndex;
			QString buff;
			while ((!fansStream.atEnd()))
			{
				fansStream >> buff;
				fansIndex.append(buff.toUInt());
			}
			pMesh->addTrianglesFan(pCurrentMaterial, fansIndex);
		}
	}

}

// Clear material hash
void GLC_3dxmlToWorld::clearMaterialHash()
{
	MaterialHash::iterator iMaterial= m_MaterialHash.begin();
	while (m_MaterialHash.constEnd() != iMaterial)
	{
		if (iMaterial.value()->isUnused())
		{
			delete iMaterial.value();
		}
		++iMaterial;
	}
	m_MaterialHash.clear();
}

// get material
GLC_Material* GLC_3dxmlToWorld::getMaterial()
{
	GLC_Material* pMaterial= NULL;
	const QString red(m_pStreamReader->attributes().value("red").toString());
	const QString green(m_pStreamReader->attributes().value("green").toString());
	const QString blue(m_pStreamReader->attributes().value("blue").toString());

	const QString matKey= red + green + blue;
	if (m_MaterialHash.contains(matKey))
	{
		pMaterial= m_MaterialHash.value(matKey);
	}
	else
	{
		qreal redReal= red.toDouble();
		qreal greenReal= green.toDouble();
		qreal blueReal= blue.toDouble();
		QColor diffuse;
		diffuse.setRgbF(redReal, greenReal, blueReal);
		pMaterial= new GLC_Material(diffuse);
		pMaterial->setName("Material_" + QString::number(m_MaterialHash.size()));
		pMaterial->setAmbientColor(Qt::black);
		pMaterial->setSpecularColor(Qt::white);
		pMaterial->setShininess(25.0);
		m_MaterialHash.insert(matKey, pMaterial);
	}

	return pMaterial;
}

// Set the stream reader to the specified file
bool GLC_3dxmlToWorld::setStreamReaderToFile(QString fileName)
{
	if (m_IsInArchive)
	{
		delete m_p3dxmlFile;
		// Create QuaZip File
		m_p3dxmlFile= new QuaZipFile(m_p3dxmlArchive);

		// Get the manifest of the 3dxml
		if (not m_p3dxmlArchive->setCurrentFile(fileName, QuaZip::csInsensitive))
		{
			QString message(QString("GLC_3dxmlToWorld::setStreamReaderToFile File ") + m_FileName + " doesn't contains " + fileName);
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
			clear();
			throw(fileFormatException);
		}

		// Open the manifest of the 3dxml
		if(not m_p3dxmlFile->open(QIODevice::ReadOnly))
	    {
			QString message(QString("GLC_3dxmlToWorld::setStreamReaderToFile Unable to Open ") + fileName);
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, fileName, GLC_FileFormatException::FileNotSupported);
			clear();
			throw(fileFormatException);
	    }

		// Set the stream reader
		m_pStreamReader= new QXmlStreamReader(m_p3dxmlFile);
	}
	else
	{
		delete m_pCurrentFile;
		// Create the file to load
		if (fileName != m_FileName)
		{
			fileName= QFileInfo(m_FileName).absolutePath() + QDir::separator() + fileName;
		}
		m_pCurrentFile= new QFile(fileName);
		if (not m_pCurrentFile->open(QIODevice::ReadOnly))
		{
			QString message(QString("GLC_3dxmlToWorld::setStreamReaderToFile File ") + fileName + QString(" not found"));
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, fileName, GLC_FileFormatException::FileNotFound);
			clear();
			throw(fileFormatException);
		}
		// Set the stream reader
		m_pStreamReader= new QXmlStreamReader(m_pCurrentFile);
	}
	return true;
}

// Load the local representation
void GLC_3dxmlToWorld::loadLocalRepresentations()
{
	if (m_LocalRepLinkList.isEmpty()) return;
	QHash<const QString, GLC_Instance> repHash;

	// Load all local ref
	goToElement("GeometricRepresentationSet");
	while (endElementNotReached("GeometricRepresentationSet"))
	{
		if (m_pStreamReader->name() == "Representation")
		{
			QString id= m_pStreamReader->attributes().value("id").toString();

			GLC_StructReference* pRef= createReferenceRep("Local");
			GLC_Instance instance= pRef->instanceRepresentation();
			delete pRef;
			repHash.insert(id, instance);
		}
		m_pStreamReader->readNext();
	}
	qDebug() << "Local rep loaded";

	// Attach the ref to the structure reference
	RepLinkList::iterator iLocalRep= m_LocalRepLinkList.begin();
	while (iLocalRep != m_LocalRepLinkList.constEnd())
	{
		unsigned int referenceId= (*iLocalRep).m_ReferenceId;
		unsigned int refId= (*iLocalRep).m_RepId;

		GLC_StructReference* pReference= m_ReferenceHash.value(referenceId);
		const QString representationID= m_ReferenceRepHash.value(refId);
		pReference->setRepresentation(repHash.value(representationID));

		++iLocalRep;
	}
}

// Load the extern representation
void GLC_3dxmlToWorld::loadExternRepresentations()
{
	if (m_ExternRepLinkList.isEmpty()) return;

	QHash<const unsigned int, GLC_Instance> repHash;

	// Progress bar variables
	const int size= m_ReferenceRepHash.size();
	int previousQuantumValue= 0;
	int currentQuantumValue= 0;
	int currentFileIndex= 0;
	emit currentQuantum(currentQuantumValue);

	// Load all external rep
	ReferenceRepHash::iterator iRefRep= m_ReferenceRepHash.begin();
	while (iRefRep != m_ReferenceRepHash.constEnd())
	{
		const QString currentRefFileName= iRefRep.value();
		const unsigned int id= iRefRep.key();

		setStreamReaderToFile(currentRefFileName);

		GLC_Instance instance= loadCurrentExtRep();
		repHash.insert(id, instance);

		// Progrees bar indicator
		++currentFileIndex;
		currentQuantumValue = static_cast<int>((static_cast<double>(currentFileIndex) / size) * 100);
		if (currentQuantumValue > previousQuantumValue)
		{
			emit currentQuantum(currentQuantumValue);
		}
		previousQuantumValue= currentQuantumValue;

		++iRefRep;
	}

	// Attach the ref to the structure reference
	RepLinkList::iterator iExtRep= m_ExternRepLinkList.begin();
	while (iExtRep != m_ExternRepLinkList.constEnd())
	{
		unsigned int referenceId= (*iExtRep).m_ReferenceId;
		unsigned int refId= (*iExtRep).m_RepId;

		GLC_StructReference* pReference= m_ReferenceHash.value(referenceId);
		const QString representationID= m_ReferenceRepHash.value(refId);
		pReference->setRepresentation(repHash.value(refId));

		++iExtRep;
	}

}

// Return the instance of the current extern representation
GLC_Instance GLC_3dxmlToWorld::loadCurrentExtRep()
{
	GLC_ExtendedMesh* pMesh= new GLC_ExtendedMesh();
	GLC_Instance currentMeshInstance(pMesh);

	goToElement("Faces");
	while (endElementNotReached("Faces"))
	{
		m_pStreamReader->readNext();
		if ( m_pStreamReader->name() == "Face")
		{
			loadFace(pMesh);
		}
	}
	checkForXmlError("End of Faces not found");

	goToElement("VertexBuffer");
	checkForXmlError("Element VertexBuffer not found");

	{
		QString verticePosition= getContent("Positions").replace(',', ' ');
		//qDebug() << "Position " << verticePosition;
		checkForXmlError("Error while retrieving Position ContentVertexBuffer");
		// Load Vertice position
		QTextStream verticeStream(&verticePosition);
		QList<GLfloat> verticeValues;
		QString buff;
		while ((!verticeStream.atEnd()))
		{
			verticeStream >> buff;
			verticeValues.append(buff.toFloat());
		}
		pMesh->addVertices(verticeValues.toVector());

	}

	{
		QString normals= getContent("Normals").replace(',', ' ');
		//qDebug() << "Normals " << normals;
		checkForXmlError("Error while retrieving Normals values");
		// Load Vertice Normals
		QTextStream normalsStream(&normals);
		QList<GLfloat> normalValues;
		QString buff;
		while ((!normalsStream.atEnd()))
		{
			normalsStream >> buff;
			normalValues.append(buff.toFloat());
		}
		pMesh->addNormals(normalValues.toVector());
	}

	pMesh->finished();

	return currentMeshInstance;

}
