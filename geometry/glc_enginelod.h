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

//! \file glc_enginelod.h interface for the GLC_EngineLod class.

#ifndef GLC_ENGINELOD_H_
#define GLC_ENGINELOD_H_

#include <QVector>
#include "../glc_ext.h"

class GLC_EngineLod
{
public:
	//! Enum of IBO TYPE
	enum IboType
	{
		GLC_Triangles= 1,
		GLC_TrianglesStrip,
		GLC_TrianglesFan,
	};

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_EngineLod(double accuracy= 0.0);

	//! Copy constructor
	GLC_EngineLod(const GLC_EngineLod&);

	//!Destructor
	virtual ~GLC_EngineLod();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the accuracy of the load
	inline double accuracy() const
	{return m_Accuracy;}

	//! Return true if contains triangles
	inline bool containsTriangles() const
	{return not m_TrianglesIbo.isEmpty();}

	//! Return the Triangle Index Vector handle
	inline QVector<GLuint>* trianglesIndexVectorHandle()
	{ return &m_TrianglesIbo;}

	//! Return the size of the triangles index Vector
	inline int trianglesIndexVectorSize() const
	{return m_TrianglesIbo.size();}

	//! Return true if contains triangles strip
	inline bool containsTrianglesStrip() const
	{ return not m_TrianglesStripIbo.isEmpty();}

	//! Return the Triangle Strip Index Vector handle
	inline QVector<GLuint>* trianglesStripIndexVectorHandle()
	{ return &m_TrianglesStripIbo;}

	//! Return true if contains triangles fan
	inline bool containsTrianglesFan() const
	{ return not m_TrianglesFanIbo.isEmpty();}

	//! Return the Triangle Strip Index Vector handle
	inline QVector<GLuint>* trianglesFanIndexVectorHandle()
	{ return &m_TrianglesFanIbo;}

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
	//! IBO creation
	void createIBOs();

	//! Ibo Usage
	void useIBO(GLC_EngineLod::IboType);
//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! The accuracy of the load
	double m_Accuracy;

	//! Triangles IBO ID
	GLuint m_TrianglesIboId;

	//! Triangle IBO Vector
	QVector<GLuint> m_TrianglesIbo;

	//! Triangles strip IBO ID
	GLuint m_TrianglesStripIboId;

	//! Triangle strip IBO Vector
	QVector<GLuint> m_TrianglesStripIbo;

	//! Triangles fan IBO ID
	GLuint m_TrianglesFanIboId;

	//! Triangles fan IBO Vector
	QVector<GLuint> m_TrianglesFanIbo;

};

#endif /* GLC_ENGINELOD_H_ */
