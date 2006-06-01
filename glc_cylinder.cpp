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

//! \file glc_cylinder.cpp implementation of the GLC_Cylinder class.

#include <QVector>
#include <QtDebug>

#include "glc_cylinder.h"



//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////


GLC_Cylinder::GLC_Cylinder(double dRadius, double dLength, const char *pName
						   , const GLfloat *pColor)
:GLC_Geometry(pName, pColor)
, m_Radius(dRadius)
, m_dLength(dLength)
, m_nDiscret(GLC_POLYDISCRET)	//GLC_DISCRET
, m_bCapEnded(true)				// Bouts fermés par défaut	
{

}

GLC_Cylinder::GLC_Cylinder(const GLC_Cylinder& sourceCylinder)
:GLC_Geometry(sourceCylinder)
, m_Radius(sourceCylinder.m_Radius)
, m_dLength(sourceCylinder.m_dLength)
, m_nDiscret(sourceCylinder.m_nDiscret)
, m_bCapEnded(sourceCylinder.m_bCapEnded)
{
	
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////
// return the cylinder bounding box
GLC_BoundingBox* GLC_Cylinder::getBoundingBox(void) const
{
	GLC_BoundingBox* pBoundingBox= new GLC_BoundingBox();
	
	GLC_Vector3d lower(-m_Radius, -m_Radius, 0.0);
	GLC_Vector3d upper(m_Radius, m_Radius, m_dLength);
	pBoundingBox->combine(lower);
	pBoundingBox->combine(upper);
	
    
    pBoundingBox->transform(m_MatPos);	
	return pBoundingBox;	
}

//////////////////////////////////////////////////////////////////////
// Fonctions OpenGL privées
//////////////////////////////////////////////////////////////////////

// Dessin du GLC_Cylinder
void GLC_Cylinder::GlDraw(void)
{
	QVector<double> TableauCos;
	QVector<double> TableauSin;
	GLC_Vector4d Vect;

	// Calcul des coordonnées des points du pourtour
	for (int i= 0; i <= m_nDiscret; i++)
	{
		TableauCos.append(m_Radius * cos(i * (2 * PI) / m_nDiscret));
		TableauSin.append(m_Radius * sin(i * (2 * PI) / m_nDiscret));
	}

	double Longueur= 0;

	// Affichage du Cylindre
	glBegin(GL_QUADS);

	float fCoordx;

		for (int i= 0; i <= m_nDiscret; i++)
		{
			fCoordx= static_cast<float>(i) / static_cast<float>(m_nDiscret);

			Vect.SetVect(TableauCos[i], TableauSin[i], 0.0).SetNormal(1);
			glNormal3dv(Vect.Return_dVect());

			Vect.SetVect(TableauCos[i], TableauSin[i], Longueur );

			glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_dLength)); 

			glVertex3dv(Vect.Return_dVect());

			if(!Longueur)
				Longueur= m_dLength;
			else
				Longueur= 0;

			Vect.SetVect(TableauCos[i], TableauSin[i], Longueur );

			glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_dLength)); 

			glVertex3dv(Vect.Return_dVect());

			if(!Longueur)
			{
				Vect.SetVect(TableauCos[i], TableauSin[i], 0.0).SetNormal(1);
				glNormal3dv(Vect.Return_dVect());
				glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_dLength)); 


				Vect.SetVect(TableauCos[i], TableauSin[i], Longueur );
				glVertex3dv(Vect.Return_dVect());
				Longueur= m_dLength;
				glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_dLength));
				Vect.SetVect(TableauCos[i], TableauSin[i], Longueur );
				glVertex3dv(Vect.Return_dVect());
			}

		}

	glEnd();

	if (m_bCapEnded)
	{	// Les bouts doivent être fermés
	// Fermeture du cylindre
		// Coordonnées de texture
		float TextureX= 0.5f;	// Centre
		float TextureY= 0.5f;	// Centre
		float fRayon= static_cast<float>(m_Radius); // Pour éviter trop de conversion de données

		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(TextureX, TextureY);
			// Normal et fermeture du bas
			glNormal3d(0.0, 0.0, -1.0);
			glVertex3d(0.0, 0.0, 0.0);
			for (int i= 0; i <= m_nDiscret; i++)
			{
				// Calcule des coordonnées de textures
				TextureX= (static_cast<float>(TableauCos[i]) + fRayon) / (2 * fRayon);
				TextureY= (static_cast<float>(TableauSin[i]) + fRayon) / (2 * fRayon);
				glTexCoord2f(TextureX, TextureY);
				// coordonnées du vertex
				Vect.SetVect(TableauCos[i], TableauSin[i], 0.0 );
				glVertex3dv(Vect.Return_dVect());
			}
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
			// Coordonnées de texture
			TextureX= 0.5f;	// Centre
			TextureY= 0.5f;	// Centre
			glTexCoord2f(TextureX, TextureY);
			// Normal et fermeture du haut
			glNormal3d(0.0, 0.0, 1.0);
			glVertex3d(0.0, 0.0, m_dLength);
			for (int i= 0; i <= m_nDiscret; i++)
			{
				// Calcul des coordonnées de texture
				TextureX= (static_cast<float>(TableauCos[i]) + fRayon) / (2 * fRayon);
				TextureY= (static_cast<float>(TableauSin[i]) + fRayon) / (2 * fRayon);
				glTexCoord2f(TextureX, TextureY);
				// coordonnées du vertex
				Vect.SetVect(TableauCos[i], TableauSin[i], m_dLength );
				glVertex3dv(Vect.Return_dVect());
			}
		glEnd();
	}
	TableauCos.clear();
	TableauSin.clear();

	// Fin de l'affichage du cylindre
	// Gestion erreur OpenGL
	if (glGetError() != GL_NO_ERROR)
	{
		qDebug("GLC_Cylinder::GlDraw ERROR OPENGL\n");
	}

}
// Virtual interface for OpenGL Geomtry properties. (Color, thiknness, position..)
void GLC_Cylinder::GlPropGeom(void)
{
		// Update Current matrix
		glMultMatrixd(m_MatPos.Return_dMat());
		if(!m_pMaterial || (m_PolyMode != GL_FILL))
		{
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);

			if (m_IsBlended)
			{
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			glColor4fv(GetfRGBA());			// Sa Couleur
		}
		else if (m_pMaterial->GetAddRgbaTexture())
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);

			if (m_IsBlended)
			{
				glDisable(GL_LIGHTING);
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
			}
			else
			{
				glDisable(GL_BLEND);
			}
			glColor4fv(GetfRGBA());			// Sa Couleur

			m_pMaterial->GlExecute();
		}

		else
		{
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);

			if (m_IsBlended)
			{
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			m_pMaterial->GlExecute();
		}

		glLineWidth(GetThickness());	// Son Epaisseur

		// Polygons display mode
		glPolygonMode(m_PolyFace, m_PolyMode);

		// OpenGL error management
		if (glGetError() != GL_NO_ERROR)
		{
			qDebug("GLC_Cylinder::GlPropGeom ERROR OPENGL\n");
		}

}
