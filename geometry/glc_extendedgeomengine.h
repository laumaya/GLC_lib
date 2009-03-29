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

//! \file glc_extendedgeomengine.h Interface for the GLC_ExtendedGeomEngine class.

#ifndef GLC_EXTENDEDGEOMENGINE_H_
#define GLC_EXTENDEDGEOMENGINE_H_

#include <QVector>
#include "glc_geomengine.h"
#include "glc_enginelod.h"

//! QVector of GLfloat
typedef QVector<GLfloat> GLfloatVector;

//! QVector of GLuint
typedef QVector<GLuint> GLuintVector;

class GLC_ExtendedGeomEngine : public GLC_GeomEngine
{
public:

	//! Enum of VBO TYPE
	enum VboType
	{
		GLC_Vertex= 30,
		GLC_Normal,
		GLC_Texel,
	};

public:
	//! Default constructor
	GLC_ExtendedGeomEngine();

	//! Copy constructor
	GLC_ExtendedGeomEngine(const GLC_ExtendedGeomEngine&);

	virtual ~GLC_ExtendedGeomEngine();


//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the number of lod
	inline int numberOfLod()
	{return m_EngineLodList.size();}

	//! Return the Position Vector
	GLfloatVector positionVector() const;

	//! Return the normal Vector
	GLfloatVector normalVector() const;

	//! Return the Position Vector handle
	inline GLfloatVector* positionVectorHandle()
	{ return &m_Positions;}

	//! Return the Normal Vector handle
	inline GLfloatVector* normalVectorHandle()
	{ return &m_Normals;}

	//! Return the Texel Vector handle
	inline GLfloatVector* texelVectorHandle()
	{ return &m_Texels;}

	//! Return true if contains triangles
	inline bool containsTriangles(const int i= 0) const
	{
		Q_ASSERT(i < m_EngineLodList.size());
		return m_EngineLodList.at(i)->containsTriangles();
	}

	//! Return the Triangle Index Vector handle
	inline GLuintVector* trianglesIndexVectorHandle(const int i= 0)
	{
		Q_ASSERT(i < m_EngineLodList.size());
		return m_EngineLodList.at(i)->trianglesIndexVectorHandle();
	}

	//! Return the size of the triangles index Vector
	inline int trianglesIndexVectorSize(const int i= 0) const
	{
		Q_ASSERT(i < m_EngineLodList.size());
		return m_EngineLodList.at(i)->trianglesIndexVectorSize();
	}

	//! Return true if contains triangles strip
	inline bool containsTrianglesStrip(const int i= 0) const
	{
		Q_ASSERT(i < m_EngineLodList.size());
		return m_EngineLodList.at(i)->containsTrianglesStrip();
	}

	//! Return the Triangle Strip Index Vector handle
	inline GLuintVector* trianglesStripIndexVectorHandle(const int i= 0)
	{
		Q_ASSERT(i < m_EngineLodList.size());
		return m_EngineLodList.at(i)->trianglesStripIndexVectorHandle();
	}

	//! Return true if contains triangles fan
	inline bool containsTrianglesFan(const int i= 0) const
	{
		Q_ASSERT(i < m_EngineLodList.size());
		return m_EngineLodList.at(i)->containsTrianglesFan();
	}

	//! Return the Triangle Strip Index Vector handle
	inline GLuintVector* trianglesFanIndexVectorHandle(const int i= 0)
	{
		Q_ASSERT(i < m_EngineLodList.size());
		return m_EngineLodList.at(i)->trianglesFanIndexVectorHandle();
	}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:


//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Vbo creation
	void createVBOs();

	//! Ibo Usage
	bool useVBO(bool, GLC_ExtendedGeomEngine::VboType);

	//! Ibo Usage
	void useIBO(bool, GLC_EngineLod::IboType);
//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Vertex Position Vector
	GLfloatVector m_Positions;

	//! Vertex Normal Vector
	GLfloatVector m_Normals;

	//! Vertex Texture coordinate
	GLfloatVector m_Texels;

	//! Normals VBO ID
	GLuint m_NormalVboId;

	//! Texture VBO ID
	GLuint m_TexelVboId;

	//! The list of LOD
	QList<GLC_EngineLod*> m_EngineLodList;

};

#endif /* GLC_EXTENDEDGEOMENGINE_H_ */
