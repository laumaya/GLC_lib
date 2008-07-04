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
#include "glc_material.h"
// Lib3ds Header
#include "lib3ds/file.h"
#include "lib3ds/mesh.h"
#include "lib3ds/node.h"
#include "lib3ds/matrix.h"
#include "lib3ds/material.h"

GLC_3dsToWorld::GLC_3dsToWorld(const QGLContext *pContext)
: m_pWorld(NULL)
, m_FileName()
, m_pQGLContext(pContext)
, m_pCurrentMesh(NULL)
, m_CurVertexIndex(0)
, m_CurNormalIndex(0)
, m_pLib3dsFile(NULL)
, m_Materials()
, m_MaterialsIndex()
, m_NextMaterialIndex(0)
, m_pCurrentMaterial(NULL)
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
	if (m_pWorld->collection()->isEmpty())
	{
		QString message= "GLC_3dsToWorld::CreateWorldFrom3ds : No mesh found !";
		GLC_FileFormatException fileFormatException(message, m_FileName);
		clear();
		throw(fileFormatException);
		
	}
	
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
	
	// Remove unused material
	QHash<QString, GLC_Material*>::iterator i;
	for (i= m_Materials.begin(); i != m_Materials.end(); ++i)
	{
		if (i.value()->isUnused()) delete i.value();
	}
	m_Materials.clear();
	// Clear the material index hash table
	m_MaterialsIndex.clear();
	m_NextMaterialIndex= 0;
	// Set the current material to NULL
	m_pCurrentMaterial= NULL;
	
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
	pMesh->setName(p3dsMesh->name);
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
	
	int normalIndex= 0;
	int textureIndex= 0;
	
	for (unsigned int i= 0; i < p3dsMesh->faces; ++i)
	{
		//Add the Normal
		QVector<int> normal;
		normal.append(normalIndex++);
		normal.append(normalIndex++);
		normal.append(normalIndex++);
		QVector<int> vertex;
		Lib3dsFace *p3dsFace=&p3dsMesh->faceL[i];
		// Load the material
		// The material current face index
		QVector<int> material;
		
		if (p3dsFace->material[0])
		{
			Lib3dsMaterial* p3dsMat=lib3ds_file_material_by_name(m_pLib3dsFile, p3dsFace->material);
			// Check it this material as already been loaded
			const QString materialName(p3dsFace->material);
			if (!m_Materials.contains(materialName))
			{ // Material not already loaded, load it
				loadMaterial(p3dsMat);
				// Add the loaded material to the mesh
				pMesh->addMaterial(m_MaterialsIndex[materialName], m_Materials[materialName]);
			}
			// Set the current material
			m_pCurrentMaterial= m_Materials[materialName];
			
			const int index= m_MaterialsIndex[materialName];
			material << index << index << index;
		}
		else // No material
		{
			if (NULL != m_pCurrentMaterial)
			{
				const int index= m_MaterialsIndex[m_pCurrentMaterial->getName()];
				pMesh->addMaterial(index, m_pCurrentMaterial);
				material << index << index << index;
			}
			else
			{
				material << -1 << -1 << -1;
			}
		}
		// End of loading material
		vertex << p3dsFace->points[0] << p3dsFace->points[1] << p3dsFace->points[2];
		// check if the mesh have texture coordinate
		if (p3dsMesh->texels > 0)
		{
			QVector<int> texture;
			for (int i= 0; i < 3; ++i)
			{
				// Load texture coordinate
				float x= p3dsMesh->texelL[p3dsFace->points[i]][0];
				float y= p3dsMesh->texelL[p3dsFace->points[i]][1];
				GLC_Vector2d texel(static_cast<double>(x), static_cast<double>(y));
				texture.append(textureIndex);
				pMesh->addTextureCoordinate(textureIndex++, texel);
			}
			pMesh->addFace(material, vertex, normal, texture);
			
		}
		else
		{
			pMesh->addFace(material, vertex, normal);
		}
		
	}
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
	//pMaterial->setShininess(pow(2, 10.0 * p3dsMaterial->shininess));
	// Transparency
	pMaterial->setTransparency(1.0 - p3dsMaterial->transparency);
	
	// Add the material to the hash table
	m_Materials.insert(materialName, pMaterial);
	// Add the material index
	m_MaterialsIndex.insert(materialName, m_NextMaterialIndex++);
}

