/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.8, packaged on January, 2008.

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

//! \file glc_factory.cpp implementation of the GLC_Factory class.

#include "glc_factory.h"
#include "glc_objtomesh2.h"
#include "glc_objtoworld.h"

// init static member
GLC_Factory* GLC_Factory::m_pFactory= NULL;

//////////////////////////////////////////////////////////////////////
// static method
//////////////////////////////////////////////////////////////////////
// Return the unique instance of the factory
GLC_Factory* GLC_Factory::instance(const QGLContext *pContext)
{
	if(m_pFactory == NULL)
	{
		m_pFactory= new GLC_Factory(pContext);
	}
	return m_pFactory;
}

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////

// Protected constructor
GLC_Factory::GLC_Factory(const QGLContext *pContext)
: m_pQGLContext(pContext)
{
	
}

// Destructor
GLC_Factory::~GLC_Factory()
{
	m_pFactory= NULL;
}

//////////////////////////////////////////////////////////////////////
// Create functions
//////////////////////////////////////////////////////////////////////

// Create an GLC_Point
GLC_Instance GLC_Factory::createPoint(const GLC_Vector4d &coord) const
{
	GLC_Instance newPoint(new GLC_Point(coord));
	return newPoint;
}
// Create an GLC_Point
GLC_Instance GLC_Factory::createPoint(double x, double y, double z) const
{
	GLC_Instance newPoint(new GLC_Point(x, y, z));
	return newPoint;
}

//  Create an GLC_Circle
GLC_Instance GLC_Factory::createCircle(double radius, double angle) const
{
	GLC_Instance newCircle(new GLC_Circle(radius, angle));
	return newCircle;
}
// Create an GLC_Box
GLC_Instance GLC_Factory::createBox(double lx, double ly, double lz) const
{
	GLC_Instance newBox(new GLC_Box(lx, ly, lz));
	return newBox;
}

// Create an GLC_Box
GLC_Instance GLC_Factory::createBox(const GLC_BoundingBox& boundingBox) const
{
	const double lx= boundingBox.getUpper().getX() - boundingBox.getLower().getX();
	const double ly= boundingBox.getUpper().getY() - boundingBox.getLower().getY();
	const double lz= boundingBox.getUpper().getZ() - boundingBox.getLower().getZ();
	GLC_Box* pBox= new GLC_Box(lx, ly, lz);
	GLC_Instance newBox(pBox);
	newBox.translate(boundingBox.getCenter().getX(), boundingBox.getCenter().getY()
					, boundingBox.getCenter().getZ());
	return newBox;
}

// Create an GLC_Cylinder
GLC_Instance GLC_Factory::createCylinder(double radius, double length) const
{
	GLC_Instance newCylinder(new GLC_Cylinder(radius, length));
	return newCylinder;
}
// Create an GLC_Mesh with a QFile
GLC_Instance GLC_Factory::createMesh(QFile &file) const
{
	GLC_ObjToMesh2 objToMesh(m_pQGLContext);
	connect(&objToMesh, SIGNAL(currentQuantum(int)), this, SIGNAL(currentQuantum(int)));
	GLC_Instance newMesh(objToMesh.CreateMeshFromObj(file));
	return newMesh;
}

// Create an GLC_World* with a QFile
GLC_World* GLC_Factory::createWorld(QFile &file) const
{
	GLC_ObjToWorld objToWorld(m_pQGLContext);
	connect(&objToWorld, SIGNAL(currentQuantum(int)), this, SIGNAL(currentQuantum(int)));
	return objToWorld.CreateWorldFromObj(file);
}

// Create an GLC_Material
GLC_Material* GLC_Factory::createMaterial() const
{
	return new GLC_Material();
}

// Create an GLC_Material
GLC_Material* GLC_Factory::createMaterial(const GLfloat *pAmbiantColor) const
{
	return new GLC_Material("Material", pAmbiantColor);
}
// Create an GLC_Material
GLC_Material* GLC_Factory::createMaterial(const QColor &color) const
{
	return new GLC_Material(color);
}

GLC_Material* GLC_Factory::createMaterial(GLC_Texture* pTexture) const
{
	return new GLC_Material(pTexture, "TextureMaterial");
}
// create an material textured with a image file name
GLC_Material* GLC_Factory::createMaterial(const QString &textureFullFileName) const
{
	GLC_Texture* pTexture= createTexture(textureFullFileName);
	return createMaterial(pTexture);	
}

// Create an GLC_Texture

GLC_Texture* GLC_Factory::createTexture(const QString &textureFullFileName) const
{
	return new GLC_Texture(m_pQGLContext, textureFullFileName);
}
