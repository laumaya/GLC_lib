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

//! \file glc_material.h interface for the GLC_Material class.

#ifndef GLC_MATERIAL_H_
#define GLC_MATERIAL_H_


#include "glc_object.h"
#include "glc_texture.h"
#include <QHash>
#include <QColor>

class GLC_Geometry;

typedef QHash< GLC_uint, GLC_Geometry*> CWhereUsed;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Material
/*! \brief GLC_Material : OpenGL surface material properties */

/*! An GLC_Material specifies surface material properties */
//////////////////////////////////////////////////////////////////////


class GLC_Material :
	public GLC_Object
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Construct Colored GLC_Material
	//! Default constructor
	GLC_Material();
	
	/*! By default, Ambiant Color is dark grey*/
	GLC_Material(const QColor &ambientColor);

	/*! By default, Ambiant Color is dark grey*/
	GLC_Material(const char *pName, const GLfloat *pAmbientColor);
	
	//! Construct textured GLC_Material
	GLC_Material(GLC_Texture* pTexture, const char *pName);
	
	//! Copy constructor
	GLC_Material(const GLC_Material &InitMaterial);
	
	//! Remove material where used geometry
	virtual ~GLC_Material(void);
	
	//! Return true if the material is used
	bool isUnused() const {return m_WhereUsed.isEmpty();}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Return true is material add attached texture
	bool getAddRgbaTexture()
	{
		return m_pTexture != NULL;
	}
	//! Get Ambiant color
	QColor getAmbientColor() const;

	//! Get diffuse color
	QColor getDiffuseColor() const;

	//! Get specular color
	QColor getSpecularColor() const;

	//! Get the emissive color
	QColor getLightEmission() const;

	//! Get Shininess
	GLfloat getShininess() const
	{
		return m_fShininess;
	}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Set Ambiant Color
	void setAmbientColor(const QColor& ambientColor);

	//! Set Diffuse color
	void setDiffuseColor(const QColor& diffuseColor);
	
	//! Set Specular color
	void setSpecularColor(const QColor& specularColor);

	//! Set Emissive
	void setLightEmission(const QColor& lightEmission);

	//! Set Shininess
	void setShininess(GLfloat Shininess)
	{
		m_fShininess= Shininess;

		updateUsed();
	}

	//! Set Texture
	void setTexture(GLC_Texture* pTexture)
	{
		qDebug() << "GLC_Material::SetTexture";
		m_pTexture= pTexture;
		
		updateUsed();
	}


	//! Add Geometry to the "where used" hash table
	bool addGLC_Geom(GLC_Geometry* pGeom);

	//! Remove Geometry to the "where used" hash table
	bool delGLC_Geom(GLC_uint Key);
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Execute OpenGL Material
	virtual void glExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);

//@}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
private:
	//! Update geometries which used material
	void updateUsed(void);
	
	//! Init Ambiant Color
	void initAmbientColor(void);
	
	//! Init other color
	void initOtherColor(void);


//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////

private:

	//! Ambiant Color
	QColor m_AmbientColor;
	//! Diffuse Color
	QColor m_DiffuseColor;
	//! Specular Color
	QColor m_SpecularColor;

	//! emmisive lighting
	QColor m_LightEmission;

	//! Shiness
	GLfloat m_fShininess;

	//! Where Used Hash table
	CWhereUsed m_WhereUsed;

	//! material's texture
	GLC_Texture* m_pTexture;

};

#endif //GLC_MATERIAL_H_
