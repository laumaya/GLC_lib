/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

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

//! \file glc_objToworld.cpp implementation of the GLC_ObjToWorld class.


#include "glc_objtoworld.h"
#include "glc_world.h"
#include "glc_objmtlloader.h"
#include "glc_fileformatexception.h"
#include "glc_geomtools.h"

#include <QTextStream>
#include <QFileInfo>
#include <QGLContext>

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
GLC_ObjToWorld::GLC_ObjToWorld(const QGLContext *pContext)
: m_pWorld(NULL)
, m_FileName()
, m_pQGLContext(pContext)
, m_pMtlLoader(NULL)
, m_CurrentLineNumber(0)
, m_pCurrentMesh(NULL)
, m_VertexHash()
, m_NormalHash()
, m_TextCoordinateHash()
, m_CurVertexIndex(0)
, m_CurNormalIndex(0)
, m_CurTextureCoordinateIndex(0)
, m_FaceType(notSet)
, m_CurrentMeshMaterials()
, m_pCurrentMaterial(NULL)
, m_CurrentListOfVertex()
, m_ListOfAttachedFileName()
{
}

GLC_ObjToWorld::~GLC_ObjToWorld()
{
	clear();
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Create an GLC_World from an input OBJ File
GLC_World* GLC_ObjToWorld::CreateWorldFromObj(QFile &file)
{
	m_ListOfAttachedFileName.clear();
	m_FileName= file.fileName();
	//////////////////////////////////////////////////////////////////
	// Test if the file exist and can be opened
	//////////////////////////////////////////////////////////////////
	if (!file.open(QIODevice::ReadOnly))
	{
		QString message(QString("GLC_ObjToWorld::CreateWorldFromObj File ") + m_FileName + QString(" doesn't exist"));
		//qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName);
		throw(fileFormatException);
	}

	//////////////////////////////////////////////////////////////////
	// Init member
	//////////////////////////////////////////////////////////////////
	m_pWorld= new GLC_World;

	// Create Working variables
	int currentQuantumValue= 0;
	int previousQuantumValue= 0;
	int numberOfLine= 0;

	// Create the input file stream
	QTextStream objStream(&file);

	// QString buffer
	QString lineBuff;

	QString mtlLibLine;

	//////////////////////////////////////////////////////////////////
	// Searching mtllib attribute
	//////////////////////////////////////////////////////////////////
	while (!objStream.atEnd() && !lineBuff.contains("mtllib"))
	{
		++numberOfLine;
		lineBuff= objStream.readLine();
		if (lineBuff.contains("mtllib")) mtlLibLine= lineBuff;
	}

	//////////////////////////////////////////////////////////////////
	// Count the number of lines of the OBJ file
	//////////////////////////////////////////////////////////////////
	while (!objStream.atEnd())
	{
		++numberOfLine;
		objStream.readLine();
	}

	//////////////////////////////////////////////////////////////////
	// Reset the stream
	//////////////////////////////////////////////////////////////////
	objStream.resetStatus();
	objStream.seek(0);

	//////////////////////////////////////////////////////////////////
	// if mtl file found, load it
	//////////////////////////////////////////////////////////////////
	QString mtlLibFileName(getMtlLibFileName(mtlLibLine));
	if (!mtlLibFileName.isEmpty())
	{
		m_pMtlLoader= new GLC_ObjMtlLoader(m_pQGLContext, mtlLibFileName);
		if (!m_pMtlLoader->loadMaterials())
		{
			delete m_pMtlLoader;
			m_pMtlLoader= NULL;
			//qDebug() << "GLC_ObjToWorld::CreateWorldFromObj: Failed to load materials";
		}
		else
		{
			// Update Attached file name list
			m_ListOfAttachedFileName << mtlLibFileName;
			m_ListOfAttachedFileName << m_pMtlLoader->listOfAttachedFileName();
		}
	}
	else
	{
		//qDebug() << "GLC_ObjToWorld::CreateWorldFromObj: mtl file not found";
	}

	//////////////////////////////////////////////////////////////////
	// Read Buffer and create the world
	//////////////////////////////////////////////////////////////////
	emit currentQuantum(currentQuantumValue);
	m_CurrentLineNumber= 0;
	while (!objStream.atEnd())
	{
		++m_CurrentLineNumber;
		lineBuff= objStream.readLine();

		mergeLines(&lineBuff, &objStream);

		scanLigne(lineBuff);
		currentQuantumValue = static_cast<int>((static_cast<double>(m_CurrentLineNumber) / numberOfLine) * 100);
		if (currentQuantumValue > previousQuantumValue)
		{
			emit currentQuantum(currentQuantumValue);
		}
		previousQuantumValue= currentQuantumValue;

	}
	file.close();

	if (NULL != m_pCurrentMesh)
	{
		if (0 == m_pCurrentMesh->numberOfFaces())
		{
			delete m_pCurrentMesh;
			m_pCurrentMesh= NULL;
		}
		else
		{
			m_pCurrentMesh->finished();
			GLC_Instance instance(m_pCurrentMesh);
			m_pCurrentMesh= NULL;
			m_pWorld->rootProduct()->addChildPart(instance);
			// Clear the list of material already used
			m_CurrentMeshMaterials.clear();
			m_pCurrentMaterial= NULL;
		}
	}
	//! Test if there is meshes in the world
	if (m_pWorld->rootProduct()->childCount() == 0)
	{
		QString message= "GLC_ObjToWorld::CreateWorldFromObj : No mesh found!";
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
	return m_pWorld;

}

//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////

// Return the name of the mtl file
QString GLC_ObjToWorld::getMtlLibFileName(QString line)
{
	// Search mtl file with the same name than the OBJ file Name
	QString mtlFileName(m_FileName);
	mtlFileName.replace(m_FileName.size() - 3, 3, "mtl");
	QFile mtlFile(mtlFileName);
	if (!mtlFile.exists())// mtl file with same name not found
	{
		QTextStream stream(&line);
		QString header;
		if ((stream >> header >> mtlFileName).status() == QTextStream::Ok)
		{
			// If There is spaces in the string to extracts
			QString valueString2;
			while ((stream >> valueString2).status() == QTextStream::Ok)
			{
				mtlFileName.append(" ");
				mtlFileName.append(valueString2);
			}
			QFileInfo fileInfo(m_FileName);
			mtlFileName= fileInfo.absolutePath() + QDir::separator() + mtlFileName;
		}
		else
		{
			// There is no mtl file to load
			mtlFileName.clear();
		}
	}
	return mtlFileName;
}

// Scan a line previously extracted from OBJ file
void GLC_ObjToWorld::scanLigne(QString &line)
{
	line= line.trimmed();
	// Search Vertexs vectors
	if (line.startsWith("v ")|| line.startsWith(QString("v") + QString(QChar(9))))
	{
		line.remove(0,2); // Remove first 2 char
		m_VertexHash.insert(m_CurVertexIndex++, extract3dVect(line));
		m_FaceType = notSet;
	}

	// Search texture coordinate vectors
	else if (line.startsWith("vt ")|| line.startsWith(QString("vt") + QString(QChar(9))))
	{
		line.remove(0,3); // Remove first 3 char
		m_TextCoordinateHash.insert(m_CurTextureCoordinateIndex++, extract2dVect(line));
		m_FaceType = notSet;
	}

	// Search normals vectors
	else if (line.startsWith("vn ") || line.startsWith(QString("vn") + QString(QChar(9))))
	{
		line.remove(0,3); // Remove first 3 char
		m_NormalHash.insert(m_CurNormalIndex++, extract3dVect(line));
		m_FaceType = notSet;
	}

	// Search faces to update index
	else if (line.startsWith("f ") || line.startsWith(QString("f") + QString(QChar(9))))
	{
		// If there is no group or object in the OBJ file
		if (NULL == m_pCurrentMesh)
			{
				changeGroup("GLC_Default");
				//qDebug() << "Default group " << line;
			}
		line.remove(0,2); // Remove first 2 char
		extractFaceIndex(line);
	}

	// Search Material
	else if (line.startsWith("usemtl ") || line.startsWith(QString("usemtl") + QString(QChar(9))))
	{
		line.remove(0,7); // Remove first 7 char
		setCurrentMaterial(line);
		m_FaceType = notSet;
	}

	// Search Group
	else if (line.startsWith("g ") || line.startsWith("o ") || line.startsWith(QString("g") + QString(QChar(9)))
			|| line.startsWith(QString("o") + QString(QChar(9))))
	{
		m_FaceType = notSet;
		line.remove(0,2); // Remove first 2 char
		changeGroup(line);
	}

}
// Change current group
void GLC_ObjToWorld::changeGroup(QString line)
{
	//qDebug() << "GLC_ObjToWorld::changeGroup at Line :" << line;
	//////////////////////////////////////////////////////////////////
	// Parse the line containing the group name
	//////////////////////////////////////////////////////////////////
	QTextStream stream(&line);
	QString groupName;
	if ((stream >> groupName).status() == QTextStream::Ok)
	{
		// If There is an space in the string to extracts
		QString valueString2;
		while ((stream >> valueString2).status() == QTextStream::Ok)
		{
			groupName.append(" ");
			groupName.append(valueString2);
		}
		//////////////////////////////////////////////////////////////
		// If the groupName == "default" nothing to do
		//////////////////////////////////////////////////////////////
		if("default" != groupName)
		{
			if (NULL != m_pCurrentMesh) // If there is a current mesh add it as part in world
			{
				if (m_pCurrentMesh->numberOfFaces() > 0)
				{
					m_pCurrentMesh->finished();
					GLC_Instance instance(m_pCurrentMesh);
					m_pCurrentMesh= NULL;
					m_pWorld->rootProduct()->addChildPart(instance);
				}
				else
				{
					delete m_pCurrentMesh;
					m_pCurrentMesh= NULL;
				}
			}
			m_pCurrentMesh= new GLC_Mesh2();
			m_pCurrentMesh->setName(groupName);
			m_pCurrentMesh->material()->setName(QString("Mat_") + groupName);

		}
	}
	else
	{
		QString message= "GLC_ObjToWorld::changeGroup : something is wrong!!";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}

}

// Extract a Vector from a string
GLC_Vector3df GLC_ObjToWorld::extract3dVect(QString &line)
{
	float x=0.0f;
	float y=0.0f;
	float z=0.0f;

	GLC_Vector3df vectResult;
	QTextStream stringVecteur(&line);

	QString xString, yString, zString;

	if (((stringVecteur >> xString >> yString >> zString).status() == QTextStream::Ok))
	{
		bool xOk, yOk, zOk;
		x= xString.toFloat(&xOk);
		y= yString.toFloat(&yOk);
		z= zString.toFloat(&zOk);
		if (!(xOk && yOk && zOk))
		{
			QString message= "GLC_ObjToWorld::extract3dVect : failed to convert vector component to float";
			message.append("\nAt ligne : ");
			message.append(QString::number(m_CurrentLineNumber));
			//qDebug() << message;
			//GLC_FileFormatException fileFormatException(message, m_FileName);
			//clear();
			//throw(fileFormatException);
		}
		else
		{
			vectResult.setVect(x, y, z);
		}
	}

	return vectResult;

}

// Extract a Vector from a string
GLC_Vector2df GLC_ObjToWorld::extract2dVect(QString &line)
{
	float x=0.0f;
	float y=0.0f;
	GLC_Vector2df vectResult;
	QTextStream stringVecteur(&line);

	QString xString, yString;

	if (((stringVecteur >> xString >> yString).status() == QTextStream::Ok))
	{
		bool xOk, yOk;
		x= xString.toFloat(&xOk);
		y= yString.toFloat(&yOk);
		if (!(xOk && yOk))
		{
			QString message= "GLC_ObjToWorld::extract2dVect : failed to convert vector component to double";
			message.append("\nAt ligne : ");
			message.append(QString::number(m_CurrentLineNumber));
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);
		}
		vectResult.setVect(x, y);
	}

	return vectResult;
}

// Extract a face from a string
void GLC_ObjToWorld::extractFaceIndex(QString &line)
{
	QString buff;

	QVector<int> vectorCoordinate;
	QVector<int> vectorNormal;
	QVector<int> vectorTextureCoordinate;

	int coordinateIndex;
	int normalIndex;
	int textureCoordinateIndex;
	//////////////////////////////////////////////////////////////////
	// Parse the line containing face index
	//////////////////////////////////////////////////////////////////
	QTextStream streamFace(&line);
	while ((!streamFace.atEnd()))
	{
		streamFace >> buff;
		extractVertexIndex(buff, coordinateIndex, normalIndex, textureCoordinateIndex);

		vectorCoordinate.append(coordinateIndex);

		if (-1 != textureCoordinateIndex)
		{	// There is a texture coordinate
			vectorTextureCoordinate.append(textureCoordinateIndex);
		}

		if (-1 != normalIndex)
		{	// There is a normal index
			vectorNormal.append(normalIndex);
		}

	}
	//////////////////////////////////////////////////////////////////
	// Check the number of face's vertex
	//////////////////////////////////////////////////////////////////
	const int size= vectorCoordinate.size();
	if (size < 3)
	{
		//qDebug() << "GLC_ObjToWorld::extractFaceIndex Face with less than 3 vertex found";
		return;
	}
	// Fill the current list of index whith empty GLC_Vertex
	fillCurrentListOfVertex(size);
	//////////////////////////////////////////////////////////////////
	// Add the face to the current mesh
	//////////////////////////////////////////////////////////////////
	if (m_FaceType == coordinateAndNormal)
	{
		addVertexsToCurrentListOfVertex(vectorCoordinate);
		addNormalsToCurrentListOfVertex(vectorNormal);
		if (size > 3)
		{
			glc::triangulatePolygon(&m_CurrentListOfVertex);
		}
		m_pCurrentMesh->addTriangles(m_CurrentListOfVertex, m_pCurrentMaterial);
	}
	else if (m_FaceType == coordinate)
	{
		addVertexsToCurrentListOfVertex(vectorCoordinate);
		computeNormal(vectorCoordinate);

		if (size > 3)
		{
			glc::triangulatePolygon(&m_CurrentListOfVertex);
		}
		m_pCurrentMesh->addTriangles(m_CurrentListOfVertex, m_pCurrentMaterial);


	}
	else if (m_FaceType == coordinateAndTexture)
	{
		addVertexsToCurrentListOfVertex(vectorCoordinate);
		addTextureCoordinatesToCurrentListOfVertex(vectorTextureCoordinate);
		computeNormal(vectorCoordinate);
		if (size > 3)
		{
			glc::triangulatePolygon(&m_CurrentListOfVertex);
		}
		m_pCurrentMesh->addTriangles(m_CurrentListOfVertex, m_pCurrentMaterial);

	}
	else if (m_FaceType == coordinateAndTextureAndNormal)
	{
		addVertexsToCurrentListOfVertex(vectorCoordinate);
		addNormalsToCurrentListOfVertex(vectorNormal);
		addTextureCoordinatesToCurrentListOfVertex(vectorTextureCoordinate);

		if (size > 3)
		{
			glc::triangulatePolygon(&m_CurrentListOfVertex);
		}
		m_pCurrentMesh->addTriangles(m_CurrentListOfVertex, m_pCurrentMaterial);
	}
	else
	{
		QString message= "GLC_ObjToWorld::extractFaceIndex : unknow face type";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
}
//! Set Current material index
void GLC_ObjToWorld::setCurrentMaterial(QString &line)
{
	QTextStream streamString(&line);
	QString materialName;

	if (!((streamString >> materialName).status() == QTextStream::Ok))
	{
		QString message= "GLC_ObjToWorld::SetCurrentMaterial : failed to extract materialName";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
	//////////////////////////////////////////////////////////////////
	// Check if the material is already loaded from the current mesh
	//////////////////////////////////////////////////////////////////
	//qDebug() << "Material Name" << materialName;
	if ((NULL != m_pMtlLoader) && m_pMtlLoader->contains(materialName))
	{
		m_pCurrentMaterial= m_pMtlLoader->material(materialName);
	}

}
// Extract a vertex from a string
void GLC_ObjToWorld::extractVertexIndex(QString ligne, int &Coordinate, int &Normal, int &TextureCoordinate)
{
 	if (m_FaceType == notSet)
 	{
 		setObjType(ligne);
 	}

 	if (m_FaceType == coordinateAndTextureAndNormal)
 	{
		// Replace "/" with " "
		ligne.replace('/', ' ');
		QTextStream streamVertex(&ligne);
		QString coordinateString, textureCoordinateString, normalString;
	 	if ((streamVertex >> coordinateString >> textureCoordinateString >> normalString).status() == QTextStream::Ok)
		{
			bool coordinateOk, textureCoordinateOk, normalOk;
			Coordinate= coordinateString.toInt(&coordinateOk);
			--Coordinate;
			TextureCoordinate= textureCoordinateString.toInt(&textureCoordinateOk);
			--TextureCoordinate;
			Normal= normalString.toInt(&normalOk);
			--Normal;
			if (!(coordinateOk && textureCoordinateOk && normalOk))
			{
				QString message= "GLC_ObjToWorld::extractVertexIndex failed to convert String to int";
				message.append("\nAt line : ");
				message.append(QString::number(m_CurrentLineNumber));
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_FileName);
				clear();
				throw(fileFormatException);
			}
		}
		else
		{
			QString message= "GLC_ObjToWorld::extractVertexIndex this Obj file type is not supported";
			message.append("\nAt line : ");
			message.append(QString::number(m_CurrentLineNumber));
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);
		}

 	}
 	else if (m_FaceType == coordinateAndTexture)
 	{
		// Replace "/" with " "
		ligne.replace('/', ' ');
		QTextStream streamVertex(&ligne);
		QString coordinateString, textureCoordinateString;
	 	if ((streamVertex >> coordinateString >> textureCoordinateString).status() == QTextStream::Ok)
		{
			bool coordinateOk, textureCoordinateOk;
			Coordinate= coordinateString.toInt(&coordinateOk);
			--Coordinate;
			TextureCoordinate= textureCoordinateString.toInt(&textureCoordinateOk);
			--TextureCoordinate;
			Normal= -1;
			if (!(coordinateOk && textureCoordinateOk))
			{
				QString message= "GLC_ObjToWorld::extractVertexIndex failed to convert String to int";
				message.append("\nAt line : ");
				message.append(QString::number(m_CurrentLineNumber));
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_FileName);
				clear();
				throw(fileFormatException);
			}
		}
		else
		{
			QString message= "GLC_ObjToWorld::extractVertexIndex this Obj file type is not supported";
			message.append("\nAt line : ");
			message.append(QString::number(m_CurrentLineNumber));
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);
		}
 	}
 	else if (m_FaceType == coordinateAndNormal)
 	{
		// Replace "/" with " "
		ligne.replace('/', ' ');
		QTextStream streamVertex(&ligne);
		QString coordinateString, normalString;
	 	if ((streamVertex >> coordinateString >> normalString).status() == QTextStream::Ok)
		{
			bool coordinateOk, normalOk;
			Coordinate= coordinateString.toInt(&coordinateOk);
			--Coordinate;
			TextureCoordinate= -1;
			Normal= normalString.toInt(&normalOk);
			--Normal;
			if (!(coordinateOk && normalOk))
			{
				QString message= "GLC_ObjToWorld::extractVertexIndex failed to convert String to int";
				message.append("\nAt line : ");
				message.append(QString::number(m_CurrentLineNumber));
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_FileName);
				clear();
				throw(fileFormatException);
			}
		}
		else
		{
			QString message= "GLC_ObjToWorld::extractVertexIndex this Obj file type is not supported";
			message.append("\nAt line : ");
			message.append(QString::number(m_CurrentLineNumber));
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);
		}
 	}
  	else if (m_FaceType == coordinate)
 	{
 		QTextStream streamVertex(&ligne);
 		QString coordinateString;
	 	if ((streamVertex >> coordinateString).status() == QTextStream::Ok)
		{
			bool coordinateOk;
			Coordinate= coordinateString.toInt(&coordinateOk);
			--Coordinate;
			TextureCoordinate= -1;
			Normal= -1;
			if (!coordinateOk)
			{
				QString message= "GLC_ObjToWorld::extractVertexIndex failed to convert String to int";
				message.append("\nAt line : ");
				message.append(QString::number(m_CurrentLineNumber));
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_FileName);
				clear();
				throw(fileFormatException);
			}
		}
		else
		{
			QString message= "GLC_ObjToWorld::extractVertexIndex this Obj file type is not supported";
			message.append("\nAt line : ");
			message.append(QString::number(m_CurrentLineNumber));
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);
		}
 	}
 	else
 	{
		QString message= "GLC_ObjToWorld::extractVertexIndex OBJ file not reconize";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
 	}
}

// set the OBJ File type
void GLC_ObjToWorld::setObjType(QString& ligne)
{
	const QRegExp coordinateOnlyRegExp("^\\d{1,}$"); // ex. 10
 	const QRegExp coordinateTextureNormalRegExp("^\\d{1,}/\\d{1,}/\\d{1,}$"); // ex. 10/30/54
 	const QRegExp coordinateNormalRegExp("^\\d{1,}//\\d{1,}$"); // ex. 10//54
 	const QRegExp coordinateTextureRegExp("^\\d{1,}/\\d{1,}$"); // ex. 10/56

 	if (coordinateTextureNormalRegExp.exactMatch(ligne))
 	{
 		m_FaceType= coordinateAndTextureAndNormal;
 	}
 	else if (coordinateTextureRegExp.exactMatch(ligne))
 	{
 		m_FaceType= coordinateAndTexture;
 	}
 	else if (coordinateNormalRegExp.exactMatch(ligne))
 	{
 		m_FaceType= coordinateAndNormal;
 	}
  	else if (coordinateOnlyRegExp.exactMatch(ligne))
 	{
 		m_FaceType= coordinate;
 	}
 	else
 	{
		QString message= "GLC_ObjToWorld::setObjType OBJ file not reconize";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
 	}
}

// compute face normal
void GLC_ObjToWorld::computeNormal(QVector<int> &listIndex)
{
	Q_ASSERT(listIndex.size() > 2);

	const GLC_Vector4d vect1(m_VertexHash.value(listIndex[0]));
	const GLC_Vector4d vect2(m_VertexHash.value(listIndex[1]));
	const GLC_Vector4d vect3(m_VertexHash.value(listIndex[2]));

	const GLC_Vector4d edge1(vect2 - vect1);
	const GLC_Vector4d edge2(vect3 - vect2);

	GLC_Vector4d normal(edge1 ^ edge2);
	normal.setNormal(1);

	GLC_Vector3df curNormal= normal.toVector3df();

	const int max= listIndex.count();
	Q_ASSERT(max == m_CurrentListOfVertex.size());

	for (int i= 0; i < max; ++i)
	{
		m_CurrentListOfVertex[i].nx= curNormal.X();
		m_CurrentListOfVertex[i].ny= curNormal.Y();
		m_CurrentListOfVertex[i].nz= curNormal.Z();
	}
}

//! Fill the current list of vertex by empty vertex
void GLC_ObjToWorld::fillCurrentListOfVertex(const int size)
{
	m_CurrentListOfVertex.clear();
	GLC_Vertex empty;
	empty.x= 0.0f;
	empty.y= 0.0f;
	empty.z= 0.0f;

	empty.nx= 0.0f;
	empty.ny= 0.0f;
	empty.nz= 0.0f;

	empty.s= 0.0f;
	empty.t= 0.0f;

	empty.r= 0.0f;
	empty.g= 0.0f;
	empty.b= 0.0f;
	empty.a= 0.0f;

	for (int i= 0; i < size; ++i)
	{
		m_CurrentListOfVertex.append(empty);
	}
}

// Add Vertexs in the current list of vertex
void GLC_ObjToWorld::addVertexsToCurrentListOfVertex(QVector<int> & vertexs)
{
	const int max= vertexs.count();
	if(max == m_CurrentListOfVertex.size())
	{
		for (int i= 0; i < max; ++i)
		{
			GLC_Vector3df curVect= m_VertexHash[vertexs[i]];
			m_CurrentListOfVertex[i].x= curVect.X();
			m_CurrentListOfVertex[i].y= curVect.Y();
			m_CurrentListOfVertex[i].z= curVect.Z();
		}
	}
	else
	{
		QString message= "GLC_ObjToWorld::addVertexsToCurrentListOfVertex this Obj file type is not supported";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
}

// Add Normals in the current list of vertex
void GLC_ObjToWorld::addNormalsToCurrentListOfVertex(QVector<int> & normals)
{
	const int max= normals.count();
	if(max == m_CurrentListOfVertex.size())
	{
		for (int i= 0; i < max; ++i)
		{
			GLC_Vector3df curVect= m_NormalHash[normals[i]];
			m_CurrentListOfVertex[i].nx= curVect.X();
			m_CurrentListOfVertex[i].ny= curVect.Y();
			m_CurrentListOfVertex[i].nz= curVect.Z();
		}
	}
	else
	{
		QString message= "GLC_ObjToWorld::addNormalsToCurrentListOfVertex this Obj file type is not supported";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
}

// Add TextureCoordinate in the current list of vertex
void GLC_ObjToWorld::addTextureCoordinatesToCurrentListOfVertex(QVector<int> & textureCoordinates)
{
	const int max= textureCoordinates.count();
	if (max == m_CurrentListOfVertex.size())
	{
		for (int i= 0; i < max; ++i)
		{
			GLC_Vector2df curVect= m_TextCoordinateHash[textureCoordinates[i]];
			m_CurrentListOfVertex[i].s= curVect.X();
			m_CurrentListOfVertex[i].t= curVect.Y();
		}
	}
	else
	{
		QString message= "GLC_ObjToWorld::addTextureCoordinatesToCurrentListOfVertex this Obj file type is not supported";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
}

// clear objToWorld allocate memmory
void GLC_ObjToWorld::clear()
{
	m_VertexHash.clear();
	m_NormalHash.clear();
	m_TextCoordinateHash.clear();
	m_CurrentMeshMaterials.clear();
	m_CurrentListOfVertex.clear();
	m_ListOfAttachedFileName.clear();

	if (NULL != m_pMtlLoader)
	{
		delete m_pMtlLoader;
		m_pMtlLoader= NULL;
	}
	if (NULL != m_pCurrentMesh)
	{
		delete m_pCurrentMesh;
		m_pCurrentMesh= NULL;
	}

}
// Merge Mutli line in one
void GLC_ObjToWorld::mergeLines(QString* pLineBuff, QTextStream* p0bjStream)
{
	if (pLineBuff->endsWith(QChar('\\')))
	{
		pLineBuff->replace(QChar('\\'), QChar(' '));
		pLineBuff->append(p0bjStream->readLine());
		++m_CurrentLineNumber;
		mergeLines(pLineBuff, p0bjStream);
	}
}



