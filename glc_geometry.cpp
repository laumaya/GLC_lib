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

//! \file glc_geometry.cpp Implementation of the GLC_Geometry class.
#include <QtDebug>

#include "glc_geometry.h"

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////
GLC_Geometry::GLC_Geometry(const char *pName, const GLfloat *pColor)
:GLC_Object(pName)
, m_ListID(0)				// By default Display List = 0
, m_ListIsValid(false)	// By default Display List is invalid
, m_GeometryIsValid(false)		// By default geometry is invalid
, m_pMaterial(NULL)			// By default No material
, m_IsBlended(false)		// By default No Blending
, m_PolyFace(GL_FRONT_AND_BACK)	// Default Faces style
, m_PolyMode(GL_FILL)			// Default polyganal mode
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

GLC_Geometry::~GLC_Geometry(void)
{
	DeleteList();
	if (!!m_pMaterial)
	{
		m_pMaterial->DelGLC_Geom(GetID());	//Remove Geometry from the material usage collection
		m_pMaterial= NULL;
	}
}
/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Geometry translation
void GLC_Geometry::Translate(double Tx, double Ty, double Tz)
{
	GLC_Matrix4x4 MatTrans(Tx, Ty, Tz);
	
	MultMatrix(MatTrans);
}

// Material
void GLC_Geometry::SetMaterial(GLC_Material* pMat)
{
	if (pMat != m_pMaterial)
	{
		if (!!pMat)
		{
			if (!pMat->AddGLC_Geom(this))
			{
				return;
			}
		}
	
		if (!!m_pMaterial)
		{
			m_pMaterial->DelGLC_Geom(GetID());
		}

		m_pMaterial= pMat;

		pMat->GetDiffuseColor(m_RGBAColor);

		m_GeometryIsValid = false;
	}
	else
	{	
		// Force la mise à jour
		m_GeometryIsValid = false;
	}
}
// Remove Geometry from the material without update material usage table
void GLC_Geometry::DelMaterial(GLC_Geometry* pGeom)
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
bool GLC_Geometry::CreateList(GLenum Mode)
{
	if(!m_ListID)		// The list doesn't exist
	{
		m_ListID= glGenLists(1);

		if (!m_ListID)	// List ID not obtain
		{
			GlDraw();
			qDebug("GLC_Geometrie::ERREUR Liste d'affichage NON créé");
			return false;	// Display geometry whithout OpenGL display list
		}
	}
	// List setting up
	glNewList(m_ListID, Mode);
				
		// Geometrie set up and display
		GlDraw();	// Virtual function defined in concrete class

	glEndList();

	
	// List is valid
	m_ListIsValid= true;
	return true;	// Display geometry with OpenGL display list
	
	//! \todo Add error handler
}
// Geometry display
bool GLC_Geometry::GlExecute(GLenum Mode)
{
	if (GetIsVisible())
	{
		// Object ID for selection purpose
		glLoadName(GetID());

		// Save current OpenGL Matrix
		glPushMatrix();

		// Define Geometry's property
		GlPropGeom();	// Virtual function defined in concrete class

		// Geometry validity set to true
		m_GeometryIsValid= true;

		if (!GetListIsValid())
		{
			// The list is not up to date or doesn't exist

			CreateList(Mode);
		}
		else
		{
			glCallList(m_ListID);
		}

		// To avoid Blending issue
		glDepthMask(GL_TRUE);

		glPopMatrix();
		
		//! \todo Add Error handler
		return true;
	}
	else return true;
}
