/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.4, packaged on January, 2006.

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
	/*! By default, Ambiant Color is dark grey*/
	GLC_Material(const char *pName= "Mat", const GLfloat *pAmbiantColor = NULL);
	
	//! Construct textured GLC_Material
	GLC_Material(GLC_Texture* pTexture, const char *pName= "TextureMat");
	
	//! Copy constructor
	GLC_Material(const GLC_Material &InitMaterial);
	
	//! Remove material where used geometry
	virtual ~GLC_Material(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Return true is material add attached texture
	bool GetAddRgbaTexture()
	{
		return m_pTexture != NULL;
	}
	//! Get Ambiant color
	void GetAmbientColor(GLfloat* pfCol) const;	// pfCol[4]

	//! Get diffuse color
	void GetDiffuseColor(GLfloat* pfCol) const;	// pfCol[4]

	//! Get specular color
	void GetSpecularColor(GLfloat* pfCol) const;	// pfCol[4]

	//! Get the emissive color
	void GetLightEmission(GLfloat* pfCol) const;	// pfCol[4]

	//! Get Shininess
	GLfloat GetShininess() const
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
	void SetAmbientColor(const GLfloat* pfCol);

	//! Set Diffuse color
	void SetDiffuseColor(const GLfloat* pfCol);
	
	//! Set Specular color
	void SetSpecularColor(const GLfloat* pfCol);

	//! Set Emissive
	void SetLightEmission(const GLfloat* pfCol);

	//! Set Shininess
	void SetShininess(GLfloat Shininess)
	{
		m_fShininess= Shininess;

		UpdateUsed();
	}

	//! Set Texture
	void SetTexture(GLC_Texture* pTexture)
	{
		qDebug() << "GLC_Material::SetTexture";
		m_pTexture= pTexture;
		
		UpdateUsed();
	}


	//! Add Geometry to the "where used" hash table
	bool AddGLC_Geom(GLC_Geometry* pGeom);

	//! Remove Geometry to the "where used" hash table
	bool DelGLC_Geom(GLC_uint Key);
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Execute OpenGL Material
	virtual bool GlExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);

//@}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
private:
	//! Update geometries which used material
	void UpdateUsed(void);


//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////

private:

	//! Ambiant Color
	GLfloat m_AmbientColor[4];
	//! Diffuse Color
	GLfloat m_DiffuseColor[4];
	//! Specular Color
	GLfloat m_SpecularColor[4];

	//! emmisive lighting
	GLfloat m_LightEmission[4];

	//! Shiness
	GLfloat m_fShininess;

	//! Where Used Hash table
	CWhereUsed m_WhereUsed;

	//! material's texture
	GLC_Texture* m_pTexture;

};

#endif //GLC_MATERIAL_H_
