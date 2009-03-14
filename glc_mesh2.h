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
#include "glc_simplegeomengine.h"

typedef QList<GLC_Vertex> VertexList;
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
	//! Return the number of faces
	inline unsigned int numberOfFaces() const {return m_NumberOfFaces;}

	//! Return the number of vertex
	inline unsigned int numberOfVertex() const {return m_NumberOfFaces * 3;}

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

	//! Add triangles with the same material to the mesh
	void addTriangles(const VertexList &, GLC_Material*);

	//! Reverse mesh normal
	void reverseNormal();

	//! Set color per vertex flag
	inline void setColorPearVertex(bool flag){m_ColorPearVertex= flag;}

	//! Copy vertex list in a vector list for Vertex Array Use
	inline void finished()
	{
		if (m_SimpleGeomEngine.vertexVectorHandle()->isEmpty())
		{
			// Copy vertex Data to a vector
			(*m_SimpleGeomEngine.vertexVectorHandle())= m_Vertex.toVector();
			m_Vertex.clear();
			// Create mesh Bounding Box
			boundingBox();
		}
	}

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
//! Private services Functions
//////////////////////////////////////////////////////////////////////
private:


//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! Vertex List (Use When creating Mesh)
	VertexList m_Vertex;

	//! Hash table of Vector Index grouped by material
	MaterialGroupHash m_MaterialGroup;

	//! Mesh number of faces
	unsigned int m_NumberOfFaces;

	//! Selection state
	bool m_IsSelected;

	//! Color pear vertex
	bool m_ColorPearVertex;

	//! Geom engine
	GLC_SimpleGeomEngine m_SimpleGeomEngine;

};
#endif //GLC_MESH2_H_
