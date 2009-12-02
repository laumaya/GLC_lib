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
#include "../glc_state.h"
#include "../shading/glc_selectionmaterial.h"

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

	//! Overload "=" operator
	GLC_Mesh& operator=(const GLC_Mesh&);

	//! Destructor
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

	//! Return a copy of the Mesh as GLC_Geometry pointer
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

	//! Return true if the mesh contains triangles in the specified LOD
	bool containsTriangles(int lod, GLC_uint materialId) const;

	//! Return the triangle index
	/*! The specified LOD must exists and uses the specified material id*/
	QVector<GLuint> getTrianglesIndex(int lod, GLC_uint materialId) const;

	//! Return the number of triangles in the specified LOD
	int numberOfTriangles(int lod, GLC_uint materialId) const;

	//! Return true if the mesh contains trips in the specified LOD with the specified material id
	bool containsStrips(int lod, GLC_uint materialId) const;

	//! Return the strips index
	/*! The specified LOD must exists and uses the specified material id*/
	QList<QVector<GLuint> > getStripsIndex(int lod, GLC_uint materialId) const;

	//! Return the number of strips in the specified LOD with the specified material id
	int numberOfStrips(int lod, GLC_uint materialId) const;

	//! Return true if the mesh contains fans in the specified LOD with the specified material id
	bool containsFans(int lod, GLC_uint materialId) const;

	//! Return the fans index
	/*! The specified LOD must exists and uses the specified material id*/
	QList<QVector<GLuint> > getFansIndex(int lod, GLC_uint materialId) const;

	//! Return the number of fans in the specified LOD with the specified material id
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
	/*! The specified LOD must exists*/
	inline double getLodAccuracy(int lod) const
	{
		Q_ASSERT(containsLod(lod));
		return m_MeshData.getLod(lod)->accuracy();
	}

	//! Return the next primitive local id
	inline GLC_uint nextPrimitiveLocalId() const
	{return m_NextPrimitiveLocalId;}

	//! Return true if the mesh position data is empty
	inline bool isEmpty() const
	{return m_MeshData.isEmpty();}


//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Clear the content of the mesh and super class and makes them empty
	virtual void clear();

	//! Clear only the content off the mesh and makes it empty
	void clearOnlyMesh();

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
	GLC_uint addTriangles(GLC_Material*, const IndexList&, const int lod= 0, double accuracy= 0.0);

	//! Add triangles Strip and return his id
	GLC_uint addTrianglesStrip(GLC_Material*, const IndexList&, const int lod= 0, double accuracy= 0.0);

	//! Add triangles Fan and return his id
	GLC_uint addTrianglesFan(GLC_Material*, const IndexList&, const int lod= 0, double accuracy= 0.0);

	//! Reverse mesh normal
	void reverseNormals();

	//! Set color per vertex flag to use indexed color
	inline void setColorPearVertex(bool flag)
	{m_ColorPearVertex= flag;}

	//! Copy vertex list in a vector list for Vertex Array Use
	void finish();

	//! Set the lod Index
	virtual void setCurrentLod(const int);

	//! Replace the Master material
	virtual void replaceMasterMaterial(GLC_Material*);

	//! Replace the material specified by id with another one
	void replaceMaterial(const GLC_uint, GLC_Material*);

	//! Set the mesh next primitive local id
	inline void setNextPrimitiveLocalId(GLC_uint id)
	{m_NextPrimitiveLocalId= id;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Binary serialisation Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Load the mesh from binary data stream
	/*! The MaterialHash contains a hash table of GLC_Material that the mesh can use
	 *  The QHash<GLC_uint, GLC_uint> is used to map serialised material ID to the new
	 *  constructed materials
	 */
	void loadFromDataStream(QDataStream&, const MaterialHash&, const QHash<GLC_uint, GLC_uint>&);

	//! Save the mesh to binary data stream
	void saveToDataStream(QDataStream&) const;

//@}
//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Specific glExecute method
	virtual void glExecute(const GLC_RenderProperties&);

protected:

	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.
	 *  Throw GLC_OpenGlException*/
	virtual void glDraw(const GLC_RenderProperties&);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! Set the current material
	GLC_uint setCurrentMaterial(GLC_Material*, const int, double);

	//! Fill VBOs and IBOs
	void fillVbosAndIbos();

	//! Set primitive group offset after loading mesh from binary
	void finishSerialized();

	//! Move Indexs from the primitive groups to the mesh Data LOD and Set IBOs offsets
	void finishVbo();

	//! Move Indexs from the primitive groups to the mesh Data LOD and Set Index offsets
	void finishNonVbo();

	//! Use VBO to Draw primitives from the specified GLC_PrimitiveGroup
	inline void vboDrawPrimitivesOf(GLC_PrimitiveGroup*);

	//! Use Vertex Array to Draw primitives from the specified GLC_PrimitiveGroup
	inline void vertexArrayDrawPrimitivesOf(GLC_PrimitiveGroup*);

	//! Use VBO to Draw primitives in selection mode from the specified GLC_PrimitiveGroup
	inline void vboDrawInSelectionModePrimitivesOf(GLC_PrimitiveGroup*);

	//! Use Vertex Array to Draw primitives in selection mode from the specified GLC_PrimitiveGroup
	inline void vertexArrayDrawInSelectionModePrimitivesOf(GLC_PrimitiveGroup*);

	//! Use VBO to Draw primitives with specific materials from the specified GLC_PrimitiveGroup
	inline void vboDrawPrimitivesGroupOf(GLC_PrimitiveGroup*, GLC_Material*, bool, QHash<GLC_uint, GLC_Material*>*);

	//! Use Vertex Array to Draw primitives with specific materials from the specified GLC_PrimitiveGroup
	inline void vertexArrayDrawPrimitivesGroupOf(GLC_PrimitiveGroup*, GLC_Material*, bool, QHash<GLC_uint, GLC_Material*>*);

	//! Activate mesh VBOs and IBO of the current LOD
	inline void activateVboAndIbo();

	//! Activate vertex Array
	inline void activateVertexArray();

	//! The normal display loop
	inline void normalRenderLoop(const GLC_RenderProperties&, bool);

	//! The overwrite material render loop
	inline void OverwriteMaterialRenderLoop(const GLC_RenderProperties&, bool);

	//! The overwrite transparency render loop
	inline void OverwriteTransparencyRenderLoop(const GLC_RenderProperties&, bool);

	//! The body selection render loop
	inline void bodySelectionRenderLoop(bool);

	//! The primitive selection render loop
	inline void primitiveSelectionRenderLoop(bool);

	//! The primitive rendeder loop
	inline void primitiveRenderLoop(const GLC_RenderProperties&, bool);

//@}


//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The next primitive local id
	GLC_uint m_NextPrimitiveLocalId;

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

	//! Data of the mesh (Bulk Data + LOD with indexs)
	GLC_MeshData m_MeshData;

	//! The current LOD index
	int m_CurrentLod;
};

// Inline functions

// Use VBO to Draw triangles from the specified GLC_PrimitiveGroup
void GLC_Mesh::vboDrawPrimitivesOf(GLC_PrimitiveGroup* pCurrentGroup)
{
	// Draw triangles
	if (pCurrentGroup->containsTriangles())
	{
		glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSize(), GL_UNSIGNED_INT, pCurrentGroup->trianglesIndexOffset());
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStrip())
	{
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffset().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->stripsOffset().at(i));
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFan())
	{
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffset().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->fansOffset().at(i));
		}
	}
}
// Use Vertex Array to Draw triangles from the specified GLC_PrimitiveGroup
void GLC_Mesh::vertexArrayDrawPrimitivesOf(GLC_PrimitiveGroup* pCurrentGroup)
{
	// Draw triangles
	if (pCurrentGroup->containsTriangles())
	{
		GLvoid* pOffset= &(m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesIndexOffseti()]);
		glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSize(), GL_UNSIGNED_INT, pOffset);
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStrip())
	{
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffseti().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->stripsOffseti().at(i)];
			glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFan())
	{
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffseti().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->fansOffseti().at(i)];
			glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}
	}
}

// Use VBO to Draw primitives in selection mode from the specified GLC_PrimitiveGroup
void GLC_Mesh::vboDrawInSelectionModePrimitivesOf(GLC_PrimitiveGroup* pCurrentGroup)
{
	GLubyte colorId[4];
	// Draw triangles
	if (pCurrentGroup->containsTrianglesGroupId())
	{
		const GLsizei trianglesGroupCount= static_cast<GLsizei>(pCurrentGroup->trianglesGroupOffset().size());
		for (GLint i= 0; i < trianglesGroupCount; ++i)
		{
			glc::encodeRgbId(pCurrentGroup->triangleGroupId(i), colorId);
			glColor3ubv(colorId);
			glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->trianglesGroupOffset().at(i));
		}
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStripGroupId())
	{
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffset().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			glc::encodeRgbId(pCurrentGroup->stripGroupId(i), colorId);
			glColor3ubv(colorId);
			glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->stripsOffset().at(i));
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFanGroupId())
	{
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffset().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			glc::encodeRgbId(pCurrentGroup->fanGroupId(i), colorId);
			glColor3ubv(colorId);

			glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->fansOffset().at(i));
		}
	}

}

// Use Vertex Array to Draw primitives in selection mode from the specified GLC_PrimitiveGroup
void GLC_Mesh::vertexArrayDrawInSelectionModePrimitivesOf(GLC_PrimitiveGroup* pCurrentGroup)
{
	GLubyte colorId[4];
	// Draw triangles
	if (pCurrentGroup->containsTrianglesGroupId())
	{
		const GLsizei trianglesGroupCount= static_cast<GLsizei>(pCurrentGroup->trianglesGroupOffseti().size());
		for (GLint i= 0; i < trianglesGroupCount; ++i)
		{
			glc::encodeRgbId(pCurrentGroup->triangleGroupId(i), colorId);
			glColor3ubv(colorId);

			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesGroupOffseti().at(i)];
			glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}

		GLvoid* pOffset= &(m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesIndexOffseti()]);
		glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSize(), GL_UNSIGNED_INT, pOffset);
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStripGroupId())
	{
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffseti().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			glc::encodeRgbId(pCurrentGroup->stripGroupId(i), colorId);
			glColor3ubv(colorId);

			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->stripsOffseti().at(i)];
			glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFanGroupId())
	{
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffseti().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			glc::encodeRgbId(pCurrentGroup->fanGroupId(i), colorId);
			glColor3ubv(colorId);

			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->fansOffseti().at(i)];
			glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}
	}
}

// Use VBO to Draw primitives with specific materials from the specified GLC_PrimitiveGroup
void GLC_Mesh::vboDrawPrimitivesGroupOf(GLC_PrimitiveGroup* pCurrentGroup, GLC_Material* pCurrentMaterial, bool materialIsRenderable, QHash<GLC_uint, GLC_Material*>* pMaterialHash)
{
	// Draw triangles
	if (pCurrentGroup->containsTrianglesGroupId())
	{
		const GLsizei trianglesGroupCount= static_cast<GLsizei>(pCurrentGroup->trianglesGroupOffset().size());
		for (GLint i= 0; i < trianglesGroupCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->triangleGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				pMaterialHash->value(currentPrimitiveId)->glExecute();
			}
			else
			{
				pCurrentMaterial->glExecute();
			}
			glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->trianglesGroupOffset().at(i));
		}
	}
	else if (pCurrentGroup->containsTriangles() && materialIsRenderable)
	{
		pCurrentMaterial->glExecute();
		glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSize(), GL_UNSIGNED_INT, pCurrentGroup->trianglesIndexOffset());
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStripGroupId())
	{
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffset().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->stripGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				pMaterialHash->value(currentPrimitiveId)->glExecute();
			}
			else
			{
				pCurrentMaterial->glExecute();
			}
			glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->stripsOffset().at(i));
		}
	}
	else if (pCurrentGroup->containsStrip() && materialIsRenderable)
	{
		pCurrentMaterial->glExecute();
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffset().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->stripsOffset().at(i));
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFanGroupId())
	{
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffset().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->fanGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				pMaterialHash->value(currentPrimitiveId)->glExecute();
			}
			else
			{
				pCurrentMaterial->glExecute();
			}
			glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->fansOffset().at(i));
		}
	}
	else if (pCurrentGroup->containsFan() && materialIsRenderable)
	{
		pCurrentMaterial->glExecute();
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffset().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->fansOffset().at(i));
		}
	}

}

// Use Vertex Array to Draw primitives with specific materials from the specified GLC_PrimitiveGroup
void GLC_Mesh::vertexArrayDrawPrimitivesGroupOf(GLC_PrimitiveGroup* pCurrentGroup, GLC_Material* pCurrentMaterial, bool materialIsRenderable, QHash<GLC_uint, GLC_Material*>* pMaterialHash)
{
	// Draw triangles
	if (pCurrentGroup->containsTrianglesGroupId())
	{
		const GLsizei trianglesGroupCount= static_cast<GLsizei>(pCurrentGroup->trianglesGroupOffseti().size());
		for (GLint i= 0; i < trianglesGroupCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->triangleGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				pMaterialHash->value(currentPrimitiveId)->glExecute();
			}
			else
			{
				pCurrentMaterial->glExecute();
			}
			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesGroupOffseti().at(i)];
			glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}

		GLvoid* pOffset= &(m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesIndexOffseti()]);
		glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSize(), GL_UNSIGNED_INT, pOffset);
	}
	else if (pCurrentGroup->containsTriangles() && materialIsRenderable)
	{
		pCurrentMaterial->glExecute();
		GLvoid* pOffset= &(m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesIndexOffseti()]);
		glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSize(), GL_UNSIGNED_INT, pOffset);
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStripGroupId())
	{
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffseti().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->stripGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				pMaterialHash->value(currentPrimitiveId)->glExecute();
			}
			else
			{
				pCurrentMaterial->glExecute();
			}
			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->stripsOffseti().at(i)];
			glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}
	}
	else if (pCurrentGroup->containsStrip() && materialIsRenderable)
	{
		pCurrentMaterial->glExecute();
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffseti().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->stripsOffseti().at(i)];
			glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFanGroupId())
	{
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffseti().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->fanGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				pMaterialHash->value(currentPrimitiveId)->glExecute();
			}
			else
			{
				pCurrentMaterial->glExecute();
			}
			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->fansOffseti().at(i)];
			glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}
	}
	else if (pCurrentGroup->containsFan() && materialIsRenderable)
	{
		pCurrentMaterial->glExecute();
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffseti().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->fansOffseti().at(i)];
			glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pOffset);
		}
	}

}

// Activate mesh VBOs and IBO of the current LOD
void GLC_Mesh::activateVboAndIbo()
{
	// Activate Vertices VBO
	m_MeshData.useVBO(true, GLC_MeshData::GLC_Vertex);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	// Activate Normals VBO
	m_MeshData.useVBO(true, GLC_MeshData::GLC_Normal);
	glNormalPointer(GL_FLOAT, 0, 0);
	glEnableClientState(GL_NORMAL_ARRAY);

	// Activate texel VBO if needed
	if (m_MeshData.useVBO(true, GLC_MeshData::GLC_Texel))
	{
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	// Activate Color VBO if needed
	if ((m_ColorPearVertex && !m_IsSelected && !GLC_State::isInSelectionMode()) && m_MeshData.useVBO(true, GLC_MeshData::GLC_Color))
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		glColorPointer(4, GL_FLOAT, 0, 0);
		glEnableClientState(GL_COLOR_ARRAY);
	}

	m_MeshData.useIBO(true, m_CurrentLod);
}

// Activate vertex Array
void GLC_Mesh::activateVertexArray()
{
	// Use Vertex Array
	glVertexPointer(3, GL_FLOAT, 0, m_MeshData.positionVectorHandle()->data());
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_FLOAT, 0, m_MeshData.normalVectorHandle()->data());
	glEnableClientState(GL_NORMAL_ARRAY);

	// Activate texel if needed
	if (!m_MeshData.texelVectorHandle()->isEmpty())
	{
		glTexCoordPointer(2, GL_FLOAT, 0, m_MeshData.texelVectorHandle()->data());
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	// Activate Color VBO if needed
	if ((m_ColorPearVertex && !m_IsSelected && !GLC_State::isInSelectionMode()) && !m_MeshData.colorVectorHandle()->isEmpty())
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		glColorPointer(4, GL_FLOAT, 0, m_MeshData.colorVectorHandle()->data());
		glEnableClientState(GL_COLOR_ARRAY);
	}
}

// The normal display loop
void GLC_Mesh::normalRenderLoop(const GLC_RenderProperties& renderProperties, bool vboIsUsed)
{
	PrimitiveGroups::iterator iGroup= m_PrimitiveGroups.value(m_CurrentLod)->begin();
	while (iGroup != m_PrimitiveGroups.value(m_CurrentLod)->constEnd())
	{
		GLC_PrimitiveGroup* pCurrentGroup= iGroup.value();
		GLC_Material* pCurrentMaterial= m_MaterialHash.value(pCurrentGroup->id());

		// Test if the current material is renderable
		bool materialIsrenderable = (pCurrentMaterial->isTransparent() == renderProperties.transparentMaterialRenderFlag());

		// Choose the material to render
   		if ((materialIsrenderable || m_IsSelected) && !GLC_State::isInSelectionMode())
    	{
			// Execute current material
			pCurrentMaterial->glExecute();

			if (m_IsSelected) GLC_SelectionMaterial::glExecute();
		}

   		// Choose the primitives to render
		if (m_IsSelected || GLC_State::isInSelectionMode() || materialIsrenderable)
		{

			if (vboIsUsed)
				vboDrawPrimitivesOf(pCurrentGroup);
			else
				vertexArrayDrawPrimitivesOf(pCurrentGroup);
		}

		++iGroup;
	}
}

//  The overwrite material render loop
void GLC_Mesh::OverwriteMaterialRenderLoop(const GLC_RenderProperties& renderProperties, bool vboIsUsed)
{
	// Get the overwrite material
	GLC_Material* pOverwriteMaterial= renderProperties.overwriteMaterial();
	Q_ASSERT(NULL != pOverwriteMaterial);
	pOverwriteMaterial->glExecute();
	if (m_IsSelected) GLC_SelectionMaterial::glExecute();

	PrimitiveGroups::iterator iGroup= m_PrimitiveGroups.value(m_CurrentLod)->begin();
	while (iGroup != m_PrimitiveGroups.value(m_CurrentLod)->constEnd())
	{
		GLC_PrimitiveGroup* pCurrentGroup= iGroup.value();

		// Test if the current material is renderable
		bool materialIsrenderable = (pOverwriteMaterial->isTransparent() == renderProperties.transparentMaterialRenderFlag());

   		// Choose the primitives to render
		if (m_IsSelected || materialIsrenderable)
		{

			if (vboIsUsed)
				vboDrawPrimitivesOf(pCurrentGroup);
			else
				vertexArrayDrawPrimitivesOf(pCurrentGroup);
		}

		++iGroup;
	}
}
// The overwrite transparency render loop
void GLC_Mesh::OverwriteTransparencyRenderLoop(const GLC_RenderProperties& renderProperties, bool vboIsUsed)
{
	// Get transparency value
	const float alpha= renderProperties.overwriteTransparency();
	Q_ASSERT(-1.0f != alpha);

	// Test if the current material is renderable
	bool materialIsrenderable = (true == renderProperties.transparentMaterialRenderFlag());

	if (materialIsrenderable || m_IsSelected)
	{
		PrimitiveGroups::iterator iGroup= m_PrimitiveGroups.value(m_CurrentLod)->begin();
		while (iGroup != m_PrimitiveGroups.value(m_CurrentLod)->constEnd())
		{
			GLC_PrimitiveGroup* pCurrentGroup= iGroup.value();
			GLC_Material* pCurrentMaterial= m_MaterialHash.value(pCurrentGroup->id());

			// Execute current material
			pCurrentMaterial->glExecute(alpha);

			if (m_IsSelected) GLC_SelectionMaterial::glExecute();

	   		// Choose the primitives to render
			if (m_IsSelected || materialIsrenderable)
			{

				if (vboIsUsed)
					vboDrawPrimitivesOf(pCurrentGroup);
				else
					vertexArrayDrawPrimitivesOf(pCurrentGroup);
			}
			++iGroup;
		}
	}
}

// The body selection render loop
void GLC_Mesh::bodySelectionRenderLoop(bool vboIsUsed)
{
	Q_ASSERT(GLC_State::isInSelectionMode());

	PrimitiveGroups::iterator iGroup= m_PrimitiveGroups.value(m_CurrentLod)->begin();
	while (iGroup != m_PrimitiveGroups.value(m_CurrentLod)->constEnd())
	{
		GLC_PrimitiveGroup* pCurrentGroup= iGroup.value();

		if (vboIsUsed)
			vboDrawPrimitivesOf(pCurrentGroup);
		else
			vertexArrayDrawPrimitivesOf(pCurrentGroup);

		++iGroup;
	}
}

// The primitive selection render loop
void GLC_Mesh::primitiveSelectionRenderLoop(bool vboIsUsed)
{
	Q_ASSERT(GLC_State::isInSelectionMode());

	PrimitiveGroups::iterator iGroup= m_PrimitiveGroups.value(m_CurrentLod)->begin();

	while (iGroup != m_PrimitiveGroups.value(m_CurrentLod)->constEnd())
	{
		GLC_PrimitiveGroup* pCurrentGroup= iGroup.value();

		if (vboIsUsed)
			vboDrawInSelectionModePrimitivesOf(pCurrentGroup);
		else
			vertexArrayDrawInSelectionModePrimitivesOf(pCurrentGroup);

		++iGroup;
	}
}

// The primitive rendeder loop
void GLC_Mesh::primitiveRenderLoop(const GLC_RenderProperties& renderProperties, bool vboIsUsed)
{
	PrimitiveGroups::iterator iGroup= m_PrimitiveGroups.value(m_CurrentLod)->begin();
	while (iGroup != m_PrimitiveGroups.value(m_CurrentLod)->constEnd())
	{
		GLC_PrimitiveGroup* pCurrentGroup= iGroup.value();
		GLC_Material* pCurrentMaterial= m_MaterialHash.value(pCurrentGroup->id());

		// Test if the current material is renderable
		bool materialIsrenderable = (pCurrentMaterial->isTransparent() == renderProperties.transparentMaterialRenderFlag());

		// Choose the material to render
   		if (materialIsrenderable)
    	{
			// Execute current material
			pCurrentMaterial->glExecute();

		}

		if (vboIsUsed)
			vboDrawPrimitivesGroupOf(pCurrentGroup, pCurrentMaterial, materialIsrenderable, renderProperties.hashOfOverwritePrimitiveMaterials());
		else
			vertexArrayDrawPrimitivesGroupOf(pCurrentGroup, pCurrentMaterial, materialIsrenderable, renderProperties.hashOfOverwritePrimitiveMaterials());

		++iGroup;
	}

}


#endif /* GLC_MESH_H_ */
