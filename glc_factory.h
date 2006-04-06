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

// Declartion of class to built
class GLC_Point;
class GLC_Circle;
class GLC_Box;
class GLC_Cylinder;
class GLC_Mesh2;
class GLC_Material;
class GLC_Texture;
// end of declaration of class to built

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
	GLC_Point* createPoint(const GLC_Vector3d coord) const;
	GLC_Point* createPoint(double x, double y, double z) const;
		
	//!  Create an GLC_Circle
	GLC_Circle* createCircle(double radius, double angle= 2 * PI) const;
	
	//! Create an GLC_Box
	GLC_Box* createBox(double lx, double ly, double lz) const;
	
	//! Create an GLC_Cylinder
	GLC_Cylinder* createCylinder(double radius, double length) const;
	
	//! Create an GLC_Mesh
	GLC_Mesh2* createMesh(std::string fullfileName) const;
	
	//! Create an GLC_Material
	GLC_Material* createMaterial(const GLfloat *pAmbiantColor=NULL) const;
	GLC_Material* createMaterial(const QColor *pColor) const;
	GLC_Material* createMaterial(const GLC_Texture* pTexture) const;
	GLC_Material* createMaterial(std::string textureFullFileName) const;
	
	//! Create an GLC_Texture
	GLC_Texture* createTexture(std::string textureFullFileName) const;
	
//@}

//////////////////////////////////////////////////////////////////////
// Private members 
//////////////////////////////////////////////////////////////////////

private:
	//! The unique instance of the factory
	static GLC_Factory* m_pFactory;
	
	//! The QGLWidget attached to the factory (rendering context)
	QGLWidget* m_pQGLWidget;
	
	//! Class which can read a Mesh from File System
	GLC_MeshLoader* m_pMeshLoader;
		
}

#endif /*GLC_FACTORY_*/
