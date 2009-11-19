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

//! \file glc_geometry.h Interface for the GLC_Geometry class.

#ifndef GLC_GEOMETRY_H_
#define GLC_GEOMETRY_H_
#include "../shading/glc_material.h"
#include "../glc_boundingbox.h"

#include "glc_config.h"

typedef QHash<GLC_uint, GLC_Material*> MaterialHash;
typedef QHash<GLC_uint, GLC_uint> MaterialHashMap;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Geometry
/*! \brief GLC_Geometry : parent class for all geometry*/

/*! GLC_Geometry is a abstract class. \n \n
 *  Main attributes of GLC_Geometry:
 *		- Materials Hash table : 	QHash<GLC_Material*>
 *
 * GLC_Geometry provides :
 * 		- Method to draw Geometry                                                             : GLC_Geometry::glExecute()
 * 		- Virtual method to overload for visual property                                      : GLC_Geometry::glPropGeom()
 * 		- Virtual method to load and generate Opengl textures for each materials              : GLC_Geometry::glLoadTexture()
 *
 * 		- Pure virtual method to overload for Object topology                                 : GLC_Geometry::glDraw()
 * 		- Pure virtual Clone method                                                           : GLC_Geometry::clone()
 * 		- Pure virtual method to get geometry bounding box                                    : GLC_Geometry::boundingBox()
 *
 * 		- Empty virtual method for reversing normals                                          : GLC_Geometry::reverseNormals()
 *      - Empty virtual method for setting the current level of detail (between 0 and 100)    : GLC_Geometry::setCurrentLod()
 *      - Empty virtual method to get the number of vertex                                    : GLC_Geometry::numberOfVertex()
 *      - Empty virtual method to get the number of faces                                     : GLC_Geoetry::numberOfFaces()
 *
 */
//////////////////////////////////////////////////////////////////////

class GLC_LIB_EXPORT GLC_Geometry
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
	GLC_Geometry(const QString &, const bool);
	//! Copy constructor
	/*!
	 * const GLC_VboGeom geometry to copy
	 */
	GLC_Geometry(const GLC_Geometry&);
	//! Destructor
	virtual ~GLC_Geometry();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Get Object ID
	inline GLC_uint id() const {return m_Uid;}

	//! Get Object Name
	inline const QString name() const {return m_Name;}

	//! Return true if the geometry is valid
	inline bool isValid(void) const
	{return m_GeometryIsValid;}

	//! Return first material of geometry
	inline GLC_Material* firstMaterial(void) const
	{
		if (!m_MaterialHash.isEmpty())
		{
			return m_MaterialHash.begin().value();
		}
		else return NULL;
	}

	//! Return the number of materials
	inline unsigned int numberOfMaterials() const
	{return m_MaterialHash.size();}

	//! Return the specified mesh sub material
	inline GLC_Material* material(const GLC_uint key)
	{return m_MaterialHash[key];}

	//! Get materials Set
	inline QSet<GLC_Material*> materialSet() const
	{return m_MaterialHash.values().toSet();}

	//! Get materials ID List
	inline QList<GLC_uint> materialIds() const
	{return m_MaterialHash.keys();}

	//! Return true if Material key is in the mesh
	inline const bool containsMaterial(const GLC_uint key) const
	{return m_MaterialHash.contains(key);}

	//! Return the geometry bounding box
	virtual GLC_BoundingBox& boundingBox(void) = 0;

	//! Return true if the bounding box is valid
	inline bool boundingBoxIsValid() const {return NULL != m_pBoundingBox;}

	//! Clone the geometry
	virtual GLC_Geometry* clone() const = 0;

	//! Get the geometry transparency
	inline bool isTransparent() const
	{return (m_TransparentMaterialNumber == m_MaterialHash.size()) && hasTransparentMaterials();}

	//! Return true if the geometry contains transparent materials
	inline bool hasTransparentMaterials() const
	{return m_TransparentMaterialNumber > 0;}

	//! Return true if color per vertex is used
	inline bool usedColorPerVertex() const
	{return m_UseColorPerVertex;}

	//! Return true if the geometry type is wireframe
	inline bool typeIsWire() const
	{return m_IsWire;}

	//! Get the number of faces
	virtual unsigned int numberOfFaces() const;

	//! Get the number of vertex
	virtual unsigned int numberOfVertex() const;

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Replace the Master material
	//! The number of materials must be <= 1
	void replaceMasterMaterial(GLC_Material*);

	//! Add material to the geometry
	void addMaterial(GLC_Material *);

	//! Set the color per vertex usage
	inline void colorPerVertex(const bool colorPerVertex)
	{
		if (m_UseColorPerVertex != colorPerVertex)
		{
			m_UseColorPerVertex= colorPerVertex;
			m_GeometryIsValid= false;
		}
	}

	//! Update the transparent material number
	void updateTransparentMaterialNumber();

	//! Reverse normal
	virtual void reverseNormals() {}

	//! Set the lod Index
	/*! The value must be between 0 and 100*/
	virtual void setCurrentLod(const int) {}

	//! Set Geometry Id
	inline void setId(const GLC_uint id)
	{m_Uid= id;}

	//! Set geometry name
	inline void setName(const QString name)
	{m_Name= name;}

//@}
//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Load each textures of materials
	virtual void glLoadTexture(void);

	//! Virtual interface for OpenGL execution.
	virtual void glExecute(bool, bool transparent= false);


protected:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function have to be implemented in concrete class.*/
	virtual void glDraw(bool transparent= false) = 0;

	//! Virtual interface for OpenGL Geometry properties.
	virtual void glPropGeom(bool);

//@}
//////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////
protected:

	//! Geometry validity
	bool m_GeometryIsValid;

	//! Bounding box
	GLC_BoundingBox* m_pBoundingBox;

	//! Material Hash table
	MaterialHash m_MaterialHash;

	//! Color per vertex usage
	bool m_UseColorPerVertex;

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! Geometry type is wire
	bool m_IsWire;

	//! The number of transparent materials
	int m_TransparentMaterialNumber;

	//! The Unique id of an Geometry
	/*! Generated on creation*/
	GLC_uint m_Uid;

	//! Name of geometry
	QString m_Name;

};

#endif /*GLC_GEOMETRY_H_*/
