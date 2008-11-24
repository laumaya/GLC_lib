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

//! \file glc_3dstoworld.cpp implementation of the GLC_3dsToWorld class.

#include "glc_3dstoworld.h"

#include "glc_mesh2.h"
#include "glc_world.h"
#include "glc_fileformatexception.h"
#include "glc_circle.h"
#include "glc_material.h"
#include "glc_vector2df.h"
#include "glc_vector3df.h"
// Lib3ds Header
#include "lib3ds/file.h"
#include "lib3ds/mesh.h"
#include "lib3ds/node.h"
#include "lib3ds/matrix.h"
#include "lib3ds/material.h"

#include <QFileInfo>
#include <QGLContext>

GLC_3dsToWorld::GLC_3dsToWorld(const QGLContext *pContext)
: m_pWorld(NULL)
, m_FileName()
, m_pQGLContext(pContext)
, m_pCurrentMesh(NULL)
, m_pLib3dsFile(NULL)
, m_Materials()
, m_NextMaterialIndex(0)
, m_LoadedMeshes()
, m_InitQuantumValue(50)
, m_CurrentQuantumValue(0)
, m_PreviousQuantumValue(0)
, m_NumberOfMeshes(0)
, m_CurrentMeshNumber(0)
{
}

GLC_3dsToWorld::~GLC_3dsToWorld()
{
	clear();
}

// Create an GLC_World from an input 3DS File
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
	// Close the file before open it with lib3ds
	file.close();

	//////////////////////////////////////////////////////////////////
	// Init member
	//////////////////////////////////////////////////////////////////
	m_pWorld= new GLC_World;

	//Load 3ds File
	m_pLib3dsFile=lib3ds_file_load(m_FileName.toLatin1().data());
	if (!m_pLib3dsFile)
	{
		QString message= "GLC_3dsToWorld::CreateWorldFrom3ds : Loading Failed";
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}
	// Evaluate Nodes Matrix for the first frame (Needed by instances)
	lib3ds_file_eval(m_pLib3dsFile, 0.0);
	m_CurrentQuantumValue= m_InitQuantumValue;
	m_PreviousQuantumValue= m_CurrentQuantumValue;

	emit currentQuantum(m_CurrentQuantumValue);
	// Count the number of meshes
	for(Lib3dsMesh *pMesh= m_pLib3dsFile->meshes; pMesh != NULL; pMesh = pMesh->next)
	{
		++m_NumberOfMeshes;
	}
	// Check if there is some meshes in the 3ds file
	if (0 == m_NumberOfMeshes)
	{
		QString message= "GLC_3dsToWorld::CreateWorldFrom3ds : No mesh found !";
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
	}

	// Create GLC_Instance with Node
	for (Lib3dsNode *pNode=m_pLib3dsFile->nodes; pNode!=0; pNode=pNode->next)
	{
		createMeshes(m_pWorld->rootProduct(), pNode);
	}

	// Load unloaded mesh name
	for(Lib3dsMesh *pMesh= m_pLib3dsFile->meshes; pMesh != NULL; pMesh = pMesh->next)
	{
		if (!m_LoadedMeshes.contains(QString(pMesh->name)))
		{
			//qDebug() << "Mesh without parent found" << QString(pMesh->name);
			Lib3dsNode *pNode= lib3ds_node_new_object();
			strcpy(pNode->name, pMesh->name);
			pNode->parent_id= LIB3DS_NO_PARENT;
			lib3ds_file_insert_node(m_pLib3dsFile, pNode);
			createMeshes(m_pWorld->rootProduct(), pNode);
		}
	}

	// Free Lib3dsFile and all its ressources
	lib3ds_file_free(m_pLib3dsFile);
	m_pLib3dsFile= NULL;
	emit currentQuantum(100);
	// Create the world bounding box
	m_pWorld->collection()->boundingBox();
	return m_pWorld;
}

/////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

// clear 3dsToWorld allocate memory and reset member
void GLC_3dsToWorld::clear()
{
	if (NULL != m_pCurrentMesh)
	{
		delete m_pCurrentMesh;
		m_pCurrentMesh= NULL;
	}
	m_pWorld= NULL;
	m_FileName.clear();
	if (NULL != m_pLib3dsFile)
	{
		lib3ds_file_free(m_pLib3dsFile);
		m_pLib3dsFile= NULL;
	}

	// Remove unused material
	QHash<QString, GLC_Material*>::iterator i;
	for (i= m_Materials.begin(); i != m_Materials.end(); ++i)
	{
		if (i.value()->isUnused()) delete i.value();
	}
	m_Materials.clear();
	m_NextMaterialIndex= 0;
	// Clear the loaded meshes Set
	m_LoadedMeshes.clear();
	// Progress indicator
	m_CurrentQuantumValue= 0;
	m_PreviousQuantumValue= 0;
	m_NumberOfMeshes= 0;
	m_CurrentMeshNumber= 0;

}

// Create meshes from the 3ds File
void GLC_3dsToWorld::createMeshes(GLC_Product* pProduct, Lib3dsNode* pFatherNode)
{
	GLC_Product* pChildProduct= NULL;
	Lib3dsMesh *pMesh= NULL;

	if (pFatherNode->type == LIB3DS_OBJECT_NODE)
	{
		//qDebug() << "Node type LIB3DS_OBJECT_NODE is named : " << QString(pFatherNode->name);
		//qDebug() << "Node Matrix :";
		//qDebug() << GLC_Matrix4x4(&(pFatherNode->matrix[0][0])).toString();

		// Check if the node is a mesh or dummy
		if (!(strcmp(pFatherNode->name,"$$$DUMMY")==0))
		{
	    	pMesh = lib3ds_file_mesh_by_name(m_pLib3dsFile, pFatherNode->name);
		    if( pMesh != NULL )
		    {
		    	GLC_Instance instance(createInstance(pMesh));
		    	// Test if there is vertex in the mesh
		    	GLC_Mesh2* pGLCMesh= dynamic_cast<GLC_Mesh2*>(instance.getGeometry());
		    	if (0 != pGLCMesh->numberOfVertex())
		    	{
		    		m_LoadedMeshes.insert(pGLCMesh->name());
			    	// Load node matrix
			    	GLC_Matrix4x4 nodeMat(&(pFatherNode->matrix[0][0]));
					// The mesh matrix to inverse
			    	GLC_Matrix4x4 matInv(&(pMesh->matrix[0][0]));
					matInv.invert();
					// Get the node pivot
					Lib3dsObjectData *pObjectData;
					pObjectData= &pFatherNode->data.object;
					GLC_Matrix4x4 trans(-pObjectData->pivot[0], -pObjectData->pivot[1], -pObjectData->pivot[2]);
					// Compute the part matrix
					nodeMat= nodeMat * trans * matInv; // I don't know why...
					// move the part by the matrix
					pProduct->addChildPart(instance)->move(nodeMat);
		    	}
		    }
		} // End If DUMMY
	}
	else return;
	// If there is a child, create a child product
	if (NULL != pFatherNode->childs)
	{
		pChildProduct= pProduct->addNewChildProduct();
		//pChildProduct->move(GLC_Matrix4x4(&(pFatherNode->matrix[0][0])));

		// Create Childs meshes if exists
		for (Lib3dsNode* pNode= pFatherNode->childs; pNode!=0; pNode= pNode->next)
		{
			createMeshes(pChildProduct, pNode);
		}
	}


}
//! Create Instance from a Lib3dsNode
GLC_Instance GLC_3dsToWorld::createInstance(Lib3dsMesh* p3dsMesh)
{
	QString meshName(p3dsMesh->name);
	if (m_LoadedMeshes.contains(meshName))
	{
		// This mesh as been already loaded
		QList<GLC_Instance*> instancesList(m_pWorld->collection()->getInstancesHandle());
		GLC_Instance* pCurrentInstance= NULL;
		int currentIndex= -1;
		do
		{
			pCurrentInstance= instancesList[++currentIndex];
		} while (pCurrentInstance->name() != meshName);
		// return an instance.
		//qDebug() << "instance";
		return pCurrentInstance->instanciate().resetMatrix();
	}
	GLC_Mesh2 * pMesh= new GLC_Mesh2();
	pMesh->setName(p3dsMesh->name);
	// The mesh normals
	const int normalsNumber= p3dsMesh->faces * 3;
	Lib3dsVector *normalL= static_cast<Lib3dsVector*>(malloc(normalsNumber * sizeof(Lib3dsVector)));
	lib3ds_mesh_calculate_normals(p3dsMesh, normalL);

	int normalIndex= 0;
	VertexList triangle;
	GLC_Vertex testVertex;
	triangle.append(testVertex);
	triangle.append(testVertex);
	triangle.append(testVertex);
	for (unsigned int i= 0; i < p3dsMesh->faces; ++i)
	{
		Lib3dsFace *p3dsFace=&p3dsMesh->faceL[i];
		for (int i=0; i < 3; ++i)
		{
			// Add vertex coordinate
			triangle[i].x= p3dsMesh->pointL[p3dsFace->points[i]].pos[0];
			triangle[i].y= p3dsMesh->pointL[p3dsFace->points[i]].pos[1];
			triangle[i].z= p3dsMesh->pointL[p3dsFace->points[i]].pos[2];
			// Add vertex Normal
			triangle[i].nx= normalL[normalIndex][0];
			triangle[i].ny= normalL[normalIndex][1];
			triangle[i].nz= normalL[normalIndex][2];
			++normalIndex;
			// Add texel
			if (p3dsMesh->texels > 0)
			{
				triangle[i].s= p3dsMesh->texelL[p3dsFace->points[i]][0];
				triangle[i].t= p3dsMesh->texelL[p3dsFace->points[i]][1];
			}
			else
			{
				triangle[i].s= 0.0f;
				triangle[i].t= 0.0f;
			}
		}

		// Load the material
		// The material current face index
		GLC_Material* pCurMaterial= NULL;
		if (p3dsFace->material[0])
		{
			Lib3dsMaterial* p3dsMat=lib3ds_file_material_by_name(m_pLib3dsFile, p3dsFace->material);
			if (NULL != p3dsMat)
			{
				// Check it this material as already been loaded
				const QString materialName(p3dsFace->material);

				if (!m_Materials.contains(materialName))
				{ // Material not already loaded, load it
					loadMaterial(p3dsMat);
				}
				pCurMaterial= m_Materials[materialName];
			}
		}
		pMesh->addTriangles(triangle, pCurMaterial);
	}
	// free normal memmory
	delete normalL;
	// Compute loading progress
	++m_CurrentMeshNumber;
	m_CurrentQuantumValue = static_cast<int>((static_cast<double>(m_CurrentMeshNumber) / m_NumberOfMeshes) * (100 - m_InitQuantumValue)) + m_InitQuantumValue;
	if (m_CurrentQuantumValue > m_PreviousQuantumValue)
	{
		emit currentQuantum(m_CurrentQuantumValue);
	}
	m_PreviousQuantumValue= m_CurrentQuantumValue;

	pMesh->finished();
	return GLC_Instance(pMesh);
}

// Load Material
void GLC_3dsToWorld::loadMaterial(Lib3dsMaterial* p3dsMaterial)
{
	GLC_Material* pMaterial= new GLC_Material;
	// Set the material name
	const QString materialName(p3dsMaterial->name);
	pMaterial->setName(materialName);
	// Check if there is a texture
	if (p3dsMaterial->texture1_map.name[0])
	{
		const QString textureName(p3dsMaterial->texture1_map.name);
		// Retrieve the .3ds file path
		QFileInfo fileInfo(m_FileName);
		QString textureFileName(fileInfo.absolutePath() + QDir::separator());
		textureFileName.append(textureName);
		QFile textureFile(textureFileName);

		if (textureFile.open(QIODevice::ReadOnly))
		{
			// Create the texture and assign it to the material
			GLC_Texture *pTexture = new GLC_Texture(m_pQGLContext, textureFile);
			pMaterial->setTexture(pTexture);
		}
		textureFile.close();
	}

	// Ambient Color
	QColor ambient;
	ambient.setRgbF(p3dsMaterial->ambient[0], p3dsMaterial->ambient[1], p3dsMaterial->ambient[2]);
	ambient.setAlphaF(p3dsMaterial->ambient[3]);
	pMaterial->setAmbientColor(ambient);
	// Diffuse Color
	QColor diffuse;
	diffuse.setRgbF(p3dsMaterial->diffuse[0], p3dsMaterial->diffuse[1], p3dsMaterial->diffuse[2]);
	diffuse.setAlphaF(p3dsMaterial->diffuse[3]);
	pMaterial->setDiffuseColor(diffuse);
	// Specular Color
	QColor specular;
	specular.setRgbF(p3dsMaterial->specular[0], p3dsMaterial->specular[1], p3dsMaterial->specular[2]);
	specular.setAlphaF(p3dsMaterial->specular[3]);
	pMaterial->setSpecularColor(specular);
	// Shininess

	if (0 != p3dsMaterial->shininess)
	{
		float matShininess= p3dsMaterial->shininess * 128.0;
		if (matShininess > 128.0) matShininess= 128.0;
		pMaterial->setShininess(matShininess);
	}
	// Transparency

	pMaterial->setTransparency(1.0 - p3dsMaterial->transparency);

	// Add the material to the hash table
	m_Materials.insert(materialName, pMaterial);
}

