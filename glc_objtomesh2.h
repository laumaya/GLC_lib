/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.7, packaged on September, 2007.

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

//! \file glc_objToMesh2.h interface for the GLC_ObjToMesh2 class.

#ifndef GLC_OBJ_TO_MESH2_H_
#define GLC_OBJ_TO_MESH2_H_

#include <QObject>
#include <QString>
#include <QHash>

#include "glc_mesh2.h"

typedef QHash<QString, int> MaterialHashMap;

enum FaceType
{
	notSet,
	coordinate,
	coordinateAndTexture,
	coordinateAndNormal,
	coordinateAndTextureAndNormal
};

//////////////////////////////////////////////////////////////////////
//! \class GLC_ObjToMesh2
/*! \brief GLC_ObjToMesh2 : Create an GLC_Mesh from obj file */

/*! An GLC_ObjToMesh2 extract a single mesh from an .obj file \n
 * 	List of elements extracted from the OBJ
 * 		- Vertex
 * 		- Face
 * 		- Texture coordinate
 * 		- Normal coordinate
  */
//////////////////////////////////////////////////////////////////////

class GLC_ObjToMesh2 : public QObject
{
	Q_OBJECT
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	/*! Create an empty ObjToMesh object */
	GLC_ObjToMesh2(const QGLContext *pContext);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an GLC_Mesh from an input OBJ File
	GLC_Mesh2* CreateMeshFromObj(QFile &file);
//@}

//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////
private:
	//! Scan a line previously extracted from OBJ file
	void scanLigne(QString &line);

	//! Extract a 3D Vector from a string
	GLC_Vector3d extract3dVect(QString &line);

	//! Extract a 2D Vector from a string
	GLC_Vector2d extract2dVect(QString &line);

	//! Extract a face from a string
	void extractFaceIndex(QString &line);
	
	//! Set Current material index
	void setCurrentMaterial(QString &line);	

	//! Extract a vertex from a string
	void extractVertexIndex(QString ligne, int &Coordinate, int &Normal, int &TextureCoordinate);
	
	//! Check if a material file exist
	void loadMaterial(QString fileName);
	
	//! Extract String
	void extractMaterialName(QString &ligne, GLC_Material *pMaterial);
	
	//! Extract String
	void extractTextureFileName(QString &ligne, GLC_Material *pMaterial);
	
	//! Extract RGB value
	void extractRGBValue(QString &ligne, GLC_Material *pMaterial);
	
	//! Extract One value
	void extractOneValue(QString &ligne, GLC_Material *pMaterial);
	
	//! set the OBJ File type
	void setObjType(QString &);
	
	//! compute face normal
	GLC_Vector3d computeNormal(QVector<int> &, GLC_Mesh2*);
	
	//! Get texture file name without parameters
	QString getTextureName(QTextStream &, const QString &);

//////////////////////////////////////////////////////////////////////
// Qt Signals
//////////////////////////////////////////////////////////////////////
	signals:
	void currentQuantum(int);
	
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! pointer to a GLC_Mesh
	GLC_Mesh2* m_pMesh;
	
	//! Current material
	GLC_Material* m_pCurrentMaterial;
	
	//! Current material Index
	int m_CurrentMaterialIndex;
	
	
//! @name Array of Vertex Vector
//@{
	//! Number of Vertex Vector to extract
	int m_nNbrVectPos;
	//! Index of current vertex vector
	int m_nCurVectPos;
//@} End of Array of Vertex Vector

//! @name Array of Nomal Vector
//@{
	//! Number of normal vector to extract
	int m_nNbrVectNorm;
	//! Index of current normal vector
	int m_nCurVectNorm;
	//! Index of current computed normal vector
	int m_CurComputedVectNorm;
//@} End of array of normal vector

//! @name Array of texture coordinate vector
//@{
	//! Number of Texture coordinate vector to extract
	int m_nNbrVectTexture;
	//! Index of current Texture coordinate vector
	int m_nCurVectTexture;
//@} End of array of texture coordinate vector

	//! OBJ File name
	QString m_sFile;
	
	//! Material name <=> index Hash table
	MaterialHashMap m_MaterialNameIndex;
	
	//! OpenGL Context
	const QGLContext *m_pQGLContext;
	
	//! Face type
	FaceType m_FaceType;
	
	//! Current Line number
	int m_CurrentLineNumber;

};
#endif //GLC_OBJ_TO_MESH2_H_
