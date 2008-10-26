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
#include <QVector>
#include <QList>

#include "glc_material.h"
#include "glc_vbogeom.h"
#include "glc_vector2df.h"
#include "glc_vector3df.h"

typedef QHash<GLC_uint, GLC_Material*> MaterialHash;
typedef QList<GLC_Vertex> VertexList;
typedef QVector<GLC_Vertex> VertexVector;
typedef QList<GLuint> IndexList;
typedef QHash<GLC_uint, IndexList*> MaterialGroupHash;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Mesh2
/*! \brief GLC_Mesh2 : OpenGL 3D Mesh*/

/*! An GLC_Mesh2 is Mesh composed of polygons
 * 		- Each polygon must be a planar Face
 * 		- Each face is composed by vertexs
 * */
//////////////////////////////////////////////////////////////////////
class GLC_Mesh2 : public GLC_VboGeom
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
	inline unsigned int getNumberOfFaces() const {return m_NumberOfFaces;}
	//! Get number of vertex
	inline unsigned int getNumberOfVertex() const {return m_VertexVector.size();}
	//! Get number of submaterial
	inline unsigned int getNumberOfSubMaterial() const {return m_MaterialHash.size();}
	//! Get specified mesh sub material
	inline GLC_Material* getSubMaterial(const GLC_uint key) {return m_MaterialHash[key];}
	//! Get All mesh sub material
	inline QList<GLC_Material*> getSubMaterials() {return m_MaterialHash.values();}
	//! return true if Material key is in the mesh
	inline const bool containsMaterial(const GLC_uint key) const {return m_MaterialHash.contains(key);}
	//! Return material index if Material is the same than a material already in the mesh
	/*! Return -1 if the material is not found
	 */
	GLC_uint materialIndex(const GLC_Material& mat) const;
	//! return the mesh bounding box
	virtual GLC_BoundingBox* getBoundingBox(void) const;
	//! Return a copy of the geometry
	virtual GLC_VboGeom* clone() const;
	//! Return true if color pear vertex is activated
	bool ColorPearVertexIsAcivated() const {return m_ColorPearVertex;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Add material to mesh
	void addMaterial(GLC_Material *);

	//! Add triangles with the same material to the mesh
	void addTriangles(const VertexList &, GLC_Material*);

	//! Reverse mesh normal
	void reverseNormal();

	//! Set color per vertex flag
	void setColorPearVertex(bool flag){m_ColorPearVertex= flag;}

	//! Copy vertex list in a vector list for Vertex Array Use
	inline void finished()
	{
		if (0 == m_VertexVector.size())
		{
			m_VertexVector= m_Vertex.toVector();
			m_Vertex.clear();
		}
	}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Specific glExecute method
	virtual void glExecute(bool, bool forceWire= false);

private:

	//! if the geometry have a texture, load it
	virtual void glLoadTexture(void);

	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n
	 *  Throw GLC_OpenGlException*/
	virtual void glDraw(void);

//@}

//////////////////////////////////////////////////////////////////////
//! Private services Functions
//////////////////////////////////////////////////////////////////////
private:


//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! Vertex List (Use When creating Mesh)
	VertexList m_Vertex;

	//! Vertex Vectors (Use When Mesh is Finished)
	VertexVector m_VertexVector;

	//! Hash table of Vector Index grouped by material
	MaterialGroupHash m_MaterialGroup;

	//! Material Hash table
	MaterialHash m_MaterialHash;

	//! Mesh number of faces
	unsigned int m_NumberOfFaces;

	//! Selection state
	bool m_IsSelected;

	//! Color pear vertex
	bool m_ColorPearVertex;

};
#endif //GLC_MESH2_H_
