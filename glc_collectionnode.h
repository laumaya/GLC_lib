/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.7, packaged on September, 2007.

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

//! \file glc_collectionnode.h interface for the GLC_CollectionNode class.

#ifndef GLC_COLLECTIONNODE_H_
#define GLC_COLLECTIONNODE_H_

#include "glc_geometry.h"
#include "glc_enum.h"
#include "glc_boundingbox.h"
#include "glc_object.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_CollectionNode
/*! \brief GLC_CollectionNode : GLC_Geometry + OpenGL list + bounding box*/

/*! An GLC_CollectionNode contain  :
 * 		- GLC_Geometry pointer
 * 		- OpenGL sub list ID
 * 		- Geomtry Bounding box
 */
//////////////////////////////////////////////////////////////////////

class GLC_CollectionNode : public GLC_Object
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_CollectionNode();
	
	//! Contruct node with a geometry
	GLC_CollectionNode(GLC_Geometry* pGeom);

	//! Copy constructor
	GLC_CollectionNode(const GLC_CollectionNode& );

	//! Destructor
	~GLC_CollectionNode();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Get the geometry of the node
	GLC_Geometry* getGeometry(void);
	
	//! Get the validity of the OpenGL list
	bool getListValidity(void) const;
	
	//! Get the bounding box
	GLC_BoundingBox getBoundingBox(void);

	//! Get the validity of the Bounding Box
	bool getBoundingBoxValidity(void) const;
	
	//! Return transfomation 4x4Matrix
	const GLC_Matrix4x4 getMatrix(void) const
	{return m_MatPos;}
	
	
	
//@}	
	
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set the node Geometry
	/*! 
	 *  Geometry haven't to be already set
	 */
	bool setGeometry(GLC_Geometry* pGeom);

	//! translate Geometry
	void translate(double Tx, double Ty, double Tz);

	//! move Geometry with a 4x4Matrix
	void multMatrix(const GLC_Matrix4x4 &MultMat);
	
	//! Replace the Geometry Matrix
	void setMatrix(const GLC_Matrix4x4 &SetMat);
	
	//! Reset the Geometry Matrix
	void resetMatrix(void);
	
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Display the Node
	void glExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);
//@}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////
private:
	//! compute the node bounding box
	void computeBoundingBox(void);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	
	//! Geometry of the node
	GLC_Geometry* m_pGeom;
	
	//! OpenGL list of the node
	GLuint m_ListID;
	
	//! BoundingBox of the node
	GLC_BoundingBox* m_pBoundingBox;
	
	//! Number of collection node instance
	int* m_pNumberOfInstance;

	//! Geometry matrix
	GLC_Matrix4x4 m_MatPos;

	//! Node validity
	bool m_NodeIsValid;

	
};


#endif /*GLC_COLLECTIONNODE_H_*/
