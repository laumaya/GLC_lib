/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/
//! \file glc_worldto3ds.cpp implementation of the GLC_WorldTo3ds class.

#include <QFile>
#include <QtDebug>

#include "../geometry/glc_3drep.h"
#include "../geometry/glc_geometry.h"
#include "../geometry/glc_mesh.h"

#include "../shading/glc_material.h"

// Lib3ds Header
#include "3rdparty/lib3ds/file.h"
#include "3rdparty/lib3ds/mesh.h"
#include "3rdparty/lib3ds/node.h"
#include "3rdparty/lib3ds/matrix.h"
#include "3rdparty/lib3ds/material.h"

#include "glc_worldto3ds.h"

GLC_WorldTo3ds::GLC_WorldTo3ds(const GLC_World& world)
: QObject()
, m_World(world)
, m_pLib3dsFile(NULL)
, m_FileName()
, m_ReferenceToMesh()
, m_NameToMaterial()
, m_pRootLib3dsNode(NULL)
, m_CurrentNodeId(0)
, m_OccIdToNodeId()
{


}

GLC_WorldTo3ds::~GLC_WorldTo3ds()
{

}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
bool GLC_WorldTo3ds::exportToFile(const QString& fileName)
{
	qDebug() << "GLC_WorldTo3ds::exportToFile";
	m_ReferenceToMesh.clear();
	m_NameToMaterial.clear();
	m_pRootLib3dsNode= NULL;
	m_CurrentNodeId= 0;
	m_OccIdToNodeId.clear();

	m_FileName= fileName;
	bool subject= false;
	{
		QFile exportFile(m_FileName);
		subject= exportFile.open(QIODevice::WriteOnly);
		exportFile.close();
	}
	if (subject)
	{
		m_pLib3dsFile= lib3ds_file_new();
		saveWorld();
		subject= lib3ds_file_save(m_pLib3dsFile, fileName.toLocal8Bit().data());
	}

	qDebug() << "export success = " << subject;
	return subject;
}


//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////

void GLC_WorldTo3ds::saveWorld()
{
	qDebug() << "GLC_WorldTo3ds::saveWorld()";
	saveMeshes();
	saveBranch(m_World.rootOccurence());
}

void GLC_WorldTo3ds::saveMeshes()
{
	qDebug() << "GLC_WorldTo3ds::saveMeshes()";
	// Retrieve the list of mesh and save them into the 3ds
	QList<GLC_StructReference*> refList= m_World.references();
	const int refCount= refList.count();
	int meshIndex= 0;
	for (int i= 0; i < refCount; ++i)
	{
		GLC_StructReference* pRef= refList.at(i);
		if (pRef->hasRepresentation())
		{
			GLC_3DRep* pRep= dynamic_cast<GLC_3DRep*>(pRef->representationHandle());
			if (NULL != pRep)
			{
				// This reference has a mesh
				const QString meshName= pRep->name() + '_' + QString::number(++meshIndex);
				QList<Lib3dsMesh*> meshes= createMeshsFrom3DRep(pRep, meshName);
				{
					const int count= meshes.count();
					for (int i= 0; i < count; ++i)
					{
						lib3ds_file_insert_mesh(m_pLib3dsFile, meshes.at(i));
						m_ReferenceToMesh.insertMulti(pRef, meshes.at(i));
					}
				}
			}
		}
	}
}

void GLC_WorldTo3ds::saveBranch(GLC_StructOccurence* pOcc)
{
	createNodeFromOccurrence(pOcc);

	const int childCount= pOcc->childCount();
	for (int i= 0; i < childCount; ++i)
	{
		saveBranch(pOcc->child(i));
	}
}

void GLC_WorldTo3ds::createNodeFromOccurrence(GLC_StructOccurence* pOcc)
{
	Lib3dsNode* p3dsNode = lib3ds_node_new_object();
	m_OccIdToNodeId.insert(pOcc->id(), m_CurrentNodeId++);

	// Set the parent id
	if (!pOcc->hasParent())
	{
		p3dsNode->parent_id= LIB3DS_NO_PARENT;
	}
	else
	{
		Q_ASSERT(m_OccIdToNodeId.contains(pOcc->parent()->id()));
		p3dsNode->parent_id= m_OccIdToNodeId.value(pOcc->parent()->id());
	}

	// Set mesh name if necessary
	GLC_StructReference* pRef= pOcc->structReference();
	if (m_ReferenceToMesh.contains(pRef))
	{
		strcpy(p3dsNode->name, m_ReferenceToMesh.value(pRef)->name);
	}

	// Set the matrix
	const GLC_Matrix4x4 relativeMatrix= pOcc->structInstance()->relativeMatrix();
	p3dsNode->matrix[0][0]= relativeMatrix.getData()[0];
	p3dsNode->matrix[1][0]= relativeMatrix.getData()[1];
	p3dsNode->matrix[2][0]= relativeMatrix.getData()[2];
	p3dsNode->matrix[3][0]= relativeMatrix.getData()[3];

	p3dsNode->matrix[0][1]= relativeMatrix.getData()[4];
	p3dsNode->matrix[1][1]= relativeMatrix.getData()[5];
	p3dsNode->matrix[2][1]= relativeMatrix.getData()[6];
	p3dsNode->matrix[3][1]= relativeMatrix.getData()[7];

	p3dsNode->matrix[0][2]= relativeMatrix.getData()[8];
	p3dsNode->matrix[1][2]= relativeMatrix.getData()[9];
	p3dsNode->matrix[2][2]= relativeMatrix.getData()[10];
	p3dsNode->matrix[3][2]= relativeMatrix.getData()[11];

	p3dsNode->matrix[0][3]= relativeMatrix.getData()[12];
	p3dsNode->matrix[1][3]= relativeMatrix.getData()[13];
	p3dsNode->matrix[2][3]= relativeMatrix.getData()[14];
	p3dsNode->matrix[3][3]= relativeMatrix.getData()[15];


	lib3ds_file_insert_node(m_pLib3dsFile, p3dsNode);
}

QList<Lib3dsMesh*> GLC_WorldTo3ds::createMeshsFrom3DRep(GLC_3DRep* pRep, const QString& name)
{
	QList<Lib3dsMesh*> subject;


	int bodyIndex= 0;

	const int bodyCount= pRep->numberOfBody();
	for (int i= 0; i < bodyCount; ++i)
	{
		GLC_Mesh* pCurrentMesh= dynamic_cast<GLC_Mesh*>(pRep->geomAt(i));
		if ((NULL != pCurrentMesh) && !pCurrentMesh->isEmpty())
		{
			bool deleteCurrentMesh= false;
			if (pCurrentMesh->lodCount() > 1)
			{
				// Keep only the first level of detail
				pCurrentMesh= pCurrentMesh->createMeshOfGivenLod(0);
				deleteCurrentMesh= true;
			}
			const QString bodyMeshName= name + '_' + QString::number(bodyIndex++);
			Lib3dsMesh* p3dsMesh= create3dsMeshFromGLC_Mesh(pCurrentMesh, bodyMeshName);

			if (deleteCurrentMesh) delete pCurrentMesh;
			subject.append(p3dsMesh);
		}
	}


	return subject;
}

Lib3dsMesh* GLC_WorldTo3ds::create3dsMeshFromGLC_Mesh(GLC_Mesh* pMesh, const QString& meshName)
{
	// Create empty 3ds mesh with the given name
	Lib3dsMesh* p3dsMesh= lib3ds_mesh_new(meshName.toLocal8Bit().data());
	const uint pointsCount= pMesh->VertexCount();

	// Add points to the 3DS mesh
	lib3ds_mesh_new_point_list(p3dsMesh, pointsCount);
	GLfloatVector vertice= pMesh->positionVector();
	for (uint i= 0; i < pointsCount; ++i)
	{
		Lib3dsPoint point;
		point.pos[0]= vertice[i * 3];
		point.pos[1]= vertice[i * 3 + 1];
		point.pos[2]= vertice[i * 3 + 2];

		p3dsMesh->pointL[i]= point;
	}

	// Add texel to the 3DS mesh
	GLfloatVector texelVector= pMesh->texelVector();
	if(!texelVector.isEmpty())
	{
		lib3ds_mesh_new_texel_list(p3dsMesh, pointsCount);
		for (uint i= 0; i < pointsCount; ++i)
		{
			p3dsMesh->texelL[i][0]= texelVector[i * 2];
			p3dsMesh->texelL[i][1]= texelVector[i * 2 + 1];
		}
	}

	// Add faces to the 3ds mesh
	const uint totalFaceCount= pMesh->faceCount(0);
	lib3ds_mesh_new_face_list(p3dsMesh, totalFaceCount);

	QSet<GLC_Material*> materialSet= pMesh->materialSet();
	QSet<GLC_Material*>::iterator iMat= materialSet.begin();
	uint currentFaceIndex= 0;
	while(iMat != materialSet.end())
	{
		GLC_Material* pCurrentGLCMat= *iMat;
		Lib3dsMaterial* pMaterial= get3dsMaterialFromGLC_Material(pCurrentGLCMat);
		IndexList currentTriangleIndex= pMesh->getEquivalentTrianglesStripsFansIndex(0, pCurrentGLCMat->id());
		const int faceCount= currentTriangleIndex.count() / 3;
		for (int i= 0; i < faceCount; ++i)
		{
			Lib3dsFace face;
			strcpy(face.material, pMaterial->name);
			face.points[0]= currentTriangleIndex.at(i * 3);
			face.points[1]= currentTriangleIndex.at(i * 3 + 1);
			face.points[2]= currentTriangleIndex.at(i * 3 + 2);

			p3dsMesh->faceL[currentFaceIndex++]= face;
			Q_ASSERT(currentFaceIndex <= totalFaceCount);
		}
		++iMat;
	}

	return p3dsMesh;
}

Lib3dsMaterial* GLC_WorldTo3ds::get3dsMaterialFromGLC_Material(GLC_Material* pMat)
{
	Lib3dsMaterial* pSubject= NULL;
	const QString matName= materialName(pMat);
	if (m_NameToMaterial.contains(matName))
	{
		pSubject= m_NameToMaterial.value(matName);
	}
	else
	{
		pSubject= create3dsMaterialFromGLC_Material(pMat, matName);
	}

	return pSubject;
}

Lib3dsMaterial* GLC_WorldTo3ds::create3dsMaterialFromGLC_Material(GLC_Material* pMat, const QString& matName)
{
	Lib3dsMaterial* pSubject= lib3ds_material_new();
	strcpy(pSubject->name, matName.toLocal8Bit().data());

	lib3ds_file_insert_material(m_pLib3dsFile, pSubject);
	m_NameToMaterial.insert(matName, pSubject);

	return pSubject;
}

QString GLC_WorldTo3ds::materialName(GLC_Material* pMat) const
{
	QString subject= pMat->name() + '_' + QString::number(pMat->id());
	subject= subject.right(63);

	return subject;
}


