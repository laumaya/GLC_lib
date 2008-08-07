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

//! \file glc_mesh2.h interface for the GLC_Mesh2 class.

#ifndef GLC_MESH2_H_
#define GLC_MESH2_H_
#include <QHash>
#include <QList>
#include <QVector>

#include "glc_material.h"
#include "glc_geometry.h"
#include "glc_vector2df.h"
#include "glc_vector3df.h"

//! The mesh's faces list
typedef QHash<int, GLC_Vector3df> Vector3dHash;
typedef QHash<int, GLC_Vector2df> Vector2dHash;

typedef QHash<int, GLC_Material*> MaterialHash;

typedef QList<int> IndexedList;

enum FaceType
{
	notSet,
	coordinate,
	coordinateAndTexture,
	coordinateAndNormal,
	coordinateAndTextureAndNormal
};

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
	explicit GLC_Mesh2();
	
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
	//! Get number of faces
	inline int getNumberOfFaces() const {return m_NumberOfFaces;}
	//! Get number of vertex
	inline int getNumberOfVertex() const {return m_CoordinateHash.size();}
	//! Get number of submaterial
	inline int getNumberOfSubMaterial() const {return m_MaterialHash.size();}
	//! Get specified mesh sub material
	inline GLC_Material* getSubMaterial(const int key) {return m_MaterialHash[key];}
	//! return true if Material key is in the mesh
	inline const bool containsMaterial(const int key) const {return m_MaterialHash.contains(key);}	
	//! return the mesh bounding box
	virtual GLC_BoundingBox* getBoundingBox(void) const;
	//! Return a copy of the geometry
	virtual GLC_Geometry* clone() const;
	//! return a vertex with key
	inline const GLC_Vector3df getVertex(const int key) const {return m_CoordinateHash.value(key);}
	//! return true if vertex key is in the mesh
	inline const bool containsVertex(const int key) const {return m_CoordinateHash.contains(key);}
	//! return a normal with key
	inline const GLC_Vector3df getNormal(const int key) const {return m_NormalHash.value(key);}
	//! return true if normal key is in the mesh
	inline const bool containsNormal(const int key) const {return m_NormalHash.contains(key);}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Add material to mesh
	void addMaterial(int Index, GLC_Material *);
	
	//! Remove material from the mesh
	int removeMaterial(int);
		
	//! Add a vertex to mesh
	inline void addVertex(const int Index, const GLC_Vector3df& Coordinate)
	{
		// Check if the key is already use		
		if (m_CoordinateHash.find(Index) == m_CoordinateHash.end())
		{
			// Add the coordinate to coordinate hash table
			m_CoordinateHash.insert(Index, Coordinate);
		}
	}
	
	//! Add Normal
	inline void addNormal(const int index, const GLC_Vector3df& Normal)
	{
		// Check if the key is already use		
		if (m_NormalHash.find(index) == m_NormalHash.end())
		{
			// Add the coordinate to coordinate hash table
			m_NormalHash.insert(index, Normal);
		}
	}
	
	//! Add texture coordinate
	inline void addTextureCoordinate(const int index, const GLC_Vector2df& textureCoordinate)
	{
		// Check if the key is already in use	
		if (m_TextCoordinateHash.find(index) == m_TextCoordinateHash.end())
		{	
			// Add the coordinate to coordinate hash table
			m_TextCoordinateHash.insert(index, textureCoordinate);
		}		
	}

	//! Add a face without texture coordinate and Normal
	/*! Mesh list of texture index must be empty.
	 */  
	inline void addFace(const QVector<int> &Material, const QVector<int> &Coordinate)
	{		
		addMaterialIndex(Material);
		addCoordIndex(Coordinate);
			
		// Increment number of faces
		m_NumberOfFaces++;
		m_ListIsValid= false;
	}

		 
	//! Add a face without texture coordinate
	/*! Mesh list of texture index must be empty.
	 */  
	inline void addFace(const QVector<int> &Material, const QVector<int> &Coordinate, const QVector<int> &Normal)
	{
		addMaterialIndex(Material);
		addCoordAndNormIndex(Coordinate, Normal);
			
		// Increment number of faces
		m_NumberOfFaces++;
		m_ListIsValid= false;		
	}

	
	//! Add a face with texture coordinate
	/*! Number of coordinate, Normal and texture must be equal
	 */
	inline void addFace(const QVector<int> &Material, const QVector<int> &Coordinate, const QVector<int> &Normal,
				const QVector<int> &TextureCoordinate)
	{
		addMaterialIndex(Material);
		addCoordAndNormIndex(Coordinate, Normal);
		addTextureIndex(TextureCoordinate);
			
		// Increment number of faces
		m_NumberOfFaces++;
		m_ListIsValid= false;
	}

	//! Reverse mesh normal
	void reverseNormal();
	 
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:

	//! if the geometry have a texture, load it
	virtual void glLoadTexture(void);
	
	//! Specific glExecute method
	virtual void glExecute(GLenum Mode, bool, bool forceWire= false);

	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n
	 *  Throw GLC_OpenGlException*/
	virtual void glDraw(void);
	
	//! Specific createList method
	virtual void createList(GLenum Mode);

//@}

//////////////////////////////////////////////////////////////////////
//! Private services Functions
//////////////////////////////////////////////////////////////////////
private:

	//! Create selection lis
	void createSelectionList(GLenum Mode);
	
	//! Add coordinate index of a face
	inline void addCoordIndex(const QVector<int> &Coordinate)
	{	
		m_CoordinateIndex+= Coordinate.toList();
		m_CoordinateIndex.append(-1); // End of the face's coordinate index
	}
	
	//! Add coordinate index of a face
	inline void addNormalIndex(const QVector<int> &Normal)
	{	
		m_NormalIndex+= Normal.toList();
		m_NormalIndex.append(-1); // End of the face's normal index
	}

	//! Add coordinate and normal index of a face
	inline void addCoordAndNormIndex(const QVector<int> &Coordinate, const QVector<int> &Normal)
	{	
		m_CoordinateIndex+= Coordinate.toList();
		m_CoordinateIndex.append(-1); // End of the face's coordinate index

		m_NormalIndex+= Normal.toList();
		m_NormalIndex.append(-1); // End of the face's normal index
		
		// Check if indexed list have the same size
		Q_ASSERT(m_NormalIndex.size() == m_CoordinateIndex.size());
	}
	
	//! Add Texture coordinate index of a face
	inline void addTextureIndex(const QVector<int> &TextureCoordinate)
	{
		m_TextureIndex+= TextureCoordinate.toList();
		m_TextureIndex.append(-1); // End of the face's texture coordinate index
	}
	
	//! Add Material index of a face
	inline void addMaterialIndex(const QVector<int> &Material)
	{
		m_MaterialIndex+= Material.toList();
		m_MaterialIndex.append(-1); // End of the face's material index
	}
	
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
	
	//! Mesh number of faces
	int m_NumberOfFaces;
	
	//! Selection Display list ID
	GLuint m_SelectionListID;
	
	//! Selection state
	bool m_IsSelected;
	
};
#endif //GLC_MESH2_H_
