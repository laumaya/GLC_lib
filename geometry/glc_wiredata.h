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
//! \file glc_wiredata.h Interface for the GLC_WireData class.

#ifndef GLC_WIREDATA_H_
#define GLC_WIREDATA_H_

#include "../glc_global.h"
#include "../glc_boundingbox.h"
#include "../shading/glc_renderproperties.h"

#include "../glc_config.h"
//////////////////////////////////////////////////////////////////////
//! \class GLC_WireData
/*! \brief GLC_WireData : Contains all data of wires geometries
 */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_WireData
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_WireData();

	//! Copy constructor
	GLC_WireData(const GLC_WireData&);

	//! Overload "=" operator
	GLC_WireData& operator=(const GLC_WireData&);

	//! Destructor
	virtual ~GLC_WireData();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the Position Vector handle
	inline GLfloatVector* positionVectorHandle()
	{ return &m_Positions;}

	//! Return the Position Vector
	GLfloatVector positionVector() const;

	//! Return true if the wire data is empty
	inline bool isEmpty() const
	{return ((m_PositionSize == 0) && m_Positions.isEmpty());}

	//! Return the wire bounding box
	GLC_BoundingBox& boundingBox();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Clear the content of the wire Data and makes it empty
	void clear();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! The Object wich use the data is finished and VBO is used
	void finishVbo();

	//! Ibo Usage
	void useVBO(bool);

	//! Display wires data
	void glDraw(const GLC_RenderProperties&);

private:
	//! Vbo creation
	void createVBOs();

	//! Fill the VBO
	void fillVBOs();
//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! VBO ID
	GLuint m_VboId;

	//! Vertex Position Vector
	GLfloatVector m_Positions;

	//! The size of the VBO
	int m_PositionSize;

	//! Wire data bounding box
	GLC_BoundingBox* m_pBoundingBox;

};

#endif /* GLC_WIREDATA_H_ */
