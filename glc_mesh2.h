/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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

//! \file glc_mesh2.h interface for the GLC_Mesh2 class.

#ifndef GLC_MESH2_H_
#define GLC_MESH2_H_
#include <QHash>
#include <QList>
#include <QVector>

#include "glc_material.h"
#include "glc_geometry.h"

//! The mesh's faces list
typedef QHash<int, GLC_Vector3d> Vector3dHash;
typedef QHash<int, GLC_Vector2d> Vector2dHash;

typedef QHash<int, GLC_Material> MaterialHash;

typedef QList<int> IndexedList;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Mesh2
/*! \brief GLC_Mesh2 : OpenGL 3D Mesh*/

/*! An GLC_Mesh2 is Mesh composed of polygons
 * 		- Each polygon must be a planar Face
 * 		- Each face is composed by vertexs
 * */
//////////////////////////////////////////////////////////////////////
class GLC_Mesh2 : public GLC_Geometry
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Construct an empty Mesh
	explicit GLC_Mesh2(const GLfloat *pColor= 0);
	
	//! Copy constructor
	explicit GLC_Mesh2(const GLC_Mesh2 &meshToCopy);
	
	//! Delete Mesh's faces and clear faces list
	virtual ~GLC_Mesh2(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Get mesh material
	void GetOneMaterial(int Index, GLC_Material &Material);

	//! return the mesh bounding box
	virtual GLC_BoundingBox* getBoundingBox(void) const;

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Add material to mesh
	void AddMaterial(int Index, GLC_Material &Material);
	
	//! Modify mesh material
	void ModifyMaterial(int Index, GLC_Material &Material);
	
	//! Add a vertex to mesh
	void AddCoordinate(int Index, GLC_Vector3d Coordinate);
	
	//! Add Normal
	void AddNormal(int index, GLC_Vector3d Normal);
	
	//! Add texture coordinate
	void AddTextureCoordinate(int Index, GLC_Vector2d TextureCoordinate);
		 
	//! Add a face without texture coordinate
	/*! Mesh list of texture index must be empty.
	 */  
	void AddFace(const QVector<int> &Material, const QVector<int> &Coordinate, const QVector<int> &Normal);

	//! Add a face with texture coordinate
	/*! Number of coordinate, Normal and texture must be equal
	 */
	void AddFace(const QVector<int> &Material, const QVector<int> &Coordinate, const QVector<int> &Normal,
				const QVector<int> &TextureCoordinate);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n
	 *  Throw GLC_OpenGlException*/
	virtual void GlDraw(void);

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function is implemented here.\n
	 *  Throw GLC_OpenGlException*/
	virtual void GlPropGeom(void);
//@}

//////////////////////////////////////////////////////////////////////
//! Private services Functions
//////////////////////////////////////////////////////////////////////
private:

	//! Add coordinate and normal index of a face
	void AddCoordAndNormIndex(const QVector<int> &Coordinate, const QVector<int> &Normal);
	
	//! Add Texture coordinate index of a face
	void AddTextureIndex(const QVector<int> &TextureCoordinate);
	
	//! Add Material index of a face
	void AddMaterialIndex(const QVector<int> &Material);
	
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

		
	//! Coordinate hash table
	Vector3dHash m_CoordinateHash;
	//! Coordinate index
	IndexedList m_CoordinateIndex;
	
	//! Material Hash table
	MaterialHash m_MaterialHash;
	//! Material index
	IndexedList m_MaterialIndex;
	
	//! Normal hash table
	Vector3dHash m_NormalHash;
	//! Normal Index
	IndexedList m_NormalIndex;
	
	//! Texture coordinate Hash table
	Vector2dHash m_TextCoordinateHash;
	//! Texture index
	IndexedList m_TextureIndex;
	
	int m_NumberOfFaces;
	
};
#endif //GLC_MESH2_H_
