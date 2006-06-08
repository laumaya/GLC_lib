/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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

//! \file glc_geometry.h Interface for the GLC_Geometry class.

#ifndef GLC_GEOMETRY_H_
#define GLC_GEOMETRY_H_

#include "glc_object.h"
#include "glc_material.h"
#include "glc_boundingbox.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Geometry
/*! \brief GLC_Geometry : parent class for all GLC class which contain
 *  geometrical data*/

/*! GLC_Geometry is a abstract class. \n \n
 *  Main attributes of GLC_Geometry:
 *		- Material : 	GLC_Material
 * 		- Graphic properties
 * 		- Transformation Matrix
 * 
 * GLC_Geometry provide :
 * 		- Function to create OpenGL list : GLC_Geometry::CreateList
 * 		- Function to draw Geometrie : GLC_Geometry::GlExecute
 * 		- Virtual function to overload for visual property: GLC_Geometry::GlPropGeom
 * 		- Virtual function to overload for Object topologie: GLC_Geometry::GlDraw
 *
 * \todo Add link to a sample inherit class.
 */
//////////////////////////////////////////////////////////////////////

class GLC_Geometry :
	public GLC_Object
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	// default constructor
	GLC_Geometry(const char *pName, const GLfloat *pColor);
	//! Copy constructor
	GLC_Geometry(const GLC_Geometry& sourceGeom);
	
	//! Destructor
	virtual ~GLC_Geometry(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public:

	//! Return Visibility state of geometry
	const bool GetIsVisible(void) const;

	//! Return an array of 4 GLfloat which represent the color
	const GLfloat *GetfRGBA(void) const;

	//! Return Color Red component
	GLfloat GetfRed(void) const;

	//! Return Color Green component
	GLfloat GetfGreen(void) const;
	
	//! Return Color blue component
	GLfloat GetfBlue(void) const;
	
	//! Return Color Alpha component
	GLfloat GetfAlpha(void) const;
	
	//! Return transfomation 4x4Matrix
	const GLC_Matrix4x4 GetMatrix(void) const;
	
	//! Return thickness
	const float GetThickness(void) const;
	
	//! Return associated OpenGL list ID
	GLuint GetListID(void);
	
	//! Return Validity of associated OpenGL list
	bool GetListIsValid(void) const;

	//! Return Validity of geometry
	bool GetValidity(void) const;

	//! Return material of geometry
	GLC_Material* GetMaterial(void) const;

	//! Return true if blending is enable
	bool GetBlending(void) const;
	
	//! return the geometry bounding box
	virtual GLC_BoundingBox* getBoundingBox(void) const;
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public:

	//! Set visibility statement
	void SetVisibility(bool v);

	//! Set Color RGBA component
	void SetRGBAColor(GLfloat Rouge, GLfloat Vert, GLfloat Bleu, GLfloat Alpha= 1);

	//! Set Color RGBA component with an array of 4 GLfloat
	void SetRGBAColor(const GLfloat* SetCol);	// SetCol[4]


// Set Position	

	//! Translate Geometry
	void Translate(double Tx, double Ty, double Tz);

	//! Move Geometry with a 4x4Matrix
	void MultMatrix(const GLC_Matrix4x4 &MultMat);
	
	//! Replace the Geometry Matrix
	void SetMatrix(const GLC_Matrix4x4 &SetMat);
	
	//! Reset the Geometry Matrix
	void ResetMatrix(void);
	
	//! Set Wire thickness
	void SetThikness(float SetEp);

	//! Set Blending
	void SetBlending(bool Blending);
	
	//! Polygon's display style
	void SetPolygonMode(GLenum Face, GLenum Mode);

	// Material
	//! Set the Geometry material
	void SetMaterial(GLC_Material* pMat);
	
	//! Remove Geometry from the material without update material usage table
	/*! This function is call by GLC_Material, do not call it manualy ! */
	void DelMaterial(GLC_Geometry* pGeom);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public:

	//! Virtual interface for OpenGL execution from GLC_Object.
	/*! This Virtual function is implemented here.\n
	 * At the first call, this function call virtual function
	 * GLC_Geometry::GlPropGeom and set up :
	 * 		- Geometry
	 * 		- OpenGL list
	 * .
	 * AfterWard this function
	 *		- Call virtual function GLC_Geometry::GlPropGeom
	 * 		- If Display list exist or is Valid Call Display List
	 * 		- If Display list doesn't exist try to create IT by calling
	 *        virtual function GLC_Geometry::GlDraw
	 */
	virtual void GlExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);

	//! OpenGL list creation
	bool CreateList(GLenum Mode= GL_COMPILE_AND_EXECUTE);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
protected:

	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function have to be implemented in concrete class.*/
	virtual void GlDraw(void) = 0;

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function have to be implemented in concrete class.*/
	virtual void GlPropGeom(void) = 0;
//@}

//////////////////////////////////////////////////////////////////////
// Protected services functions
//////////////////////////////////////////////////////////////////////
protected:

	//! Delete OpenGL list
	/*! Call by GLC_Geometry::~GLC_Geometry*/
	void DeleteList();

//////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////
protected:
	
	//! Geometry matrix
	GLC_Matrix4x4	m_MatPos;

	//! Display list ID
	GLuint m_ListID;

	//! Display list validity
	bool m_ListIsValid;

	//! Geometry validity
	bool m_GeometryIsValid;

	//! Material
	GLC_Material* m_pMaterial;

	//! Blending
	bool m_IsBlended;

	//! Polygons display style
	GLenum m_PolyFace;
	GLenum m_PolyMode;

//////////////////////////////////////////////////////////////////////
// Private members 
//////////////////////////////////////////////////////////////////////
private:

	// RGBA geometry color
	GLfloat	m_RGBAColor[4];

	// Thikness of geometry's Edge
	float m_Thikness;
	
	// Geometry visibility
	bool m_IsVisible;

};
#endif //GLC_GEOMETRY_H_
