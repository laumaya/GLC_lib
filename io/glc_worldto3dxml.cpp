/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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
//! \file glc_worldto3dxml.cpp implementation of the GLC_WorldTo3dxml class.

#include "glc_worldto3dxml.h"
// Quazip library
#include "../quazip/quazip.h"
#include "../quazip/quazipfile.h"
#include "../glc_exception.h"
#include "../geometry/glc_mesh.h"

#include <QFileInfo>

GLC_WorldTo3dxml::GLC_WorldTo3dxml(const GLC_World& world)
: QObject()
, m_World(world)
, m_FileName()
, m_pOutStream(NULL)
, m_Generator("GLC_LIB")
, m_CurrentId(0)
, m_p3dxmlArchive(NULL)
, m_pCurrentZipFile(NULL)
, m_pCurrentFile(NULL)
, m_AbsolutePath()
, m_ReferenceToIdHash()
, m_InstanceToIdHash()
, m_ReferenceRepToIdHash()
{
	m_World.rootOccurence()->updateOccurenceNumber(1);
}

GLC_WorldTo3dxml::~GLC_WorldTo3dxml()
{
	delete m_p3dxmlArchive;
	delete m_pCurrentZipFile;
	delete m_pCurrentFile;
}

bool GLC_WorldTo3dxml::exportToFile(const QString& filename, GLC_WorldTo3dxml::ExportType exportType)
{
	m_FileName= filename;
	bool isExported= false;
	if (exportType == Compressed3dxml)
	{
		m_p3dxmlArchive= new QuaZip(m_FileName);
		isExported= m_p3dxmlArchive->open(QuaZip::mdCreate);
		// Add the manifest
		addManifest();

	}
	else
	{
		m_AbsolutePath= QFileInfo(m_FileName).absolutePath() + QDir::separator();
		QFile exportFile(m_FileName);
		isExported= exportFile.open(QIODevice::WriteOnly);
		exportFile.close();
	}
	if (isExported)
	{

		// Export the assembly structure from the list of structure reference
		exportAssemblyStructure();

		// Export the representation
		QHash<const GLC_3DRep*, unsigned int>::const_iterator iRep= m_ReferenceRepToIdHash.constBegin();
		while (m_ReferenceRepToIdHash.constEnd() != iRep)
		{
			write3DRep(iRep.key());
			++iRep;
		}

	}


	return isExported;
}

void GLC_WorldTo3dxml::writeHeader()
{
	const QString title(QFileInfo(m_FileName).fileName());

	m_pOutStream->writeStartElement("Header");
		m_pOutStream->writeTextElement("ShemaVersion", "4.0");
		m_pOutStream->writeTextElement("Title", title);
		m_pOutStream->writeTextElement("Generator", m_Generator);
		m_pOutStream->writeTextElement("Created", QDate::currentDate().toString("YYYY-MM-DD"));
	m_pOutStream->writeEndElement();
}

void GLC_WorldTo3dxml::writeReference3D(const GLC_StructReference* pRef)
{
	m_pOutStream->writeStartElement("Reference3D");
		m_pOutStream->writeAttribute("xsi:type", "Reference3DType");
		m_pOutStream->writeAttribute("id", QString::number(++m_CurrentId));
		m_pOutStream->writeAttribute("name", pRef->name());
	m_pOutStream->writeEndElement();

	m_ReferenceToIdHash.insert(pRef, m_CurrentId);
}

void GLC_WorldTo3dxml::writeReferenceRep(const GLC_3DRep* p3DRep)
{
	const QString id(QString::number(++m_CurrentId));
	m_ReferenceRepToIdHash.insert(p3DRep, m_CurrentId);
	const QString associateFile(representationFileName(p3DRep));

	m_pOutStream->writeStartElement("ReferenceRep");
		m_pOutStream->writeAttribute("xsi:type", "ReferenceRepType");
		m_pOutStream->writeAttribute("id", id);
		m_pOutStream->writeAttribute("name", p3DRep->name());
		m_pOutStream->writeAttribute("format", "TESSELATED");
		m_pOutStream->writeAttribute("associatedFile", associateFile);
		m_pOutStream->writeAttribute("version", "1.2");
	m_pOutStream->writeEndElement();


}
void GLC_WorldTo3dxml::writeInstance3D(const GLC_StructInstance* pInstance, unsigned int parentId)
{
	const GLC_StructReference* pRef= pInstance->structReference();
	const unsigned int referenceId= m_ReferenceToIdHash.value(pRef);
	const QString instanceMatrix(matrixString(pInstance->relativeMatrix()));

	m_pOutStream->writeStartElement("Instance3D");
		m_pOutStream->writeAttribute("xsi:type", "Instance3DType");
		m_pOutStream->writeAttribute("id", QString::number(++m_CurrentId));
		m_pOutStream->writeAttribute("name", pInstance->name());
		m_pOutStream->writeTextElement("IsAggregatedBy", QString::number(parentId));
		m_pOutStream->writeTextElement("IsInstanceOf", QString::number(referenceId));
		m_pOutStream->writeTextElement("RelativeMatrix", instanceMatrix);
	m_pOutStream->writeEndElement();

	m_InstanceToIdHash.insert(pInstance, m_CurrentId);
}

void  GLC_WorldTo3dxml::writeInstanceRep(const GLC_3DRep* p3DRep, unsigned int parentId)
{
	const unsigned int referenceId= m_ReferenceRepToIdHash.value(p3DRep);
	m_pOutStream->writeStartElement("InstanceRep");
		m_pOutStream->writeAttribute("xsi:type", "InstanceRepType");
		m_pOutStream->writeAttribute("id", QString::number(++m_CurrentId));
		m_pOutStream->writeAttribute("name", p3DRep->name());
		m_pOutStream->writeTextElement("IsAggregatedBy", QString::number(parentId));
		m_pOutStream->writeTextElement("IsInstanceOf", QString::number(referenceId));
	m_pOutStream->writeEndElement();

}
void GLC_WorldTo3dxml::setStreamWriterToFile(const QString& fileName)
{
	delete m_pOutStream;
	m_pOutStream= NULL;

	bool success= false;
	if (NULL != m_p3dxmlArchive)
	{
		if (NULL != m_pCurrentZipFile)
		{
			m_pCurrentZipFile->close();
			delete m_pOutStream;
			delete m_pCurrentZipFile;
		}
		QuaZipNewInfo quazipNewInfo(fileName);
		m_pCurrentZipFile= new QuaZipFile(m_p3dxmlArchive);
		success= m_pCurrentZipFile->open(QIODevice::WriteOnly, quazipNewInfo);
		if (success)
		{
			m_pOutStream= new QXmlStreamWriter(m_pCurrentZipFile);
		}
	}
	else
	{
		delete m_pCurrentFile;
		m_pCurrentFile= new QFile(m_AbsolutePath + fileName);
		success= m_pCurrentFile->open(QIODevice::WriteOnly);
		if (success)
		{
			m_pOutStream= new QXmlStreamWriter(m_pCurrentFile);
		}
	}

	if (NULL == m_pOutStream)
	{
		QString message(QString("GLC_WorldTo3dxml::setStreamWriterToFile Unable to create ") + fileName);
		GLC_Exception fileException(message);
		throw(fileException);
	}
	else
	{
		m_pOutStream->setAutoFormatting(true);
	}
}

void GLC_WorldTo3dxml::addManifest()
{
	setStreamWriterToFile("Manifest.xml");
	m_pOutStream->writeStartDocument();

	m_pOutStream->writeStartElement("Manifest");
		m_pOutStream->writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
		m_pOutStream->writeAttribute("xsi:noNamespaceSchemaLocation", "Manifest.xsd");
		m_pOutStream->writeTextElement("Root", QFileInfo(m_FileName).fileName());
		m_pOutStream->writeEndElement();
	m_pOutStream->writeEndElement();

	m_pOutStream->writeEndDocument();
}

void GLC_WorldTo3dxml::exportAssemblyStructure()
{
	m_ReferenceToIdHash.clear();
	m_InstanceToIdHash.clear();
	m_ReferenceRepToIdHash.clear();

	// Create the assembly file
	setStreamWriterToFile(QFileInfo(m_FileName).fileName());
	m_pOutStream->writeStartDocument();
	m_pOutStream->writeStartElement("Model_3dxml");
	m_pOutStream->writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	m_pOutStream->writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
	m_pOutStream->writeAttribute("xsi:schemaLocation", "http://www.3ds.com/xsd/3DXML ./3DXML.xsd");

	writeHeader();

	// Product Structure
	m_pOutStream->writeStartElement("ProductStructure");
	m_pOutStream->writeAttribute("root", "1");
	exportAssemblyFromOccurence(m_World.rootOccurence());
	m_pOutStream->writeEndElement(); // ProductStructure

	m_pOutStream->writeEndElement(); // Model_3dxml

	m_pOutStream->writeEndDocument();
}

void GLC_WorldTo3dxml::exportAssemblyFromOccurence(const GLC_StructOccurence* pOccurence)
{
	if (pOccurence->isOrphan())
	{
		writeReference3D(pOccurence->structReference());
	}
	else
	{
		// Reference 3D
		GLC_StructReference* pCurrentRef= pOccurence->structReference();
		if (!m_ReferenceToIdHash.contains(pCurrentRef))
		{
			writeReference3D(pCurrentRef);
			// Reference Rep
			if (pCurrentRef->hasRepresentation())
			{
				GLC_3DRep* pCurrentRep= dynamic_cast<GLC_3DRep*>(pCurrentRef->representationHandle());
				if (NULL != pCurrentRep && !m_ReferenceRepToIdHash.contains(pCurrentRep))
				{
					writeReferenceRep(pCurrentRep);
				}
			}
		}
		// Instance 3D and instance rep
		GLC_StructInstance* pCurrentInstance= pOccurence->structInstance();
		if (!m_InstanceToIdHash.contains(pCurrentInstance))
		{
			// Instance Rep
			if (pCurrentRef->hasRepresentation())
			{
				GLC_3DRep* pCurrentRep= dynamic_cast<GLC_3DRep*>(pCurrentRef->representationHandle());
				if (NULL != pCurrentRep)
				{
					writeInstanceRep(pCurrentRep, m_ReferenceToIdHash.value(pCurrentRef));
				}
			}
			// Instance 3D
			const unsigned int parentId= m_ReferenceToIdHash.value(pOccurence->parent()->structReference());
			writeInstance3D(pCurrentInstance, parentId);
		}
	}
	// Process children
	const int childCount= pOccurence->childCount();
	for (int i= 0; i < childCount; ++i)
	{
		exportAssemblyFromOccurence(pOccurence->child(i));
	}

}

QString GLC_WorldTo3dxml::matrixString(const GLC_Matrix4x4& matrix)
{
	QString resultMatrix;
	const QChar spaceChar(' ');
	// Rotation
	resultMatrix+= QString::number(matrix.data()[0]) + spaceChar;
	resultMatrix+= QString::number(matrix.data()[1]) + spaceChar;
	resultMatrix+= QString::number(matrix.data()[2]) + spaceChar;

	resultMatrix+= QString::number(matrix.data()[4]) + spaceChar;
	resultMatrix+= QString::number(matrix.data()[5]) + spaceChar;
	resultMatrix+= QString::number(matrix.data()[6]) + spaceChar;

	resultMatrix+= QString::number(matrix.data()[8]) + spaceChar;
	resultMatrix+= QString::number(matrix.data()[9]) + spaceChar;
	resultMatrix+= QString::number(matrix.data()[10]) + spaceChar;

	// Translation
	resultMatrix+= QString::number(matrix.data()[12]) + spaceChar;
	resultMatrix+= QString::number(matrix.data()[13]) + spaceChar;
	resultMatrix+= QString::number(matrix.data()[14]);

	return resultMatrix;
}

void GLC_WorldTo3dxml::write3DRep(const GLC_3DRep* pRep)
{
	const QString fileName(representationFileName(pRep).remove("urn:3DXML:"));
	setStreamWriterToFile(fileName);

	m_pOutStream->writeStartDocument();
	m_pOutStream->writeStartElement("XMLRepresentation");
	m_pOutStream->writeAttribute("version", "1.2");
	m_pOutStream->writeAttribute("xmlns", "http://www.3ds.com/xsd/3DXML");
	m_pOutStream->writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	m_pOutStream->writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

	m_pOutStream->writeStartElement("Root"); // Root
	m_pOutStream->writeAttribute("xsi:type", "BagRepType");
	m_pOutStream->writeStartElement("Rep"); // BagRep
	m_pOutStream->writeAttribute("xsi:type", "BagRepType");
	const int bodyCount= pRep->numberOfBody();
	for (int i= 0; i < bodyCount; ++i)
	{
		GLC_Mesh* pMesh= dynamic_cast<GLC_Mesh*>(pRep->geomAt(i));
		if (NULL != pMesh)
		{
			m_pOutStream->writeStartElement("Rep"); // BagRep
			m_pOutStream->writeAttribute("xsi:type", "BagRepType");
			m_pOutStream->writeStartElement("Rep"); // BagRep
			m_pOutStream->writeAttribute("xsi:type", "BagRepType");
			writeGeometry(pMesh);
			m_pOutStream->writeEndElement(); // BagRep
			m_pOutStream->writeEndElement(); // BagRep
		}
	}
	m_pOutStream->writeEndElement(); // BagRep
	m_pOutStream->writeEndElement(); // Root

	m_pOutStream->writeEndElement(); // XMLRepresentation

	m_pOutStream->writeEndDocument();
}

QString GLC_WorldTo3dxml::representationFileName(const GLC_3DRep* pRep)
{
	Q_ASSERT(m_ReferenceRepToIdHash.contains(pRep));
	const QString id(QString::number(m_ReferenceRepToIdHash.value(pRep)));
	QString fileName("urn:3DXML:Representation_" + id + ".3DRep");
	return fileName;
}

void GLC_WorldTo3dxml::writeGeometry(const GLC_Mesh* pMesh)
{
	// Get the list of material id
	QList<GLC_uint> materialList= pMesh->materialIds();
	const int materialCount= materialList.size();

	m_pOutStream->writeStartElement("Rep");
	m_pOutStream->writeAttribute("xsi:type", "PolygonalRepType");
	const double masterAccuracy= pMesh->getLodAccuracy(0);
	m_pOutStream->writeAttribute("accuracy", QString::number(masterAccuracy));
	const int lodCount= pMesh->lodCount();
	if (lodCount > 1)
	{
		// The mesh contains LOD
		for (int i= 1; i < lodCount; ++i)
		{
			const double lodAccuracy= pMesh->getLodAccuracy(i);
			m_pOutStream->writeStartElement("PolygonalLOD");
			m_pOutStream->writeAttribute("accuracy", QString::number(lodAccuracy));
			m_pOutStream->writeStartElement("Faces");
			for (int matIndex= 0; matIndex < materialCount; ++matIndex)
			{
				const GLC_uint materialId= materialList.at(matIndex);
				if (pMesh->lodContainsMaterial(i, materialId))
				{
					writeGeometryFace(pMesh, i, materialId);
				}
			}
			m_pOutStream->writeEndElement(); // Faces
			m_pOutStream->writeEndElement(); // PolygonalLOD
		}
	}

	// Master LOD
	m_pOutStream->writeStartElement("Faces");
	for (int matIndex= 0; matIndex < materialCount; ++matIndex)
	{
		const GLC_uint materialId= materialList.at(matIndex);
		if (pMesh->lodContainsMaterial(0, materialId))
		{
			writeGeometryFace(pMesh, 0, materialId);
		}
	}
	m_pOutStream->writeEndElement(); // Faces

	// Save Bulk data
	m_pOutStream->writeStartElement("VertexBuffer");
	{
		// Get positions
		GLfloatVector positionVector= pMesh->positionVector();
		QString positions;
		const int size= positionVector.size();
		for (int i= 0; i < size; i+=3)
		{
			positions.append(QString::number(positionVector.at(i)));
			positions.append(' ');
			positions.append(QString::number(positionVector.at(i + 1)));
			positions.append(' ');
			positions.append(QString::number(positionVector.at(i + 2)));
			positions.append(", ");
		}
		positions.remove(positions.size() - 2, 2);
		m_pOutStream->writeTextElement("Positions", positions);
	}
	{
		// Get normals
		GLfloatVector normalVector= pMesh->normalVector();
		QString normals;
		const int size= normalVector.size();
		for (int i= 0; i < size; i+=3)
		{
			normals.append(QString::number(normalVector.at(i)));
			normals.append(' ');
			normals.append(QString::number(normalVector.at(i + 1)));
			normals.append(' ');
			normals.append(QString::number(normalVector.at(i + 2)));
			normals.append(", ");
		}
		normals.remove(normals.size() - 2, 2);
		m_pOutStream->writeTextElement("Normals", normals);
	}
	{
		// Get texture coordinates
		GLfloatVector texelVector= pMesh->texelVector();
		if (!texelVector.isEmpty())
		{
			QString texels;
			const int size= texelVector.size();
			for (int i= 0; i < size; i+=2)
			{
				texels.append(QString::number(texelVector.at(i)));
				texels.append(' ');
				texels.append(QString::number(texelVector.at(i + 1)));
				texels.append(", ");
			}
			texels.remove(texels.size() - 2, 2);

			m_pOutStream->writeStartElement("TexturesCoordinates");
			m_pOutStream->writeAttribute("dimension", "2D");
			m_pOutStream->writeAttribute("channel", "0");
			m_pOutStream->writeCharacters(texels);
			m_pOutStream->writeEndElement(); // TexturesCoordinates
		}
	}


	m_pOutStream->writeEndElement(); // VertexBuffer
	m_pOutStream->writeEndElement(); // Rep

}
void GLC_WorldTo3dxml::writeGeometryFace(const GLC_Mesh* pMesh, int lod, GLC_uint materialId)
{
	m_pOutStream->writeStartElement("Face");
	if (pMesh->containsTriangles(lod, materialId))
	{
		QVector<GLuint> triangleIndex= pMesh->getTrianglesIndex(lod, materialId);
		QString indexString;
		const int indexSize= triangleIndex.size();
		for (int index= 0; index < indexSize; ++index)
		{
			indexString.append(QString::number(triangleIndex.at(index)));
			indexString.append(' ');
		}
		indexString.remove(indexString.size() - 1, 1);
		m_pOutStream->writeAttribute("triangles", indexString);
	}
	if (pMesh->containsStrips(lod, materialId))
	{
		QList<QVector<GLuint> > stripsIndex= pMesh->getStripsIndex(lod, materialId);
		QString indexStrips;
		const int stripCount= stripsIndex.size();
		for (int stripIndex= 0; stripIndex < stripCount; ++stripIndex)
		{
			QVector<GLuint> currentStripIndex= stripsIndex.at(stripIndex);
			QString currentIndex;
			const int indexSize= currentStripIndex.size();
			for (int index= 0; index < indexSize; ++index)
			{
				currentIndex.append(QString::number(currentStripIndex.at(index)));
				currentIndex.append(' ');
			}
			currentIndex.remove(currentIndex.size() - 1, 1);
			indexStrips.append(currentIndex);
			indexStrips.append(',');
		}
		indexStrips.remove(indexStrips.size() - 1, 1);
		m_pOutStream->writeAttribute("strips", indexStrips);
	}
	if (pMesh->containsFans(lod, materialId))
	{
		QList<QVector<GLuint> > fansIndex= pMesh->getFansIndex(lod, materialId);
		QString indexFans;
		const int fanCount= fansIndex.size();
		for (int fanIndex= 0; fanIndex < fanCount; ++fanIndex)
		{
			QVector<GLuint> currentFanIndex= fansIndex.at(fanIndex);
			QString currentIndex;
			const int indexSize= currentFanIndex.size();
			for (int index= 0; index < indexSize; ++index)
			{
				currentIndex.append(QString::number(currentFanIndex.at(index)));
				currentIndex.append(' ');
			}
			currentIndex.remove(currentIndex.size() - 1, 1);
			indexFans.append(currentIndex);
			indexFans.append(',');
		}
		indexFans.remove(indexFans.size() - 1, 1);
		m_pOutStream->writeAttribute("fans", indexFans);
	}

	writeSurfaceAttributes(pMesh->material(materialId));

	m_pOutStream->writeEndElement(); // Face

}

void GLC_WorldTo3dxml::writeSurfaceAttributes(const GLC_Material* pMaterial)
{
	QColor diffuseColor= pMaterial->diffuseColor();
	m_pOutStream->writeStartElement("SurfaceAttributes");
		m_pOutStream->writeStartElement("Color");
			m_pOutStream->writeAttribute("xsi:type", "RGBAColorType");
			m_pOutStream->writeAttribute("red", QString::number(diffuseColor.redF()));
			m_pOutStream->writeAttribute("green", QString::number(diffuseColor.greenF()));
			m_pOutStream->writeAttribute("blue", QString::number(diffuseColor.blueF()));
			m_pOutStream->writeAttribute("alpha", QString::number(diffuseColor.alphaF()));
		m_pOutStream->writeEndElement(); // Color
	m_pOutStream->writeEndElement(); // SurfaceAttributes
}
