/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9, packaged on Novemeber, 2005.

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

//! \file glc_objToMesh.cpp implementation of the GLC_ObjToMesh class.

#include <QtDebug>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "glc_objtomesh.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
GLC_ObjToMesh::GLC_ObjToMesh(void)
: m_pMesh(NULL)
, m_pArrayPos(NULL)
, m_nNbrVectPos(0)
, m_nCurVectPos(0)
, m_pArrayNorm(NULL)
, m_nNbrVectNorm(0)
, m_nCurVectNorm(0)
, m_pArrayTexture(NULL)
, m_nNbrVectTexture(0)
, m_nCurVectTexture(0)
{

}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Create an GLC_Mesh from an input OBJ File
GLC_Mesh* GLC_ObjToMesh::CreateMeshFromObj(string sFile)
{
	// Initialisation
	m_sFile= sFile;
	m_pMesh= NULL;	// In case of multi obj read

	// Allocate Mesh memmory
	m_pMesh= new GLC_Mesh();
	
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

	istringstream Tampon(Buffer);
	delete [] Buffer;
	Buffer= NULL;

	while (!Tampon.eof())
	{
		Tampon.getline(strBuff, GLC_OBJ_LIGNE_LENGHT);
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

	// Allocate Vector memmory
	m_pArrayPos= new GLC_Vector4d[m_nNbrVectPos]; 
	m_pArrayNorm= new GLC_Vector4d[m_nNbrVectNorm];
	m_pArrayTexture= new GLC_Vector4d[m_nNbrVectTexture];
	qDebug("GLC_ObjToMesh::CreateMeshFromObj Vectors memmory allocated");

	Tampon.clear();
	Tampon.seekg (0, ios::beg);
	// Read Buffer and create mesh
	while (!Tampon.eof())
	{
		Tampon.getline(strBuff, GLC_OBJ_LIGNE_LENGHT);
		ScanLigne(strBuff);		
	}
	// free vectors memmory
	delete [] m_pArrayPos;
	m_pArrayPos= NULL;
	m_nNbrVectPos= 0;
	m_nCurVectPos= 0;
	
	delete [] m_pArrayNorm;
	m_pArrayNorm= NULL;
	m_nNbrVectNorm=0;
	m_nCurVectNorm=0;
		
	delete [] m_pArrayTexture;
	m_pArrayTexture= NULL;
	m_nNbrVectTexture=0;
	m_nCurVectTexture=0;

	qDebug("GLC_ObjToMesh::CreateMeshFromObj End of mesh creation");

	return m_pMesh;
}

/////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////
// Scan a line previously extracted from OBJ file
void GLC_ObjToMesh::ScanLigne(const char* c_strLigne)
{
	// Search Vertexs vectors
	if (strncmp(c_strLigne, ("v "), 2) == 0)
	{
		m_pArrayPos[m_nCurVectPos++]= ExtractVect(&c_strLigne[2]);	// On elève les 2 premier char
	}

	// Search texture coordinate vectors
	else if (strncmp(c_strLigne, "vt ", 3) == 0)
	{
		m_pArrayTexture[m_nCurVectTexture++]= ExtractVect(&c_strLigne[3]); // On elève les 3 premier char
	}

	// Search normals vectors
	else if (strncmp(c_strLigne, "vn ", 3) == 0)
	{
		m_pArrayNorm[m_nCurVectNorm++]= ExtractVect(&c_strLigne[3]);	// On elève les 3 premier char
	}

	// Search faces
	else if (strncmp(c_strLigne, "f ", 2) == 0)
	{
		m_pMesh->AddFace(ExtractFace(&c_strLigne[2]));	// On elève les 2 premier char
	}

}

// Extract a Vector from a string
GLC_Vector4d GLC_ObjToMesh::ExtractVect(const char* c_strLigne)
{
	double x=0.0;
	double y=0.0;
	double z=0.0;

	istringstream stringVecteur(c_strLigne);

	stringVecteur >> x >> y >> z;

	GLC_Vector4d VectResult(x, y, z);

	return VectResult;
}

// Extract a face from a string
GLC_Face* GLC_ObjToMesh::ExtractFace(const char* c_strLigne)
{
	GLC_Face* pFace= new GLC_Face();
	string Buff;
	int Nbr= 0;
	GLC_Vertex* pArrayVertex[GLC_FACEVERTEXNBR];

	istringstream stringFace(c_strLigne);

	while (stringFace >> Buff)
	{
		pArrayVertex[Nbr]= ExtractVertex(Buff);
		Nbr++;
	}
	pFace->AddVertexs(&pArrayVertex[0], Nbr);

	return pFace;
}

// Extract a vertex from a string
GLC_Vertex* GLC_ObjToMesh::ExtractVertex(string sLigne)
{
	GLC_Vertex* pVertex;
	int IndicePosition;
	int IndiceTexture;
	int IndiceNormale;

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
	StreamVertex >> IndicePosition >> IndiceTexture;

	// When there is texture coordinate information the pattern is like "1//2"
	if (!(StreamVertex >> IndiceNormale))
	{
		IndiceNormale= IndiceTexture;
		// Allocate vertex memmory
		pVertex= new GLC_Vertex(m_pArrayPos[--IndicePosition], m_pArrayNorm[--IndiceNormale]);

	}
	else
	{
		// Allocate vertex memmory
		pVertex= new GLC_Vertex(m_pArrayPos[--IndicePosition], m_pArrayNorm[--IndiceNormale]
		, m_pArrayTexture[--IndiceTexture]);

	}

	return pVertex;

}
