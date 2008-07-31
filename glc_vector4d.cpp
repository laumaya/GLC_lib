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


//! \file glc_vector4d.cpp implementation of the GLC_Vector4d class.
#include <QtDebug>
#include "glc_vector4d.h"

using namespace glc;
//////////////////////////////////////////////////////////////////////
// Operators overload
//////////////////////////////////////////////////////////////////////


// Surcharge opérateur du produit vectoriel "^"
GLC_Vector4d GLC_Vector4d::operator^ (const GLC_Vector4d &Vect) const
{
	GLC_Vector4d VectResult;
	VectResult.dVecteur[0]= dVecteur[1] * Vect.dVecteur[2] - dVecteur[2] * Vect.dVecteur[1];
	VectResult.dVecteur[1]= dVecteur[2] * Vect.dVecteur[0] - dVecteur[0] * Vect.dVecteur[2];
	VectResult.dVecteur[2]= dVecteur[0] * Vect.dVecteur[1] - dVecteur[1] * Vect.dVecteur[0];

	return VectResult;
}

// Surcharge operateur d'égalité "=="
bool GLC_Vector4d::operator == (const GLC_Vector4d &Vect) const
{
	bool bResult= fabs(dVecteur[0] - Vect.dVecteur[0]) < EPSILON;
	bResult= (fabs(dVecteur[1] - Vect.dVecteur[1]) < EPSILON) && bResult;
	bResult= (fabs(dVecteur[2] - Vect.dVecteur[2]) < EPSILON) && bResult;
	bResult= (fabs(dVecteur[3] - Vect.dVecteur[3]) < EPSILON) && bResult;

	return bResult;
}
//////////////////////////////////////////////////////////////////////
// Fonction Set
//////////////////////////////////////////////////////////////////////

GLC_Vector4d& GLC_Vector4d::setW(const double &dW)
{		
	if (dW != 0)
	{
		dVecteur[0]/= dW;
		dVecteur[1]/= dW;
		dVecteur[2]/= dW;
		dVecteur[3]= 1.0;		// Pour le calcul, W = 1.
	}
	return *this;
}

GLC_Vector4d& GLC_Vector4d::setVect(const double &dX, const double &dY,
	const double &dZ, const double &dW)
{
	if ((dW == 1.0) || (dW <= 0.0))
	{
		dVecteur[0]= dX;
		dVecteur[1]= dY;
		dVecteur[2]= dZ;
	}
	else
	{
		dVecteur[0]= dX / dW;
		dVecteur[1]= dY / dW;
		dVecteur[2]= dZ / dW;
	}
	
	dVecteur[3]= 1.0;		// Pour le calcul, W = 1.

	return *this;
}

GLC_Vector4d& GLC_Vector4d::setNormal(const double &Norme)
{
	const double dNormeCur= sqrt( dVecteur[0] * dVecteur[0] + dVecteur[1] * dVecteur[1] + dVecteur[2] * dVecteur[2]);
	
	if (dNormeCur != 0)
	{
		const double Coef = Norme / dNormeCur;
		
		dVecteur[0] = dVecteur[0] * Coef;
		dVecteur[1] = dVecteur[1] * Coef;
		dVecteur[2] = dVecteur[2] * Coef;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Fonction Get
//////////////////////////////////////////////////////////////////////

// Angle avec un autre vecteur
const double GLC_Vector4d::getAngleWithVect(GLC_Vector4d Vect) const
{
	GLC_Vector4d ThisVect(*this);
	ThisVect.setNormal(1);
	Vect.setNormal(1);
	// Rotation axis
	const GLC_Vector4d VectAxeRot(ThisVect ^ Vect);
	// Check if the rotation axis vector is null
	if (!VectAxeRot.isNull())
	{
		return acos(ThisVect * Vect);
	}
	else return 0.0;
}
// return the vector string
QString GLC_Vector4d::toString() const
{
	QString result("[");
	
	result+= QString::number(dVecteur[0]) + QString(" , ");
	result+= QString::number(dVecteur[1]) + QString(" , ");
	result+= QString::number(dVecteur[2]) + QString(" , ");
	result+= QString::number(dVecteur[3]) + QString("]");
	
	return result;
}

// return the 2D vector 
GLC_Vector2d GLC_Vector4d::toVector2d(const GLC_Vector4d& mask) const
{
	double x;
	double y;
	if (mask.dVecteur[0] == 0.0)
	{
		x= dVecteur[0];
		if (mask.dVecteur[1] == 0.0)
			y= dVecteur[1];
		else		
			y= dVecteur[2];
		
	}
	else
	{
		x= dVecteur[1];
		y= dVecteur[2];
		
	}
	return GLC_Vector2d(x, y);
}

//////////////////////////////////////////////////////////////////////
// Fonctions de Service privée
//////////////////////////////////////////////////////////////////////

// setWToNull le vecteur -> Composante W=1
void GLC_Vector4d::setWToNull(void)
{
	if (fabs(dVecteur[3]) > 0.00001)
	{
		dVecteur[0]/= dVecteur[3];
		dVecteur[1]/= dVecteur[3];
		dVecteur[2]/= dVecteur[3];
	}
	dVecteur[3]= 1.0;
}


