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

//! \file glc_3dstoworld.cpp implementation of the GLC_3dsToWorld class.

#include "glc_3dstoworld.h"
#include <QFileInfo>
#include <QGLContext>

#include "glc_mesh2.h"
#include "glc_world.h"
#include "glc_fileformatexception.h"
#include "glc_circle.h"
// Lib3ds Header
#include "lib3ds/file.h"
#include "lib3ds/mesh.h"
#include "lib3ds/node.h"
#include "lib3ds/matrix.h"

GLC_3dsToWorld::GLC_3dsToWorld(const QGLContext *pContext)
: m_pWorld(NULL)
, m_FileName()
, m_pQGLContext(pContext)
, m_pCurrentMesh(NULL)
, m_CurVertexIndex(0)
, m_CurNormalIndex(0)
, m_pLib3dsFile(NULL)
{
}

GLC_3dsToWorld::~GLC_3dsToWorld()
{
}

// Create an GLC_World from an input STL File
GLC_World* GLC_3dsToWorld::CreateWorldFrom3ds(QFile &file)
{
	clear();
	m_FileName= file.fileName();
	//////////////////////////////////////////////////////////////////
	// Test if the file exist and can be opened
	//////////////////////////////////////////////////////////////////
	if (!file.open(QIODevice::ReadOnly))
	{
		QString message(QString("GLC_3dsToWorld::CreateWorldFrom3ds File ") + m_FileName + QString(" doesn't exist"));
		GLC_FileFormatException fileFormatException(message, m_FileName);
		throw(fileFormatException);
	}
	//////////////////////////////////////////////////////////////////
	// Init member
	//////////////////////////////////////////////////////////////////
	m_pWorld= new GLC_World;
	
	//Load 3ds File
	m_pLib3dsFile=lib3ds_file_load(m_FileName.toStdString().c_str());
	if (!m_pLib3dsFile)
	{
		QString message= "GLC_3dsToWorld::CreateWorldFrom3ds : Loading Failed";
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
	
	/* No nodes?  Fabricate nodes to display all the meshes. */
	if(!m_pLib3dsFile->nodes)
	{
		qDebug() << "No nodes";
		Lib3dsMesh *pMesh;
		Lib3dsNode *pNode;
	
		for(pMesh= m_pLib3dsFile->meshes; pMesh != NULL; pMesh = pMesh->next)
		{
			pNode= lib3ds_node_new_object();
			strcpy(pNode->name, pMesh->name);
			pNode->parent_id= LIB3DS_NO_PARENT;
			lib3ds_file_insert_node(m_pLib3dsFile, pNode);
		}
	  }
	  // Create GLC_Instance with Node
	   for (Lib3dsNode *pNode=m_pLib3dsFile->nodes; pNode!=0; pNode=pNode->next)
	   {
		   createMeshes(m_pWorld->rootProduct(), pNode);
	   }
	
	// Free Lib3dsFile and all its ressources
	lib3ds_file_free(m_pLib3dsFile);
	m_pLib3dsFile= NULL;
	return m_pWorld;
}

/////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

// clear stlToWorld allocate memmory and reset member
void GLC_3dsToWorld::clear()
{
	if (NULL != m_pCurrentMesh)
	{
		delete m_pCurrentMesh;
		m_pCurrentMesh= NULL;
	}
	m_pWorld= NULL;
	m_FileName.clear();
	m_pCurrentMesh= NULL;
	m_CurVertexIndex= 0;
	m_CurNormalIndex= 0;
	if (NULL != m_pLib3dsFile)
	{
		lib3ds_file_free(m_pLib3dsFile);
		m_pLib3dsFile= NULL;
	}
}

// Create meshes from the 3ds File
void GLC_3dsToWorld::createMeshes(GLC_Product* pProduct, Lib3dsNode* pFatherNode)
{
	GLC_Product* pChildProduct= NULL;
	Lib3dsMesh *pMesh= NULL;
	
	if (pFatherNode->type == LIB3DS_OBJECT_NODE)
	{
		// Check if the node is a mesh or dummy
		if (strcmp(pFatherNode->name,"$$$DUMMY")==0)
		{
			return;
		}		
		pMesh = lib3ds_file_mesh_by_name(m_pLib3dsFile, pFatherNode->data.object.morph);
	    if( pMesh == NULL )
	    {
	    	pMesh = lib3ds_file_mesh_by_name(m_pLib3dsFile, pFatherNode->name);
	    }
	    	
	    GLC_Instance instance(createInstance(pMesh));		   
	    pProduct->addChildPart(instance);
	}
	
	// If there is a child, create a child product
	if (NULL != pFatherNode->childs)
	{
		pChildProduct= pProduct->addNewChildProduct();
		if (NULL != pMesh)
		{
			Lib3dsMatrix M;
			lib3ds_matrix_copy(M, pMesh->matrix);
			lib3ds_matrix_inv(M);

			GLC_Matrix4x4 mat(&M[0][0]);
			
			//pChildProduct->move(mat);
		}
	}
	// Create Childs meshes if exists
	for (Lib3dsNode* pNode= pFatherNode->childs; pNode!=0; pNode= pNode->next)
	{	
		createMeshes(pChildProduct, pNode);
	}
	

}
//! Create Instance from a Lib3dsNode
GLC_Instance GLC_3dsToWorld::createInstance(Lib3dsMesh* p3dsMesh)
{
	GLC_Mesh2 * pMesh= new GLC_Mesh2();
	
	// The mesh normals
	const int normalsNumber= p3dsMesh->faces * 3;
	Lib3dsVector *normalL= static_cast<Lib3dsVector*>(malloc(normalsNumber * sizeof(Lib3dsVector)));
	lib3ds_mesh_calculate_normals(p3dsMesh, normalL);
	// Add the normal to the mesh
	for (int i= 0; i < normalsNumber; ++i)
	{
		pMesh->addNormal(i, GLC_Vector3d(normalL[i][0], normalL[i][1], normalL[i][2]));
	}
	delete normalL;
	
	// Add Vertex to the mesh
	const int vertexNumber= p3dsMesh->points;
	for (int i= 0; i < vertexNumber; ++i)
	{
		pMesh->addVertex(i, GLC_Vector3d(p3dsMesh->pointL[i].pos[0], p3dsMesh->pointL[i].pos[1], p3dsMesh->pointL[i].pos[2]));
	}
	QVector<int> material;
	material << -1 << -1 << -1;
	int normalIndex= 0;
	for (unsigned int i= 0; i < p3dsMesh->faces; ++i)
	{
		//Add the Normal
		QVector<int> normal;
		normal.append(normalIndex++);
		normal.append(normalIndex++);
		normal.append(normalIndex++);
		QVector<int> vertex;
		Lib3dsFace *p3dsFace=&p3dsMesh->faceL[i];
		vertex << p3dsFace->points[0] << p3dsFace->points[1] << p3dsFace->points[2];
		pMesh->addFace(material, vertex, normal);
	}
	return GLC_Instance(pMesh);
}

