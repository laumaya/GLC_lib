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

//! \file glc_cylindre.cpp implementation of the GLC_Cylindre class.

#include <QVector>
#include <QtDebug>

#include "glc_cylindre.h"



//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////


GLC_Cylindre::GLC_Cylindre(double dRayon, double dLongeur, const char *pName
						   , const GLfloat *pColor)


:GLC_Geometrie(pName, pColor)
, m_dRayon(dRayon)
, m_dLg(dLongeur)
, m_nDiscret(GLC_POLYDISCRET)	//GLC_DISCRET
, m_bCapEnded(true)				// Bouts fermés par défaut	
, m_PolyFace(GL_FRONT_AND_BACK)	// Style par défaut des faces
, m_PolyMode(GL_FILL)			// Style par défaut du mode
{

}

//////////////////////////////////////////////////////////////////////
// Fonctions OpenGL privées
//////////////////////////////////////////////////////////////////////

// Dessin du GLC_Cylindre
void GLC_Cylindre::GlDraw(void)
{
	QVector<double> TableauCos;
	QVector<double> TableauSin;
	GLC_Vector4d Vect;

	// Calcul des coordonnées des points du pourtour
	for (int i= 0; i <= m_nDiscret; i++)
	{
		TableauCos.append(m_dRayon * cos(i * (2 * PI) / m_nDiscret));
		TableauSin.append(m_dRayon * sin(i * (2 * PI) / m_nDiscret));
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

			glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_dLg)); 

			glVertex3dv(Vect.Return_dVect());

			if(!Longueur)
				Longueur= m_dLg;
			else
				Longueur= 0;

			Vect.SetVect(TableauCos[i], TableauSin[i], Longueur );

			glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_dLg)); 

			glVertex3dv(Vect.Return_dVect());

			if(!Longueur)
			{
				Vect.SetVect(TableauCos[i], TableauSin[i], 0.0).SetNormal(1);
				glNormal3dv(Vect.Return_dVect());
				glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_dLg)); 


				Vect.SetVect(TableauCos[i], TableauSin[i], Longueur );
				glVertex3dv(Vect.Return_dVect());
				Longueur= m_dLg;
				glTexCoord2f(fCoordx, static_cast<float>(Longueur / m_dLg));
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
		float fRayon= static_cast<float>(m_dRayon); // Pour éviter trop de conversion de données

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
			glVertex3d(0.0, 0.0, m_dLg);
			for (int i= 0; i <= m_nDiscret; i++)
			{
				// Calcul des coordonnées de texture
				TextureX= (static_cast<float>(TableauCos[i]) + fRayon) / (2 * fRayon);
				TextureY= (static_cast<float>(TableauSin[i]) + fRayon) / (2 * fRayon);
				glTexCoord2f(TextureX, TextureY);
				// coordonnées du vertex
				Vect.SetVect(TableauCos[i], TableauSin[i], m_dLg );
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
		qDebug("GLC_Cylindre::GlDraw ERREUR OPENGL\n");
	}

}
// Fonction définissant le propriétés de la géométrie (Couleur, position, epaisseur)
void GLC_Cylindre::GlPropGeom(void)
{
		// Modification de la matrice courante
		glMultMatrixd(m_MatPos.Return_dMat());
		if(!m_pMatiere || (m_PolyMode != GL_FILL))
		{
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);

			if (m_bBlending)
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
		else if (m_pMatiere->GetAddRgbaTexture())
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);

			if (m_bBlending)
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

			m_pMatiere->GlExecute();
		}

		else
		{
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);

			if (m_bBlending)
			{
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			m_pMatiere->GlExecute();
		}

		glLineWidth(GetEpaisseur());	// Son Epaisseur

		// Mode d'affichage des polygons
		glPolygonMode(m_PolyFace, m_PolyMode);

		// Gestion erreur OpenGL
		if (glGetError() != GL_NO_ERROR)
		{
			qDebug("GLC_Cylindre::GlPropGeom ERREUR OPENGL\n");
		}

}
