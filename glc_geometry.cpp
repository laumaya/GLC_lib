/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9, packaged on Novemeber, 2005.

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
, m_ListeID(0)				// By default Display List = 0
, m_bListeIsValid(false)	// By default Display List is invalid
, m_bValidite(false)		// By default geometry is invalid
, m_pMatiere(NULL)			// By default No material
, m_bBlending(false)		// By default No Blending
, m_Epaisseur(1.0)			// By default thickness = 1.0
, m_bVisible(true)			// By default Visibility is true

{

	// Set Color Array
	if (pColor != 0)
	{
		m_fColor[0]= pColor[0];
		m_fColor[1]= pColor[1];
		m_fColor[2]= pColor[2];
		m_fColor[3]= pColor[3];
	}
	else
	{	// By default color is white
		m_fColor[0]= 1;
		m_fColor[1]= 1;
		m_fColor[2]= 1;
		m_fColor[3]= 1;
	}

}

GLC_Geometry::~GLC_Geometry(void)
{
	DeleteListe();
	if (!!m_pMatiere)
	{
		m_pMatiere->DelGLC_Geom(GetID());	//Remove Geometry from the material usage collection
		m_pMatiere= NULL;
	}
}
/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Geometry translation
void GLC_Geometry::Translate(double Tx, double Ty, double Tz)
{
	GLC_Matrix4x4 MatTrans(Tx, Ty, Tz);
	
	MultMatrice(MatTrans);
}

// Material
void GLC_Geometry::SetMatiere(GLC_Material* pMat)
{
	if (pMat != m_pMatiere)
	{
		if (!!pMat)
		{
			if (!pMat->AddGLC_Geom(this))
			{
				return;
			}
		}
	
		if (!!m_pMatiere)
		{
			m_pMatiere->DelGLC_Geom(GetID());
		}

		m_pMatiere= pMat;

		pMat->GetColDiffuse(m_fColor);

		m_bValidite = false;
	}
	else
	{	
		// Force la mise à jour
		m_bValidite = false;
	}
}
// Remove Geometry from the material without update material usage table
void GLC_Geometry::DelMatiere(GLC_Geometry* pGeom)
{
	//! \todo modify this algo
	
	if (this == pGeom)
	{
		m_pMatiere= NULL;
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
bool GLC_Geometry::CreationListe(GLenum Mode)
{
	if(!m_ListeID)		// The list doesn't exist
	{
		m_ListeID= glGenLists(1);

		if (!m_ListeID)	// List ID not obtain
		{
			GlDraw();
			qDebug("GLC_Geometrie::ERREUR Liste d'affichage NON créé");
			return false;	// Display geometry whithout OpenGL display list
		}
	}
	// List setting up
	glNewList(m_ListeID, Mode);
				
		// Geometrie set up and display
		GlDraw();	// Virtual function defined in concrete class

	glEndList();

	
	// List is valid
	m_bListeIsValid= true;
	return true;	// Display geometry with OpenGL display list
	
	//! \todo Add error handler
}
// Geometry display
bool GLC_Geometry::GlExecute(GLenum Mode)
{
	if (GetVisible())
	{
		// Object ID for selection purpose
		glLoadName(GetID());

		// Save current OpenGL Matrix
		glPushMatrix();

		// Define Geometry's property
		GlPropGeom();	// Virtual function defined in concrete class

		// Geometry validity set to true
		m_bValidite= true;

		if (!GetListeValidite())
		{
			// The list is not up to date or doesn't exist

			CreationListe(Mode);
		}
		else
		{
			glCallList(m_ListeID);
		}

		// To avoid Blending issue
		glDepthMask(GL_TRUE);

		glPopMatrix();
		
		//! \todo Add Error handler
		return true;
	}
	else return true;
}
