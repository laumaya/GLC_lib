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

//! \file glc_factory.h Interface for the GLC_Factory class.

#ifndef GLC_FACTORY_
#define GLC_FACTORY_

#include <QObject>
#include <QtOpenGL>
#include <QString>

//class to built
#include "geometry/glc_point.h"
#include "geometry/glc_circle.h"
#include "geometry/glc_box.h"
#include "geometry/glc_cylinder.h"
#include "geometry/glc_mesh2.h"
#include "shading/glc_material.h"
#include "shading/glc_texture.h"
#include "sceneGraph/glc_instance.h"
#include "sceneGraph/glc_world.h"
#include "glc_boundingbox.h"

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
	//! Get unique instance of the factory
	inline static GLC_Factory* instance()
	{return m_pFactory;}

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

	//! Create an GLC_World* with a QFile
	GLC_World* createWorld(QFile &file, QStringList* pAttachedFileName= NULL) const;

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
	//! For progress bar management
	void currentQuantum(int) const;

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
