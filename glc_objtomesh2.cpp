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
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <assert.h>

#include "glc_objtomesh2.h"
#include "glc_texture.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
GLC_ObjToMesh2::GLC_ObjToMesh2(QGLWidget *GLWidget)
: m_pMesh(NULL)
, m_pCurrentMaterial(NULL)
, m_CurrentMaterialIndex(-1)
, m_nNbrVectPos(0)
, m_nCurVectPos(0)
, m_nNbrVectNorm(0)
, m_nCurVectNorm(0)
, m_nNbrVectTexture(0)
, m_nCurVectTexture(0)
, m_pGLWidget(GLWidget)
{

}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Create an GLC_Mesh from an input OBJ File
GLC_Mesh2* GLC_ObjToMesh2::CreateMeshFromObj(string sFile)
{
	// Initialisation
	m_sFile= QString::fromStdString(sFile);
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
	
	
	// Create the input file stream
	qDebug() << "GLC_ObjToMesh2::CreateMeshFromObj Create an input file Stream";
	ifstream ObjFile;
	
	ObjFile.open(m_sFile.toAscii().data(), ios::binary | ios::in);
	
	// QString buffer	
	string lineBuff;
	

	if (!ObjFile)
	{
		qDebug() << "GLC_ObjToMesh2::CreateMeshFromObj ERROR File " << m_sFile << " not opened";
		return NULL;
	}
	else
	{
		qDebug() << "GLC_ObjToMesh2::CreateMeshFromObj OK File " << m_sFile << " Open";
		loadMaterial(m_sFile.toStdString());		
	}
	
	while (!ObjFile.eof())
	{
		getline(ObjFile, lineBuff);
		// Search a vertex vector
		if (lineBuff.find("v ") != string::npos)
			m_nNbrVectPos++;

		// Search a texture coordinate vector
		else if (lineBuff.find("vt ") != string::npos)
			m_nNbrVectTexture++;

		// Search a normal vector
		else if (lineBuff.find("vn ") != string::npos)
			m_nNbrVectNorm++;
			
	}

	// Check the validity of OBJ file
	if (!m_nNbrVectNorm || !m_nNbrVectPos)
	{
		qDebug("GLC_ObjToMesh2::CreateMeshFromObj OBJ file without Normals not emplemented!");
		return NULL;
	}
	qDebug() << "Coordinate number : " << m_nNbrVectPos;
	qDebug() << "Normal number : " << m_nNbrVectNorm;
	
	ObjFile.clear();
	ObjFile.seekg (0, ios::beg);
	// Read Buffer and create mesh
	while (!ObjFile.eof())
	{
		getline(ObjFile, lineBuff);
		scanLigne(lineBuff);		
	}
	
	ObjFile.close();

	m_MaterialNameIndex.clear();
	qDebug("GLC_ObjToMesh2::CreateMeshFromObj End of mesh creation");

	return m_pMesh;
}

/////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////
// Scan a line previously extracted from OBJ file
void GLC_ObjToMesh2::scanLigne(std::string &line)
{
	// Search Vertexs vectors
	if (line.find("v ") != string::npos)
	{
		line.erase(0,2); // Remove first 2 char
		m_pMesh->addVertex(m_nCurVectPos++, extract3dVect(line));		
	}

	// Search texture coordinate vectors
	else if (line.find("vt ") != string::npos)
	{
		line.erase(0,3); // Remove first 3 char
		m_pMesh->addTextureCoordinate(m_nCurVectTexture++, extract2dVect(line));
	}

	// Search normals vectors
	else if (line.find("vn ") != string::npos)
	{
		line.erase(0,3); // Remove first 3 char
		m_pMesh->addNormal(m_nCurVectNorm++, extract3dVect(line));
	}

	// Search faces to update index
	else if (line.find("f ") != string::npos)
	{
		line.erase(0,2); // Remove first 2 char
		extractFaceIndex(line);	
	}

	// Search Material
	else if (line.find("usemtl ") != string::npos)
	{
		line.erase(0,7); // Remove first 7 char
		setCurrentMaterial(line);
	}

}

// Extract a Vector from a string
GLC_Vector3d GLC_ObjToMesh2::extract3dVect(const std::string &line)
{
	double x=0.0;
	double y=0.0;
	double z=0.0;

	istringstream stringVecteur(line);

	stringVecteur >> x >> y >> z;

	GLC_Vector3d VectResult(x, y, z);

	return VectResult;
}

// Extract a Vector from a string
GLC_Vector2d GLC_ObjToMesh2::extract2dVect(const std::string &line)
{
	double x=0.0;
	double y=0.0;
	
	istringstream stringVecteur(line);

	stringVecteur >> x >> y;

	GLC_Vector2d VectResult(x, y);

	return VectResult;
}

// Extract a face from a string
void GLC_ObjToMesh2::extractFaceIndex(const std::string &line)
{
	string Buff;
	
	QVector<int> VectorMaterial;
	QVector<int> VectorCoordinate;
	QVector<int> VectorNormal;
	QVector<int> VectorTextureCoordinate;
	
	//int Material;
	int Coordinate;
	int Normal;
	int TextureCoordinate;
	
	istringstream stringFace(line);

	while (stringFace >> Buff)
	{
		extractVertexIndex(Buff, Coordinate, Normal, TextureCoordinate);
		if (-1 != TextureCoordinate)
		{	// There is a texture coordinate
			VectorTextureCoordinate.append(TextureCoordinate);
		}
		
		VectorCoordinate.append(Coordinate);
		VectorNormal.append(Normal);
		VectorMaterial.append(m_CurrentMaterialIndex);
	}
	
	if (VectorTextureCoordinate.isEmpty())
	{
		m_pMesh->addFace(VectorMaterial, VectorCoordinate, VectorNormal);
	}
	else
	{
		assert(VectorTextureCoordinate.size() == VectorCoordinate.size());
		
		m_pMesh->addFace(VectorMaterial, VectorCoordinate, VectorNormal, VectorTextureCoordinate);
	}
	

}

//! Set Current material index
void GLC_ObjToMesh2::setCurrentMaterial(const std::string &line)
{
	istringstream StreamString(line);
	string MaterialName;

	if (!(StreamString >> MaterialName))
	{
		qDebug() << "GLC_ObjToMesh2::SetCurrentMaterial : something is wrong!!";
		assert(false);
	}
	
	MaterialHashMap::const_iterator iMaterial= m_MaterialNameIndex.find(QString::fromStdString (MaterialName));
	// Check if the material name is find
	if(iMaterial != m_MaterialNameIndex.end());
	{
		// Retrieve the material index
		m_CurrentMaterialIndex= m_MaterialNameIndex[QString::fromStdString(MaterialName)];
		//qDebug() << "GLC_ObjToMesh2::setCurrentMaterial : " << m_CurrentMaterialIndex;
	}		
}


// Extract a vertex from a string
void GLC_ObjToMesh2::extractVertexIndex(string sLigne, int &Coordinate, int &Normal, int &TextureCoordinate)
{
	// Replace "/" with " "
	unsigned int Pos;
	Pos= static_cast<unsigned int>(sLigne.find("/"));
	while (Pos < string::npos)
	{
		sLigne.replace(Pos, 1, " ");
		Pos= static_cast<unsigned int>(sLigne.find("/", Pos + 1));
	}
	// End of "/" replacement

	istringstream StreamVertex(sLigne);
	assert((StreamVertex >> Coordinate >> TextureCoordinate));

	// When there isn't texture coordinate information the pattern is like "1//2"
	if (!(StreamVertex >> Normal))
	{
		Normal= TextureCoordinate;
		TextureCoordinate= -1;
		
	}
	else
	{
		TextureCoordinate--;
	}
	Coordinate--;
	Normal--;		
	
}

//! Check if a material file exist
void GLC_ObjToMesh2::loadMaterial(std::string FileName)
{
	
	// Material filename is the same that OBJ filename
	FileName.replace(m_sFile.length() - 3, 3, "mtl");


	// Create the input file stream
	ifstream MtlFile;

	MtlFile.open(FileName.c_str(), ios::binary | ios::in);

	if (!MtlFile)
	{
		qDebug() << "GLC_ObjToMesh2::LoadMaterial File " << FileName.c_str() << " doesn't exist";
		return;
	}
	else
	{
		qDebug() << "GLC_ObjToMesh2::LoadMaterial OK File " << FileName.c_str() << " exist";
	}

	// static caracters array

	string lineBuff;
	string Header;
	
	int MaterialIndex= -1;
	
	while (!MtlFile.eof())
	{
		getline(MtlFile, lineBuff);
		
		istringstream StreamLine(lineBuff);
		
		if ((StreamLine >> Header))
		{
		
			// Search New material
			if (Header =="newmtl")
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
				
				QString MaterialName(m_pCurrentMaterial->getName());
				
				//MaterialHashMap::const_iterator iMaterial= m_MaterialNameIndex.find(MaterialName);
				// Check if the key is already use
				//assert(iMaterial == m_MaterialNameIndex.end());
				
				// Add the Material to Material hash table
				m_MaterialNameIndex.insert(MaterialName, MaterialIndex + 1);
				
								
			}
			else if ((Header == "Ka") || (Header == "Kd") || (Header == "Ks")) // ambiant, diffuse and specular color
			{
				extractRGBValue(lineBuff, m_pCurrentMaterial);
			}
	
			else if ((Header == "Ns"))	// shiness
			{
				extractOneValue(lineBuff, m_pCurrentMaterial);
			}
			else if ((Header == "map_Kd"))	// Texture
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
	
	MtlFile.close();
			
}

// Extract String
void GLC_ObjToMesh2::extractString(const std::string &Ligne, GLC_Material *pMaterial)
{
	QTextStream stream(QString::fromStdString(Ligne).toAscii());
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
			GLC_Texture *pTexture = new GLC_Texture(m_pGLWidget, textureFile);
			pMaterial->setTexture(pTexture);
			qDebug() << "Texture File is : " << valueString;
	}
		
	}else
	{
		qDebug() << "GLC_ObjToMesh2::extractString : something is wrong!!";
		assert(false);		
	}
	
}

// Extract RGB value
void GLC_ObjToMesh2::extractRGBValue(const std::string &Ligne, GLC_Material *pMaterial)
{
	QTextStream stream(QString::fromStdString(Ligne).toAscii());
	QString header;
	QString rColor, gColor, bColor;
	GLfloat RgbaColor[4];
	
	if ((stream >> header >> rColor >> gColor >> bColor).status() == QTextStream::Ok)
	{
		bool okr, okg, okb;
		RgbaColor[0]= rColor.toFloat(&okr);
		RgbaColor[1]= gColor.toFloat(&okg);
		RgbaColor[2]= bColor.toFloat(&okb);
		if (!(okr && okg && okb))
		{
			qDebug() << "GLC_ObjToMesh2::ExtractRGBValue : Wrong format of rgb color value!!";
			qDebug() << "Current mtl file line is : " << Ligne.c_str();
			assert(false);
		}		
		RgbaColor[3]= 1.0f;
		if (header == "Ka") // Ambiant Color
		{
			pMaterial->setAmbientColor(RgbaColor);
			qDebug() << "Ambiant Color : " <<  (RgbaColor[0]) << " " << (RgbaColor[1]) << " " << (RgbaColor[2]);
		}
		
		else if (header == "Kd") // Diffuse Color
		{
			pMaterial->setDiffuseColor(RgbaColor);
			qDebug() << "Diffuse Color : " <<  RgbaColor[0] << " " << RgbaColor[1] << " " << RgbaColor[2];
		}
		
		else if (header == "Ks") // Specular Color
		{
			pMaterial->setSpecularColor(RgbaColor);
			qDebug() << "Specular Color : " <<  RgbaColor[0] << " " << RgbaColor[1] << " " << RgbaColor[2];
		}
		
		else
		{
			qDebug() << "GLC_ObjToMesh2::ExtractRGBValue : something is wrong!!";
			assert(false);
		}
		
	}else
	{
		qDebug() << "GLC_ObjToMesh2::ExtractRGBValue : something is wrong!!";
		qDebug() << "Current mtl file line is : " << Ligne.c_str();		
		assert(false);
		
	}

		
}

// Extract One value
void GLC_ObjToMesh2::extractOneValue(const std::string &Ligne, GLC_Material *pMaterial)
{
	QTextStream stream(QString::fromStdString(Ligne).toAscii());
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
				qDebug() << "GLC_ObjToMesh2::ExtractOneValue : Wrong format of Ambient color value!!";
				qDebug() << "Current mtl file line is : " << Ligne.c_str();
				assert(false);
			}
			pMaterial->setShininess(value);
			qDebug() << "Shininess : " <<  value;
		}else
		{
			qDebug() << "GLC_ObjToMesh2::ExtractOneValue : Ambient Color not found!!";
			assert(false);			
		}
	}else
	{
		qDebug() << "GLC_ObjToMesh2::ExtractOneValue : something is wrong!!";
		qDebug() << "Current mtl file line is : " << Ligne.c_str();
		assert(false);		
	}
	
}


