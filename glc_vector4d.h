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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

//! \file glc_vector4d.h interface for the GLC_Vector4d class.

#ifndef GLC_VECTOR4D_H_
#define GLC_VECTOR4D_H_

#include "glc_utils_maths.h"
//////////////////////////////////////////////////////////////////////
// définition global
//////////////////////////////////////////////////////////////////////

//! Vector dimension
#define VECT4DIMENSION 4

//////////////////////////////////////////////////////////////////////
//! \class GLC_Vector4d
/*! \brief GLC_Vector4d is a 4 dimensions Vector*/

/*! GLC_Vector4d is used to represent 3D position and vectors. \n
 *  it had 4 Dimensions for compatibility purpose with GLC_Matrix4x4
 * */
//////////////////////////////////////////////////////////////////////

class GLC_Vector4d  
{
	/*! GLC_Matrix4x4 class*/
	friend class GLC_Matrix4x4;
	
	/*! Overload unary "-" operator*/
	friend GLC_Vector4d operator - (const GLC_Vector4d &Vect);

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	GLC_Vector4d();
	/*! Standard constructor With x, y, z and w with default value of 1.0*/
	GLC_Vector4d(const double &dX, const double &dY, const double &dZ, const double &dW= 1.0)
	{
		SetVect(dX, dY, dZ, dW);
	}
	// Recopy constructor
	GLC_Vector4d(const GLC_Vector4d &Vect);
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Operator Overload */
//@{
//////////////////////////////////////////////////////////////////////

public:

	/*! Overload binary "+" operator*/
	GLC_Vector4d operator + (const GLC_Vector4d &Vect) const;

	/*! Overload "+=" operator*/
	GLC_Vector4d* operator += (const GLC_Vector4d &Vect);
	
	/*! Overload binary "-" operator*/
	GLC_Vector4d operator - (const GLC_Vector4d &Vect) const;

	/*! Overload binary "-=" operator*/
	GLC_Vector4d* operator -= (const GLC_Vector4d &Vect);

	/*! Overload dot product "^" operator*/
	GLC_Vector4d operator ^ (const GLC_Vector4d &Vect) const;

	/*! Overload scalar product "*" operator between 2 vector*/
	const double operator * (const GLC_Vector4d &Vect) const;

	/*! Overload scalar product "*" operator between 1 vector and one scalar*/
	GLC_Vector4d GLC_Vector4d::operator * (double Scalaire) const;
	
	/*! Overload dot product "==" operator*/
	bool operator == (const GLC_Vector4d &Vect) const;

	/*! Overload dot product "!=" operator*/
	bool operator != (const GLC_Vector4d &Vect) const;
//@}
	
//////////////////////////////////////////////////////////////////////
/*! \name Set Function*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	/*! X Composante*/
	GLC_Vector4d& SetX(const double &dX);
	
	/*! Y Composante*/
	GLC_Vector4d& SetY(const double &dY);
	
	/*! Z Composante*/
	GLC_Vector4d& SetZ(const double &dZ);
	
	/*! W Composante*/
	GLC_Vector4d& SetW(const double &dW);
	
	/*! All Composante*/
	GLC_Vector4d& SetVect(const double &dX, const double &dY,
		const double &dZ, const double &dW= 1);
		
	/*! From another Vector*/
	GLC_Vector4d& SetVect(const GLC_Vector4d &Vect);
	
	/*! Vector Normal*/
	GLC_Vector4d& SetNormal(const double &Norme);
	
	/*! Invert Vector*/
	GLC_Vector4d& SetInv(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Function*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	/*! X Composante*/
	inline const double GetX(void) const
	{
		return dVecteur[0];
	}
	/*! Y Composante*/
	inline const double GetY(void) const
	{
		return dVecteur[1];
	}
	/*! Z Composante*/
	inline const double GetZ(void) const
	{
		return dVecteur[2];
	}
	/*! W Composante*/
	inline const double GetW(void) const
	{
		return dVecteur[3];
	}
	/*! retourne un pointeur constant vers le tableau du vecteur.*/
	inline const double *Return_dVect(void) const
	{
		return dVecteur;
	}
	/*! Vector Norm*/
	inline const double GetNorme(void) const
	{
		return CalcNorme();
	}
	/*! Vector is null*/
	inline const bool IsNull(void) const
	{
		bool result;

		result= (fabs(dVecteur[0]) < EPSILON) && (fabs(dVecteur[1]) < EPSILON)
			&& (fabs(dVecteur[2]) < EPSILON);
		
		return result;
	}

	/*! Angle between another vector*/
	const double GetAngleWithVect(GLC_Vector4d Vect) const;
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services function*/
//@{
//////////////////////////////////////////////////////////////////////
private:
	/*! Compute Vector normal*/
	inline const double CalcNorme(void) const
	{
		return sqrt(dVecteur[0] * dVecteur[0] + dVecteur[1] * dVecteur[1]
			+ dVecteur[2] * dVecteur[2]);
	}
	
	/*! Normalize Vector w <- 0*/
	void Normalise(void);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private attributes*/
//@{
//////////////////////////////////////////////////////////////////////
private:
	/*! Vector array definition \n
	*	dVecteur[0]	X \n
	*	dVecteur[1]	Y \n
	*	dVecteur[2]	Z \n
	*	dVecteur[3]	1
	*/
	double dVecteur[4];

}; //class GLC_Vector4d

//@}

// Axis definition
/*! \var AxeX
 *  \brief X axis Vector*/
const GLC_Vector4d AxeX(1,0,0);

/*! \var AxeY
 *  \brief Y axis Vector*/
const GLC_Vector4d AxeY(0,1,0);

/*! \var AxeZ
 *  \brief Z axis Vector*/
const GLC_Vector4d AxeZ(0,0,1);

#endif /*GLC_VECTOR4D_H_*/
