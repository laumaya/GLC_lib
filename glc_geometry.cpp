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

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////
GLC_Geometry::GLC_Geometry(const char *pName, const GLfloat *pColor)
:GLC_Object(pName)
, m_MatPos()				// default constructor identity matrix
, m_ListID(0)				// By default Display List = 0
, m_ListIsValid(false)		// By default Display List is invalid
, m_GeometryIsValid(false)	// By default geometry is invalid
, m_pMaterial(NULL)			// By default No material
, m_IsBlended(false)		// By default No Blending
, m_PolyFace(GL_FRONT_AND_BACK)	// Default Faces style
, m_PolyMode(GL_FILL)		// Default polyganal mode
, m_Thikness(1.0)			// By default thickness = 1.0
, m_IsVisible(true)			// By default Visibility is true

{

	// Set Color Array
	if (pColor != 0)
	{
		m_RGBAColor[0]= pColor[0];
		m_RGBAColor[1]= pColor[1];
		m_RGBAColor[2]= pColor[2];
		m_RGBAColor[3]= pColor[3];
	}
	else
	{	// By default color is white
		m_RGBAColor[0]= 1;
		m_RGBAColor[1]= 1;
		m_RGBAColor[2]= 1;
		m_RGBAColor[3]= 1;
	}

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

{
	m_RGBAColor[0]= sourceGeom.m_RGBAColor[0];
	m_RGBAColor[1]= sourceGeom.m_RGBAColor[1];
	m_RGBAColor[2]= sourceGeom.m_RGBAColor[2];
	m_RGBAColor[3]= sourceGeom.m_RGBAColor[3];
	
	// Material is set here
	setMaterial(sourceGeom.getMaterial());
}

GLC_Geometry::~GLC_Geometry(void)
{
	deleteList();
	if (!!m_pMaterial)
	{
		m_pMaterial->delGLC_Geom(getID());	//Remove Geometry from the material usage collection
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
const GLfloat *GLC_Geometry::getfRGBA(void) const
{
	return m_RGBAColor;
}	
// Return Color Red component
GLfloat GLC_Geometry::getfRed(void) const
{
	return m_RGBAColor[0];
}
// Return Color Green component
GLfloat GLC_Geometry::getfGreen(void) const
{
	return m_RGBAColor[1];
}
// Return Color blue component
GLfloat GLC_Geometry::getfBlue(void) const
{
	return m_RGBAColor[2];
}
// Return Color Alpha component
GLfloat GLC_Geometry::getfAlpha(void) const
{
	return m_RGBAColor[3];
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

// Set visibility statement
void GLC_Geometry::setVisibility(bool v)
{
	m_IsVisible= v;

	m_GeometryIsValid= false;
}

// Set Color RGBA component
void GLC_Geometry::setRGBAColor(GLfloat Rouge, GLfloat Vert, GLfloat Bleu, GLfloat Alpha)
{
	m_RGBAColor[0]= Rouge;
	m_RGBAColor[1]= Vert;
	m_RGBAColor[2]= Bleu;
	m_RGBAColor[3]= Alpha;

	m_GeometryIsValid= false;		
}

// Set Color RGBA component with an array of 4 GLfloat
void GLC_Geometry::setRGBAColor(const GLfloat* SetCol)	// SetCol[4]
{
	m_RGBAColor[0]= SetCol[0];
	m_RGBAColor[1]= SetCol[1];
	m_RGBAColor[2]= SetCol[2];
	m_RGBAColor[3]= SetCol[3];

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
	SetEp= fabs(SetEp);		
	m_Thikness= SetEp;

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
		}

		m_pMaterial= pMat;

		pMat->getDiffuseColor(m_RGBAColor);

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
		glPropGeom();	// Virtual function defined in concrete class

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






