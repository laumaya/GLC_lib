/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.8, packaged on January, 2008.

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

//! \file glc_objToworld.h interface for the GLC_ObjToWorld class.

#ifndef GLC_OBJTOWORLD_H_
#define GLC_OBJTOWORLD_H_

#include <QFile>
#include <QString>
#include <QObject>
#include <QHash>
#include <QVector>
#include <glc_vector3d.h>
#include <glc_vector2d.h>

#include "glc_mesh2.h"

class GLC_World;
class GLC_ObjMtlLoader;
class QGLContext;

//////////////////////////////////////////////////////////////////////
//! \class GLC_ObjToWorld
/*! \brief GLC_ObjToWorld : Create an GLC_World from obj file */

/*! An GLC_ObjToWorld extract the meshs from an .obj file \n
 * 	List of elements extracted from the OBJ
 * 		- Vertex
 * 		- Face
 * 		- Texture coordinate
 * 		- Normal coordinate
  */
//////////////////////////////////////////////////////////////////////
class GLC_ObjToWorld : public QObject
{
	Q_OBJECT
	
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
	
public:
	GLC_ObjToWorld(const QGLContext*);
	virtual ~GLC_ObjToWorld();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an GLC_World from an input OBJ File
	GLC_World* CreateWorldFromObj(QFile &file);
//@}

//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////
private:
	//! Return the name of the mtl file
	QString getMtlLibFileName(QString);
	//! Scan a line previously extracted from OBJ file
	void scanLigne(QString &);
	//! Change current group
	void changeGroup(QString);
	//! Extract a 3D Vector from a string
	GLC_Vector3d extract3dVect(QString &);
	//! Extract a 2D Vector from a string
	GLC_Vector2d extract2dVect(QString &);
	//! Extract a face from a string
	void extractFaceIndex(QString &);
	//! Set Current material index
	void setCurrentMaterial(QString &line);	
	//! Extract a vertex from a string
	void extractVertexIndex(QString ligne, int &Coordinate, int &Normal, int &TextureCoordinate);
	//! set the OBJ File type
	void setObjType(QString &);
	//! compute face normal
	GLC_Vector3d computeNormal(QVector<int> &);
	//! Add Vertexs in the current mesh
	void addVertexsToCurrentMesh(QVector<int> &);
	//! Add Normals in the current mesh
	void addNormalsToCurrentMesh(QVector<int> &);
	//! Add TextureCoordinate in the current mesh
	void addTextureCoordinatesToCurrentMesh(QVector<int> &);
		
//////////////////////////////////////////////////////////////////////
// Qt Signals
//////////////////////////////////////////////////////////////////////
	signals:
	void currentQuantum(int);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! pointer to a GLC_World
	GLC_World* m_pWorld;
	//! The Obj File name
	QString m_FileName;
	//! OpenGL Context
	const QGLContext* m_pQGLContext;	
	//! the Obj Mtl loader
	GLC_ObjMtlLoader* m_pMtlLoader;
	//! The current line number
	int m_CurrentLineNumber;
	//! The current mesh
	GLC_Mesh2* m_pCurrentMesh;
	
	//! Coordinate hash table
	Vector3dHash m_VertexHash;
	//! Normal hash table
	Vector3dHash m_NormalHash;
	//! Texture coordinate Hash table
	Vector2dHash m_TextCoordinateHash;
	
	//! Index of the current vertex
	int m_CurVertexIndex;
	//! Index of the current normal
	int m_CurNormalIndex;
	//! Index of the current texture coordinate
	int m_CurTextureCoordinateIndex;

	//! Face type
	FaceType m_FaceType;
	
	//! List of material already used by the current mesh
	QHash<QString, int> m_CurrentMeshMaterials;
	
	//! current mesh material index
	int m_CurrentMeshMaterialIndex;
	
	//! Current Computed vector normal index
	int m_CurComputedVectNormIndex;

};

#endif /*GLC_OBJTOWORLD_H_*/
