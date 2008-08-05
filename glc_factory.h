/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.9, packaged on May, 2008.

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

#include <QObject>
#include <QtOpenGL>
#include <QString>

//class to built
#include "glc_point.h"
#include "glc_circle.h"
#include "glc_box.h"
#include "glc_cylinder.h"
#include "glc_mesh2.h"
#include "glc_material.h"
#include "glc_texture.h"
#include "glc_boundingbox.h"
#include "glc_instance.h"
#include "glc_world.h"

// end of class to built

//////////////////////////////////////////////////////////////////////
//! \class GLC_Factory
/*! \brief GLC_Factory : Factory for all geometrical objects
 *  this class is a singleton
 */
//////////////////////////////////////////////////////////////////////
class GLC_Factory : public QObject
{
	Q_OBJECT
	
public:
	//! Get unique instance of the factory
	static GLC_Factory* instance(const QGLContext *);
protected:	
	//! Constructor
	GLC_Factory(const QGLContext *);
public:
	//! Destructor
	~GLC_Factory();

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an GLC_Point
	GLC_Instance createPoint(const GLC_Vector4d &coord) const;
	GLC_Instance createPoint(double x, double y, double z) const;
		
	//!  Create an GLC_Circle
	GLC_Instance createCircle(double radius, double angle= 2 * glc::PI) const;
	
	//! Create an GLC_Box
	GLC_Instance createBox(double lx, double ly, double lz) const;
	GLC_Instance createBox(const GLC_BoundingBox& boundingBox) const;
	
	//! Create an GLC_Cylinder
	GLC_Instance createCylinder(double radius, double length) const;

	//! Create an GLC_Mesh with a QFile
	GLC_Instance createMesh(QFile &file) const;
	
	//! Create an GLC_World* with a QFile
	GLC_World* createWorld(QFile &file) const;	
	
	//! Create default material
	GLC_Material* createMaterial() const;
	//! create a material with an ambient color
	GLC_Material* createMaterial(const GLfloat *pAmbiantColor) const;
	//! create a material with an ambient color
	GLC_Material* createMaterial(const QColor &color) const;
	//! create an material textured with a texture
	GLC_Material* createMaterial(GLC_Texture* pTexture) const;
	//! create an material textured with a image file name
	GLC_Material* createMaterial(const QString &textureFullFileName) const;
	
	//! Create an GLC_Texture
	GLC_Texture* createTexture(const QString &textureFullFileName) const;
	
//@}

	signals:
	void currentQuantum(int);

//////////////////////////////////////////////////////////////////////
// Private members 
//////////////////////////////////////////////////////////////////////

private:
	//! The unique instance of the factory
	static GLC_Factory* m_pFactory;
	
	//! The QGLContext attached to the factory (rendering context)
	const QGLContext* m_pQGLContext;
			
};

#endif /*GLC_FACTORY_*/
