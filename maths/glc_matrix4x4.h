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

//! \file glc_matrix4x4.h interface for the GLC_Matrix4x4 class.

#ifndef GLC_MATRIX4X4_H_
#define GLC_MATRIX4X4_H_

#include <QVector>
#include "glc_vector3d.h"

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Matrix4x4
/*! \brief GLC_Matrix4x4 is a 4 dimensions Matrix*/

/*! GLC_Matrix4x4 is used to represent 3D homogeneous 3d transformation \n
 *  GLC_Matrix4x4 is a row first matrix compatible with OpenGL Matrix
 * */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_Matrix4x4
{
	friend class GLC_Vector3d;

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
public:
//! @name Constructor
//@{
	//! Default Constructor
	/*! Create an identity Matrix */
	inline GLC_Matrix4x4();

	//! Construct a Matrix by copy
	inline GLC_Matrix4x4(const GLC_Matrix4x4 &matrix)
	{memcpy(m_Matrix, matrix.m_Matrix, sizeof(double) * 16);}

	//! Construct a Matrix by an array of 16 elements.
	inline GLC_Matrix4x4(const double *pArray)
	{memcpy(m_Matrix, pArray, sizeof(double) * 16);}

	//! Construct a Matrix by an array of 16 elements.
	inline GLC_Matrix4x4(const float *);

	//! Construct rotation matrix by a vector and an angle
	inline GLC_Matrix4x4(const GLC_Vector3d &Vect, const double &dAngleRad)
	{setMatRot(Vect, dAngleRad);}

	//! Construct rotation matrix by 2 vectors
	inline GLC_Matrix4x4(const GLC_Vector3d &Vect1, const GLC_Vector3d &Vect2)
	{setMatRot(Vect1, Vect2);}

	//! Construct translation matrix by a vector
	inline GLC_Matrix4x4(const GLC_Vector3d &Vect)
	{setMatTranslate(Vect);}

	//! Construct translation matrix by 3 coordinates
	inline GLC_Matrix4x4(const double Tx, const double Ty, const double Tz)
	{setMatTranslate(Tx, Ty, Tz);}
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Operator Overload */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Matrix cross product
	GLC_Matrix4x4 operator * (const GLC_Matrix4x4 &Mat) const;

	//! Vector transformation
	GLC_Vector3d operator * (const GLC_Vector3d &Vect) const;

	//! Return true if the 2 matrix are equals
	bool operator==(const GLC_Matrix4x4& mat) const;

	//! Return true if the 2 matrix are not equals
	inline bool operator!=(const GLC_Matrix4x4& mat) const
	{return !operator==(mat);}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Function*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the matrix determinant
	inline double determinant(void) const;

	//! Return a pointer to a row first array of 16 elements
	inline const double* data(void) const
	{return m_Matrix;}

	//! Return a pointer to a row first array of 16 elements
	inline double* data(void)
	{return m_Matrix;}

	//! Return a vector which contains radians Euler angle of the matrix
	QVector<double> toEuler(void) const;

	//! Return the matrix string
	QString toString() const;

	//! Return the rotation matrix of this matrix
	inline GLC_Matrix4x4 rotationMatrix() const;

	//! Return the isometric of this matrix
	inline GLC_Matrix4x4 isometricMatrix() const;

	//! Return the x Scaling
	inline double scalingX() const
	{return GLC_Vector3d(m_Matrix[0], m_Matrix[1], m_Matrix[2]).lenght();}

	//! Return the y Scaling
	inline double scalingY() const
	{return GLC_Vector3d(m_Matrix[4], m_Matrix[5], m_Matrix[6]).lenght();}

	//! Return the z Scaling
	inline double scalingZ() const
	{return GLC_Vector3d(m_Matrix[8], m_Matrix[9], m_Matrix[10]).lenght();}

	//! Return the inverted matrix
	inline GLC_Matrix4x4 inverted() const
	{return GLC_Matrix4x4(*this).invert();}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Function*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set matrix to a rotation matrix define by a vector and an angle in radians
	inline GLC_Matrix4x4& setMatRot(const GLC_Vector3d &, const double &);

	//! Set matrix to a rotation matrix define by 2 vectors
	inline GLC_Matrix4x4& setMatRot(const GLC_Vector3d &, const GLC_Vector3d &);

	//! Set Matrix to a translation matrix by a vector
	inline GLC_Matrix4x4& setMatTranslate(const GLC_Vector3d &);

	//! Set Matrix to a translation matrix by 3 coordinates
	inline GLC_Matrix4x4& setMatTranslate(const double, const double, const double);

	//! Set Matrix to a scaling matrix define by 3 double
	inline GLC_Matrix4x4& setMatScaling(const double, const double, const double);

	//! Reverse the Matrix
	inline GLC_Matrix4x4& invert(void);

	//! Set the matrix to identify matrix
	inline GLC_Matrix4x4& setToIdentity();

	//! Set the matrix by its transpose
	inline GLC_Matrix4x4& transpose(void);

	//! Set the matrix with Euler angle
	GLC_Matrix4x4& fromEuler(const double, const double, const double);


//@}

//////////////////////////////////////////////////////////////////////
//! Private services Functions
//////////////////////////////////////////////////////////////////////
private:

	//! Return true if the index (argument) is in the diagonal of the matrix
	inline bool isInDiagonal(const int index) const
	{
		if ((index == 0) || (index == 5) || (index == 10) || (index == 15))
			return true;
		else
			return false;
	}

	//! Return the determinant of a matrix cell
	double getDeterminantLC(const int, const int) const;

	//! Compute Sub 3X3 matrix
	void getSubMat(const int, const int, double *) const;

	// Return the Matrix 3X3 determinant
	double getDeterminant3x3(const double *) const;

	//! Return the transpose matrix
	GLC_Matrix4x4 getTranspose(void) const;

	//! Return the co-matrix
	GLC_Matrix4x4 getCoMat4x4(void) const;



//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! Number of elements in the matrix
	enum {TAILLEMAT4X4 = 16};
	//! Matrix size
	enum {DIMMAT4X4 = 4};
	//! Matrix row first array
	double m_Matrix[TAILLEMAT4X4];
/*
the matrix :
					a[00] a[04] a[08] a[12]

					a[01] a[05] a[09] a[13]

					a[02] a[06] a[10] a[14]

					a[03] a[07] a[11] a[15]
 */
//					Tx = 12,	Ty = 13,	Tz = 14
};

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

// Default Constructor
GLC_Matrix4x4::GLC_Matrix4x4()
{
	m_Matrix[0]= 1.0; m_Matrix[4]= 0.0; m_Matrix[8]=  0.0; m_Matrix[12]= 0.0;
	m_Matrix[1]= 0.0; m_Matrix[5]= 1.0; m_Matrix[9]=  0.0; m_Matrix[13]= 0.0;
	m_Matrix[2]= 0.0; m_Matrix[6]= 0.0; m_Matrix[10]= 1.0; m_Matrix[14]= 0.0;
	m_Matrix[3]= 0.0; m_Matrix[7]= 0.0; m_Matrix[11]= 0.0; m_Matrix[15]= 1.0;
}

// Construct a Matrix by an array of 16 elements.
GLC_Matrix4x4::GLC_Matrix4x4(const float *Tableau)
{

	for (int i=0; i < TAILLEMAT4X4; i++)
	{
		m_Matrix[i]= static_cast<double>(Tableau[i]);
	}
}

// Return the rotation matrix of this matrix
GLC_Matrix4x4 GLC_Matrix4x4::rotationMatrix() const
{
	GLC_Matrix4x4 result(*this);
	const double invScaleX= 1.0 / scalingX();
	const double invScaleY= 1.0 / scalingY();
	const double invScaleZ= 1.0 / scalingZ();
	result.m_Matrix[0]= result.m_Matrix[0] * invScaleX;
	result.m_Matrix[1]= result.m_Matrix[1] * invScaleX;
	result.m_Matrix[2]= result.m_Matrix[2] * invScaleX;

	result.m_Matrix[4]= result.m_Matrix[4] * invScaleY;
	result.m_Matrix[5]= result.m_Matrix[5] * invScaleY;
	result.m_Matrix[6]= result.m_Matrix[6] * invScaleY;

	result.m_Matrix[8]= result.m_Matrix[8] * invScaleZ;
	result.m_Matrix[9]= result.m_Matrix[9] * invScaleZ;
	result.m_Matrix[10]= result.m_Matrix[10] * invScaleZ;

	result.m_Matrix[12]= 0.0; result.m_Matrix[13]= 0.0; result.m_Matrix[14]= 0.0;
	result.m_Matrix[3]= 0.0; result.m_Matrix[7]= 0.0; result.m_Matrix[11]= 0.0;
	result.m_Matrix[15]= 1.0;
	return result;
}

// Return the isometric of this matrix
GLC_Matrix4x4 GLC_Matrix4x4::isometricMatrix() const
{
	GLC_Matrix4x4 result(*this);
	const double invScaleX= 1.0 / scalingX();
	const double invScaleY= 1.0 / scalingY();
	const double invScaleZ= 1.0 / scalingZ();
	result.m_Matrix[0]= result.m_Matrix[0] * invScaleX;
	result.m_Matrix[1]= result.m_Matrix[1] * invScaleX;
	result.m_Matrix[2]= result.m_Matrix[2] * invScaleX;

	result.m_Matrix[4]= result.m_Matrix[4] * invScaleY;
	result.m_Matrix[5]= result.m_Matrix[5] * invScaleY;
	result.m_Matrix[6]= result.m_Matrix[6] * invScaleY;

	result.m_Matrix[8]= result.m_Matrix[8] * invScaleZ;
	result.m_Matrix[9]= result.m_Matrix[9] * invScaleZ;
	result.m_Matrix[10]= result.m_Matrix[10] * invScaleZ;
	return result;
}
// Set matrix to a rotation matrix define by a vector and an angle in radians
GLC_Matrix4x4& GLC_Matrix4x4::setMatRot(const GLC_Vector3d &Vect, const double &dAngleRad)
{
	// Normalize the vector
	GLC_Vector3d VectRot(Vect);
	VectRot.normalize();

	// Code optimisation
	const double SinAngleSur2= sin(dAngleRad / 2.0);

	// Quaternion computation
	const double q0= cos(dAngleRad / 2);
	const double q1= VectRot.m_Vector[0] * SinAngleSur2;
	const double q2= VectRot.m_Vector[1] * SinAngleSur2;
	const double q3= VectRot.m_Vector[2] * SinAngleSur2;

	// Code optimisation
	const double q0Carre= (q0 * q0);
	const double q1Carre= (q1 * q1);
	const double q2Carre= (q2 * q2);
	const double q3Carre= (q3 * q3);

	m_Matrix[0]= q0Carre + q1Carre - q2Carre - q3Carre;
	m_Matrix[1]= 2.0 * (q1 *q2 + q0 * q3);
	m_Matrix[2]= 2.0 * (q1 * q3 - q0 * q2);
	m_Matrix[3]= 0.0;
	m_Matrix[4]= 2.0 * (q1 * q2 - q0 * q3);
	m_Matrix[5]= q0Carre + q2Carre - q3Carre - q1Carre;
	m_Matrix[6]= 2.0 * (q2 * q3 + q0 * q1);
	m_Matrix[7]= 0.0;
	m_Matrix[8]= 2.0 * (q1 * q3 + q0 * q2);
	m_Matrix[9]= 2.0 * (q2 * q3 - q0 * q1);
	m_Matrix[10]= q0Carre + q3Carre - q1Carre - q2Carre;
	m_Matrix[11]= 0.0;

	m_Matrix[12]= 0.0;	//TX
	m_Matrix[13]= 0.0;	//TY
	m_Matrix[14]= 0.0;	//TZ
	m_Matrix[15]= 1.0;

	return *this;
}

// Set matrix to a rotation matrix define by 2 vectors
GLC_Matrix4x4& GLC_Matrix4x4::setMatRot(const GLC_Vector3d &Vect1, const GLC_Vector3d &Vect2)
{

	// Compute rotation matrix
	const GLC_Vector3d VectAxeRot(Vect1 ^ Vect2);
	// Check if rotation vector axis is not null
	if (!VectAxeRot.isNull())
	{  // Ok, vector not null
		const double Angle= acos(Vect1 * Vect2);
		setMatRot(VectAxeRot, Angle);
	}

	return *this;
}

// Set Matrix to a translation matrix by a vector
GLC_Matrix4x4& GLC_Matrix4x4::setMatTranslate(const GLC_Vector3d &Vect)
{
	m_Matrix[0]= 1.0; m_Matrix[4]= 0.0; m_Matrix[8]=  0.0; m_Matrix[12]= Vect.m_Vector[0];
	m_Matrix[1]= 0.0; m_Matrix[5]= 1.0; m_Matrix[9]=  0.0; m_Matrix[13]= Vect.m_Vector[1];
	m_Matrix[2]= 0.0; m_Matrix[6]= 0.0; m_Matrix[10]= 1.0; m_Matrix[14]= Vect.m_Vector[2];
	m_Matrix[3]= 0.0; m_Matrix[7]= 0.0; m_Matrix[11]= 0.0; m_Matrix[15]= 1.0;

	return *this;
}
// Set Matrix to a translation matrix by 3 coordinates
GLC_Matrix4x4& GLC_Matrix4x4::setMatTranslate(const double Tx, const double Ty, const double Tz)
{
	m_Matrix[0]= 1.0; m_Matrix[4]= 0.0; m_Matrix[8]=  0.0; m_Matrix[12]= Tx;
	m_Matrix[1]= 0.0; m_Matrix[5]= 1.0; m_Matrix[9]=  0.0; m_Matrix[13]= Ty;
	m_Matrix[2]= 0.0; m_Matrix[6]= 0.0; m_Matrix[10]= 1.0; m_Matrix[14]= Tz;
	m_Matrix[3]= 0.0; m_Matrix[7]= 0.0; m_Matrix[11]= 0.0; m_Matrix[15]= 1.0;

	return *this;
}

// Set Matrix to a scaling matrix define by 3 double
GLC_Matrix4x4& GLC_Matrix4x4::setMatScaling(const double sX, const double sY, const double sZ)
{
	m_Matrix[0]= sX; m_Matrix[4]= 0.0; m_Matrix[8]=  0.0; m_Matrix[12]= 0.0;
	m_Matrix[1]= 0.0; m_Matrix[5]= sY; m_Matrix[9]=  0.0; m_Matrix[13]= 0.0;
	m_Matrix[2]= 0.0; m_Matrix[6]= 0.0; m_Matrix[10]= sZ; m_Matrix[14]= 0.0;
	m_Matrix[3]= 0.0; m_Matrix[7]= 0.0; m_Matrix[11]= 0.0; m_Matrix[15]= 1.0;

	return *this;
}


// Reverse the Matrix
GLC_Matrix4x4& GLC_Matrix4x4::invert(void)
{
	const double det= determinant();

	// Test if the inverion is possible
	if (det == 0.0f) return *this;

	const double invDet = 1.0 / det;
	GLC_Matrix4x4 TCoMat= getCoMat4x4().getTranspose();

	for (int i= 0; i < TAILLEMAT4X4; i++)
	{
		m_Matrix[i]= TCoMat.m_Matrix[i] * invDet;
	}

	return *this;
}
// Set the matrix to identify matrix
GLC_Matrix4x4& GLC_Matrix4x4::setToIdentity()
{
	m_Matrix[0]= 1.0; m_Matrix[4]= 0.0; m_Matrix[8]=  0.0; m_Matrix[12]= 0.0;
	m_Matrix[1]= 0.0; m_Matrix[5]= 1.0; m_Matrix[9]=  0.0; m_Matrix[13]= 0.0;
	m_Matrix[2]= 0.0; m_Matrix[6]= 0.0; m_Matrix[10]= 1.0; m_Matrix[14]= 0.0;
	m_Matrix[3]= 0.0; m_Matrix[7]= 0.0; m_Matrix[11]= 0.0; m_Matrix[15]= 1.0;

	return *this;
}

// Set the matrix by its transpose
GLC_Matrix4x4& GLC_Matrix4x4::transpose(void)
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

	// Load the transposed in matrix in this matrix
	memcpy(m_Matrix, MatT.m_Matrix, sizeof(double) * 16);

	return *this;
}

// Compute matrix determinant
double GLC_Matrix4x4::determinant(void) const
{
	double Determinant= 0.0;
	double SubMat3x3[9];
	int Signe= 1;

	for (int Colonne= 0; Colonne < DIMMAT4X4; Colonne++, Signe*= -1)
	{
		getSubMat(0, Colonne, SubMat3x3);
		Determinant+= Signe * m_Matrix[Colonne * DIMMAT4X4] * getDeterminant3x3(SubMat3x3);
	}

	return Determinant;

}

#endif /*GLC_MATRIX4X4_H_*/
