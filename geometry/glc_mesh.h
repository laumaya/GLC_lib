/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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

//! \file glc_mesh.h interface for the GLC_Mesh class.

#ifndef GLC_MESH_H_
#define GLC_MESH_H_

#include <QHash>
#include <QList>
#include "../maths/glc_vector2df.h"
#include "../maths/glc_vector3df.h"
#include "../glc_enum.h"
#include "../shading/glc_material.h"
#include "glc_meshdata.h"
#include "glc_geometry.h"
#include "glc_primitivegroup.h"

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Mesh
/*! \brief GLC_Mesh : OpenGL 3D Mesh*/

/*! An GLC_Mesh is Mesh composed of triangles, strips and fan
*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_Mesh : public GLC_Geometry
{
	friend QDataStream &operator<<(QDataStream &, const GLC_Mesh &);
	friend QDataStream &operator>>(QDataStream &, GLC_Mesh &);

public:
	typedef QHash<GLC_uint, GLC_PrimitiveGroup*> PrimitiveGroups;
	typedef QHash<const int, PrimitiveGroups*> PrimitiveGroupsHash;

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_Mesh();

	//! Copy constructor
	GLC_Mesh(const GLC_Mesh&);

	virtual ~GLC_Mesh();
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Get number of faces
	virtual unsigned int numberOfFaces() const;

	//! Get number of vertex
	virtual unsigned int numberOfVertex() const;

	//! Get number of normals
	inline unsigned int numberOfNormals() const
	{ return m_NumberOfNormals;}

	//! return the mesh bounding box
	virtual GLC_BoundingBox& boundingBox(void);

	//! Return a copy of the geometry
	virtual GLC_Geometry* clone() const;

	//! Return true if color pear vertex is activated
	inline bool ColorPearVertexIsAcivated() const
	{return m_ColorPearVertex;}

	//! Return the number of lod
	inline int numberOfLod()
	{return m_MeshData.lodCount();}

	//! Return the Position Vector
	inline GLfloatVector positionVector() const
	{return m_MeshData.positionVector();}

	//! Return the normal Vector
	inline GLfloatVector normalVector() const
	{return m_MeshData.normalVector();}

	//! Return the texel Vector
	inline GLfloatVector texelVector() const
	{return m_MeshData.texelVector();}

	//! Return true if the mesh contains triangles
	bool containsTriangles(int lod, GLC_uint materialId) const;

	//! Return the triangle index
	QVector<GLuint> getTrianglesIndex(int lod, GLC_uint materialId) const;

	//! Return the number of triangles
	int numberOfTriangles(int lod, GLC_uint materialId) const;

	//! Return true if the mesh contains trips
	bool containsStrips(int lod, GLC_uint materialId) const;

	//! Return the strips index
	QList<QVector<GLuint> > getStripsIndex(int lod, GLC_uint materialId) const;

	//! Return the number of strips
	int numberOfStrips(int lod, GLC_uint materialId) const;

	//! Return true if the mesh contains fans
	bool containsFans(int lod, GLC_uint materialId) const;

	//! Return the fans index
	QList<QVector<GLuint> > getFansIndex(int lod, GLC_uint materialId) const;

	//! Return the number of fans
	int numberOfFans(int lod, GLC_uint materialId) const;

	//! Return true if the mesh contains the specified LOD
	inline bool containsLod(int lod) const
	{return (NULL != m_MeshData.getLod(lod));}

	//! Return true if the specified LOD conatins the specified material
	inline bool lodContainsMaterial(int lod, GLC_uint materialId) const
	{
		if (!m_PrimitiveGroups.contains(lod))return false;
		else return m_PrimitiveGroups.value(lod)->contains(materialId);
	}

	//! Return the specified LOD accuracy
	inline double getLodAccuracy(int lod) const
	{
		Q_ASSERT(containsLod(lod));
		return m_MeshData.getLod(lod)->accuracy();
	}

	//! Return the next primitive local id
	inline GLC_uint nextPrimitiveLocalId() const
	{return m_LocalId;}


//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Add vertices coordinate
	inline void addVertices(const GLfloatVector& vertices)
	{
		*(m_MeshData.positionVectorHandle())+= vertices;
		m_NumberOfVertice+= vertices.size() / 3;
	}

	//! Add Normals
	inline void addNormals(const GLfloatVector& normals)
	{
		*(m_MeshData.normalVectorHandle())+= normals;
		m_NumberOfNormals+= normals.size() / 3;
	}

	//! Add texel
	inline void addTexels(const GLfloatVector& texels)
	{*(m_MeshData.texelVectorHandle())+= texels;}

	//! Add Colors
	inline void addColors(const GLfloatVector& colors)
	{*(m_MeshData.colorVectorHandle())+= colors;}

	//! Add triangles
	void addTriangles(GLC_Material*, const IndexList&, const int lod= 0, double accuracy= 0.0);

	//! Add triangles Strip and return his id
	GLC_uint addTrianglesStrip(GLC_Material*, const IndexList&, const int lod= 0, double accuracy= 0.0);

	//! Add triangles Fan and return his id
	GLC_uint addTrianglesFan(GLC_Material*, const IndexList&, const int lod= 0, double accuracy= 0.0);

	//! Reverse mesh normal
	void reverseNormals();

	//! Set color per vertex flag
	inline void setColorPearVertex(bool flag){m_ColorPearVertex= flag;}

	//! Copy vertex list in a vector list for Vertex Array Use
	void finish();

	//! Set the lod Index
	virtual void setCurrentLod(const int);

	//! Replace the material specified by id with another one
	void replaceMaterial(const GLC_uint, GLC_Material*);

	//! Load the mesh from binary data stream
	void loadFromDataStream(QDataStream&, MaterialHash&, QHash<GLC_uint, GLC_uint>&);

	//! Save the mesh to binary data stream
	void saveToDataStream(QDataStream&);

	//! Set the mesh next primitive local id
	inline void setNextPrimitiveLocalId(GLC_uint id)
	{m_LocalId= id;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Specific glExecute method
	virtual void glExecute(bool, bool transparent= false);

private:

	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n
	 *  Throw GLC_OpenGlException*/
	virtual void glDraw(bool transparent= false);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! Set the current material
	GLC_uint setCurrentMaterial(GLC_Material*, const int, double);

	//! Create VBO and IBO
	void createVbos();

	//! set primitive group offset
	void finishSerialized();

	//! Finish VBO mesh
	void finishVbo();

	//! Finish non Vbo mesh
	void finishNonVbo();
//@}


//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The next primitive local id
	GLC_uint m_LocalId;

	//! The hash table of Hash table of primitive group
	PrimitiveGroupsHash m_PrimitiveGroups;

	//! The default material Id
	GLC_uint m_DefaultMaterialId;

	//! Mesh number of faces
	unsigned int m_NumberOfFaces;

	//! Mesh number of vertice
	unsigned int m_NumberOfVertice;

	//! Mesh number of normals
	unsigned int m_NumberOfNormals;

	//! Selection state
	bool m_IsSelected;

	//! Color pear vertex
	bool m_ColorPearVertex;

	//! Geom engine
	GLC_MeshData m_MeshData;

	//! The current LOD index
	int m_CurrentLod;

};

#endif /* GLC_MESH_H_ */
