/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.5, packaged on February, 2006.

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
	m_sFile= sFile;
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
	ifstream ObjFile;

	ObjFile.open(m_sFile.c_str(), ios::binary | ios::in);
	
	// static caracters array
	char strBuff[GLC_OBJ_LIGNE_LENGHT];

	if (!ObjFile)
	{
		qDebug("GLC_ObjToMesh2::CreateMeshFromObj ERROR File \"%s\" Not opened", m_sFile.c_str());
		return NULL;
	}
	else
	{
		qDebug("GLC_ObjToMesh2::CreateMeshFromObj OK File %s Open", m_sFile.c_str());
		LoadMaterial(m_sFile);
		qDebug() << "GLC_ObjToMesh2::CreateMeshFromObj Material loaded";
	}
	
	while (!ObjFile.eof())
	{
		ObjFile.getline(strBuff, GLC_OBJ_LIGNE_LENGHT);
		// Search a vertex vector
		if (strncmp(strBuff, ("v "), 2) == 0)
			m_nNbrVectPos++;

		// Search a texture coordinate vector
		else if (strncmp(strBuff, "vt ", 3) == 0)
			m_nNbrVectTexture++;

		// Search a normal vector
		else if (strncmp(strBuff, "vn ", 3) == 0)
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
		ObjFile.getline(strBuff, GLC_OBJ_LIGNE_LENGHT);
		ScanLigne(strBuff);		
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
void GLC_ObjToMesh2::ScanLigne(const char* c_strLigne)
{
	// Search Vertexs vectors
	if (strncmp(c_strLigne, ("v "), 2) == 0)
	{
		m_pMesh->AddCoordinate(m_nCurVectPos++, Extract3dVect(&c_strLigne[2])); // first 2 char are removed		
	}

	// Search texture coordinate vectors
	else if (strncmp(c_strLigne, "vt ", 3) == 0)
	{
		m_pMesh->AddTextureCoordinate(m_nCurVectTexture++, Extract2dVect(&c_strLigne[3])); // first 3 char are removed
	}

	// Search normals vectors
	else if (strncmp(c_strLigne, "vn ", 3) == 0)
	{
		m_pMesh->AddNormal(m_nCurVectNorm++, Extract3dVect(&c_strLigne[3])); // first 3 char are removed
	}

	// Search faces to update index
	else if (strncmp(c_strLigne, "f ", 2) == 0)
	{
		ExtractFaceIndex(&c_strLigne[2]);	// On elève les 2 premier char
	}

	// Search Material
	else if (strncmp(c_strLigne, "usemtl ", 7) == 0)
	{
		SetCurrentMaterial(&c_strLigne[7]);	// On elève les 7 premier char
	}

}

// Extract a Vector from a string
GLC_Vector3d GLC_ObjToMesh2::Extract3dVect(const char* c_strLigne)
{
	double x=0.0;
	double y=0.0;
	double z=0.0;

	istringstream stringVecteur(c_strLigne);

	stringVecteur >> x >> y >> z;

	GLC_Vector3d VectResult(x, y, z);

	return VectResult;
}

// Extract a Vector from a string
GLC_Vector2d GLC_ObjToMesh2::Extract2dVect(const char* c_strLigne)
{
	double x=0.0;
	double y=0.0;
	
	istringstream stringVecteur(c_strLigne);

	stringVecteur >> x >> y;

	GLC_Vector2d VectResult(x, y);

	return VectResult;
}

// Extract a face from a string
void GLC_ObjToMesh2::ExtractFaceIndex(const char* c_strLigne)
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
	
	istringstream stringFace(c_strLigne);

	while (stringFace >> Buff)
	{
		ExtractVertexIndex(Buff, Coordinate, Normal, TextureCoordinate);
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
		m_pMesh->AddFace(VectorMaterial, VectorCoordinate, VectorNormal);
	}
	else
	{
		assert(VectorTextureCoordinate.size() == VectorCoordinate.size());
		
		m_pMesh->AddFace(VectorMaterial, VectorCoordinate, VectorNormal, VectorTextureCoordinate);
	}
	

}

//! Set Current material index
void GLC_ObjToMesh2::SetCurrentMaterial(const char* c_strLigne)
{
	istringstream StreamString(c_strLigne);
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
		//qDebug() << "GLC_ObjToMesh2::SetCurrentMaterial : " << m_CurrentMaterialIndex;
	}		
}


// Extract a vertex from a string
void GLC_ObjToMesh2::ExtractVertexIndex(string sLigne, int &Coordinate, int &Normal, int &TextureCoordinate)
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
void GLC_ObjToMesh2::LoadMaterial(std::string FileName)
{
	
	// Material filename is the same that OBJ filename
	FileName.replace(FileName.length() - 3, 3, "mtl");

	// Create the input file stream
	ifstream MtlFile;

	MtlFile.open(FileName.c_str(), ios::binary | ios::in);

	if (!MtlFile)
	{
		qDebug("GLC_ObjToMesh2::LoadMaterial File \"%s\" Not opened", FileName.c_str());
		return;
	}
	else
	{
		qDebug("GLC_ObjToMesh2::LoadMaterial OK File %s Open", FileName.c_str());
	}

	// static caracters array
	char strBuff[GLC_OBJ_LIGNE_LENGHT];
	string Header;
	
	int MaterialIndex= -1;
	
	while (!MtlFile.eof())
	{
		MtlFile.getline(strBuff, GLC_OBJ_LIGNE_LENGHT);
		
		istringstream StreamLine(strBuff);
		
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
					m_pMesh->AddMaterial(MaterialIndex,*m_pCurrentMaterial);					
					delete m_pCurrentMaterial;
					m_pCurrentMaterial= NULL;
				}
								
				m_pCurrentMaterial= new GLC_Material;
				
				// Extract the material name
				ExtractString(strBuff, m_pCurrentMaterial);
				
				QString MaterialName(m_pCurrentMaterial->GetName());
				
				//MaterialHashMap::const_iterator iMaterial= m_MaterialNameIndex.find(MaterialName);
				// Check if the key is already use
				//assert(iMaterial == m_MaterialNameIndex.end());
				
				// Add the Material to Material hash table
				m_MaterialNameIndex.insert(MaterialName, MaterialIndex + 1);
				
								
			}
			else if ((Header == "Ka") || (Header == "Kd") || (Header == "Ks")) // ambiant, diffuse and specular color
			{
				ExtractRGBValue(strBuff, m_pCurrentMaterial);
			}
	
			else if ((Header == "Ns"))	// shiness
			{
				ExtractOneValue(strBuff, m_pCurrentMaterial);
			}
			else if ((Header == "map_Kd"))	// Texture
			{
				qDebug() << "Texture detected";
				ExtractString(strBuff, m_pCurrentMaterial);
			}
						
		}
	}

	if (NULL != m_pCurrentMaterial)
	{
		MaterialIndex++;
		qDebug() << "Add material : " << MaterialIndex;
		m_pMesh->AddMaterial(MaterialIndex,*m_pCurrentMaterial);					
		delete m_pCurrentMaterial;
		m_pCurrentMaterial= NULL;
	}
	
	MtlFile.close();
			
}

// Extract String
void GLC_ObjToMesh2::ExtractString(const std::string &Ligne, GLC_Material *pMaterial)
{
	istringstream StreamString(Ligne);
	string Header;
	string Value;
	
	assert((StreamString >> Header >> Value));
	// If There is an space in the string to extracts
	string Value2;
	while ((StreamString >> Value2))
	{
		Value.append(" ");
		Value.append(Value2);
	}
	
		
	if (Header == "newmtl")	// The string to extract is the material name
	{		
		pMaterial->SetName(Value.c_str());
		qDebug() << "Material name is : " << Value.c_str();
	}
	else if (Header == "map_Kd")// The string to extract is the texture file name
	{
		// Retrieve the .obj file path
		string TextureFile(m_sFile);
		int start= m_sFile.find_last_of('/');
		start++;
		TextureFile.erase(start);
		
		// concatenate File Path with texture filename
		TextureFile.append(Value);
		
		// Create the texture and assign it to current material
		GLC_Texture *pTexture = new GLC_Texture(m_pGLWidget, QString::fromStdString(TextureFile));
		pMaterial->SetTexture(pTexture);
		qDebug() << "Texture File is : " << Value.c_str();
	}
	
}

// Extract RGB value
void GLC_ObjToMesh2::ExtractRGBValue(const std::string &Ligne, GLC_Material *pMaterial)
{
	istringstream StreamString(Ligne);
	string Header;
	GLfloat RgbaColor[4];

	if (!(StreamString >> Header >> RgbaColor[0] >> RgbaColor[1] >> RgbaColor[2]))
	{
		qDebug() << "GLC_ObjToMesh2::ExtractRGBValue : something is wrong!!";
		assert(false);
	}
	RgbaColor[3]= 1.0f;
	
	if (Header == "Ka") // Ambiant Color
	{
		pMaterial->SetAmbientColor(RgbaColor);
		qDebug() << "Ambiant Color : " <<  (RgbaColor[0]) << " " << (RgbaColor[1]) << " " << (RgbaColor[2]);
	}
	
	else if (Header == "Kd") // Diffuse Color
	{
		pMaterial->SetDiffuseColor(RgbaColor);
		qDebug() << "Diffuse Color : " <<  RgbaColor[0] << " " << RgbaColor[1] << " " << RgbaColor[2];
	}
	
	else if (Header == "Ks") // Specular Color
	{
		pMaterial->SetSpecularColor(RgbaColor);
		qDebug() << "Specular Color : " <<  RgbaColor[0] << " " << RgbaColor[1] << " " << RgbaColor[2];
	}
	
	else
	{
		qDebug() << "GLC_ObjToMesh2::ExtractRGBValue : something is wrong!!";
		assert(false);
	}

		
}

// Extract One value
void GLC_ObjToMesh2::ExtractOneValue(const std::string &Ligne, GLC_Material *pMaterial)
{
	istringstream StreamString(Ligne);
	string Header;
	GLfloat Value;

	if (!(StreamString >> Header >> Value))
	{
		qDebug() << "GLC_ObjToMesh2::ExtractOneValue : something is wrong!!";
		qDebug() << "Current mtl file line is : " << Ligne.c_str();
		assert(false);
	}
	
	if (Header == "Ns") // Ambiant Color
	{
		pMaterial->SetShininess(Value);
		qDebug() << "Shininess : " <<  Value;
	}		
	
	else
	{
		qDebug() << "GLC_ObjToMesh2::ExtractOneValue : something is wrong!!";
		assert(false);
	}

		

}


