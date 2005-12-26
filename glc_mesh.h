/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9, packaged on Novemeber, 2005.

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
#include <QList>

#include "glc_geometry.h"
#include "glc_face.h"

//! The mesh's faces list
typedef QList<GLC_Face*> CFaceList;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Mesh
/*! \brief GLC_Mesh : OpenGL 3D Mesh*/

/*! An GLC_Mesh is Mesh composed of polygons
 * 		- Each polygon must be a planar Face (GLC_Face)
 * 		- Each face is composed by vertexs (GLC_Vertex)
 * */
//////////////////////////////////////////////////////////////////////
class GLC_Mesh : public GLC_Geometry
{
//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
public:
	//! Construct an empty Mesh
	GLC_Mesh(const char *pName= "Mesh", const GLfloat *pColor= 0);
	
	//! Copy Constructor
	GLC_Mesh(GLC_Mesh *pMesh);

	//! Delete Mesh's faces and clear faces list
	virtual ~GLC_Mesh(void);

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Return the number of mesh's face
	int GetNbrFaces(void) const
	{
		return m_nNbrFaces;
	}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
public:

	//! Add a face to the Mesh
	void AddFace(GLC_Face* pFace);

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
private:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function is implemented here.\n*/
	virtual void GlDraw(void);

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function is implemented here.*/
	virtual void GlPropGeom(void);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! Mesh's list of GLC_Face
	CFaceList m_Liste_pFace;
	
	//! Mesh's Number of faces
	int m_nNbrFaces;

	//! Polygons display style
	GLenum m_PolyFace;
	GLenum m_PolyMode;
	
};
#endif //GLC_MESH_H_
