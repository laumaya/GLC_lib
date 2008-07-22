/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.9, packaged on May, 2008.

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

//! \file glc_stltoworld.cpp implementation of the GLC_StlToWorld class.

#include <QTextStream>
#include <QFileInfo>
#include <QGLContext>
#include <QDataStream>

#include "glc_mesh2.h"
#include "glc_stltoworld.h"
#include "glc_world.h"
#include "glc_fileformatexception.h"

GLC_StlToWorld::GLC_StlToWorld(const QGLContext *pContext)
: m_pWorld(NULL)
, m_FileName()
, m_pQGLContext(pContext)
, m_CurrentLineNumber(0)
, m_StlStream()
, m_pCurrentMesh(NULL)
, m_CurVertexIndex(0)
, m_CurNormalIndex(0)
{
	
}

GLC_StlToWorld::~GLC_StlToWorld()
{
	clear();
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Create an GLC_World from an input STL File
GLC_World* GLC_StlToWorld::CreateWorldFromStl(QFile &file)
{
	clear();
	m_FileName= file.fileName();
	//////////////////////////////////////////////////////////////////
	// Test if the file exist and can be opened
	//////////////////////////////////////////////////////////////////
	if (!file.open(QIODevice::ReadOnly))
	{
		QString message(QString("GLC_StlToWorld::CreateWorldFromStl File ") + m_FileName + QString(" doesn't exist"));
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
	
	// Attach the stream to the file
	m_StlStream.setDevice(&file);
	
	// QString buffer	
	QString lineBuff;
	
	//////////////////////////////////////////////////////////////////
	// Count the number of lines of the STL file
	//////////////////////////////////////////////////////////////////		
	while (!m_StlStream.atEnd())
	{
		++numberOfLine;
		m_StlStream.readLine();
	}

	//////////////////////////////////////////////////////////////////
	// Reset the stream
	//////////////////////////////////////////////////////////////////			
	m_StlStream.resetStatus();
	m_StlStream.seek(0);
	//////////////////////////////////////////////////////////////////
	// Read Buffer and create the world
	//////////////////////////////////////////////////////////////////
	
	emit currentQuantum(currentQuantumValue);
	m_CurrentLineNumber= 0;
	// Search Object section in the STL
	
	// Test if the STL File is ASCII or Binary
	++m_CurrentLineNumber;
	lineBuff= m_StlStream.readLine();
	lineBuff= lineBuff.trimmed().toLower();
	if (!lineBuff.startsWith("solid"))
	{
		// The STL File is not ASCII trying to load Binary STL File
		m_pCurrentMesh= new GLC_Mesh2();
		file.reset();
		LoadBinariStl(file);
		GLC_Instance instance(m_pCurrentMesh);
		m_pCurrentMesh= NULL;
		m_pWorld->rootProduct()->addChildPart(instance);
	}
	else
	{
		// The STL File is ASCII
		lineBuff.remove(0, 5);
		lineBuff= lineBuff.trimmed();
		m_pCurrentMesh= new GLC_Mesh2();
		m_pCurrentMesh->setName(lineBuff);
		// Read the mesh facet
		while (!m_StlStream.atEnd())
		{
			scanFacet();
			
			currentQuantumValue = static_cast<int>((static_cast<double>(m_CurrentLineNumber) / numberOfLine) * 100);
			if (currentQuantumValue > previousQuantumValue)
			{
				emit currentQuantum(currentQuantumValue);
			}
			previousQuantumValue= currentQuantumValue;		
		}
	}	
	file.close();
		
	return m_pWorld;
}

/////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

// clear stlToWorld allocate memmory and reset member
void GLC_StlToWorld::clear()
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
}

// Scan a line previously extracted from STL file
void GLC_StlToWorld::scanFacet()
{
////////////////////////////////////////////// Test end of solid section////////////////////	
	++m_CurrentLineNumber;
	QString lineBuff(m_StlStream.readLine());
	lineBuff= lineBuff.trimmed().toLower();
	// Test if this is the end of current solid
	if (lineBuff.startsWith("endsolid") || lineBuff.startsWith("end solid"))
	{
		GLC_Instance instance(m_pCurrentMesh);
		m_pCurrentMesh= NULL;
		m_pWorld->rootProduct()->addChildPart(instance);
		return;
	}
	// Test if this is the start of new solid
	if (lineBuff.startsWith("solid"))
	{
		// The STL File is ASCII
		lineBuff.remove(0, 5);
		lineBuff= lineBuff.trimmed();
		m_pCurrentMesh= new GLC_Mesh2();
		m_pCurrentMesh->setName(lineBuff);
		return;
	}

////////////////////////////////////////////// Facet Normal////////////////////////////////	
	// lineBuff Must begin with "facet normal"
	if (!lineBuff.startsWith("facet normal"))
	{
		QString message= "GLC_StlToWorld::scanFacet : \"facet normal\" not found!";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));		
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
	lineBuff.remove(0,12); // Remove first 12 chars
	lineBuff= lineBuff.trimmed().toLower();
	m_pCurrentMesh->addNormal(m_CurNormalIndex++, extract3dVect(lineBuff));
	
////////////////////////////////////////////// Outer Loop////////////////////////////////
	++m_CurrentLineNumber;
	lineBuff= m_StlStream.readLine();
	lineBuff= lineBuff.trimmed().toLower();
	// lineBuff Must begin with "outer loop"
	if (!lineBuff.startsWith("outer loop"))
	{
		QString message= "GLC_StlToWorld::scanFacet : \"outer loop\" not found!";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));		
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}

////////////////////////////////////////////// Vertex ////////////////////////////////	
	QVector<int> vectorMaterial;
	QVector<int> vectorCoordinate;
	QVector<int> vectorNormal;

	for (int i= 0; i < 3; ++i)
	{
		++m_CurrentLineNumber;
		lineBuff= m_StlStream.readLine();
		lineBuff= lineBuff.trimmed().toLower();
		// lineBuff Must begin with "vertex"
		if (!lineBuff.startsWith("vertex"))
		{
			QString message= "GLC_StlToWorld::scanFacet : \"vertex\" not found!";
			message.append("\nAt line : ");
			message.append(QString::number(m_CurrentLineNumber));		
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);
		}
		lineBuff.remove(0,6); // Remove first 6 chars
		lineBuff= lineBuff.trimmed();
		vectorCoordinate.append(m_CurVertexIndex);
		vectorMaterial.append(-1); // There is no material information
		vectorNormal.append(m_CurNormalIndex - 1);
		m_pCurrentMesh->addVertex(m_CurVertexIndex++, extract3dVect(lineBuff));
	}
	// Add the face to the current mesh
	m_pCurrentMesh->addFace(vectorMaterial, vectorCoordinate, vectorNormal);
	
////////////////////////////////////////////// End Loop////////////////////////////////
	++m_CurrentLineNumber;
	lineBuff= m_StlStream.readLine();
	lineBuff= lineBuff.trimmed().toLower();
	// lineBuff Must begin with "endloop"
	if (!lineBuff.startsWith("endloop"))
	{
		QString message= "GLC_StlToWorld::scanFacet : \"endloop\" not found!";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));		
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
	
////////////////////////////////////////////// End Facet////////////////////////////////
	++m_CurrentLineNumber;
	lineBuff= m_StlStream.readLine();
	lineBuff= lineBuff.trimmed().toLower();
	// lineBuff Must begin with "endfacet"
	if (!lineBuff.startsWith("endfacet"))
	{
		QString message= "GLC_StlToWorld::scanFacet : \"endfacet\" not found!";
		message.append("\nAt line : ");
		message.append(QString::number(m_CurrentLineNumber));		
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}

}

// Extract a Vector from a string
GLC_Vector3df GLC_StlToWorld::extract3dVect(QString &line)
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
			QString message= "GLC_StlToWorld::extract3dVect : failed to convert vector component to float";
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
// Load Binarie STL File
void GLC_StlToWorld::LoadBinariStl(QFile &file)
{
	// Create Working variables
	int currentQuantumValue= 0;
	int previousQuantumValue= 0;

	QDataStream stlBinFile(&file);

	stlBinFile.setByteOrder(QDataStream::LittleEndian);
	
	// Skip 80 Bytes STL header
	int SkipedData= stlBinFile.skipRawData(80);
	// Check if an error occur
	if (-1 == SkipedData)
	{
		QString message= "GLC_StlToWorld::LoadBinariStl : Failed to skip Header of binary STL";
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);				
	}
	// Read the number of facet
	quint32 numberOfFacet= 0;
	stlBinFile >> numberOfFacet;
	// Check if an error occur
	if (QDataStream::Ok != stlBinFile.status())
	{
		QString message= "GLC_StlToWorld::LoadBinariStl : Failed to read the number of facets of binary STL";
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
	for (quint32 i= 0; i < numberOfFacet; ++i)
	{
		// Extract the facet normal
		float x, y, z;
		stlBinFile >> x >> y >> z;
		// Check if an error occur
		if (QDataStream::Ok != stlBinFile.status())
		{
			QString message= "GLC_StlToWorld::LoadBinariStl : Failed to read the Normal of binary STL";
			GLC_FileFormatException fileFormatException(message, m_FileName);
			clear();
			throw(fileFormatException);
		}

		m_pCurrentMesh->addNormal(m_CurNormalIndex++, GLC_Vector3df(x, y, z));
		QVector<int> vectorMaterial;
		QVector<int> vectorCoordinate;
		QVector<int> vectorNormal;
		
		// Extract the 3 Vertexs
		for (int j= 0; j < 3; ++j)
		{
			stlBinFile >> x >> y >> z;
			// Check if an error occur
			if (QDataStream::Ok != stlBinFile.status())
			{
				QString message= "GLC_StlToWorld::LoadBinariStl : Failed to read the Vertex of binary STL";
				GLC_FileFormatException fileFormatException(message, m_FileName);
				clear();
				throw(fileFormatException);
			}
			vectorCoordinate.append(m_CurVertexIndex);
			vectorMaterial.append(-1); // There is no material information
			vectorNormal.append(m_CurNormalIndex - 1);
			m_pCurrentMesh->addVertex(m_CurVertexIndex++, GLC_Vector3df(x, y, z));
		}
		currentQuantumValue = static_cast<int>((static_cast<double>(i + 1) / numberOfFacet) * 100);
		if (currentQuantumValue > previousQuantumValue)
		{
			emit currentQuantum(currentQuantumValue);
		}
		previousQuantumValue= currentQuantumValue;

		// Add the face to the current mesh
		m_pCurrentMesh->addFace(vectorMaterial, vectorCoordinate, vectorNormal);
		// Skip 2 fill-bytes not needed !!!!
		stlBinFile.skipRawData(2);

	}
}

