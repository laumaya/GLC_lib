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
, m_IsSelected(false)		// By default geometry is not selected
, m_MatPos()				// default constructor identity matrix
, m_ListID(0)				// By default Display List = 0
, m_ListIsValid(false)		// By default Display List is invalid
, m_GeometryIsValid(false)	// By default geometry is invalid
, m_pMaterial(NULL)			// have to be set later in constructor
, m_IsBlended(false)		// By default No Blending
, m_PolyFace(GL_FRONT_AND_BACK)	// Default Faces style
, m_PolyMode(GL_FILL)		// Default polyganal mode
, m_Thikness(1.0)			// By default thickness = 1.0
, m_IsVisible(true)			// By default Visibility is true
, m_IsWire(typeIsWire)			// the geometry type

{
		// Material is set here
	setMaterial(new GLC_Material());	
}

GLC_Geometry::GLC_Geometry(const GLC_Geometry& sourceGeom)
:GLC_Object(sourceGeom)
, m_MatPos(sourceGeom.m_MatPos)
, m_ListID(0)				// By default Display List = 0
, m_ListIsValid(false)	// By default Display List is invalid
, m_GeometryIsValid(false)		// By default geometry is invalid
, m_pMaterial(NULL)			// have to be set later in constructor
, m_IsBlended(sourceGeom.m_IsBlended)
, m_PolyFace(sourceGeom.m_PolyFace)
, m_PolyMode(sourceGeom.m_PolyMode)
, m_Thikness(sourceGeom.m_Thikness)
, m_IsVisible(sourceGeom.m_IsVisible)
, m_IsWire(sourceGeom.m_IsWire)

{	
	// Material is set here
	setMaterial(sourceGeom.getMaterial());
}

GLC_Geometry::~GLC_Geometry(void)
{
	deleteList();
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

// Return Visibility state of geometry
const bool GLC_Geometry::isVisible(void) const
{
	return m_IsVisible;
}

// Return an array of 4 GLfloat which represent the color
QColor GLC_Geometry::getRGBA(void) const
{
	return m_pMaterial->getAmbientColor();
}	
// Return Color Red component
GLdouble GLC_Geometry::getdRed(void) const
{
	return static_cast<GLdouble>(m_pMaterial->getAmbientColor().redF());
}
// Return Color Green component
GLdouble GLC_Geometry::getdGreen(void) const
{
	return static_cast<GLdouble>(m_pMaterial->getAmbientColor().greenF());
}
// Return Color blue component
GLdouble GLC_Geometry::getdBlue(void) const
{
	return static_cast<GLdouble>(m_pMaterial->getAmbientColor().blueF());
}
// Return Color Alpha component
GLdouble GLC_Geometry::getdAlpha(void) const
{
	return static_cast<GLdouble>(m_pMaterial->getAmbientColor().alphaF());
}
// Return transfomation 4x4Matrix
const GLC_Matrix4x4 GLC_Geometry::getMatrix(void) const
{
	return m_MatPos;
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

// Return true if blending is enable
bool GLC_Geometry::getBlending(void) const
{
	return m_IsBlended;
}

// return the geometry bounding box
GLC_BoundingBox* GLC_Geometry::getBoundingBox(void) const
{
	return NULL;	
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Select the Geometry
void GLC_Geometry::select(void)
{
	m_IsSelected= true;
	m_GeometryIsValid = false;
	m_ListIsValid= false;	// GLC_Mesh2 compatibility
	
}

// Unselect the Geometry
void GLC_Geometry::unselect(void)
{
	m_IsSelected= false;
	m_GeometryIsValid = false;
	m_ListIsValid= false;	// GLC_Mesh2 compatibility
	
}

// Set visibility statement
void GLC_Geometry::setVisibility(bool v)
{
	m_IsVisible= v;

	m_GeometryIsValid= false;
}

// Set Color RGBA component
void GLC_Geometry::setRGBAColor(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
	QColor setColor;
	setColor.setRgbF(red, green, blue, alpha);
	m_pMaterial->setAmbientColor(setColor);
	m_GeometryIsValid= false;		
}

// Set Color RGBA component with an array of 4 GLfloat
void GLC_Geometry::setRGBAColor(const QColor& setCol)
{
	m_pMaterial->setAmbientColor(setCol);
	m_GeometryIsValid= false;		
}


// Geometry translation
void GLC_Geometry::translate(double Tx, double Ty, double Tz)
{
	GLC_Matrix4x4 MatTrans(Tx, Ty, Tz);
	
	multMatrix(MatTrans);
}


// move Geometry with a 4x4Matrix
void GLC_Geometry::multMatrix(const GLC_Matrix4x4 &MultMat)
{
	m_MatPos= MultMat * m_MatPos;

	m_GeometryIsValid= false;
}

// Replace the Geometry Matrix
void GLC_Geometry::setMatrix(const GLC_Matrix4x4 &SetMat)
{
	m_MatPos= SetMat;

	m_GeometryIsValid= false;
}

// Reset the Geometry Matrix
void GLC_Geometry::resetMatrix(void)
{
	m_MatPos.setToIdentity();

	m_GeometryIsValid= false;
}

// Set Wire thickness
void GLC_Geometry::setThikness(float SetEp)
{
	m_Thikness= fabs(SetEp);

	m_GeometryIsValid= false;
}

// Set Blending
void GLC_Geometry::setBlending(bool Blending)
{
	m_IsBlended= Blending;

	m_GeometryIsValid= false;
}

// Polygon's display style
void GLC_Geometry::setPolygonMode(GLenum Face, GLenum Mode)
{
	m_PolyFace= Face;
	m_PolyMode= Mode;
	m_GeometryIsValid = false;
}

// Material
void GLC_Geometry::setMaterial(GLC_Material* pMat)
{
	if (pMat != m_pMaterial)
	{
		if (!!pMat)
		{
			if (!pMat->addGLC_Geom(this))
			{
				return;
			}
		}
	
		if (!!m_pMaterial)
		{
			m_pMaterial->delGLC_Geom(getID());
			if (m_pMaterial->isUnused()) delete m_pMaterial;
		}

		m_pMaterial= pMat;

		m_GeometryIsValid = false;
		m_ListIsValid= false;	// GLC_Mesh2 compatibility
		
	}
	else
	{	
		// Force la mise � jour
		m_GeometryIsValid = false;
		m_ListIsValid= false;	// GLC_Mesh2 compatibility
	}
}
// Remove Geometry from the material without update material usage table
void GLC_Geometry::delMaterial(GLC_Geometry* pGeom)
{
	//! \todo modify this algo
	
	if (this == pGeom)
	{
		m_pMaterial= NULL;
	}
	else
	{
		qDebug("GLC_Geometrie::DelMatiere : Erreur GLC_Geometrie* not Match");
	}
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Create and execute Geometry's display list
bool GLC_Geometry::createList(GLenum Mode)
{
	if(!m_ListID)		// The list doesn't exist
	{
		m_ListID= glGenLists(1);

		if (!m_ListID)	// List ID not obtain
		{
			glDraw();
			qDebug("GLC_Geometry::createList ERROR Display list not created");
			return false;	// Display geometry whithout OpenGL display list
		}
	}
	// List setting up
	glNewList(m_ListID, Mode);
				
		// Geometrie set up and display
		glDraw();	// Virtual function defined in concrete class

	glEndList();

	
	// List is valid
	m_ListIsValid= true;
	return true;	// Display geometry with OpenGL display list
	
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
void GLC_Geometry::glExecute(GLenum Mode)
{
	if (isVisible())
	{
		// Object ID for selection purpose
		glLoadName(getID());

		// Save current OpenGL Matrix
		glPushMatrix();

		// Define Geometry's property
		glPropGeom();

		// Geometry validity set to true
		m_GeometryIsValid= true;

		if (!getListIsValid())
		{
			// The list is not up to date or doesn't exist

			createList(Mode);
		}
		else
		{
			glCallList(m_ListID);
		}

		// To avoid Blending issue
		glDepthMask(GL_TRUE);

		glPopMatrix();
		
		// OpenGL error handler
		GLenum error= glGetError();	
		if (error != GL_NO_ERROR)
		{
			GLC_OpenGlException OpenGlException("GLC_Geometry::GlExecute ", error);
			throw(OpenGlException);
		}
	}
	else
	{
		m_GeometryIsValid= true; // Used to avoid multi list creation
	}
}

//////////////////////////////////////////////////////////////////////
// Protected services functions
//////////////////////////////////////////////////////////////////////
// Delete OpenGL list
/* Call by GLC_Geometry::~GLC_Geometry*/
void GLC_Geometry::deleteList()
{
	// If display list is valid : delete it
	if (glIsList(m_ListID))
	{
		glDeleteLists(m_ListID, 1);
	}
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Geometry::DeleteList ", error);
		throw(OpenGlException);
	}

}

// Virtual interface for OpenGL Geometry properties.
void GLC_Geometry::glPropGeom()
{
	//! Change the current matrix
	glMultMatrixd(m_MatPos.return_dMat());
	
	// Polygons display mode
	glPolygonMode(m_PolyFace, m_PolyMode);
	if (m_IsBlended && !m_IsSelected)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
	}
	else
	{
		glDisable(GL_BLEND);
	}		
	
	if((m_PolyMode != GL_FILL) || m_IsWire)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glLineWidth(getThickness());	// is thikness
		glPointSize(getThickness());
		
		if (m_IsSelected) GLC_SelectionMaterial::glExecute();
		else glColor4d(getdRed(), getdGreen(), getdBlue(), getdAlpha());			// is color
	}
	else if (m_pMaterial->getAddRgbaTexture() && !m_IsSelected)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		m_pMaterial->glExecute();
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		if (m_IsSelected) GLC_SelectionMaterial::glExecute();
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






