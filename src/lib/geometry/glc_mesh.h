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

//! \file glc_mesh.h interface for the GLC_Mesh class.

#ifndef GLC_MESH_H_
#define GLC_MESH_H_

#include <QHash>
#include <QList>
#include "../glc_global.h"
#include "../shading/glc_material.h"
#include "glc_meshdata.h"
#include "glc_geometry.h"
#include "glc_primitivegroup.h"
#include "../glc_state.h"
#include "../shading/glc_selectionmaterial.h"
#include "../glc_context.h"
#include "../glc_contextmanager.h"

#include "../glc_config.h"

class GLC_Triangle;
class SharpEdgeContainer;

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
	typedef QHash<GLC_uint, GLC_PrimitiveGroup*> LodPrimitiveGroups;
	typedef QHash<const int, LodPrimitiveGroups*> PrimitiveGroupsHash;

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_Mesh();

	//! Copy constructor
    GLC_Mesh(const GLC_Mesh&other);

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
    //! Return the number of primitive of this mesh
    int primitiveCount() const override;

	//! Return the class Chunk ID
	static quint32 chunckID();

	//! Get number of faces
    unsigned int faceCount(int lod) const override;

	//! Get number of vertex
    unsigned int vertexCount() const override;

	//! Get number of normals
	inline unsigned int numberOfNormals() const
	{ return m_NumberOfNormals;}

	//! return the mesh bounding box
    const GLC_BoundingBox& boundingBox(void) override;

	//! Return a copy of the Mesh as GLC_Geometry pointer
    GLC_Geometry* clone() const override;

	//! Return true if color pear vertex is activated
	inline bool ColorPearVertexIsAcivated() const
	{return m_ColorPearVertex;}

	//! Return the number of lod
	inline int lodCount() const
	{return m_MeshData.lodCount();}

	//! Return the Position Vector
    const GLfloatVector& positionVector() const
	{return m_MeshData.positionVector();}

	//! Return the normal Vector
    const GLfloatVector& normalVector() const
	{return m_MeshData.normalVector();}

	//! Return the texel Vector
    const GLfloatVector& texelVector() const
	{return m_MeshData.texelVector();}

	//! Return true if the mesh contains triangles in the specified LOD
	bool containsTriangles(int lod, GLC_uint materialId) const;

	//! Return the triangle index
	/*! The specified LOD must exists and uses the specified material id*/
	QVector<GLuint> getTrianglesIndex(int lod, GLC_uint materialId) const;

	//! Return the equivalent triangle index of (triangle, strip and fan)
    IndexList getEquivalentTrianglesStripsFansIndex(int lod, GLC_uint materialId) const;

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

	//! Return true if the specified LOD contains the specified material
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

    //! Return the GLC_Material applyed on the given primitive id of the given lod
    GLC_Material* MaterialOfPrimitiveId(GLC_uint id, int lod= 0) const;

	//! Return the set of primitives id
	QSet<GLC_uint> setOfPrimitiveId() const;

	//! Return true if the mesh position data is empty
	inline bool isEmpty() const
	{return m_MeshData.isEmpty();}

	//! Return the mesh wire color
	inline QColor wireColor() const
	{return m_WireColor;}

	//! Create a mesh of the given LOD index
	GLC_Mesh* createMeshOfGivenLod(int lodIndex);

	//! Create a mesh from the given LOD index
	GLC_Mesh* createMeshFromGivenLod(int lodIndex);

    //! Transform vertice by the given matrix
    void transformVertice(const GLC_Matrix4x4& matrix) override;

	//! Return the volume of this mesh
    double volume() override;

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Clear the content of the mesh and super class and makes them empty
    void clear() override;

	//! Clear only the content off the mesh and makes it empty
	void clearMeshWireAndBoundingBox();

	//! Add vertices coordinate
	inline void addVertice(const GLfloatVector& vertices)
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
	
	//! Replace colors
	inline void setColors(const GLfloatVector& colors)
	{*(m_MeshData.colorVectorHandle()) = colors;}

	//! Add triangles
	GLC_uint addTriangles(GLC_Material*, const IndexList&, const int lod= 0, double accuracy= 0.0);

	//! Add triangles Strip and return his id
	GLC_uint addTrianglesStrip(GLC_Material*, const IndexList&, const int lod= 0, double accuracy= 0.0);

	//! Add triangles Fan and return his id
	GLC_uint addTrianglesFan(GLC_Material*, const IndexList&, const int lod= 0, double accuracy= 0.0);

	//! Reverse mesh normal
    void reverseNormals() override;

	//! Set color per vertex flag to use indexed color
	inline void setColorPearVertex(bool flag)
	{m_ColorPearVertex= flag;}

	//! Copy vertex list in a vector list for Vertex Array Use
	void finish();

	//! Set the lod Index
    void setCurrentLod(const int) override;

	//! Replace the Master material
    void replaceMasterMaterial(GLC_Material*) override;

	//! Replace the material specified by id with another one
    void replaceMaterial(const GLC_uint, GLC_Material*) override;

	//! Set the mesh next primitive local id
	inline void setNextPrimitiveLocalId(GLC_uint id)
	{m_NextPrimitiveLocalId= id;}

	//! Set the mesh wire color
	inline void setWireColor(const QColor& color)
	{m_WireColor= color;}

	//! Release client VBO
    void releaseVboClientSide(bool update) override;

	//! Set VBO usage
    void setVboUsage(bool usage) override;

    void createSharpEdges(double precision, double angleThreshold);

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
protected:

	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.*/
    void glDraw(const GLC_RenderProperties&) override;

    void setClientState();
    void restoreClientState(GLC_Context *pContext);
    void drawMeshWire(const GLC_RenderProperties &renderProperties, GLC_Context *pContext);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Protected services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
protected:
    GLC_uint newMaterialIdFromOldMaterialId(GLC_uint oldMaterialId) const
    {return m_OldToNewMaterialId.value(oldMaterialId);}

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
	//void finishVbo();

	//! Move Indexs from the primitive groups to the mesh Data LOD and Set Index offsets
	void moveIndexToMeshDataLod();

	//! Use VBO to Draw primitives from the specified GLC_PrimitiveGroup
	inline void vboDrawPrimitivesOf(GLC_PrimitiveGroup*);

	//! Use Vertex Array to Draw primitives from the specified GLC_PrimitiveGroup
	inline void vertexArrayDrawPrimitivesOf(GLC_PrimitiveGroup*);

	//! Use VBO to Draw primitives in selection mode from the specified GLC_PrimitiveGroup
	inline void vboDrawInSelectionModePrimitivesOf(GLC_PrimitiveGroup*);

	//! Use Vertex Array to Draw primitives in selection mode from the specified GLC_PrimitiveGroup
	inline void vertexArrayDrawInSelectionModePrimitivesOf(GLC_PrimitiveGroup*);

	//! Use VBO to Draw primitives with specific materials from the specified GLC_PrimitiveGroup
	inline void vboDrawPrimitivesGroupOf(GLC_PrimitiveGroup*, GLC_Material*, bool, bool, QHash<GLC_uint, GLC_Material*>*);

	//! Use Vertex Array to Draw primitives with specific materials from the specified GLC_PrimitiveGroup
	inline void vertexArrayDrawPrimitivesGroupOf(GLC_PrimitiveGroup*, GLC_Material*, bool, bool, QHash<GLC_uint, GLC_Material*>*);

	//! Use VBO to Draw primitives with selection materials from the specified GLC_PrimitiveGroup
	inline void vboDrawSelectedPrimitivesGroupOf(GLC_PrimitiveGroup*, GLC_Material*, bool, bool, const GLC_RenderProperties&);

	//! Use Vertex Array to Draw primitives with selection materials from the specified GLC_PrimitiveGroup
	inline void vertexArrayDrawSelectedPrimitivesGroupOf(GLC_PrimitiveGroup*, GLC_Material*, bool, bool, const GLC_RenderProperties&);

	//! Activate mesh VBOs and IBO of the current LOD
	inline void activateVboAndIbo();

	//! Activate vertex Array
	inline void activateVertexArray();

	//! The normal display loop
	void normalRenderLoop(const GLC_RenderProperties&, bool);

	//! The overwrite material render loop
	void OverwriteMaterialRenderLoop(const GLC_RenderProperties&, bool);

	//! The overwrite transparency render loop
	void OverwriteTransparencyRenderLoop(const GLC_RenderProperties&, bool);

	//! The overwrite transparency and material render loop
	void OverwriteTransparencyAndMaterialRenderLoop(const GLC_RenderProperties&, bool);

	//! The body selection render loop
	void bodySelectionRenderLoop(bool);

	//! The primitive selection render loop
	void primitiveSelectionRenderLoop(bool);

	//! The primitive render loop
	void primitiveRenderLoop(const GLC_RenderProperties&, bool);

	//! The primitive Selected render loop
	void primitiveSelectedRenderLoop(const GLC_RenderProperties&, bool);

	//! The outline silhouette render loop (draws in special colors for edge detection, passes extra data encoded in color)
	void outlineSilhouetteRenderLoop(const GLC_RenderProperties&, bool);

	//! Copy index of this mesh from the given LOD into the given mesh
	void copyIndex(int lod, GLC_Mesh* pLodMesh, QHash<GLuint, GLuint>& sourceToTargetIndexMap, QHash<GLuint, GLuint>& tagetToSourceIndexMap, int& maxIndex, int targetLod);

	//! Copy Bulk data
	void copyBulkData(GLC_Mesh* pLodMesh, const QHash<GLuint, GLuint>& tagetToSourceIndexMap, int maxIndex);

	//! Return the equivalent triangles index of the strips index of given LOD and material ID
    IndexList equivalentTrianglesIndexOfstripsIndex(int lodIndex, GLC_uint materialId) const;

	//! Return the equivalent triangles index of the fan index of given LOD and material ID
    IndexList equivalentTrianglesIndexOfFansIndex(int lodIndex, GLC_uint materialId) const;

    static SharpEdgeContainer* computeSharEdgeMappedFunction(SharpEdgeContainer* pContainer);

    void innerCopy(const GLC_Mesh& other);

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

	//! Mesh number of vertice
	unsigned int m_NumberOfVertice;

	//! Mesh number of normals
	unsigned int m_NumberOfNormals;

	//! Color pear vertex
	bool m_ColorPearVertex;

	//! Data of the mesh (Bulk Data + LOD with indexs)
	GLC_MeshData m_MeshData;

	//! The current LOD index
	int m_CurrentLod;

    QHash<GLC_uint, GLC_uint> m_OldToNewMaterialId;

	//! Class chunk id
	static quint32 m_ChunkId;

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
void GLC_Mesh::vboDrawPrimitivesGroupOf(GLC_PrimitiveGroup* pCurrentGroup, GLC_Material* pCurrentMaterial, bool materialIsRenderable
		, bool isTransparent,  QHash<GLC_uint, GLC_Material*>* pMaterialHash)
{
	GLC_Material* pCurrentLocalMaterial= pCurrentMaterial;
	// Draw triangles
	if (pCurrentGroup->containsTriangles())
	{
		Q_ASSERT(pCurrentGroup->containsTrianglesGroupId());
		const GLsizei trianglesGroupCount= static_cast<GLsizei>(pCurrentGroup->trianglesGroupOffset().size());
		for (GLint i= 0; i < trianglesGroupCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->triangleGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				if (pCurrentLocalMaterial != pMaterialHash->value(currentPrimitiveId))
				{
					pCurrentLocalMaterial= pMaterialHash->value(currentPrimitiveId);
					if (pCurrentLocalMaterial->isTransparent() == isTransparent) pCurrentLocalMaterial->glExecute();
				}
			}
			else if (pCurrentLocalMaterial != pCurrentMaterial)
			{
				pCurrentLocalMaterial= pCurrentMaterial;
				if (materialIsRenderable) pCurrentLocalMaterial->glExecute();
			}
			if (pCurrentLocalMaterial->isTransparent() == isTransparent)
			{
				glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->trianglesGroupOffset().at(i));
			}
		}
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStrip())
	{
		Q_ASSERT(pCurrentGroup->containsStripGroupId());
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffset().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->stripGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				if (pCurrentLocalMaterial != pMaterialHash->value(currentPrimitiveId))
				{
					pCurrentLocalMaterial= pMaterialHash->value(currentPrimitiveId);
					if (pCurrentLocalMaterial->isTransparent() == isTransparent) pCurrentLocalMaterial->glExecute();
				}
			}
			else if (pCurrentLocalMaterial != pCurrentMaterial)
			{
				pCurrentLocalMaterial= pCurrentMaterial;
				if (materialIsRenderable) pCurrentLocalMaterial->glExecute();
			}
			if (pCurrentLocalMaterial->isTransparent() == isTransparent)
			{
				glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->stripsOffset().at(i));
			}
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFan())
	{
		Q_ASSERT(pCurrentGroup->containsFanGroupId());
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffset().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->fanGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				if (pCurrentLocalMaterial != pMaterialHash->value(currentPrimitiveId))
				{
					pCurrentLocalMaterial= pMaterialHash->value(currentPrimitiveId);
					if (pCurrentLocalMaterial->isTransparent() == isTransparent) pCurrentLocalMaterial->glExecute();
				}
			}
			else if (pCurrentLocalMaterial != pCurrentMaterial)
			{
				pCurrentLocalMaterial= pCurrentMaterial;
				if (materialIsRenderable) pCurrentLocalMaterial->glExecute();
			}
			if (pCurrentLocalMaterial->isTransparent() == isTransparent)
			{
				glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->fansOffset().at(i));
			}
		}
	}

}

// Use Vertex Array to Draw primitives with specific materials from the specified GLC_PrimitiveGroup
void GLC_Mesh::vertexArrayDrawPrimitivesGroupOf(GLC_PrimitiveGroup* pCurrentGroup, GLC_Material* pCurrentMaterial, bool materialIsRenderable
		, bool isTransparent, QHash<GLC_uint, GLC_Material*>* pMaterialHash)
{
	GLC_Material* pCurrentLocalMaterial= pCurrentMaterial;
	// Draw triangles
	if (pCurrentGroup->containsTriangles())
	{
		Q_ASSERT(pCurrentGroup->containsTrianglesGroupId());
		const GLsizei trianglesGroupCount= static_cast<GLsizei>(pCurrentGroup->trianglesGroupOffseti().size());
		for (GLint i= 0; i < trianglesGroupCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->triangleGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				if (pCurrentLocalMaterial != pMaterialHash->value(currentPrimitiveId))
				{
					pCurrentLocalMaterial= pMaterialHash->value(currentPrimitiveId);
					if (pCurrentLocalMaterial->isTransparent() == isTransparent) pCurrentLocalMaterial->glExecute();
				}
			}
			else if (pCurrentLocalMaterial != pCurrentMaterial)
			{
				pCurrentLocalMaterial= pCurrentMaterial;
				if (materialIsRenderable) pCurrentLocalMaterial->glExecute();
			}
			if (pCurrentLocalMaterial->isTransparent() == isTransparent)
			{
				GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesGroupOffseti().at(i)];
				glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pOffset);
			}
		}
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStrip())
	{
		Q_ASSERT(pCurrentGroup->containsStripGroupId());
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffseti().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->stripGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				if (pCurrentLocalMaterial != pMaterialHash->value(currentPrimitiveId))
				{
					pCurrentLocalMaterial= pMaterialHash->value(currentPrimitiveId);
					if (pCurrentLocalMaterial->isTransparent() == isTransparent) pCurrentLocalMaterial->glExecute();
				}
			}
			else if (pCurrentLocalMaterial != pCurrentMaterial)
			{
				pCurrentLocalMaterial= pCurrentMaterial;
				if (materialIsRenderable) pCurrentLocalMaterial->glExecute();
			}
			if (pCurrentLocalMaterial->isTransparent() == isTransparent)
			{
				GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->stripsOffseti().at(i)];
				glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pOffset);
			}
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFan())
	{
		Q_ASSERT(pCurrentGroup->containsFanGroupId());
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffseti().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->fanGroupId(i);
			if (pMaterialHash->contains(currentPrimitiveId))
			{
				if (pCurrentLocalMaterial != pMaterialHash->value(currentPrimitiveId))
				{
					pCurrentLocalMaterial= pMaterialHash->value(currentPrimitiveId);
					if (pCurrentLocalMaterial->isTransparent() == isTransparent) pCurrentLocalMaterial->glExecute();
				}
			}
			else if (pCurrentLocalMaterial != pCurrentMaterial)
			{
				pCurrentLocalMaterial= pCurrentMaterial;
				if (materialIsRenderable) pCurrentLocalMaterial->glExecute();
			}
			if (pCurrentLocalMaterial->isTransparent() == isTransparent)
			{
				GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->fansOffseti().at(i)];
				glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pOffset);
			}
		}
	}

}

// Use VBO to Draw primitives with specific materials from the specified GLC_PrimitiveGroup
void GLC_Mesh::vboDrawSelectedPrimitivesGroupOf(GLC_PrimitiveGroup* pCurrentGroup, GLC_Material* pCurrentMaterial, bool materialIsRenderable
		, bool isTransparent, const GLC_RenderProperties& renderProperties)
{
    Q_ASSERT(nullptr != pCurrentMaterial);
	QSet<GLC_uint>* pSelectedPrimitive= renderProperties.setOfSelectedPrimitivesId();
    Q_ASSERT(nullptr != pSelectedPrimitive);

	QHash<GLC_uint, GLC_Material*>* pMaterialHash= NULL;
	if (!renderProperties.hashOfOverwritePrimitiveMaterialsIsEmpty())
	{
		pMaterialHash= renderProperties.hashOfOverwritePrimitiveMaterials();
	}

	GLC_Material* pCurrentLocalMaterial= pCurrentMaterial;
	// Draw triangles
	if (pCurrentGroup->containsTriangles())
	{
		Q_ASSERT(pCurrentGroup->containsTrianglesGroupId());
		const GLsizei trianglesGroupCount= static_cast<GLsizei>(pCurrentGroup->trianglesGroupOffset().size());
		for (GLint i= 0; i < trianglesGroupCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->triangleGroupId(i);
			if (pSelectedPrimitive->contains(currentPrimitiveId))
			{
				if (!isTransparent)
				{
					GLC_SelectionMaterial::glExecute();
                    pCurrentLocalMaterial= nullptr;
					glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->trianglesGroupOffset().at(i));
				}
			}
			else if ((NULL != pMaterialHash) && pMaterialHash->contains(currentPrimitiveId))
			{
				if (pMaterialHash->value(currentPrimitiveId)->isTransparent() == isTransparent)
				{
					GLC_Material* pMat= pMaterialHash->value(currentPrimitiveId);
					if (pMat != pCurrentLocalMaterial)
					{
						pCurrentLocalMaterial= pMat;
						pCurrentLocalMaterial->glExecute();
					}
					glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->trianglesGroupOffset().at(i));
				}

			}
			else if (materialIsRenderable)
			{
				if (pCurrentLocalMaterial != pCurrentMaterial)
				{
					pCurrentLocalMaterial= pCurrentMaterial;
					pCurrentLocalMaterial->glExecute();
				}
				glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->trianglesGroupOffset().at(i));
			}
		}
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStrip())
	{
		Q_ASSERT(pCurrentGroup->containsStripGroupId());
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffset().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->stripGroupId(i);
			if (pSelectedPrimitive->contains(currentPrimitiveId))
			{
				if (!isTransparent)
				{
					GLC_SelectionMaterial::glExecute();
                    pCurrentLocalMaterial= nullptr;
					glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->stripsOffset().at(i));
				}
			}
			else if ((NULL != pMaterialHash) && pMaterialHash->contains(currentPrimitiveId))
			{
				if (pMaterialHash->value(currentPrimitiveId)->isTransparent() == isTransparent)
				{
					GLC_Material* pMat= pMaterialHash->value(currentPrimitiveId);
					if (pMat != pCurrentLocalMaterial)
					{
						pCurrentLocalMaterial= pMat;
						pCurrentLocalMaterial->glExecute();
					}
					glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->stripsOffset().at(i));
				}

			}
			else if (materialIsRenderable)
			{
				if (pCurrentLocalMaterial != pCurrentMaterial)
				{
					pCurrentLocalMaterial= pCurrentMaterial;
					pCurrentLocalMaterial->glExecute();
				}
				glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->stripsOffset().at(i));
			}
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFan())
	{
		Q_ASSERT(pCurrentGroup->containsFanGroupId());
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffset().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->fanGroupId(i);
			if (pSelectedPrimitive->contains(currentPrimitiveId))
			{
				if (!isTransparent)
				{
					GLC_SelectionMaterial::glExecute();
                    pCurrentLocalMaterial= nullptr;
					glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->fansOffset().at(i));
				}
			}
			else if ((NULL != pMaterialHash) && pMaterialHash->contains(currentPrimitiveId))
			{
				if (pMaterialHash->value(currentPrimitiveId)->isTransparent() == isTransparent)
				{
					GLC_Material* pMat= pMaterialHash->value(currentPrimitiveId);
					if (pMat != pCurrentLocalMaterial)
					{
						pCurrentLocalMaterial= pMat;
						pCurrentLocalMaterial->glExecute();
					}
					glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->fansOffset().at(i));
				}

			}
			else if (materialIsRenderable)
			{
				if (pCurrentLocalMaterial != pCurrentMaterial)
				{
					pCurrentLocalMaterial= pCurrentMaterial;
					pCurrentLocalMaterial->glExecute();
				}
				glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pCurrentGroup->fansOffset().at(i));
			}
		}
	}

}

// Use Vertex Array to Draw primitives with specific materials from the specified GLC_PrimitiveGroup
void GLC_Mesh::vertexArrayDrawSelectedPrimitivesGroupOf(GLC_PrimitiveGroup* pCurrentGroup, GLC_Material* pCurrentMaterial, bool materialIsRenderable
		, bool isTransparent, const GLC_RenderProperties& renderProperties)
{
	QSet<GLC_uint>* pSelectedPrimitive= renderProperties.setOfSelectedPrimitivesId();
	Q_ASSERT(NULL != pSelectedPrimitive);

	QHash<GLC_uint, GLC_Material*>* pMaterialHash= NULL;
	if (!renderProperties.hashOfOverwritePrimitiveMaterialsIsEmpty())
	{
		pMaterialHash= renderProperties.hashOfOverwritePrimitiveMaterials();
	}

	GLC_Material* pCurrentLocalMaterial= pCurrentMaterial;
	// Draw triangles
	if (pCurrentGroup->containsTriangles())
	{
		Q_ASSERT(pCurrentGroup->containsTrianglesGroupId());
		const GLsizei trianglesGroupCount= static_cast<GLsizei>(pCurrentGroup->trianglesGroupOffseti().size());
		for (GLint i= 0; i < trianglesGroupCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->triangleGroupId(i);
			if (pSelectedPrimitive->contains(currentPrimitiveId))
			{
				if (!isTransparent)
				{
					GLC_SelectionMaterial::glExecute();
					pCurrentLocalMaterial= NULL;
					GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesGroupOffseti().at(i)];
					glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pOffset);
				}
			}
			else if ((NULL != pMaterialHash) && pMaterialHash->contains(currentPrimitiveId))
			{
				if (pMaterialHash->value(currentPrimitiveId)->isTransparent() == isTransparent)
				{
					GLC_Material* pMat= pMaterialHash->value(currentPrimitiveId);
					if (pMat != pCurrentLocalMaterial)
					{
						pCurrentLocalMaterial= pMat;
						pCurrentLocalMaterial->glExecute();
					}
					GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesGroupOffseti().at(i)];
					glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pOffset);
				}

			}
			else if (materialIsRenderable)
			{
				if (pCurrentLocalMaterial != pCurrentMaterial)
				{
					pCurrentLocalMaterial= pCurrentMaterial;
					pCurrentLocalMaterial->glExecute();
				}
				GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->trianglesGroupOffseti().at(i)];
				glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndexSizes().at(i), GL_UNSIGNED_INT, pOffset);
			}
		}
	}

	// Draw Triangles strip
	if (pCurrentGroup->containsStrip())
	{
		Q_ASSERT(pCurrentGroup->containsStripGroupId());
		const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffseti().size());
		for (GLint i= 0; i < stripsCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->stripGroupId(i);
			if (pSelectedPrimitive->contains(currentPrimitiveId))
			{
				if (!isTransparent)
				{
					GLC_SelectionMaterial::glExecute();
					pCurrentLocalMaterial= NULL;
					GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->stripsOffseti().at(i)];
					glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pOffset);
				}
			}
			else if ((NULL != pMaterialHash) && pMaterialHash->contains(currentPrimitiveId))
			{
				if (pMaterialHash->value(currentPrimitiveId)->isTransparent() == isTransparent)
				{
					GLC_Material* pMat= pMaterialHash->value(currentPrimitiveId);
					if (pMat != pCurrentLocalMaterial)
					{
						pCurrentLocalMaterial= pMat;
						pCurrentLocalMaterial->glExecute();
					}
					GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->stripsOffseti().at(i)];
					glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pOffset);
				}

			}
			else if (materialIsRenderable)
			{
				if (pCurrentLocalMaterial != pCurrentMaterial)
				{
					pCurrentLocalMaterial= pCurrentMaterial;
					pCurrentLocalMaterial->glExecute();
				}
				GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->stripsOffseti().at(i)];
				glDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().at(i), GL_UNSIGNED_INT, pOffset);
			}
		}
	}

	// Draw Triangles fan
	if (pCurrentGroup->containsFan())
	{
		Q_ASSERT(pCurrentGroup->containsFanGroupId());
		const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffseti().size());
		for (GLint i= 0; i < fansCount; ++i)
		{
			GLC_uint currentPrimitiveId= pCurrentGroup->fanGroupId(i);
			if (pSelectedPrimitive->contains(currentPrimitiveId))
			{
				if (!isTransparent)
				{
					GLC_SelectionMaterial::glExecute();
					pCurrentLocalMaterial= NULL;
					GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->fansOffseti().at(i)];
					glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pOffset);
				}
			}
			else if ((NULL != pMaterialHash) && pMaterialHash->contains(currentPrimitiveId))
			{
				if (pMaterialHash->value(currentPrimitiveId)->isTransparent() == isTransparent)
				{
					GLC_Material* pMat= pMaterialHash->value(currentPrimitiveId);
					if (pMat != pCurrentLocalMaterial)
					{
						pCurrentLocalMaterial= pMat;
						pCurrentLocalMaterial->glExecute();
					}
					GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->fansOffseti().at(i)];
					glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pOffset);
				}

			}
			else if (materialIsRenderable)
			{
				if (pCurrentLocalMaterial != pCurrentMaterial)
				{
					pCurrentLocalMaterial= pCurrentMaterial;
					pCurrentLocalMaterial->glExecute();
				}
				GLvoid* pOffset= &m_MeshData.indexVectorHandle(m_CurrentLod)->data()[pCurrentGroup->fansOffseti().at(i)];
				glDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().at(i), GL_UNSIGNED_INT, pOffset);
			}
		}
	}

}

// Activate mesh VBOs and IBO of the current LOD
void GLC_Mesh::activateVboAndIbo()
{
    GLC_Context* pContext= GLC_ContextManager::instance()->currentContext();

	// Activate Vertices VBO
    m_MeshData.useVBO(GLC_MeshData::GLC_Vertex);
    pContext->glcUseVertexPointer(0);

	// Activate Normals VBO
    m_MeshData.useVBO(GLC_MeshData::GLC_Normal);
    pContext->glcUseNormalPointer(0);

	// Activate texel VBO if needed
    if (m_MeshData.useVBO(GLC_MeshData::GLC_Texel))
	{
        pContext->glcUseTexturePointer(0);
	}

	// Activate Color VBO if needed
    if ((m_ColorPearVertex && !m_IsSelected && !GLC_State::isInSelectionMode()) && m_MeshData.useVBO(GLC_MeshData::GLC_Color))
	{
        pContext->glcEnableColorMaterial(true);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
        pContext->glcUseColorPointer(0);
	}

	m_MeshData.useIBO(true, m_CurrentLod);
}

// Activate vertex Array
void GLC_Mesh::activateVertexArray()
{
    GLC_Context* pContext= GLC_ContextManager::instance()->currentContext();

	// Use Vertex Array
    pContext->glcUseVertexPointer(m_MeshData.positionVectorHandle()->data());

    pContext->glcUseNormalPointer(m_MeshData.normalVectorHandle()->data());

	// Activate texel if needed
	if (!m_MeshData.texelVectorHandle()->isEmpty())
	{
        pContext->glcUseTexturePointer(m_MeshData.texelVectorHandle()->data());
	}

	// Activate Color array if needed
    if ((m_ColorPearVertex && !m_IsSelected && !GLC_State::isInSelectionMode()) && !m_MeshData.colorVectorHandle()->isEmpty())
	{
        pContext->glcEnableColorMaterial(true);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
        pContext->glcUseColorPointer(m_MeshData.colorVectorHandle()->data());
	}
}



#endif /* GLC_MESH_H_ */
