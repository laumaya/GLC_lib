/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.7, packaged on September, 2007.

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


//////////////////////////////////////////////////////////////////////
// Friend Functions
//////////////////////////////////////////////////////////////////////

GLC_Vector4d operator - (const GLC_Vector4d &Vect)
{
	GLC_Vector4d VectResult;
	// La composant W est ignorée.
	for (int i=0; i < VECT4DIMENSION - 1; i++)
	{
		VectResult.dVecteur[i]= -Vect.dVecteur[i];
	}

	return VectResult;
}
//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

/*! Default constructor
*  Value is set to 
* \n X = 0.0
* \n Y =  0.0
* \n Z =  0.0
* \n W =  1.0*/
GLC_Vector4d::GLC_Vector4d()
{
	dVecteur[0]= 0.0;
	dVecteur[1]= 0.0;
	dVecteur[2]= 0.0;

	dVecteur[3]= 1.0;

}
/*! Recopy constructor
 * Sample use
 * \code
 * NewVect = new GLC_Vector4d(OldVect);
 * \endcode
 */
GLC_Vector4d::GLC_Vector4d(const GLC_Vector4d &Vect)
{
	dVecteur[0]= Vect.dVecteur[0];
	dVecteur[1]= Vect.dVecteur[1];
	dVecteur[2]= Vect.dVecteur[2];
	dVecteur[3]= Vect.dVecteur[3];

}
// Copy from an GLC_Vector3d
GLC_Vector4d::GLC_Vector4d(const GLC_Vector3d &Vect)
{
	dVecteur[0]= Vect.getX();
	dVecteur[1]= Vect.getY();
	dVecteur[2]= Vect.getZ();
	dVecteur[3]= 1.0;

}

// Copy from an GLC_Vector3d
GLC_Vector4d::GLC_Vector4d(const GLC_Vector2d &Vect)
{
	dVecteur[0]= Vect.getX();
	dVecteur[1]= Vect.getY();
	dVecteur[2]= 0.0;
	dVecteur[3]= 1.0;

}

//////////////////////////////////////////////////////////////////////
// Surdéfinition des opérateurs
//////////////////////////////////////////////////////////////////////


//Addition de 2 vecteurs
GLC_Vector4d GLC_Vector4d::operator + (const GLC_Vector4d &Vect) const
{
	// La composante W est ignorée.
	GLC_Vector4d VectResult(dVecteur[0] + Vect.dVecteur[0], dVecteur[1] + Vect.dVecteur[1],
		dVecteur[2] + Vect.dVecteur[2]);

	return VectResult;
}

// Surcharge opérateur d'affectation "="
GLC_Vector4d& GLC_Vector4d::operator = (const GLC_Vector4d &Vect)
{
	dVecteur[0]= Vect.dVecteur[0];
	dVecteur[1]= Vect.dVecteur[1];
	dVecteur[2]= Vect.dVecteur[2];
	dVecteur[3]= Vect.dVecteur[3];
	
	return *this;
}

// Surcharge opérateur adition affectation "+="
GLC_Vector4d* GLC_Vector4d::operator += (const GLC_Vector4d &Vect)
{
	*this= *this + Vect;
	return this;
}

// Surcharge opérateur de soustraction "-"
GLC_Vector4d GLC_Vector4d::operator- (const GLC_Vector4d &Vect) const
{
	// La composant W est ignorée.
	GLC_Vector4d VectResult(dVecteur[0] - Vect.dVecteur[0], dVecteur[1] - Vect.dVecteur[1],
		dVecteur[2] - Vect.dVecteur[2]);

	return VectResult;
}

// Surcharge opérateur soustraction affectation "-="
GLC_Vector4d* GLC_Vector4d::operator -= (const GLC_Vector4d &Vect)
{
	*this= *this - Vect;
	return this;
}

// Surcharge opérateur du produit vectoriel "^"
GLC_Vector4d GLC_Vector4d::operator^ (const GLC_Vector4d &Vect) const
{
	// La composante W est ignorée.
	GLC_Vector4d VectResult;
	VectResult.dVecteur[0]= dVecteur[1] * Vect.dVecteur[2] - dVecteur[2] * Vect.dVecteur[1];
	VectResult.dVecteur[1]= dVecteur[2] * Vect.dVecteur[0] - dVecteur[0] * Vect.dVecteur[2];
	VectResult.dVecteur[2]= dVecteur[0] * Vect.dVecteur[1] - dVecteur[1] * Vect.dVecteur[0];
		
	if (VectResult.isNull() )
	{
		qDebug("Vecteur4d::operator^ : Retourne un vecteur Null");
	}
	return VectResult;
}

// Surcharge opérateur du produit scalaire "*" avec une autre Vecteur
const double GLC_Vector4d::operator * (const GLC_Vector4d &Vect) const
{
	// La composante W est ignorée.
	return dVecteur[0] * Vect.dVecteur[0] + dVecteur[1] * Vect.dVecteur[1] +
		dVecteur[2] * Vect.dVecteur[2];
}


// Surcharge opérateur du produit scalaire "*" avec un scalaire (double)
GLC_Vector4d GLC_Vector4d::operator * (double Scalaire) const
{
	GLC_Vector4d VectResult;
	// La composante W est ignorée.
	VectResult.dVecteur[0]= dVecteur[0] * Scalaire;
	VectResult.dVecteur[1]= dVecteur[1] * Scalaire;
	VectResult.dVecteur[2]= dVecteur[2] * Scalaire;
	
	return VectResult;
}

// Surcharge operateur d'égalité "=="
bool GLC_Vector4d::operator == (const GLC_Vector4d &Vect) const
{
	bool bResult;
	bResult= fabs(dVecteur[0] - Vect.dVecteur[0]) < EPSILON;
	bResult= (fabs(dVecteur[1] - Vect.dVecteur[1]) < EPSILON) && bResult;
	bResult= (fabs(dVecteur[2] - Vect.dVecteur[2]) < EPSILON) && bResult;
	bResult= (fabs(dVecteur[3] - Vect.dVecteur[3]) < EPSILON) && bResult;

	return bResult;
}

// Surcharge de l'opérateur d'inégalité "!="
bool GLC_Vector4d::operator != (const GLC_Vector4d &Vect) const
{
	return !(*this == Vect);
}

//////////////////////////////////////////////////////////////////////
// Fonction Set
//////////////////////////////////////////////////////////////////////
GLC_Vector4d& GLC_Vector4d::setX(const double &dX)
{
	dVecteur[0]= dX;
	return *this;
}

GLC_Vector4d& GLC_Vector4d::setY(const double &dY)
{
	dVecteur[1]= dY;
	return *this;
}

GLC_Vector4d& GLC_Vector4d::setZ(const double &dZ)
{
	dVecteur[2]= dZ;
	return *this;
}

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
	if ((dW == 1) || (dW <= 0))
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
GLC_Vector4d& GLC_Vector4d::setVect(const GLC_Vector4d &Vect)
{
	dVecteur[0]= Vect.dVecteur[0];
	dVecteur[1]= Vect.dVecteur[1];
	dVecteur[2]= Vect.dVecteur[2];
	dVecteur[3]= Vect.dVecteur[3];
	return *this;
}

GLC_Vector4d& GLC_Vector4d::setNormal(const double &Norme)
{
	double Coef;
	double dNormeCur;
	// La composant W est ignorée.
	dNormeCur= sqrt( dVecteur[0] * dVecteur[0] + dVecteur[1] * dVecteur[1] + dVecteur[2] * dVecteur[2]);
	
	if (dNormeCur != 0)
	{
		Coef = Norme / dNormeCur;
		
		dVecteur[0] = dVecteur[0] * Coef;
		dVecteur[1] = dVecteur[1] * Coef;
		dVecteur[2] = dVecteur[2] * Coef;
	}
	return *this;
}

GLC_Vector4d& GLC_Vector4d::setInv(void)
{
	dVecteur[0]= - dVecteur[0];
	dVecteur[1]= - dVecteur[1];
	dVecteur[2]= - dVecteur[2];
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Fonction Get
//////////////////////////////////////////////////////////////////////

// Angle avec un autre vecteur
const double GLC_Vector4d::getAngleWithVect(GLC_Vector4d Vect) const
{
	// Mise en Conformité des vecteurs
	GLC_Vector4d ThisVect(*this);
	ThisVect.setNormal(1);
	Vect.setNormal(1);
	// Axe de rotation
	const GLC_Vector4d VectAxeRot(ThisVect ^ Vect);
	// Vérifie que le Vecteur de l'axe de rotation n'est pas null
	if (!VectAxeRot.isNull())
	{  // Ok, il n'est pas null
		const double Angle= acos(ThisVect * Vect);
		return Angle;
	}
	else return 0;
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
	dVecteur[3]= 1;
}


