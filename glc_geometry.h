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

//! \file glc_geometry.h Interface for the GLC_Geometry class.

#ifndef GLC_GEOMETRY_H_
#define GLC_GEOMETRY_H_

#include <QColor>
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
 * 		- Function to create OpenGL list : GLC_Geometry::createList
 * 		- Function to draw Geometrie : GLC_Geometry::glExecute
 * 		- Virtual function to overload for visual property: GLC_Geometry::glPropGeom
 * 		- Virtual function to overload for Object topologie: GLC_Geometry::glDraw
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
	GLC_Geometry(const QString &name, const bool typeIsWire);
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
	
	//! Return an array of 4 GLfloat which represent the color
	QColor getRGBA(void) const;

	//! Return Color Red component
	GLdouble getdRed(void) const;

	//! Return Color Green component
	GLdouble getdGreen(void) const;
	
	//! Return Color blue component
	GLdouble getdBlue(void) const;
	
	//! Return Color Alpha component
	GLdouble getdAlpha(void) const;
	
	//! Return thickness
	const float getThickness(void) const;
	
	//! Return associated OpenGL list ID
	GLuint getListID(void);
	
	//! Return Validity of associated OpenGL list
	bool getListIsValid(void) const;

	//! Return Validity of geometry
	bool getValidity(void) const;

	//! Return material of geometry
	GLC_Material* getMaterial(void) const;
	
	//! return the geometry bounding box
	virtual GLC_BoundingBox* getBoundingBox(void) const;
	
	//! clone the geometry
	virtual GLC_Geometry* clone() const = 0;
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public:

	//! Set Diffuse Color RGBA component
	void setColor(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha= 1.0);

	//! Set Diffuse Color RGBA component with an QColor Object
	void setColor(const QColor&);


// Set Position	

	//! Set Wire thickness
	void setThikness(float SetEp);

	// Material
	//! Set the Geometry material
	void setMaterial(GLC_Material* pMat);
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public:

	//! if the geometry have a texture, load it
	virtual void glLoadTexture(void);
	
	//! Virtual interface for OpenGL execution from GLC_Object.
	/*! This Virtual function is implemented here.\n
	 * At the first call, this function call virtual function
	 * GLC_Geometry::glPropGeom and set up :
	 * 		- Geometry
	 * 		- OpenGL list
	 * .
	 * AfterWard this function
	 *		- Call virtual function GLC_Geometry::glPropGeom
	 * 		- If Display list exist or is Valid Call Display List
	 * 		- If Display list doesn't exist try to create IT by calling
	 *        virtual function GLC_Geometry::glDraw
	 */
	virtual void glExecute(GLenum Mode, bool, bool forceWire= false);
	//GL_COMPILE_AND_EXECUTE
	//! OpenGL list creation
	virtual void createList(GLenum Mode);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
protected:

	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function have to be implemented in concrete class.*/
	virtual void glDraw(void) = 0;

	//! Virtual interface for OpenGL Geometry properties.
	/*! This Virtual function can be modify in concrete class.*/
	virtual void glPropGeom(bool, bool);
	
//@}

//////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////
protected:

	//! Display list ID
	GLuint m_ListID;

	//! Display list validity
	bool m_ListIsValid;

	//! Geometry validity
	bool m_GeometryIsValid;

	//! Material
	GLC_Material* m_pMaterial;

//////////////////////////////////////////////////////////////////////
// Private members 
//////////////////////////////////////////////////////////////////////
private:

	//! Thikness of geometry's Edge
	float m_Thikness;
		
	//! Geometry type is wire
	bool m_IsWire;
	
};
#endif //GLC_GEOMETRY_H_
