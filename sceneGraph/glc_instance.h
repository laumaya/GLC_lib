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

//! \file glc_instance.h interface for the GLC_Instance class.

#ifndef GLC_INSTANCE_H_
#define GLC_INSTANCE_H_

#include "../geometry/glc_vbogeom.h"
#include "../glc_enum.h"
#include "../glc_boundingbox.h"
#include "../glc_object.h"
#include "../maths/glc_matrix4x4.h"
#include "../glc_state.h"

class GLC_Viewport;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Instance
/*! \brief GLC_Instance : GLC_VboGeom + bounding box*/

/*! An GLC_Instance contain  :
 * 		- GLC_VboGeom pointer
 * 		- Geometry Bounding box
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
	GLC_Instance(GLC_VboGeom* pGeom);

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
	//! Return true if the all instance's mesh are transparent
	inline bool isTransparent() const
	{
		if (NULL == m_pGeomList) return false;
		const int size= m_pGeomList->size();
		bool result= true;
		int i= 0;
		while((i < size) and result)
		{
			result= result and m_pGeomList->at(i)->isTransparent();
			++i;
		}
		return result;
	}

	//! Return true if the instance contains mesh which contains transparent material
	inline bool hasTransparentMaterials() const
	{
		if (NULL == m_pGeomList) return false;
		const int size= m_pGeomList->size();
		bool result= false;
		int i= 0;
		while ((i < size) and not result)
		{
			result= result or m_pGeomList->at(i)->hasTransparentMaterials();
			++i;
		}
		return result;
	}

	//! Return true if the instance as no geometry
	inline const bool isEmpty() const {return (NULL == m_pGeomList);}

	//! Return true if the instance is selected
	inline const bool isSelected(void) const {return m_IsSelected;}

	//! Return the number of geometry
	inline int numberOfGeometry() const
	{
		if (NULL != m_pGeomList) return m_pGeomList->size();
		else return 0;
	}


	//! Get the geometry of the instance
	inline GLC_VboGeom* getGeometry(int index) const
	{
		if (NULL != m_pGeomList) return m_pGeomList->at(index);
		else return NULL;
	}

	//! Get the bounding box
	GLC_BoundingBox getBoundingBox(void);

	//! Get the validity of the Bounding Box
	const bool getBoundingBoxValidity(void) const;

	//! Return transfomation 4x4Matrix
	inline const GLC_Matrix4x4 getMatrix(void) const
	{return m_MatPos;}

	//! Clone the instance
	GLC_Instance clone() const;

	//! Instanciate the instance
	GLC_Instance instanciate();

	//! Get the Polygon mode off the instance
	/*! Polygon Mode can Be : GL_POINT, GL_LINE, or GL_FILL*/
	inline GLenum getPolygonMode() const {return m_PolyMode;}

	//! Get the visibility state of instance
	inline bool isVisible() const {return m_IsVisible;}

	//! Get the number of instance
	inline int getNumberOfInstance() const
	{return *m_pNumberOfInstance;}

	//! Return the GLC_uint decoded ID from RGB encoded ID
	static GLC_uint decodeRgbId(const GLubyte*);

	//! Get number of faces
	unsigned int numberOfFaces() const;

	//! Get number of vertex
	unsigned int numberOfVertex() const;

	//! Get number of materials
	unsigned int numberOfMaterials() const;

	//! Get materials List
	QSet<GLC_Material*> materialSet() const;


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
	bool setGeometry(GLC_VboGeom* pGeom);

	//! Remove empty geometries
	void removeEmptyGeometry();

	//! Reverse geometry normals
	void reverseGeometriesNormals();

	//! Translate Instance
	GLC_Instance& translate(double Tx, double Ty, double Tz);

	//! Translate Instance
	inline GLC_Instance& translate(const GLC_Vector4d& v)
	{
		return translate(v.X(), v.Y(), v.Z());
	}

	//! Move instance with a 4x4Matrix
	GLC_Instance& multMatrix(const GLC_Matrix4x4 &MultMat);

	//! Replace the instance Matrix
	GLC_Instance& setMatrix(const GLC_Matrix4x4 &SetMat);

	//! Reset the instance Matrix
	GLC_Instance& resetMatrix(void);

	//! Polygon's display style
	/*! Face Polygon Mode can be : GL_FRONT_AND_BACK, GL_FRONT, or GL_BACK
	 *  mode can be : GL_POINT, GL_LINE, or GL_FILL */
	void setPolygonMode(GLenum Face, GLenum Mode);

	//! Select the instance
	inline void select(void)
	{m_IsSelected= true;}

	//! Unselect the instance
	inline void unselect(void)
	{m_IsSelected= false;}

	//! Set instance visibility
	inline void setVisibility(const bool visibility)
	{m_IsVisible= visibility;}

	//! Set Instance Id
	inline void setId(const GLC_uint id)
	{
		GLC_Object::setId(id);
		encodeIdInRGBA();
	}


//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Display the instance
	inline void glExecute(bool transparent= false, bool useLoad= false, GLC_Viewport* pView= NULL)
	{
		if (NULL == m_pGeomList) return;
		// Save current OpenGL Matrix
		glPushMatrix();
		glVisProperties();
		if(GLC_State::isInSelectionMode())
		{
			glColor3ubv(m_colorId); // D'ont use Alpha component
		}
		const int size= m_pGeomList->size();
		if (useLoad and (NULL != pView))
		{
			for (int i= 0; i < size; ++i)
			{
				const int lodValue= choseLod(m_pGeomList->at(i)->boundingBox(), pView);
				if (lodValue <= 100)
				{
					m_pGeomList->at(i)->setCurrentLod(lodValue);
					m_pGeomList->at(i)->glExecute(m_IsSelected, transparent);
				}
			}
		}
		else
		{
			for (int i= 0; i < size; ++i)
			{
				const int lodValue= choseLod(m_pGeomList->at(i)->boundingBox(), pView);
				if (lodValue <= 100)
				{
					m_pGeomList->at(i)->setCurrentLod(10);
					m_pGeomList->at(i)->glExecute(m_IsSelected, transparent);
				}

			}
		}
		// Restore OpenGL Matrix
		glPopMatrix();
	}

private:
	//! Set instance visualisation properties
	inline void glVisProperties()
	{
		// Polygons display mode
		glPolygonMode(m_PolyFace, m_PolyMode);
		// Change the current matrix
		glMultMatrixd(m_MatPos.data());
	}


//@}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////
private:
	//! compute the instance bounding box
	void computeBoundingBox(void);

	//! Clear current instance
	void clear();

	//! Encode Id to RGBA color
	void encodeIdInRGBA();

	//! Compute LOD
	int choseLod(const GLC_BoundingBox&, GLC_Viewport*);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! Geometries of the instance
	QList<GLC_VboGeom*>* m_pGeomList;

	//! BoundingBox of the instance
	GLC_BoundingBox* m_pBoundingBox;

	//! Number of this instance
	int* m_pNumberOfInstance;

	//! Geometry matrix
	GLC_Matrix4x4 m_MatPos;

	//! Bounding box validity
	bool m_IsBoundingBoxValid;

	//! Selection state
	bool m_IsSelected;

	//! Polygons display style
	GLenum m_PolyFace;
	GLenum m_PolyMode;

	//! Visibility
	bool m_IsVisible;

	//! The instance color ID
	GLubyte m_colorId[4];

};


#endif /*GLC_INSTANCE_H_*/
