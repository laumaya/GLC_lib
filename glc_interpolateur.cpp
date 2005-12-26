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

//! \file interpolateur.cpp implementation of the Interpolateur class.

#include "glc_interpolateur.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Contructeur par défaut Interpolation Linéaire
Interpolateur::Interpolateur()
: m_InterpolType(INTERPOL_LINEAIRE)
, m_nNbrPas(1)
{

}

//////////////////////////////////////////////////////////////////////
// Fonction Set
//////////////////////////////////////////////////////////////////////
// Défini la matrice d'interpolation
void Interpolateur::SetInterpolMat(int NbrPas, const GLC_Vector4d &VectDepart, const GLC_Vector4d &VectArrive
								   , INTERPOL_TYPE Interpolation)
{
	// Mise à jour des données membre
	m_InterpolType= Interpolation;
	if (!NbrPas)
	{
		//TRACE("Interpolateur::SetInterpolMat -> NbrPas == 0 \n");		
	}
	else m_nNbrPas= NbrPas;

	m_VectDepart= VectDepart;
	m_VectArrive= VectArrive;
	// Calcul de la matrice d'interpolation
	CalcInterpolMat();
}
// Type d'interpolation
void Interpolateur::SetType(INTERPOL_TYPE Interpolation)
{
	if (m_InterpolType != Interpolation)
	{
		m_InterpolType= Interpolation;
		// Calcul de la matrice d'interpolation
		CalcInterpolMat();
	}
}
// Nombre de pas
void Interpolateur::SetNbrPas(int NbrPas)
{
	if (!NbrPas)
	{
		//TRACE("Interpolateur::SetNbrPas -> NbrPas == 0 \n");
		return;
	}

	if (m_nNbrPas != NbrPas)
	{
		m_nNbrPas= NbrPas;
		// Calcul de la matrice d'interpolation
		CalcInterpolMat();
	}
}
// Vecteur d'arrivée et de depart
void Interpolateur::SetVecteurs(const GLC_Vector4d &VectDepart, const GLC_Vector4d &VectArrive)
{
	m_VectDepart= VectDepart;
	m_VectArrive= VectArrive;
	
	// Calcul de la matrice d'interpolation
	CalcInterpolMat();

}

//////////////////////////////////////////////////////////////////////
// Fonction Get
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Fonctions de Service privée
//////////////////////////////////////////////////////////////////////
// Calcul La matrice d'interpolation
bool Interpolateur::CalcInterpolMat(void)
{
	// Verifie que les vecteur d'arrivé et de départ sont différent
	if (m_VectDepart == m_VectArrive)
	{
		//TRACE("Interpolateur::CalcInterpolMat : Depart == Arrive\n");
		return false;
	}

	switch (m_InterpolType)
	{
	case INTERPOL_LINEAIRE:
		return CalcInterpolLineaireMat();
		break;

	case INTERPOL_ANGULAIRE:
		return CalcInterpolAngulaireMat();
		break;

	case INTERPOL_HOMOTETIE:
		return false;
		break;

	default:
		//TRACE("Interpolateur::CalcInterpolMat : Type d'interpolation non valide\n");
		return false;
	}
}

// Calcul la matrice d'interpolation linéaire
bool Interpolateur::CalcInterpolLineaireMat(void)
{	

	// Calcul la matrice de translation
	const GLC_Vector4d VectTrans= (m_VectArrive - m_VectDepart) * (1.0 / m_nNbrPas);	
	if(VectTrans.IsNull())
	{
		//TRACE("Interpolateur::CalcInterpolLineaireMat -> Translation NULL\n");
		m_InterpolMat.SetIdentite();
		return false;
	}
	else
	{
		m_InterpolMat.SetMatTranslate(VectTrans);
		return true;
	}
}

// Calcul la matrice d'interpolation angulaire
bool Interpolateur::CalcInterpolAngulaireMat(void)
{
	// Calcul de l'axe de rotation
	const GLC_Vector4d AxeRot(m_VectDepart ^ m_VectArrive);
	// Calcul de l'angle entre les vecteurs
	const double Angle= m_VectArrive.GetAngleWithVect(m_VectDepart) / m_nNbrPas;
	// Calcul de la matrice de rotation
	if (fabs(Angle) < EPSILON)
	{
		//TRACE("Interpolateur::CalcInterpolAngulaireMat -> Rotation NULL\n");
		m_InterpolMat.SetIdentite();
		return false;
	}
	else
	{
		m_InterpolMat.SetMatRot( AxeRot, Angle);
		return true;
	}
}



