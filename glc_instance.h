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

//! \file glc_instance.h interface for the GLC_Instance class.

#ifndef GLC_INSTANCE_H_
#define GLC_INSTANCE_H_

#include "glc_geometry.h"
#include "glc_enum.h"
#include "glc_boundingbox.h"
#include "glc_object.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Instance
/*! \brief GLC_Instance : GLC_Geometry + OpenGL list + bounding box*/

/*! An GLC_Instance contain  :
 * 		- GLC_Geometry pointer
 * 		- OpenGL sub list ID
 * 		- Geomtry Bounding box
 * 		- Positionning 4 x 4 matrix
 */
//////////////////////////////////////////////////////////////////////

class GLC_Instance : public GLC_Object
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_Instance();
	
	//! Contruct node with a geometry
	GLC_Instance(GLC_Geometry* pGeom);

	//! Copy constructor
	GLC_Instance(const GLC_Instance& );

	//! Assignement operator
	GLC_Instance &operator=(const GLC_Instance&);	

	//! Destructor
	~GLC_Instance();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Return true if the instance as no geometry
	const bool isNull() const {return (NULL == m_pGeom);}
	
	//! Return true if the instance is selected
	const bool isSelected(void) const {return m_IsSelected;}

	//! Get the geometry of the instance
	GLC_Geometry* getGeometry(void);
	
	//! Get the validity of the Insatnce
	const bool getValidity(void) const;
	
	//! Get the bounding box
	GLC_BoundingBox getBoundingBox(void);

	//! Get the validity of the Bounding Box
	const bool getBoundingBoxValidity(void) const;
	
	//! Return transfomation 4x4Matrix
	const GLC_Matrix4x4 getMatrix(void) const
	{return m_MatPos;}
	
	//! Clone the instance
	GLC_Instance clone() const;
	
	GLC_Instance instanciate();
	
	
//@}	
	
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Set the instance Geometry
	/*! 
	 *  instance must be null
	 */
	bool setGeometry(GLC_Geometry* pGeom);

	//! translate Geometry
	void translate(double Tx, double Ty, double Tz);

	//! move instance with a 4x4Matrix
	void multMatrix(const GLC_Matrix4x4 &MultMat);
	
	//! Replace the instance Matrix
	void setMatrix(const GLC_Matrix4x4 &SetMat);
	
	//! Reset the instance Matrix
	void resetMatrix(void);

	//! Polygon's display style
	void setPolygonMode(GLenum Face, GLenum Mode);

	//! Select the instance
	void select(void);
	
	//! Unselect the instance
	void unselect(void);
	
	//! set Instance validity in case of multiple instance.
	void setInstanceValidity(void);
		
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Display the instance
	void glExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);
	
private:
	//! Set instance visualisation properties
	void glVisProperties();
	
//@}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////
private:
	//! compute the instance bounding box
	void computeBoundingBox(void);
	//! Clear current instance
	void clear();
	
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	
	//! Geometry of the instance
	GLC_Geometry* m_pGeom;
	
	//! OpenGL list of the instance
	GLuint m_ListID;
	
	//! BoundingBox of the instance
	GLC_BoundingBox* m_pBoundingBox;
	
	//! Number of this instance
	int* m_pNumberOfInstance;

	//! Geometry matrix
	GLC_Matrix4x4 m_MatPos;

	//! instance validity
	bool m_IsValid;

	//! Selection state
	bool m_IsSelected;

	//! Polygons display style
	GLenum m_PolyFace;
	GLenum m_PolyMode;
	
};


#endif /*GLC_INSTANCE_H_*/
