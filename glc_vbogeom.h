/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

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

//! \file glc_vbogeom.h Interface for the GLC_VboGeom class.

#ifndef GLC_VBOGEOM_H_
#define GLC_VBOGEOM_H_
#include <QColor>
#include "glc_object.h"
#include "glc_material.h"
#include "glc_boundingbox.h"
#include "glc_ext.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

//////////////////////////////////////////////////////////////////////
//! \class GLC_VboGeom
/*! \brief GLC_VboGeom : parent class for all GLC class which contain
 *  vbo geometrical data*/

/*! GLC_VboGeom is a abstract class. \n \n
 *  Main attributes of GLC_VboGeom:
 *		- Material : 	GLC_Material
 * 		- Graphic properties
 * 		- Transformation Matrix
 *
 * GLC_Geometry provide :
 * 		- Function to create VBO : GLC_VboGeom::createVbo
 * 		- Function to draw Geometry : GLC_VboGeom::glExecute
 * 		- Virtual function to overload for visual property: GLC_VboGeom::glPropGeom
 * 		- Virtual function to overload for Object topology: GLC_VboGeom::glDraw
 *
 */
//////////////////////////////////////////////////////////////////////

struct GLC_Vertex
{
	// Vertex
	float x, y, z;			// 12 Bytes
	// Normal
	float nx, ny, nz;		// 12 Bytes
	// Texture
	float s, t;				// 8 Bytes
	// Color
	GLfloat r, g, b, a;		// 16 Bytes
	// => 32 Bytes
};
class GLC_VboGeom : public GLC_Object
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	/*!
	 * QString Name
	 * const bool typeIsWire
	 */
	GLC_VboGeom(const QString &, const bool);
	//! Copy constructor
	/*!
	 * const GLC_VboGeom geometry to copy
	 */
	GLC_VboGeom(const GLC_VboGeom&);
	//! Destructor
	virtual ~GLC_VboGeom();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return a QColor which represent the color
	inline QColor color(void) const
	{return m_pMaterial->getDiffuseColor();}

	//! Return Color Red component
	inline GLfloat redF(void) const
	{return m_pMaterial->getDiffuseColor().redF();}

	//! Return Color Green component
	inline GLfloat greenF(void) const
	{return m_pMaterial->getDiffuseColor().greenF();}

	//! Return Color blue component
	inline GLfloat blueF(void) const
	{return m_pMaterial->getDiffuseColor().blueF();}

	//! Return Color Alpha component
	inline GLfloat alphaF(void) const
	{return m_pMaterial->getDiffuseColor().alphaF();}

	//! Return true if the geometry is valid
	inline bool isValid(void) const
	{return m_GeometryIsValid;}

	//! Return material of geometry
	inline GLC_Material* material(void) const
	{return m_pMaterial;}

	//! return the geometry bounding box
	virtual GLC_BoundingBox& boundingBox(void) = 0;

	//! Return true if the bounding box is valid
	inline bool boundingBoxIsValid() const {return NULL != m_pBoundingBox;}

	//! clone the geometry
	virtual GLC_VboGeom* clone() const = 0;

	//! Get the geometry transparency
	inline bool isTransparent() const
	{return m_IsTransparent;}

	//! return true if color per vertex is used
	inline bool usedColorPerVertex() const
	{return m_UseColorPerVertex;}

	//! Return true if the geometry type is wireframe
	inline bool typeIsWire() const
	{return m_IsWire;}

	//! Return true if the geometry have a texture
	inline bool addTexture() const
	{return m_pMaterial->getAddRgbaTexture();}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Set Diffuse Color RGBA component with an QColor Object
	inline void setColor(const QColor& setCol)
	{
		m_pMaterial->setDiffuseColor(setCol);
	}

	// Material
	//! Set the Geometry material
	void setMaterial(GLC_Material* pMat);

	//! Set the transparency property
	inline void setTransparency(const bool transparency)
	{m_IsTransparent= transparency;}

	inline void colorPerVertex(const bool colorPerVertex)
	{
		if (m_UseColorPerVertex != colorPerVertex)
		{
			m_UseColorPerVertex= colorPerVertex;
			m_GeometryIsValid= false;
		}
	}
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
	 * GLC_VboGeom::glPropGeom and set up :
	 * 		- Geometry
	 * 		- VBO
	 * AfterWard this function
	 *		- Call virtual function GLC_VboGeom::glPropGeom
	 *        virtual function GLC_Geometry::glDraw
	 */
	virtual void glExecute(bool, bool forceWire= false);


protected:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function have to be implemented in concrete class.*/
	virtual void glDraw(void) = 0;

	//! Virtual interface for OpenGL Geometry properties.
	/*! This Virtual function can be modify in concrete class.
	 * bool IsSelected
	 * bool ForceWire
	 */
	virtual void glPropGeom(bool, bool);

	//! Vbo creation
	void createVBOs();
//@}
//////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////
protected:

	//! VBO ID
	GLuint m_VboId;

	//! IBO ID
	GLuint m_IboId;

	//! Geometry validity
	bool m_GeometryIsValid;

	//! Bounding box
	GLC_BoundingBox* m_pBoundingBox;

	//! Material
	GLC_Material* m_pMaterial;

	//! Color per vertex usage
	bool m_UseColorPerVertex;

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Geometry type is wire
	bool m_IsWire;

	//! Transparency
	bool m_IsTransparent;
};

#endif /*GLC_VBOGEOM_H_*/
