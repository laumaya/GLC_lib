/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.1.0, packaged on March, 2009.

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

//! \file glc_extendedmesh.h interface for the GLC_ExtendedMesh class.

#ifndef GLC_EXTENDEDMESH_H_
#define GLC_EXTENDEDMESH_H_

#include <QHash>
#include <QList>
#include "../maths/glc_vector2df.h"
#include "../maths/glc_vector3df.h"
#include "../glc_enum.h"
#include "../shading/glc_material.h"
#include "glc_extendedgeomengine.h"
#include "glc_vbogeom.h"
#include "glc_primitivegroup.h"


class GLC_ExtendedMesh : public GLC_VboGeom
{
public:
	typedef QHash<GLC_uint, GLC_PrimitiveGroup*> PrimitiveGroups;
	typedef QHash<const int, PrimitiveGroups*> PrimitiveGroupsHash;

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_ExtendedMesh();

	//! Copy constructor
	GLC_ExtendedMesh(const GLC_ExtendedMesh&);

	virtual ~GLC_ExtendedMesh();
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

	//! return the mesh bounding box
	virtual GLC_BoundingBox& boundingBox(void);

	//! Return a copy of the geometry
	virtual GLC_VboGeom* clone() const;

	//! Return true if color pear vertex is activated
	inline bool ColorPearVertexIsAcivated() const {return m_ColorPearVertex;}

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Add vertices coordinate
	inline void addVertices(const GLfloatVector& vertices)
	{
		*(m_ExtendedGeomEngine.positionVectorHandle())+= vertices;
		m_NumberOfVertice+= vertices.size() / 3;
	}

	//! Add Normals
	inline void addNormals(const GLfloatVector& normals)
	{*(m_ExtendedGeomEngine.normalVectorHandle())+= normals;}

	//! Add texel
	inline void addTexels(const GLfloatVector& texels)
	{*(m_ExtendedGeomEngine.texelVectorHandle())+= texels;}

	//! Add triangles
	void addTriangles(GLC_Material*, const IndexList&, const int lod= 0);

	//! Add triangles Strip
	void addTrianglesStrip(GLC_Material*, const IndexList&, const int lod= 0);

	//! Add triangles Fan
	void addTrianglesFan(GLC_Material*, const IndexList&, const int lod= 0);

	//! Reverse mesh normal
	void reverseNormal();

	//! Set color per vertex flag
	inline void setColorPearVertex(bool flag){m_ColorPearVertex= flag;}

	//! Copy vertex list in a vector list for Vertex Array Use
	void finished();

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
	GLC_uint setCurrentMaterial(GLC_Material*, const int);

	//! Create VBO and IBO
	void createVbos();
//@}


//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! the list of Hash table of primitive group
	PrimitiveGroupsHash m_PrimitiveGroups;

	//! the default material Id
	GLC_uint m_DefaultMaterialId;

	//! Mesh number of faces
	unsigned int m_NumberOfFaces;

	//! Mesh number of vertice
	unsigned int m_NumberOfVertice;

	//! Selection state
	bool m_IsSelected;

	//! Color pear vertex
	bool m_ColorPearVertex;

	//! Geom engine
	GLC_ExtendedGeomEngine m_ExtendedGeomEngine;

};

#endif /* GLC_EXTENDEDMESH_H_ */
