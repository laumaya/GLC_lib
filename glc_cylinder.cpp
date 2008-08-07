/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

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

//! \file glc_cylinder.cpp implementation of the GLC_Cylinder class.

#include <QVector>
#include "glc_cylinder.h"
#include "glc_openglexception.h"
#include "glc_selectionmaterial.h"

using namespace glc;
//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////


GLC_Cylinder::GLC_Cylinder(double dRadius, double dLength)
:GLC_Geometry("Cylinder", false)
, m_Radius(dRadius)
, m_Length(dLength)
, m_Discret(GLC_POLYDISCRET)	// Default discretion
, m_EndedIsCaped(true)				// Cylinder ended are closed	
{
	Q_ASSERT((m_Radius > 0.0) && (m_Length > 0.0));
}

GLC_Cylinder::GLC_Cylinder(const GLC_Cylinder& sourceCylinder)
:GLC_Geometry(sourceCylinder)
, m_Radius(sourceCylinder.m_Radius)
, m_Length(sourceCylinder.m_Length)
, m_Discret(sourceCylinder.m_Discret)
, m_EndedIsCaped(sourceCylinder.m_EndedIsCaped)
{
	Q_ASSERT((m_Radius > 0.0) && (m_Length > 0.0) && (m_Discret > 0));
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get Lenght of the Cylinder
double GLC_Cylinder::getLength(void) const
{
	return m_Length;
}

// Get Radius of cylinder
double GLC_Cylinder::getRadius(void) const
{
	return m_Radius;
}

// Get Cylinder discretion
int GLC_Cylinder::getDiscretion(void) const
{
	return m_Discret;
}


// return the cylinder bounding box
GLC_BoundingBox* GLC_Cylinder::getBoundingBox(void) const
{
	GLC_BoundingBox* pBoundingBox= new GLC_BoundingBox();
	
	GLC_Point3d lower(-m_Radius, -m_Radius, 0.0);
	GLC_Point3d upper(m_Radius, m_Radius, m_Length);
	pBoundingBox->combine(lower);
	pBoundingBox->combine(upper);
	
	return pBoundingBox;	
}

// Return a copy of the current geometry
GLC_Geometry* GLC_Cylinder::clone() const
{
	return new GLC_Cylinder(*this);
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Set Cylinder length
void GLC_Cylinder::setLength(double Length)
{
	Q_ASSERT(Length > 0.0);
	m_Length= Length;

	m_ListIsValid= false;	 
}

// Set Cylinder radius
void GLC_Cylinder::setRadius(double Radius)
{
	Q_ASSERT(Radius > 0.0);
	m_Radius= Radius;
	
	m_ListIsValid= false;
}

// Set Discretion
void GLC_Cylinder::setDiscretion(int TargetDiscret)
{
	Q_ASSERT(TargetDiscret > 0);
	if (TargetDiscret != m_Discret)
	{
		m_Discret= TargetDiscret;
		if (m_Discret < 6) m_Discret= 6;

		m_ListIsValid= false;
	}
}

// End Caps
void GLC_Cylinder::setEndedCaps(bool CapsEnded)
{
	if (m_EndedIsCaped != CapsEnded)
	{
		m_EndedIsCaped= CapsEnded;
		m_ListIsValid= false;
	}
}

//////////////////////////////////////////////////////////////////////
// Private Opengl functions
//////////////////////////////////////////////////////////////////////

// Dessin du GLC_Cylinder
void GLC_Cylinder::glDraw(void)
{
	QVector<double> TableauCos;
	QVector<double> TableauSin;
	GLC_Vector4d Vect;

	// Calcul des coordonn�es des points du pourtour
	for (int i= 0; i <= m_Discret; i++)
	{
		TableauCos.append(m_Radius * cos(i * (2 * PI) / m_Discret));
		TableauSin.append(m_Radius * sin(i * (2 * PI) / m_Discret));
	}

	double Longueur= 0;

	// Affichage du Cylindre
	glBegin(GL_QUADS);

	float fCoordx;

		for (int i= 0; i <= m_Discret; i++)
		{
			fCoordx= static_cast<float>(i) / static_cast<float>(m_Discret);

			Vect.setVect(TableauCos[i], TableauSin[i], 0.0).setNormal(1);
			glNormal3dv(Vect.return_dVect());

			Vect.setVect(TableauCos[i], TableauSin[i], Longueur );

			glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_Length)); 

			glVertex3dv(Vect.return_dVect());

			if(!Longueur)
				Longueur= m_Length;
			else
				Longueur= 0;

			Vect.setVect(TableauCos[i], TableauSin[i], Longueur );

			glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_Length)); 

			glVertex3dv(Vect.return_dVect());

			if(!Longueur)
			{
				Vect.setVect(TableauCos[i], TableauSin[i], 0.0).setNormal(1);
				glNormal3dv(Vect.return_dVect());
				glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_Length)); 


				Vect.setVect(TableauCos[i], TableauSin[i], Longueur );
				glVertex3dv(Vect.return_dVect());
				Longueur= m_Length;
				glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_Length));
				Vect.setVect(TableauCos[i], TableauSin[i], Longueur );
				glVertex3dv(Vect.return_dVect());
			}

		}

	glEnd();

	if (m_EndedIsCaped)
	{	// Ended must be closed
	// Cylinder cap
		// Testure coordinate
		float TextureX= 0.5f;	// Center
		float TextureY= 0.5f;	// Center
		float fRayon= static_cast<float>(m_Radius); // To avoid many casting

		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(TextureX, TextureY);
			// Normals and closed lower
			glNormal3d(0.0, 0.0, -1.0);
			glVertex3d(0.0, 0.0, 0.0);
			for (int i= 0; i <= m_Discret; i++)
			{
				// Calculate texture coordinates
				TextureX= (static_cast<float>(TableauCos[i]) + fRayon) / (2 * fRayon);
				TextureY= (static_cast<float>(TableauSin[i]) + fRayon) / (2 * fRayon);
				glTexCoord2f(TextureX, TextureY);
				// Vertex coordinates
				Vect.setVect(TableauCos[i], TableauSin[i], 0.0 );
				glVertex3dv(Vect.return_dVect());
			}
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
			// Texture coordinate
			TextureX= 0.5f;	// Centre
			TextureY= 0.5f;	// Centre
			glTexCoord2f(TextureX, TextureY);
			// Normals and closed upper
			glNormal3d(0.0, 0.0, 1.0);
			glVertex3d(0.0, 0.0, m_Length);
			for (int i= 0; i <= m_Discret; i++)
			{
				// Calculate texture coordinates
				TextureX= (static_cast<float>(TableauCos[i]) + fRayon) / (2 * fRayon);
				TextureY= (static_cast<float>(TableauSin[i]) + fRayon) / (2 * fRayon);
				glTexCoord2f(TextureX, TextureY);
				// Vertex coordinates
				Vect.setVect(TableauCos[i], TableauSin[i], m_Length );
				glVertex3dv(Vect.return_dVect());
			}
		glEnd();
	}
	TableauCos.clear();
	TableauSin.clear();

	// End of cylinder draw
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Cylinder::GlDraw ", error);
		throw(OpenGlException);
	}

}
