
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

//! \file glc_offtoworld.cpp implementation of the GLC_OffToWorld class.

#include <QTextStream>
#include <QFileInfo>
#include <QGLContext>

#include "glc_offtoworld.h"
#include "glc_mesh2.h"
#include "glc_world.h"
#include "glc_fileformatexception.h"

GLC_OffToWorld::GLC_OffToWorld(const QGLContext *pContext)
: m_pWorld(NULL)
, m_FileName()
, m_pQGLContext(pContext)
, m_CurrentLineNumber(0)
, m_pCurrentMesh(NULL)
, m_CurVertexIndex(0)
, m_CurNormalIndex(0)
, m_NbrOfVertexs(0)
, m_NbrOfFaces(0)
{
	
}

GLC_OffToWorld::~GLC_OffToWorld()
{
	clear();
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Create an GLC_World from an input OFF File
GLC_World* GLC_OffToWorld::CreateWorldFromOff(QFile &file)
{
	clear();
	m_FileName= file.fileName();
	//////////////////////////////////////////////////////////////////
	// Test if the file exist and can be opened
	//////////////////////////////////////////////////////////////////
	if (!file.open(QIODevice::ReadOnly))
	{
		QString message(QString("GLC_OffToWorld::CreateWorldFromOff File ") + m_FileName + QString(" doesn't exist"));
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
	QTextStream offStream(&file);
	
	// QString buffer	
	QString lineBuff;
	
	//////////////////////////////////////////////////////////////////
	// Count the number of lines of the OFF file
	//////////////////////////////////////////////////////////////////		
	while (!offStream.atEnd())
	{
		++numberOfLine;
		offStream.readLine();
	}
	
	//////////////////////////////////////////////////////////////////
	// Reset the stream
	//////////////////////////////////////////////////////////////////			
	offStream.resetStatus();
	offStream.seek(0);
	
	//////////////////////////////////////////////////////////////////
	// Check the OFF Header
	//////////////////////////////////////////////////////////////////
	// Check if the file begin with "OFF"
	++m_CurrentLineNumber;
	lineBuff= offStream.readLine();
	lineBuff= lineBuff.trimmed();
	if(offStream.atEnd() || (!lineBuff.startsWith("OFF")))
	{
		QString message= "GLC_OffToWorld::CreateWorldFromOff : \"OFF\" not found";
		message.append("\nAt ligne : ");
		message.append(QString::number(m_CurrentLineNumber));				
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);				
	}
	// Create the mesh
	m_pCurrentMesh= new GLC_Mesh2();

	// Get the number of vertex and faces and skip comments
	++m_CurrentLineNumber;
	lineBuff= offStream.readLine();
	lineBuff= lineBuff.trimmed();
	while(!offStream.atEnd() && lineBuff.startsWith(QChar('#')))
	{
		++m_CurrentLineNumber;
		lineBuff= offStream.readLine();
		lineBuff= lineBuff.trimmed();		
	}
	extractNbrVertexsAndNbrFaces(lineBuff);
	
	//////////////////////////////////////////////////////////////////
	// Read Buffer and load vertexs
	//////////////////////////////////////////////////////////////////	
	emit currentQuantum(currentQuantumValue);
	m_CurrentLineNumber= 0;
	
	for (int currentVertex= 0; currentVertex < m_NbrOfVertexs; ++currentVertex)
	{
		// Check it the end of file has been reached
		if(offStream.atEnd())
		{
			QString message= "GLC_OffToWorld::CreateWorldFromOff : This file seems to be incomplete";
			message.append("\nAt ligne : ");
			message.append(QString::number(m_CurrentLineNumber));				
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);		
		}

		++m_CurrentLineNumber;
		lineBuff= offStream.readLine();
		// Add current vertex to the mesh
		m_pCurrentMesh->addVertex(m_CurVertexIndex++, extract3dVect(lineBuff));
		
		// Update Current Quantum for progress bar usage.
		currentQuantumValue = static_cast<int>((static_cast<double>(currentVertex) / (m_NbrOfVertexs + m_NbrOfFaces)) * 100);
		if (currentQuantumValue > previousQuantumValue)
		{
			emit currentQuantum(currentQuantumValue);
		}
		previousQuantumValue= currentQuantumValue;
	}
	
	//////////////////////////////////////////////////////////////////
	// Read Buffer and load faces
	//////////////////////////////////////////////////////////////////	
	for (int currentFace= 0; currentFace < m_NbrOfFaces; ++currentFace)
	{
		// Check it the end of file has been reached
		if(offStream.atEnd())
		{
			QString message= "GLC_OffToWorld::CreateWorldFromOff : This file seems to be incomplete";
			message.append("\nAt ligne : ");
			message.append(QString::number(m_CurrentLineNumber));				
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);		
		}

		++m_CurrentLineNumber;
		lineBuff= offStream.readLine();

		// Add current Face to the mesh
		extractFaceIndex(lineBuff);

		// Update Current Quantum for progress bar usage.
		currentQuantumValue = static_cast<int>((static_cast<double>(currentFace + m_NbrOfVertexs + 1) / (m_NbrOfVertexs + m_NbrOfFaces)) * 100);
		if (currentQuantumValue > previousQuantumValue)
		{
			emit currentQuantum(currentQuantumValue);
		}
		previousQuantumValue= currentQuantumValue;

	}
	
	file.close();

	GLC_Instance instance(m_pCurrentMesh);
	m_pCurrentMesh= NULL;
	m_pWorld->rootProduct()->addChildPart(instance);
	
	return m_pWorld;	
}

/////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

// clear offToWorld allocate memmory and reset member
void GLC_OffToWorld::clear()
{
	if (NULL != m_pCurrentMesh)
	{
		delete m_pCurrentMesh;
		m_pCurrentMesh= NULL;
	}
	m_pWorld= NULL;
	m_FileName.clear();
	m_CurrentLineNumber= 0;
	m_pCurrentMesh= NULL;
	m_CurVertexIndex= 0;
	m_CurNormalIndex= 0;
	m_NbrOfVertexs= 0;
	m_NbrOfFaces= 0;
}

// Extract a Vector from a string
GLC_Vector3df GLC_OffToWorld::extract3dVect(QString &line)
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
			QString message= "GLC_OffToWorld::extract3dVect : failed to convert vector component to float";
			message.append("\nAt ligne : ");
			message.append(QString::number(m_CurrentLineNumber));				
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);		
		}
		else
		{
			vectResult.setVect(x, y, z);
		}		
	}

	return vectResult;
	
}

// Extract Number off Vertex and faces
void GLC_OffToWorld::extractNbrVertexsAndNbrFaces(QString &line)
{	
	QTextStream stringVecteur(&line);

	QString xString, yString;
	
	if (((stringVecteur >> xString >> yString).status() == QTextStream::Ok))
	{
		bool xOk, yOk;
		m_NbrOfVertexs= xString.toInt(&xOk);
		m_NbrOfFaces= yString.toInt(&yOk);
		if (!(xOk && yOk))
		{
			QString message= "GLC_OffToWorld::extractNbrVertexsAndNbrFaces : failed to convert text to int";
			message.append("\nAt ligne : ");
			message.append(QString::number(m_CurrentLineNumber));				
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);		
		}
	}
	else
	{
		QString message= "GLC_OffToWorld::extractNbrVertexsAndNbrFaces : failed to extract nbr of vertexs/faces";
		message.append("\nAt ligne : ");
		message.append(QString::number(m_CurrentLineNumber));				
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);		
		
	}
}

// Extract a face from a string
void GLC_OffToWorld::extractFaceIndex(QString &line)
{
	QString buff;
	
	QVector<int> vectorMaterial;
	QVector<int> vectorCoordinate;
	QVector<int> vectorNormal;
	
	//////////////////////////////////////////////////////////////////
	// Parse the line containing face index
	//////////////////////////////////////////////////////////////////
	QTextStream streamFace(&line);
	// Get the number of vertex
	if((streamFace >> buff).status() != QTextStream::Ok)
	{
		QString message= "GLC_ObjToWorld::extractFaceIndex failed to extract number of vertex index";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);		
	}
	bool conversionOk;
	// Convert the QString Number of vertex to int
	int numberOfVertex= buff.toInt(&conversionOk);
	if (!conversionOk)
	{
		QString message= "GLC_ObjToWorld::extractFaceIndex failed to convert String to int";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
	// Extract the face's vertexs index 
	for (int i= 0; i < numberOfVertex; ++i)
	{
		// Get a vertex index
		if((streamFace >> buff).status() != QTextStream::Ok)
		{
			QString message= "GLC_ObjToWorld::extractFaceIndex failed to extract vertex index";
			message.append("\nAt line : ");
			message.append(QString::number(m_CurrentLineNumber));
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);		
		}
		// Convert the QString vertex index into int
		int index= buff.toInt(&conversionOk);
		if (conversionOk)
		{
			vectorCoordinate.append(index);
			vectorMaterial.append(-1);
		}
		else
		{
			QString message= "GLC_ObjToWorld::extractFaceIndex failed to convert String to int";
			message.append("\nAt line : ");
			message.append(QString::number(m_CurrentLineNumber));
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);			
		}	
	}

	//////////////////////////////////////////////////////////////////
	// Add the face to the current mesh
	//////////////////////////////////////////////////////////////////				
	// Compute and add the face normal
	m_pCurrentMesh->addNormal(m_CurNormalIndex, computeNormal(vectorCoordinate));
	for (int i= 0; i < numberOfVertex; ++i)
	{
		vectorNormal.append(m_CurNormalIndex);
	}
	m_CurNormalIndex++;
	// Add the face to the current mesh
	m_pCurrentMesh->addFace(vectorMaterial, vectorCoordinate, vectorNormal);
}

// compute face normal
GLC_Vector3df GLC_OffToWorld::computeNormal(QVector<int> &listIndex)
{
	Q_ASSERT(listIndex.size() > 2);
	
	const GLC_Vector4d vect1(m_pCurrentMesh->getVertex(listIndex[0]));
	const GLC_Vector4d vect2(m_pCurrentMesh->getVertex(listIndex[1]));
	const GLC_Vector4d vect3(m_pCurrentMesh->getVertex(listIndex[2]));
	
	const GLC_Vector4d edge1(vect2 - vect1);
	const GLC_Vector4d edge2(vect3 - vect2);
	
	GLC_Vector4d normal(edge1 ^ edge2);
	normal.setNormal(1);
	return normal.toVector3df();
}

