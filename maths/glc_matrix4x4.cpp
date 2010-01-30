/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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

#include "glc_matrix4x4.h"

#include <QtDebug>

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
			ValInt= 0.0;
			IndexInt= Colonne * DIMMAT4X4;

			for (i= 0; i < DIMMAT4X4; i++)
			{
				ValInt+= m_Matrix[ (i * DIMMAT4X4) + Ligne] * Mat.m_Matrix[ IndexInt + i];
			}
			MatResult.m_Matrix[ IndexInt + Ligne]= ValInt;
		}
	}
	return MatResult;
}

// Vector transformation
GLC_Vector3d GLC_Matrix4x4::operator * (const GLC_Vector3d &Vect) const
{
	double ValInt;
	int i;
	GLC_Vector3d VectResult;
	double mat[4];

	for (int Index= 0; Index < DIMMAT4X4; Index++)
	{
		ValInt= 0.0;
		for (i= 0; i < DIMMAT4X4 - 1; i++)
		{
			ValInt+= m_Matrix[(i * DIMMAT4X4) + Index] * Vect.m_Vector[i];
		}
		ValInt+= m_Matrix[(3 * DIMMAT4X4) + Index];
		mat[Index]= ValInt;
	}

	double invW= 1.0;
	if (fabs(mat[3]) > 0.00001)
	{
		invW/= mat[3];
	}
	VectResult.m_Vector[0]= mat[0] * invW;
	VectResult.m_Vector[1]= mat[1] * invW;
	VectResult.m_Vector[2]= mat[2] * invW;


	return VectResult;
}

// Return true if the 2 matrix are equals
bool GLC_Matrix4x4::operator==(const GLC_Matrix4x4& mat) const
{
	bool result= true;
	int i= 0;
	while (result && (i < TAILLEMAT4X4))
	{
		result= (m_Matrix[i] == mat.m_Matrix[i]);
		++i;
	}
	return result;
}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set the matrix with Euler angle
GLC_Matrix4x4& GLC_Matrix4x4::fromEuler(const double angle_x, const double angle_y, const double angle_z)
{
    const double A= cos(angle_x);
    const double B= sin(angle_x);
    const double C= cos(angle_y);
    const double D= sin(angle_y);
    const double E= cos(angle_z);
    const double F= sin(angle_z);

    const double AD= A * D;
    const double BD= B * D;

    m_Matrix[0]  = C * E;
    m_Matrix[4]  = -C * F;
    m_Matrix[8]  = -D;
    m_Matrix[1]  = -BD * E + A * F;
    m_Matrix[5]  = BD * F + A * E;
    m_Matrix[9]  = -B * C;
    m_Matrix[2]  = AD * E + B * F;
    m_Matrix[6]  = -AD * F + B * E;
    m_Matrix[10] = A * C;

    m_Matrix[12]=  0.0; m_Matrix[13]= 0.0; m_Matrix[14]= 0.0; m_Matrix[3]= 0.0; m_Matrix[7]= 0.0; m_Matrix[11] = 0.0;
    m_Matrix[15] =  1.0;

	return *this;
}



//////////////////////////////////////////////////////////////////////
// Private services function
//////////////////////////////////////////////////////////////////////

// Compute matrix determinant
double GLC_Matrix4x4::getDeterminantLC(const int Ligne, const int Colonne) const
{
	double Mat3x3[9];
	double Determinant;

	getSubMat(Ligne, Colonne, Mat3x3);

	if ( 0 == ((Ligne + Colonne) % 2)) // Even number
		Determinant= m_Matrix[(Colonne + DIMMAT4X4) + Ligne] * getDeterminant3x3(Mat3x3);
	else
		Determinant= - m_Matrix[(Colonne + DIMMAT4X4) + Ligne] * getDeterminant3x3(Mat3x3);

	return Determinant;
}

// Return a vector which contains radians Euler angle of the matrix
QVector<double> GLC_Matrix4x4::toEuler(void) const
{
	double angle_x;
	double angle_y;
	double angle_z;
	double tracex, tracey;
	angle_y= -asin(m_Matrix[8]);
	double C= cos(angle_y);

	if (!qFuzzyCompare(C, 0.0)) // Gimball lock?
	{
		tracex= m_Matrix[10] / C;
		tracey= - m_Matrix[9] / C;
		angle_x= atan2( tracey, tracex);

		tracex= m_Matrix[0] / C;
		tracey= - m_Matrix[4] / C;
		angle_z= atan2( tracey, tracex);
	}
	else // Gimball lock?
	{
		angle_x= 0.0;
		tracex= m_Matrix[5] / C;
		tracey= m_Matrix[1] / C;
		angle_z= atan2( tracey, tracex);
	}
	QVector<double> result;
	result.append(fmod(angle_x, 2.0 * glc::PI));
	result.append(fmod(angle_y, 2.0 * glc::PI));
	result.append(fmod(angle_z, 2.0 * glc::PI));

	return result;
}

// Return the matrix string
QString GLC_Matrix4x4::toString() const
{
	QString result;
	for (int i= 0; i < DIMMAT4X4; ++i)
	{
		result+= (QString::number(m_Matrix[0 + i])) + QString(" ");
		result+= (QString::number(m_Matrix[4 + i])) + QString(" ");
		result+= (QString::number(m_Matrix[8 + i])) + QString(" ");
		result+= (QString::number(m_Matrix[12 + i])) + QString("\n");
	}
	result.remove(result.size() - 1, 1);
	return result;
}


// Compute Sub 3X3 matrix
void GLC_Matrix4x4::getSubMat(const int Ligne, const int Colonne, double *ResultMat) const
{

	int LigneResult;
	int ColonneResult;
	int IndexOrigine;
	int IndexResult;

	for (int ColonneOrigine= 0; ColonneOrigine < DIMMAT4X4; ColonneOrigine++)
	{
		if (ColonneOrigine != Colonne)
		{
			if (ColonneOrigine < Colonne)
				ColonneResult= ColonneOrigine;
			else
				ColonneResult= ColonneOrigine - 1;

			for (int LigneOrigine= 0; LigneOrigine < DIMMAT4X4; LigneOrigine++)
			{
				if (LigneOrigine != Ligne)
				{
					if (LigneOrigine < Ligne)
						LigneResult= LigneOrigine;
					else
						LigneResult= LigneOrigine - 1;
					IndexOrigine= (ColonneOrigine * DIMMAT4X4) + LigneOrigine;
					IndexResult= (ColonneResult * (DIMMAT4X4 - 1)) + LigneResult;

					ResultMat[IndexResult]= m_Matrix[IndexOrigine];
				}
			}
		}
	}
}

// Return determinant of a 3x3 matrix
double GLC_Matrix4x4::getDeterminant3x3(const double *Mat3x3) const
{
	double Determinant;

	Determinant= Mat3x3[0] * ( Mat3x3[4] * Mat3x3[8] - Mat3x3[7] * Mat3x3[5]);
	Determinant+= - Mat3x3[3] * ( Mat3x3[1] * Mat3x3[8] - Mat3x3[7] * Mat3x3[2]);
	Determinant+= Mat3x3[6] * ( Mat3x3[1] * Mat3x3[5] - Mat3x3[4] * Mat3x3[2]);

	return Determinant;
}

// Return transposed matrix
GLC_Matrix4x4 GLC_Matrix4x4::getTranspose(void) const
{
	GLC_Matrix4x4 MatT(m_Matrix);
	int IndexOrigine;
	int IndexTrans;
	for (int Colonne= 0; Colonne < DIMMAT4X4; Colonne++)
	{
		for (int Ligne=0 ; Ligne < DIMMAT4X4; Ligne++)
		{
			IndexOrigine= (Colonne * DIMMAT4X4) + Ligne;
			IndexTrans= (Ligne * DIMMAT4X4) + Colonne;

			MatT.m_Matrix[IndexTrans]= m_Matrix[IndexOrigine];
		}
	}

	return MatT;
}

// Return the comatrix
GLC_Matrix4x4 GLC_Matrix4x4::getCoMat4x4(void) const
{
	GLC_Matrix4x4 CoMat(m_Matrix);
	double SubMat3x3[9];
	int Index;

	for (int Colonne= 0; Colonne < DIMMAT4X4; Colonne++)
	{
		for (int Ligne=0 ; Ligne < DIMMAT4X4; Ligne++)
		{
			getSubMat(Ligne, Colonne, SubMat3x3);
			Index= (Colonne * DIMMAT4X4) + Ligne;
			if (((Colonne + Ligne + 2) % 2) == 0) // Even Number
				CoMat.m_Matrix[Index]= getDeterminant3x3(SubMat3x3);
			else
				CoMat.m_Matrix[Index]= -getDeterminant3x3(SubMat3x3);
		}
	}

	return CoMat;
}
