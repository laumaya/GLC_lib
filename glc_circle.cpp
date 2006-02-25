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

//! \file glc_circle.cpp implementation of the GLC_Circle class.

#include "glc_circle.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


GLC_Circle::GLC_Circle(const double &dRadius, const char *pName,
					   const GLfloat *pColor, double Angle)

:GLC_Geometry(pName, pColor)
{
	//! \todo remade init
	m_Radius= dRadius;
	m_nDiscret= GLC_DISCRET;
	m_dAngle= Angle;
}

GLC_Circle::~GLC_Circle()
{
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Circle Radius
bool GLC_Circle::SetRadius(double R)
{
	R = fabs(R);
	if ( fabs(R - m_Radius) > EPSILON)
	{	// Radius is changing
		if (R > EPSILON)
		{
			m_Radius= R;
			m_ListIsValid= false;
			return true;
		}
		else return false;	// Radius must be > 0
	}
	else return true;		// Radius doesn't change
	//! \todo Add error handler in case of invalid radius
}

// Set Circle discret
void GLC_Circle::SetDiscretion(int TargetDiscret)
{
	TargetDiscret= abs(TargetDiscret);
	if (TargetDiscret != m_nDiscret)
	{
		m_nDiscret= TargetDiscret;
		if (m_nDiscret < 6) m_nDiscret= 6;
		m_ListIsValid= false;
	}
}

// Set Circle Angle
bool GLC_Circle::SetAngle(double AngleRadians)	// Angle in Radians
{
	if ( fabs(AngleRadians - m_dAngle) > EPSILON)
	{	// Angle is changing
		if (AngleRadians > EPSILON)
		{
			m_dAngle= AngleRadians;
			m_ListIsValid= false;
			return true;
		}
		else return false;	// Angle must be > 0
	}
	else return true;		// Radius doesn't change
	//! \todo Add error handler in case of invalid angle

}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Dessin du Cercle
void GLC_Circle::GlDraw(void)
{
	double MyCos;
	double MySin;
	GLC_Vector4d Vect;

	// Affichage du Cercle
	glBegin(GL_LINE_STRIP);

		for (int i= 0; i <= m_nDiscret; i++)
		{
			MyCos= m_Radius * cos(i * m_dAngle / m_nDiscret);
			MySin= m_Radius * sin(i * m_dAngle / m_nDiscret);
			Vect.SetVect(MyCos, MySin, 0);
			glVertex3dv(Vect.Return_dVect());
		}

	glEnd();
	// Fin de l'affichage du cercle
}
// Fonction définissant le propriétés de la géométrie (Couleur, position, epaisseur)
void GLC_Circle::GlPropGeom(void)
{
		// Modification de la matrice courante
		glMultMatrixd(m_MatPos.Return_dMat());
		
		// Proprièté Graphique du cercle
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		// Pas de transparence
		glDisable(GL_BLEND);
			

		glColor4fv(GetfRGBA());			// Sa Couleur
		glLineWidth(GetThickness());	// Son Epaisseur
		
}
