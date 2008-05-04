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


//! \file glc_matrix4x4.cpp implementation of the GLC_Matrix4x4 class.

#include <QtDebug>
#include "glc_matrix4x4.h"

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

// Default Constructor
GLC_Matrix4x4::GLC_Matrix4x4()
{

	for (int i= 0; i < TAILLEMAT4X4; i++)
	{
		if (isDiagonal(i))
			dMatrice[i]= 1;
		else
			dMatrice[i]= 0;
	}
}

// Construct a Matrix by copy
GLC_Matrix4x4::GLC_Matrix4x4(const GLC_Matrix4x4 &Mat)
{

	for (int i=0; i < TAILLEMAT4X4; i++)
	{
		dMatrice[i]= Mat.dMatrice[i];
	}
}


// Contruct a Matrix by an array of 16 elements.
GLC_Matrix4x4::GLC_Matrix4x4(const double *Tableau)
{

	for (int i=0; i < TAILLEMAT4X4; i++)
	{
		dMatrice[i]= Tableau[i];
	}
}

//////////////////////////////////////////////////////////////////////
// Operator Overload
//////////////////////////////////////////////////////////////////////

// Matrix cross product
GLC_Matrix4x4 GLC_Matrix4x4::operator * (const GLC_Matrix4x4 &Mat) const
{
	int Colonne;
	int Ligne;
	int i;
	double ValInt;
	
	int IndexInt;

	GLC_Matrix4x4 MatResult;
	for (Ligne= 0; Ligne < DIMMAT4X4; Ligne++)
	{
		for (Colonne=0; Colonne < DIMMAT4X4; Colonne++)
		{
			ValInt= 0;
			IndexInt= Colonne * DIMMAT4X4;

			for (i= 0; i < DIMMAT4X4; i++)
			{
				ValInt+= dMatrice[ (i * DIMMAT4X4) + Ligne] * Mat.dMatrice[ IndexInt + i];
			}
			MatResult.dMatrice[ IndexInt + Ligne]= ValInt;
		}
	}
	return MatResult;
}

// Vector transformation
GLC_Vector4d GLC_Matrix4x4::operator * (const GLC_Vector4d &Vect) const
{
	double ValInt;
	int i;
	GLC_Vector4d VectResult;

	for (int Index= 0; Index < DIMMAT4X4; Index++)
	{
		ValInt= 0;
		for (i= 0; i < DIMMAT4X4; i++)
		{
			ValInt+= dMatrice[(i * DIMMAT4X4) + Index] * Vect.dVecteur[i];
		}
		VectResult.dVecteur[Index]= ValInt;
	}
	
	if (VectResult.dVecteur[3] != 1)
	{
		//qDebug("Matrice4x4::operator * : Changement de W");
		VectResult.setWToNull();
	}

	return VectResult;
}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Compute matrix determinant
const double GLC_Matrix4x4::getDeterminant(void) const
{
	double Determinant= 0;
	double SubMat3x3[9];
	int Signe= 1;
	
	for (int Colonne= 0; Colonne < DIMMAT4X4; Colonne++, Signe*= -1)
	{
		getSubMat(0, Colonne, SubMat3x3);
		Determinant+= Signe * dMatrice[Colonne * DIMMAT4X4] * getDeterminant3x3(SubMat3x3);
	}

	return Determinant;

}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set matrix to a rotation matrix define by a vector and an angle in radians
GLC_Matrix4x4& GLC_Matrix4x4::setMatRot(const GLC_Vector4d &Vect, const double &dAngleRad)
{
	double q0;
	double q1;
	double q2;
	double q3;
	
	// La norme du Vecteur doit ï¿½tre ï¿½ un
	GLC_Vector4d VectRot(Vect);
	VectRot.setNormal(1);

	// Optimisation du code
	
	double SinAngleSur2= sin(dAngleRad / 2);

	// Calcul du Quaternion
	q0= cos(dAngleRad / 2);
	q1= VectRot.dVecteur[0] * SinAngleSur2;
	q2= VectRot.dVecteur[1] * SinAngleSur2;
	q3= VectRot.dVecteur[2] * SinAngleSur2;
	
	// Optimisation du code
	double q0Carre= (q0 * q0);
	double q1Carre= (q1 * q1);
	double q2Carre= (q2 * q2);
	double q3Carre= (q3 * q3);

	dMatrice[0]= q0Carre + q1Carre - q2Carre - q3Carre;
	dMatrice[1]= 2 * (q1 *q2 + q0 * q3);
	dMatrice[2]= 2 * (q1 * q3 - q0 * q2);
	dMatrice[3]= 0;
	dMatrice[4]= 2 * (q1 * q2 - q0 * q3);
	dMatrice[5]= q0Carre + q2Carre - q3Carre - q1Carre;
	dMatrice[6]= 2 * (q2 * q3 + q0 * q1);
	dMatrice[7]= 0;
	dMatrice[8]= 2 * (q1 * q3 + q0 * q2);
	dMatrice[9]= 2 * (q2 * q3 - q0 * q1);
	dMatrice[10]= q0Carre + q3Carre - q1Carre - q2Carre;
	dMatrice[11]= 0;

	dMatrice[12]= 0;	//TX
	dMatrice[13]= 0;	//TY
	dMatrice[14]= 0;	//TZ
	dMatrice[15]= 1;

	return *this;
}

// Set matrix to a rotation matrix define by 2 vectors
GLC_Matrix4x4& GLC_Matrix4x4::setMatRot(const GLC_Vector4d &Vect1, const GLC_Vector4d &Vect2)
{

	// Calcul de la matrice de rotation
	const GLC_Vector4d VectAxeRot(Vect1 ^ Vect2);
	// Vï¿½rifie que le Vecteur de l'axe de rotation n'est pas null
	if (!VectAxeRot.isNull())
	{  // Ok, il n'est pas null
		const double Angle= acos(Vect1 * Vect2);
		setMatRot(VectAxeRot, Angle);
	}
	
	return *this;
}

// Set Matrix to a translation matrix by a vector
GLC_Matrix4x4& GLC_Matrix4x4::setMatTranslate(const GLC_Vector4d &Vect)
{
	for (int i= 0; i < TAILLEMAT4X4 - DIMMAT4X4; i++)
	{
		if (isDiagonal(i))
			dMatrice[i]= 1;
		else
			dMatrice[i]= 0;
	}
	dMatrice[12]= Vect.dVecteur[0];
	dMatrice[13]= Vect.dVecteur[1];
	dMatrice[14]= Vect.dVecteur[2];
	dMatrice[15]= 1;

	return *this;
}

// Set Matrix to a translation matrix by 3 coordinates
GLC_Matrix4x4& GLC_Matrix4x4::setMatTranslate(double Tx, double Ty, double Tz)
{
	for (int i= 0; i < TAILLEMAT4X4 - DIMMAT4X4; i++)
	{
		if (isDiagonal(i))
			dMatrice[i]= 1;
		else
			dMatrice[i]= 0;
	}
	dMatrice[12]= Tx;
	dMatrice[13]= Ty;
	dMatrice[14]= Tz;
	dMatrice[15]= 1;
	
	return *this;

}

// Set Matrix to a scaling matrix define by 3 double
GLC_Matrix4x4& GLC_Matrix4x4::setMatScaling(const double &sX, const double &sY, const double sZ)
{
	for (int i= 0; i < TAILLEMAT4X4; i++)
	{
		dMatrice[i]= 0;
	}
	dMatrice[0]= 1 * sX;
	dMatrice[5]= 1 * sY;
	dMatrice[10]= 1 * sZ;
	dMatrice[15]= 1;
	
	return *this;

}


// Reverse the Matrix
GLC_Matrix4x4& GLC_Matrix4x4::invert(void)
{
	
	double Determinant= getDeterminant();


	// Verifie si l'inversion est possible
	if ( ( fabs(Determinant) ) < EPSILON)
		return *this;

	double InvDet = 1 / Determinant;
	GLC_Matrix4x4 TCoMat= getCoMat4x4().getTranspose();
	
	for (int i= 0; i < TAILLEMAT4X4; i++)
	{
		dMatrice[i]= TCoMat.dMatrice[i] * InvDet;
	}

	return *this;
}

// Set the matrix to identify matrix
GLC_Matrix4x4& GLC_Matrix4x4::setToIdentity()
{
	for (int i= 0; i < TAILLEMAT4X4; i++)
	{
		if (isDiagonal(i))
			dMatrice[i]= 1;
		else
			dMatrice[i]= 0;
	}
	
	return *this;
}

// Set the matrix by its transpose
GLC_Matrix4x4& GLC_Matrix4x4::transpose(void)
{
	GLC_Matrix4x4 MatT(dMatrice);
	int Colonne;
	int Ligne;
	int IndexOrigine;
	int IndexTrans;
	for (Colonne= 0; Colonne < DIMMAT4X4; Colonne++)
	{
		for (Ligne=0 ; Ligne < DIMMAT4X4; Ligne++)
		{
			IndexOrigine= (Colonne * DIMMAT4X4) + Ligne;
			IndexTrans= (Ligne * DIMMAT4X4) + Colonne;

			MatT.dMatrice[IndexTrans]= dMatrice[IndexOrigine];
		}
	}
	
	// Charge la matrice transposï¿½ dans la matrice courante.
	for (int Index= 0; Index < TAILLEMAT4X4; Index++)
		dMatrice[Index]= MatT.dMatrice[Index];
	
	return *this;
}

// Set the matrix with Euler angle
GLC_Matrix4x4& GLC_Matrix4x4::fromEuler(double angle_x, double angle_y, double angle_z)
{
    const double A= cos(angle_x);
    const double B= sin(angle_x);
    const double C= cos(angle_y);
    const double D= sin(angle_y);
    const double E= cos(angle_z);
    const double F= sin(angle_z);
 
    const double AD= A * D;
    const double BD= B * D;
 
    dMatrice[0]  = C * E;
    dMatrice[4]  = -C * F;
    dMatrice[8]  = -D;
    dMatrice[1]  = -BD * E + A * F;
    dMatrice[5]  = BD * F + A * E;
    dMatrice[9]  = -B * C;
    dMatrice[2]  = AD * E + B * F;
    dMatrice[6]  = -AD * F + B * E;
    dMatrice[10] = A * C;
 
    dMatrice[12]  =  dMatrice[13] = dMatrice[14] = dMatrice[3] = dMatrice[7] = dMatrice[11] = 0.0;
    dMatrice[15] =  1.0;
	
	return *this;
}
	


//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////

// Compute matrix determinant
const double GLC_Matrix4x4::getDeterminantLC(const int &Ligne, const int &Colonne) const
{
	double Mat3x3[9];
	double Determinant;
	
	getSubMat(Ligne, Colonne, Mat3x3);
	
	if ( 0 == ((Ligne + Colonne) % 2)) // Even number
		Determinant= dMatrice[(Colonne + DIMMAT4X4) + Ligne] * getDeterminant3x3(Mat3x3);
	else
		Determinant= - dMatrice[(Colonne + DIMMAT4X4) + Ligne] * getDeterminant3x3(Mat3x3);

	return Determinant;
}		

// Return a vector which contains radians Euler angle of the matrix
QVector<double> GLC_Matrix4x4::toEuler(void) const
{
	double angle_x;
	double angle_y;
	double angle_z;
	double tracex, tracey;
	angle_y= -asin(dMatrice[8]);
	double C= cos(angle_y);
	
	if (fabs(C) > EPSILON) // Gimball lock?
	{
		tracex= dMatrice[10] / C;
		tracey= - dMatrice[9] / C;
		angle_x= atan2( tracey, tracex); 
		
		tracex= dMatrice[0] / C;
		tracey= - dMatrice[4] / C;
		angle_z= atan2( tracey, tracex); 
	}
	else // Gimball lock?
	{
		angle_x= 0;
		tracex= dMatrice[5] / C;
		tracey= dMatrice[1] / C;
		angle_z= atan2( tracey, tracex);
	}
	QVector<double> result;
	result.append(fmod(angle_x, 2.0 * PI));
	result.append(fmod(angle_y, 2.0 * PI));
	result.append(fmod(angle_z, 2.0 * PI));
	
	return result;
}



// Calcul d'une sous matrice
void GLC_Matrix4x4::getSubMat(const int &Ligne, const int &Colonne, double *ResultMat) const
{
	
	int LigneOrigine;
	int ColonneOrigine;
	int LigneResult;
	int ColonneResult;
	int IndexOrigine;
	int IndexResult;

	for (ColonneOrigine= 0; ColonneOrigine < DIMMAT4X4; ColonneOrigine++)
	{
		if (ColonneOrigine != Colonne)
		{
			if (ColonneOrigine < Colonne)
				ColonneResult= ColonneOrigine;
			else
				ColonneResult= ColonneOrigine - 1;

			for (LigneOrigine= 0; LigneOrigine < DIMMAT4X4; LigneOrigine++)
			{
				if (LigneOrigine != Ligne)
				{
					if (LigneOrigine < Ligne)
						LigneResult= LigneOrigine;
					else
						LigneResult= LigneOrigine - 1;
					IndexOrigine= (ColonneOrigine * DIMMAT4X4) + LigneOrigine;
					IndexResult= (ColonneResult * (DIMMAT4X4 - 1)) + LigneResult;

					ResultMat[IndexResult]= dMatrice[IndexOrigine];
				}
			}
		}
	}
}

// Calcul du déterminant d'une matrice 3x3
const double GLC_Matrix4x4::getDeterminant3x3(const double *Mat3x3) const
{
	double Determinant;

	Determinant= Mat3x3[0] * ( Mat3x3[4] * Mat3x3[8] - Mat3x3[7] * Mat3x3[5]);
	Determinant+= - Mat3x3[3] * ( Mat3x3[1] * Mat3x3[8] - Mat3x3[7] * Mat3x3[2]);
	Determinant+= Mat3x3[6] * ( Mat3x3[1] * Mat3x3[5] - Mat3x3[4] * Mat3x3[2]);

	return Determinant;
}

// Retourne la matrice transposï¿½e
const GLC_Matrix4x4 GLC_Matrix4x4::getTranspose(void) const
{
	GLC_Matrix4x4 MatT(dMatrice);
	int Colonne;
	int Ligne;
	int IndexOrigine;
	int IndexTrans;
	for (Colonne= 0; Colonne < DIMMAT4X4; Colonne++)
	{
		for (Ligne=0 ; Ligne < DIMMAT4X4; Ligne++)
		{
			IndexOrigine= (Colonne * DIMMAT4X4) + Ligne;
			IndexTrans= (Ligne * DIMMAT4X4) + Colonne;

			MatT.dMatrice[IndexTrans]= dMatrice[IndexOrigine];
		}
	}

	return MatT;
}

// Retourne la comatrice
const GLC_Matrix4x4 GLC_Matrix4x4::getCoMat4x4(void) const
{
	GLC_Matrix4x4 CoMat(dMatrice);

	double SubMat3x3[9];

	int Colonne;
	int Ligne;
	int Index;

	for (Colonne= 0; Colonne < DIMMAT4X4; Colonne++)
	{
		for (Ligne=0 ; Ligne < DIMMAT4X4; Ligne++)
		{
			
			getSubMat(Ligne, Colonne, SubMat3x3);
			Index= (Colonne * DIMMAT4X4) + Ligne;
			if (((Colonne + Ligne + 2) % 2) == 0) // Even Number
				CoMat.dMatrice[Index]= getDeterminant3x3(SubMat3x3);
			else
				CoMat.dMatrice[Index]= -getDeterminant3x3(SubMat3x3);			
		}
	}

	return CoMat;

}
