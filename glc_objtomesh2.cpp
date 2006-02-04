/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.4, packaged on January, 2006.

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

//! \file glc_objToMesh.cpp implementation of the GLC_ObjToMesh2 class.

#include <QtDebug>
#include <QVector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <assert.h>

#include "glc_objtomesh2.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
GLC_ObjToMesh2::GLC_ObjToMesh2(void)
: m_pMesh(NULL)
, m_nNbrVectPos(0)
, m_nCurVectPos(0)
, m_nNbrVectNorm(0)
, m_nCurVectNorm(0)
, m_nNbrVectTexture(0)
, m_nCurVectTexture(0)
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

	// Allocate Mesh memmory
	m_pMesh= new GLC_Mesh2();
	
	// Read Buffer
	char* Buffer;
	int Taille;
	
	// Create the input file stream
	ifstream ObjFile;

	ObjFile.open(m_sFile.c_str(), ios::binary | ios::in);
	
	// static caracters array
	char strBuff[GLC_OBJ_LIGNE_LENGHT];

	if (!ObjFile)
	{
		qDebug("GLC_ObjToMesh::CreateMeshFromObj ERROR File \"%s\" Not opened", m_sFile.c_str());
		return NULL;
	}
	else
	{
		qDebug("GLC_ObjToMesh::CreateMeshFromObj OK File %s Open", m_sFile.c_str());
	}

	// Compute obj file size
	ObjFile.seekg (0, ios::end);
	Taille = ObjFile.tellg();
	Buffer= new char[Taille + 1];
	ObjFile.seekg (0, ios::beg);

	// Read the file at once
	ObjFile.read(Buffer, Taille);
	// Close input file stream
	ObjFile.close();
	Buffer[Taille]= '\0'; //Null termination caracter

	// Create Buffer input string stream
	istringstream StreamStringBuf(Buffer);
	delete [] Buffer;
	Buffer= NULL;

	while (!StreamStringBuf.eof())
	{
		StreamStringBuf.getline(strBuff, GLC_OBJ_LIGNE_LENGHT);
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
		qDebug("GLC_ObjToMesh::CreateMeshFromObj OBJ file without Normals not emplemented!");
		return NULL;
	}
	qDebug() << "Coordinate number : " << m_nNbrVectPos;
	qDebug() << "Normal number : " << m_nNbrVectNorm;
	
	StreamStringBuf.clear();
	StreamStringBuf.seekg (0, ios::beg);
	// Read Buffer and create mesh
	while (!StreamStringBuf.eof())
	{
		StreamStringBuf.getline(strBuff, GLC_OBJ_LIGNE_LENGHT);
		ScanLigne(strBuff);		
	}

	qDebug("GLC_ObjToMesh::CreateMeshFromObj End of mesh creation");

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
		VectorMaterial.append(0);
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
	// End of "/" remplacement

	istringstream StreamVertex(sLigne);
	StreamVertex >> Coordinate >> TextureCoordinate;

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
