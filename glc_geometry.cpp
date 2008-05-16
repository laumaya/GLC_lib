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

//! \file glc_geometry.cpp Implementation of the GLC_Geometry class.
#include <QtDebug>

#include "glc_geometry.h"
#include "glc_openglexception.h"
#include "glc_selectionmaterial.h"

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////
GLC_Geometry::GLC_Geometry(const QString& name, const bool typeIsWire)
:GLC_Object(name)
, m_ListID(0)				// By default Display List = 0
, m_ListIsValid(false)		// By default Display List is invalid
, m_GeometryIsValid(false)	// By default geometry is invalid
, m_pMaterial(NULL)			// have to be set later in constructor
, m_Thikness(1.0)			// By default thickness = 1.0
, m_IsWire(typeIsWire)			// the geometry type
, m_IsTransparent(false)	// Not transparent by default

{
	// Material is set here
	setMaterial(new GLC_Material());	
}

GLC_Geometry::GLC_Geometry(const GLC_Geometry& sourceGeom)
:GLC_Object(sourceGeom)
, m_ListID(0)				// By default Display List = 0
, m_ListIsValid(false)		// By default Display List is invalid
, m_GeometryIsValid(false)	// By default geometry is invalid
, m_pMaterial(NULL)			// have to be set later in constructor
, m_Thikness(sourceGeom.m_Thikness)
, m_IsWire(sourceGeom.m_IsWire)
, m_IsTransparent(sourceGeom.m_IsTransparent)

{	
	m_Uid= GLC_GenID();
	// Material is copy here
	setMaterial(new GLC_Material());
	m_pMaterial->setMaterial(sourceGeom.getMaterial());
}

GLC_Geometry::~GLC_Geometry(void)
{
	// If display list is valid : delete it
	if (0 != m_ListID)
	{
		glDeleteLists(m_ListID, 1);
	}
	
	if (!!m_pMaterial)
	{
		m_pMaterial->delGLC_Geom(getID());	//Remove Geometry from the material usage collection
		if (m_pMaterial->isUnused()) delete m_pMaterial;
		m_pMaterial= NULL;
	}
}
/////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////


// Return an array of 4 GLfloat which represent the color
QColor GLC_Geometry::getRGBA(void) const
{
	return m_pMaterial->getDiffuseColor();
}	
// Return Color Red component
GLdouble GLC_Geometry::getdRed(void) const
{
	return static_cast<GLdouble>(m_pMaterial->getDiffuseColor().redF());
}
// Return Color Green component
GLdouble GLC_Geometry::getdGreen(void) const
{
	return static_cast<GLdouble>(m_pMaterial->getDiffuseColor().greenF());
}
// Return Color blue component
GLdouble GLC_Geometry::getdBlue(void) const
{
	return static_cast<GLdouble>(m_pMaterial->getDiffuseColor().blueF());
}
// Return Color Alpha component
GLdouble GLC_Geometry::getdAlpha(void) const
{
	return static_cast<GLdouble>(m_pMaterial->getDiffuseColor().alphaF());
}
// Return thickness
const float GLC_Geometry::getThickness(void) const
{
	return m_Thikness;
}
// Return associated OpenGL list ID
GLuint GLC_Geometry::getListID(void)
{
	return m_ListID;
}

// Return Validity of associated OpenGL list
bool GLC_Geometry::getListIsValid(void) const
{
	return m_ListIsValid;
}

// Return Validity of geometry
bool GLC_Geometry::getValidity(void) const
{
	return (m_GeometryIsValid && m_ListIsValid);
}

// Return material of geometry
GLC_Material* GLC_Geometry::getMaterial(void) const
{
	return m_pMaterial;
}

// return the geometry bounding box
GLC_BoundingBox* GLC_Geometry::getBoundingBox(void) const
{
	return NULL;	
}


/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////


// Set Diffuse Color RGBA component
void GLC_Geometry::setColor(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
	QColor setColor;
	setColor.setRgbF(red, green, blue, alpha);
	m_pMaterial->setDiffuseColor(setColor);
	m_GeometryIsValid= false;		
}

// Set Diffuse Color RGBA component with an QColor Object
void GLC_Geometry::setColor(const QColor& setCol)
{
	m_pMaterial->setDiffuseColor(setCol);
	m_GeometryIsValid= false;		
}


// Set Wire thickness
void GLC_Geometry::setThikness(float SetEp)
{
	m_Thikness= fabs(SetEp);

	m_GeometryIsValid= false;
}


// Material
void GLC_Geometry::setMaterial(GLC_Material* pMat)
{
	if (pMat != m_pMaterial)
	{
		if (pMat != NULL)
		{
			pMat->addGLC_Geom(this);
		}
	
		if (m_pMaterial != NULL)
		{
			m_pMaterial->delGLC_Geom(getID());
			if (m_pMaterial->isUnused()) delete m_pMaterial;
		}

		m_pMaterial= pMat;
		
		// Test if the material is transparent
		if (m_pMaterial->isTranparent())
		{
			setTransparency(true);
		}

		m_GeometryIsValid = false;
		m_ListIsValid= false;	// GLC_Mesh2 compatibility
		
	}
	else
	{	
		// Force update
		m_GeometryIsValid = false;
		m_ListIsValid= false;	// GLC_Mesh2 compatibility
	}
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Create and execute Geometry's display list
void GLC_Geometry::createList(GLenum Mode)
{
	if(!m_ListID)		// The list doesn't exist
	{
		m_ListID= glGenLists(1);
		Q_ASSERT(0 != m_ListID);
	}
	// List setting up
	glNewList(m_ListID, Mode);
		// Geometry set up and display
		glDraw();	// Virtual function defined in concrete class
	glEndList();

	// List is valid
	m_ListIsValid= true;
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Geometry::createList ", error);
		throw(OpenGlException);
	}
}

// if the geometry have a texture, load it
void GLC_Geometry::glLoadTexture(void)
{
	m_pMaterial->glLoadTexture();
}

// Geometry display
void GLC_Geometry::glExecute(GLenum Mode, bool isSelected, bool forceWire)
{
	// Define Geometry's property
	glPropGeom(isSelected, forceWire);

	// Geometry validity set to true
	m_GeometryIsValid= true;

	if (!m_ListIsValid)
	{
		// The list is not up to date or doesn't exist
	
		createList(Mode);
	}
	else
	{
		glCallList(m_ListID);
	}

	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Geometry::GlExecute ", error);
		throw(OpenGlException);
	}
}

//////////////////////////////////////////////////////////////////////
// Protected services functions
//////////////////////////////////////////////////////////////////////

// Virtual interface for OpenGL Geometry properties.
void GLC_Geometry::glPropGeom(bool isSelected, bool forceWire)
{
	
	if(m_IsWire || forceWire)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glLineWidth(getThickness());	// is thikness
		glPointSize(getThickness());
		
		if (isSelected) GLC_SelectionMaterial::glExecute();
		else glColor4d(getdRed(), getdGreen(), getdBlue(), getdAlpha());			// is color
	}
	else if (m_pMaterial->getAddRgbaTexture())
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		m_pMaterial->glExecute();
		if (isSelected) GLC_SelectionMaterial::glExecute();		
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		if (isSelected) GLC_SelectionMaterial::glExecute();
		else m_pMaterial->glExecute();
	}
		
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Geometry::GlPropGeom ", error);
		throw(OpenGlException);
	}
		
}






