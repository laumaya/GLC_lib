/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.5, packaged on February, 2006.

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

//! \file glc_factory.h Interface for the GLC_Factory class.

#ifndef GLC_FACTORY_
#define GLC_FACTORY_

#include <string>
#include <QtOpenGL>
#include <QString>
#include "glc_maths.h"

//class to built
#include "glc_point.h"
#include "glc_circle.h"
#include "glc_box.h"
#include "glc_cylinder.h"
#include "glc_mesh2.h"
#include "glc_material.h"
#include "glc_texture.h"
#include "glc_boundingbox.h"

// end of class to built

//////////////////////////////////////////////////////////////////////
//! \class GLC_Factory
/*! \brief GLC_Factory : Factory for all geometrical objects
 *  this class is a singleton
 */
//////////////////////////////////////////////////////////////////////
class GLC_Factory
{
public:
	//! Get unique instance of the factory
	static GLC_Factory* instance(QGLWidget *GLWidget);
protected:	
	//! Constructor
	GLC_Factory(QGLWidget *GLWidget);
public:
	//! Destructor
	~GLC_Factory();

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an GLC_Point
	GLC_Point* createPoint(const GLC_Vector4d &coord) const;
	GLC_Point* createPoint(double x, double y, double z) const;
		
	//!  Create an GLC_Circle
	GLC_Circle* createCircle(double radius, double angle= 2 * PI) const;
	
	//! Create an GLC_Box
	GLC_Box* createBox(double lx, double ly, double lz) const;
	GLC_Box* createBox(const GLC_BoundingBox& boundingBox) const;
	
	//! Create an GLC_Cylinder
	GLC_Cylinder* createCylinder(double radius, double length) const;
	
	//! Create an GLC_Mesh
	GLC_Mesh2* createMesh(const std::string &fullfileName);
	//! Create an GLC_Mesh by copying another mesh
	GLC_Mesh2* createMesh(const GLC_Geometry* pMesh);
	
	//! Create default material
	GLC_Material* createMaterial();
	//! create a material with an ambient color
	GLC_Material* createMaterial(const GLfloat *pAmbiantColor);
	//! create a material with an ambient color
	GLC_Material* createMaterial(const QColor &color);
	//! create an material textured with a texture
	GLC_Material* createMaterial(GLC_Texture* pTexture);
	//! create an material textured with a image file name
	GLC_Material* createMaterial(const QString &textureFullFileName);
	
	//! Create an GLC_Texture
	GLC_Texture* createTexture(const QString &textureFullFileName);
	
//@}

//////////////////////////////////////////////////////////////////////
// Private members 
//////////////////////////////////////////////////////////////////////

private:
	//! The unique instance of the factory
	static GLC_Factory* m_pFactory;
	
	//! The QGLWidget attached to the factory (rendering context)
	QGLWidget* m_pQGLWidget;
			
};

#endif /*GLC_FACTORY_*/
