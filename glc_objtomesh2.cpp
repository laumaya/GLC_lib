/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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

//! \file glc_objToMesh2.cpp implementation of the GLC_ObjToMesh2 class.

#include <QtDebug>
#include <QVector>
#include <QFile>
#include <assert.h>

#include "glc_objtomesh2.h"
#include "glc_texture.h"
#include "glc_fileformatexception.h"


//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
GLC_ObjToMesh2::GLC_ObjToMesh2(const QGLContext *pContext)
: QObject()
, m_pMesh(NULL)
, m_pCurrentMaterial(NULL)
, m_CurrentMaterialIndex(-1)
, m_nNbrVectPos(0)
, m_nCurVectPos(0)
, m_nNbrVectNorm(0)
, m_nCurVectNorm(0)
, m_nNbrVectTexture(0)
, m_nCurVectTexture(0)
, m_pQGLContext(pContext)
, m_ObjType(notSet)
{

}
/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Create an GLC_Mesh from an input OBJ File
GLC_Mesh2* GLC_ObjToMesh2::CreateMeshFromObj(QFile &file)
{
	// Initialisation
	m_sFile= file.fileName();
	m_pMesh= NULL;	// In case of multi obj read

	m_pCurrentMaterial= NULL;
	m_CurrentMaterialIndex= -1;
	m_nNbrVectPos= 0;
	m_nCurVectPos= 0;
	m_nNbrVectNorm= 0;
	m_nCurVectNorm= 0;
	m_nNbrVectTexture= 0;
	m_nCurVectTexture= 0;

	// Allocate Mesh memmory
	m_pMesh= new GLC_Mesh2();
	
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "GLC_ObjToMesh2::CreateMeshFromObj File " << m_sFile << " doesn't exist";
		return NULL;
	}
	else
	{
		qDebug() << "GLC_ObjToMesh2::CreateMeshFromObj OK File " << m_sFile << " exist";
	}
	loadMaterial(m_sFile);
	// Create the input file stream
	QTextStream objStream(&file);
	
	// QString buffer	
	QString lineBuff;
	
	int numberOfLine= 0;	
	while (!objStream.atEnd())
	{
		++numberOfLine;
		lineBuff= objStream.readLine();
		lineBuff= lineBuff.trimmed();
		// Search a vertex vector
		if (lineBuff.startsWith("v "))
			m_nNbrVectPos++;

		// Search a texture coordinate vector
		else if (lineBuff.startsWith("vt "))
			m_nNbrVectTexture++;

		// Search a normal vector
		else if (lineBuff.startsWith("vn "))
			m_nNbrVectNorm++;
			
	}
	// Check the validity of OBJ file
	if (!m_nNbrVectPos)
	{
		const QString message= "GLC_ObjToMesh2::CreateMeshFromObj OBJ file format not reconize!!";
		GLC_FileFormatException fileFormatException(message, m_sFile);
		throw(fileFormatException);
	}
	qDebug() << "Coordinate number : " << m_nNbrVectPos;
	qDebug() << "Normal number : " << m_nNbrVectNorm;
	
	objStream.resetStatus();
	objStream.seek(0);
	
	int currentLineNumber= 0;
	int currentQuantumValue= 0;
	int previousQuantumValue= 0;
	// Read Buffer and create mesh
	emit currentQuantum(currentQuantumValue);
	while (!objStream.atEnd())
	{
		++currentLineNumber;
		lineBuff= objStream.readLine();
		scanLigne(lineBuff);
		currentQuantumValue = static_cast<int>((static_cast<double>(currentLineNumber) / numberOfLine) * 100);
		if (currentQuantumValue > previousQuantumValue)
		{
			emit currentQuantum(currentQuantumValue);
		}
		previousQuantumValue= currentQuantumValue;
					
	}
	
	file.close();

	m_MaterialNameIndex.clear();
	qDebug("GLC_ObjToMesh2::CreateMeshFromObj End of mesh creation");
	if (m_nNbrVectNorm == 0)
	{
		m_pMesh->computeNormal();
	}
	return m_pMesh;
	
}

/////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////
// Scan a line previously extracted from OBJ file
void GLC_ObjToMesh2::scanLigne(QString &line)
{
	line= line.trimmed();
	// Search Vertexs vectors
	if (line.startsWith("v "))
	{
		line.remove(0,2); // Remove first 2 char
		m_pMesh->addVertex(m_nCurVectPos++, extract3dVect(line));
		m_ObjType = notSet;		
	}

	// Search texture coordinate vectors
	else if (line.startsWith("vt "))
	{
		line.remove(0,3); // Remove first 3 char
		m_pMesh->addTextureCoordinate(m_nCurVectTexture++, extract2dVect(line));
		m_ObjType = notSet;
	}

	// Search normals vectors
	else if (line.startsWith("vn "))
	{
		line.remove(0,3); // Remove first 3 char
		m_pMesh->addNormal(m_nCurVectNorm++, extract3dVect(line));
		m_ObjType = notSet;
	}

	// Search faces to update index
	else if (line.startsWith("f "))
	{
		line.remove(0,2); // Remove first 2 char
		extractFaceIndex(line);	
	}

	// Search Material
	else if (line.startsWith("usemtl "))
	{
		line.remove(0,7); // Remove first 7 char
		setCurrentMaterial(line);
	}

}

// Extract a Vector from a string
GLC_Vector3d GLC_ObjToMesh2::extract3dVect(QString &line)
{
	double x=0.0;
	double y=0.0;
	double z=0.0;
	
	GLC_Vector3d vectResult;
	QTextStream stringVecteur(&line);

	QString xString, yString, zString;
	
	if (((stringVecteur >> xString >> yString >> zString).status() == QTextStream::Ok))
	{
		bool xOk, yOk, zOk;
		x= xString.toDouble(&xOk);
		y= yString.toDouble(&yOk);
		z= zString.toDouble(&zOk);
		if (!(xOk && yOk && zOk))
		{
			const QString message= "GLC_Vector2d GLC_ObjToMesh2 : failed to convert vector component to double";
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_sFile);
			throw(fileFormatException);		
		}
		vectResult.setVect(x, y, z);		
	}

	return vectResult;
	
}

// Extract a Vector from a string
GLC_Vector2d GLC_ObjToMesh2::extract2dVect(QString &line)
{
	double x=0.0;
	double y=0.0;
	GLC_Vector2d vectResult;
	QTextStream stringVecteur(&line);

	QString xString, yString;
	
	if (((stringVecteur >> xString >> yString).status() == QTextStream::Ok))
	{
		bool xOk, yOk;
		x= xString.toDouble(&xOk);
		y= yString.toDouble(&yOk);
		if (!(xOk && yOk))
		{
			const QString message= "GLC_Vector2d GLC_ObjToMesh2 : failed to convert vector component to double";
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_sFile);
			throw(fileFormatException);
		}
		vectResult.setVect(x, y);		
	}

	return vectResult;
}

// Extract a face from a string
void GLC_ObjToMesh2::extractFaceIndex(QString &line)
{
	QString buff;
	
	QVector<int> vectorMaterial;
	QVector<int> vectorCoordinate;
	QVector<int> vectorNormal;
	QVector<int> vectorTextureCoordinate;
	
	//int Material;
	int coordinate;
	int normal;
	int textureCoordinate;
	
	QTextStream streamFace(&line);

	while ((!streamFace.atEnd()))
	{
		streamFace >> buff;
		extractVertexIndex(buff, coordinate, normal, textureCoordinate);
		
		vectorCoordinate.append(coordinate);
		
		if (-1 != textureCoordinate)
		{	// There is a texture coordinate
			vectorTextureCoordinate.append(textureCoordinate);
		}
		
		if (-1 != normal)
		{	// There is a normal index
			vectorNormal.append(normal);
		}
		
		vectorMaterial.append(m_CurrentMaterialIndex);
	}
	
	if (vectorTextureCoordinate.isEmpty() && !vectorNormal.isEmpty())
	{
		m_pMesh->addFace(vectorMaterial, vectorCoordinate, vectorNormal);
	}
	else if (vectorNormal.isEmpty() && vectorTextureCoordinate.isEmpty())
	{
		m_pMesh->addFace(vectorMaterial, vectorCoordinate);
	}
	else if (vectorNormal.isEmpty() && !vectorTextureCoordinate.isEmpty())
	{
		assert(vectorTextureCoordinate.size() == vectorCoordinate.size());
		
		m_pMesh->addFaceWithTexture(vectorMaterial, vectorCoordinate, vectorTextureCoordinate);
	}	
	else
	{
		assert(vectorTextureCoordinate.size() == vectorCoordinate.size());
		
		m_pMesh->addFace(vectorMaterial, vectorCoordinate, vectorNormal, vectorTextureCoordinate);
	}
	

}

//! Set Current material index
void GLC_ObjToMesh2::setCurrentMaterial(QString &line)
{
	QTextStream streamString(&line);
	QString materialName;

	if (!((streamString >> materialName).status() == QTextStream::Ok))
	{
		const QString message= "GLC_ObjToMesh2::SetCurrentMaterial : failed to extract materialName";
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_sFile);
		throw(fileFormatException);
	}
	
	MaterialHashMap::const_iterator iMaterial= m_MaterialNameIndex.find(materialName);
	// Check if the material name is find
	if(iMaterial != m_MaterialNameIndex.end());
	{
		// Retrieve the material index
		m_CurrentMaterialIndex= m_MaterialNameIndex[materialName];
		//qDebug() << "GLC_ObjToMesh2::setCurrentMaterial : " << m_CurrentMaterialIndex;
	}
			
}


// Extract a vertex from a string
void GLC_ObjToMesh2::extractVertexIndex(QString ligne, int &Coordinate, int &Normal, int &TextureCoordinate)
{ 	
 	if (m_ObjType == notSet)
 	{
 		setObjType(ligne);
 	}
 	
 	if (m_ObjType == coordinateAndTextureAndNormal)
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
				QString message= "GLC_ObjToMesh2::extractVertexIndex failed to convert String to int";
				message.append("At ligne : ");
				message.append(ligne);
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_sFile);
				throw(fileFormatException);
			}
		}
		else
		{
			QString message= "GLC_ObjToMesh2::extractVertexIndex this Obj file type is not supported";
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_sFile);
			throw(fileFormatException);
		}
				
 	}
 	else if (m_ObjType == coordinateAndTexture)
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
				QString message= "GLC_ObjToMesh2::extractVertexIndex failed to convert String to int";
				message.append("At ligne : ");
				message.append(ligne);
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_sFile);
				throw(fileFormatException);
			}
		}
		else
		{
			QString message= "GLC_ObjToMesh2::extractVertexIndex this Obj file type is not supported";
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_sFile);
			throw(fileFormatException);
		}
 	}	
 	else if (m_ObjType == coordinateAndNormal)
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
				QString message= "GLC_ObjToMesh2::extractVertexIndex failed to convert String to int";
				message.append("At ligne : ");
				message.append(ligne);
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_sFile);
				throw(fileFormatException);
			}
		}
		else
		{
			QString message= "GLC_ObjToMesh2::extractVertexIndex this Obj file type is not supported";
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_sFile);
			throw(fileFormatException);
		}
 	}
  	else if (m_ObjType == coordinate)
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
				QString message= "GLC_ObjToMesh2::extractVertexIndex failed to convert String to int";
				message.append("At ligne : ");
				message.append(ligne);
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_sFile);
				throw(fileFormatException);
			}
		}
		else
		{
			QString message= "GLC_ObjToMesh2::extractVertexIndex this Obj file type is not supported";
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_sFile);
			throw(fileFormatException);
		}
 	}
 	else
 	{
		QString message= "GLC_ObjToMesh2::extractVertexIndex OBJ file not reconize";
		message.append("At ligne : ");
		message.append(ligne);
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_sFile);
		throw(fileFormatException);
 	}
}

//! Check if a material file exist
void GLC_ObjToMesh2::loadMaterial(QString fileName)
{
	// Material filename is the same that OBJ filename	
	fileName.replace(m_sFile.size() - 3, 3, "mtl");


	// Create the input file stream
	QFile mtlFile(fileName);
	
	
	
	if (!mtlFile.open(QIODevice::ReadOnly))
	{
		qDebug() << "GLC_ObjToMesh2::LoadMaterial File " << fileName << " doesn't exist";
		return;
	}
	else
	{
		qDebug() << "GLC_ObjToMesh2::LoadMaterial OK File " << fileName << " exist";
	}
	
	QTextStream mtlStream(&mtlFile);
	// static caracters array

	QString lineBuff;
	QString header;
	
	int MaterialIndex= -1;
	
	while (!mtlStream.atEnd())
	{
		lineBuff= mtlStream.readLine();
				
		QTextStream streamLine(lineBuff.toAscii());
		
		if ((streamLine >> header).status() ==QTextStream::Ok)
		{
		
			// Search New material
			if (header =="newmtl")
			{			
				qDebug() << "New material find";
				
				if (NULL != m_pCurrentMaterial)
				{	// It's not the first material
					MaterialIndex++;
					qDebug() << "Add material : " << MaterialIndex;
					m_pMesh->addMaterial(MaterialIndex,*m_pCurrentMaterial);					
					delete m_pCurrentMaterial;
					m_pCurrentMaterial= NULL;
				}
								
				m_pCurrentMaterial= new GLC_Material;
				
				// Extract the material name
				extractString(lineBuff, m_pCurrentMaterial);
				
				QString materialName(m_pCurrentMaterial->getName());
								
				// Add the Material to Material hash table
				m_MaterialNameIndex.insert(materialName, MaterialIndex + 1);
				
								
			}
			else if ((header == "Ka") || (header == "Kd") || (header == "Ks")) // ambiant, diffuse and specular color
			{
				extractRGBValue(lineBuff, m_pCurrentMaterial);
			}
	
			else if ((header == "Ns"))	// shiness
			{
				extractOneValue(lineBuff, m_pCurrentMaterial);
			}
			else if ((header == "map_Kd"))	// Texture
			{
				qDebug() << "Texture detected";
				extractString(lineBuff, m_pCurrentMaterial);
			}
						
		}
	}

	if (NULL != m_pCurrentMaterial)
	{
		MaterialIndex++;
		qDebug() << "Add material : " << MaterialIndex;
		m_pMesh->addMaterial(MaterialIndex,*m_pCurrentMaterial);					
		delete m_pCurrentMaterial;
		m_pCurrentMaterial= NULL;
	}
	
	mtlFile.close();

}

// Extract String
void GLC_ObjToMesh2::extractString(QString &ligne, GLC_Material *pMaterial)
{
	QTextStream stream(&ligne);
	QString valueString;
	QString header;
	if ((stream >> header >> valueString).status() == QTextStream::Ok)
	{
		// If There is an space in the string to extracts
		QString valueString2;
		while ((stream >> valueString2).status() == QTextStream::Ok)
		{
			valueString.append(" ");
			valueString.append(valueString2);
		}
		if (header == "newmtl")	// The string to extract is the material name
		{		
			pMaterial->setName(valueString);
			qDebug() << "Material name is : " << valueString;
		}
		else if (header == "map_Kd")// The string to extract is the texture file name
		{
			// Retrieve the .obj file path
			QString textureFile(m_sFile);
			int start= m_sFile.lastIndexOf('/');
			start++;
			textureFile.remove(start, textureFile.size());
			
			// concatenate File Path with texture filename
			textureFile.append(valueString);
			
			// Create the texture and assign it to current material
			GLC_Texture *pTexture = new GLC_Texture(m_pQGLContext, textureFile);
			pMaterial->setTexture(pTexture);
			qDebug() << "Texture File is : " << valueString;
	}
		
	}else
	{
		const QString message= "GLC_ObjToMesh2::extractString : something is wrong!!";
		GLC_FileFormatException fileFormatException(message, m_sFile);
		throw(fileFormatException);
	}
	
}

// Extract RGB value
void GLC_ObjToMesh2::extractRGBValue(QString &ligne, GLC_Material *pMaterial)
{
	QTextStream stream(&ligne);
	QString header;
	QString rColor, gColor, bColor;
	QColor color(Qt::white);
	
	if ((stream >> header >> rColor >> gColor >> bColor).status() == QTextStream::Ok)
	{
		bool okr, okg, okb;
		color.setRedF(rColor.toDouble(&okr));
		color.setGreenF(gColor.toDouble(&okg));
		color.setBlueF(bColor.toDouble(&okb));
		if (!(okr && okg && okb))
		{
			QString message= "GLC_ObjToMesh2::ExtractRGBValue : Wrong format of rgb color value!!";
			message.append("Current mtl file line is : ");
			message.append(ligne);
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_sFile);
			throw(fileFormatException);
		}		
		color.setAlphaF(1.0);
		if (header == "Ka") // Ambiant Color
		{
			pMaterial->setAmbientColor(color);
			qDebug() << "Ambiant Color : " <<  color.redF() << " " << color.greenF() << " " << color.blueF();
		}
		
		else if (header == "Kd") // Diffuse Color
		{
			pMaterial->setDiffuseColor(color);
			qDebug() << "Diffuse Color : " <<  color.redF() << " " << color.greenF() << " " << color.blueF();
		}
		
		else if (header == "Ks") // Specular Color
		{
			pMaterial->setSpecularColor(color);
			qDebug() << "Specular Color : " <<  color.redF() << " " << color.greenF() << " " << color.blueF();
		}
		
		else
		{
			const QString message= "GLC_ObjToMesh2::ExtractRGBValue : something is wrong!!";
			GLC_FileFormatException fileFormatException(message, m_sFile);
			throw(fileFormatException);
		}
		
	}else
	{
		QString message= "GLC_ObjToMesh2::ExtractRGBValue : something is wrong!!";
		message.append("Current mtl file line is : ");
		message.append(ligne);
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_sFile);
		throw(fileFormatException);	
	}

		
}

// Extract One value
void GLC_ObjToMesh2::extractOneValue(QString &ligne, GLC_Material *pMaterial)
{
	QTextStream stream(&ligne);
	QString valueString;
	QString header;
	GLfloat value;
	
	if ((stream >> header >> valueString).status() == QTextStream::Ok)
	{
		if (header == "Ns") // Ambient color
		{
			bool ok;
			value= valueString.toFloat(&ok);
			if (!ok)
			{
				QString message= "GLC_ObjToMesh2::ExtractOneValue : Wrong format of Ambient color value!!";
				message.append("Current mtl file line is : ");
				message.append(ligne);
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_sFile);
				throw(fileFormatException);
			}
			pMaterial->setShininess(value);
			qDebug() << "Shininess : " <<  value;
		}else
		{
			const QString message= "GLC_ObjToMesh2::ExtractOneValue : Ambient Color not found!!";
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_sFile);
			throw(fileFormatException);
		}
	}else
	{
		QString message= "GLC_ObjToMesh2::ExtractOneValue : something is wrong!!";
		message.append("Current mtl file line is : ");
		message.append(ligne);
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_sFile);
		throw(fileFormatException);
	}
	
}

// set the OBJ File type
void GLC_ObjToMesh2::setObjType(QString& ligne)
{
	const QRegExp coordinateOnlyRegExp("^\\d{1,}$"); // ex. 10
 	const QRegExp coordinateTextureNormalRegExp("^\\d{1,}/\\d{1,}/\\d{1,}$"); // ex. 10/30/54
 	const QRegExp coordinateNormalRegExp("^\\d{1,}//\\d{1,}$"); // ex. 10//54
 	const QRegExp coordinateTextureRegExp("^\\d{1,}/\\d{1,}$"); // ex. 10/56
 	
 	if (coordinateTextureNormalRegExp.exactMatch(ligne))
 	{
 		m_ObjType= coordinateAndTextureAndNormal;
 	}
 	else if (coordinateTextureRegExp.exactMatch(ligne))
 	{
 		m_ObjType= coordinateAndTexture;
 	}
 	else if (coordinateNormalRegExp.exactMatch(ligne))
 	{
 		m_ObjType= coordinateAndNormal;
 	}
  	else if (coordinateOnlyRegExp.exactMatch(ligne))
 	{
 		m_ObjType= coordinate;
 	}
 	else
 	{
		QString message= "GLC_ObjToMesh2::extractVertexIndex OBJ file not reconize";
		message.append("At ligne : ");
		message.append(ligne);
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_sFile);
		throw(fileFormatException);
 	}
}



