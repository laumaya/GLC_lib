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

//! \file glc_renderproperties.h interface for the GLC_RenderProperties class.

#ifndef GLC_RENDERPROPERTIES_H_
#define GLC_RENDERPROPERTIES_H_

#include "glc_material.h"
#include "../glc_enum.h"

#include <QList>
#include <QHash>
#include <QtOpenGL>

#include "../glc_config.h"

//! Define render mode enum in glc namespace
namespace glc
{
	//! Geometry rendering mode enumeration
	enum RenderMode
	{
		NonTransparentMaterial,
		TransparentMaterial,
		OverwriteMaterial,
		OverwriteTransparency,
		PrimitiveSelected,
		OverwritePrimitiveMaterial,
		BodySelection,
		PrimitiveSelection
	};
};
//////////////////////////////////////////////////////////////////////
//! \class GLC_RenderProperties
/*! \brief GLC_RenderProperties encapsulate the render properties
 * off all GLC_Geom class*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_RenderProperties
{

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_RenderProperties();

	//! Copy constructor
	GLC_RenderProperties(const GLC_RenderProperties&);

	//! Assignement operator
	GLC_RenderProperties &operator=(const GLC_RenderProperties&);

	//! Destructor
	virtual ~GLC_RenderProperties();
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return true if it is selected
	inline const bool isSelected() const
	{return m_IsSelected;}

	//! Return the rendering mode
	inline glc::RenderMode renderingMode() const
	{return m_RenderMode;}

	//! Return an handle to the overwrite material
	inline GLC_Material* overwriteMaterial() const
	{return m_pOverwriteMaterial;}

	//! Return an handle to the list of selected primitives id
	inline QList<GLC_uint>* listOfSelectedPrimitivesId() const
	{return m_pSelectedPrimitvesId;}

	//! Return an handle to the overwrite primitive material Hash
	inline QHash<GLC_uint, GLC_Material*>* hashOfOverwritePrimitiveMaterials() const
	{return m_pOverwritePrimitiveMaterialMap;}

	//! Get the PolyFace mode
	/*! PolyFace Mode can Be : GL_FRONT_AND_BACK, GL_FRONT, or GL_BACK*/
	inline GLenum polyFaceMode() const
	{return m_PolyFace;}

	//! Get the Polygon mode
	/*! Polygon Mode can Be : GL_POINT, GL_LINE, or GL_FILL*/
	inline GLenum polygonMode() const
	{return m_PolyMode;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Clear the content of the render properties and update materials usage
	void clear();
//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Select the instance
	inline void select(void)
	{m_IsSelected= true;}

	//! Unselect the instance
	inline void unselect(void)
	{m_IsSelected= false;}

	//! Set the rendering mode
	inline void setRenderingMode(glc::RenderMode mode)
	{m_RenderMode= mode;}

	//! Set the overwrite material
	void setOverwriteMaterial(GLC_Material*);

	//! Set the list of selected primitives id
	void setlistOfSelectedPrimitivesId(const QList<GLC_uint>&);

	//! Set the overwrite primitive material Hash
	void setOfOverwritePrimitiveMaterials(const QHash<GLC_uint, GLC_Material*>&);

	//! Polygon's display style
	/*! Face Polygon Mode can be : GL_FRONT_AND_BACK, GL_FRONT, or GL_BACK
	 *  mode can be : GL_POINT, GL_LINE, or GL_FILL */
	inline void setPolygonMode(GLenum Face, GLenum Mode)
	{
		m_PolyFace= Face;
		m_PolyMode= Mode;
	}


//@}

//////////////////////////////////////////////////////////////////////
//Private attributes
//////////////////////////////////////////////////////////////////////
private:
	//! The render properties uid : GLC_GenUserID (For GLC_Material usage)
	GLC_uint m_Uid;

	//! Flag to know if it is selected
	bool m_IsSelected;

	//! OpenGL polygon rendering mode
	GLenum m_PolyFace;
	GLenum m_PolyMode;

	//! Geometry rendering mode
	glc::RenderMode m_RenderMode;

	//! The overwrite material
	GLC_Material* m_pOverwriteMaterial;

	//! The overwrite transparency
	float m_OverwriteTransparency;

	//! The selected primitive id
	QList<GLC_uint>* m_pSelectedPrimitvesId;

	//! The overwrite primitive material mapping
	QHash<GLC_uint, GLC_Material*>* m_pOverwritePrimitiveMaterialMap;

};

#endif /* GLC_RENDERPROPERTIES_H_ */
