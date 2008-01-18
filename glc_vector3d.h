/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.8, packaged on January, 2008.

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

//! \file glc_vector3d.h interface for the GLC_Vector3d class.

#ifndef GLC_VECTOR3D_H_
#define GLC_VECTOR3D_H_

#include "glc_utils_maths.h"
//////////////////////////////////////////////////////////////////////
// définition global
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//! \class GLC_Vector3d
/*! \brief GLC_Vector3d is a 3 dimensions Vector*/

/*! GLC_Vector3d is used to represent 3D position and vectors.
 * */
//////////////////////////////////////////////////////////////////////

class GLC_Vector3d  
{
	
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	GLC_Vector3d();
	/*! Standard constructor With x, y, z and w with default value of 1.0*/
	GLC_Vector3d(const double &dX, const double &dY, const double &dZ)
	{
		setVect(dX, dY, dZ);
	}
	// Recopy constructor
	GLC_Vector3d(const GLC_Vector3d &Vect);
//@}

	
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	/*! X Composante*/
	GLC_Vector3d& setX(const double &dX);
	
	/*! Y Composante*/
	GLC_Vector3d& setY(const double &dY);
	
	/*! Z Composante*/
	GLC_Vector3d& setZ(const double &dZ);
	
	/*! All Composante*/
	GLC_Vector3d& setVect(const double &dX, const double &dY,
		const double &dZ);
		
	/*! From another Vector*/
	GLC_Vector3d& setVect(const GLC_Vector3d &Vect);
	
	/*! Invert Vector*/
	GLC_Vector3d& setInv(void);
	
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	/*! X Composante*/
	inline const double getX(void) const
	{
		return dVecteur[0];
	}
	/*! Y Composante*/
	inline const double getY(void) const
	{
		return dVecteur[1];
	}
	/*! Z Composante*/
	inline const double getZ(void) const
	{
		return dVecteur[2];
	}
	/*! retourne un pointeur constant vers le tableau du vecteur.*/
	inline const double *return_dVect(void) const
	{
		return dVecteur;
	}
	/*! Vector is null*/
	inline const bool isNull(void) const
	{
		bool result;

		result= (fabs(dVecteur[0]) < EPSILON) && (fabs(dVecteur[1]) < EPSILON)
			&& (fabs(dVecteur[2]) < EPSILON);
		
		return result;
	}

//@}

//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//name Private attributes
//////////////////////////////////////////////////////////////////////
private:
	/*! Vector array definition \n
	*	dVecteur[0]	X \n
	*	dVecteur[1]	Y \n
	*	dVecteur[2]	Z \n
	*/
	double dVecteur[3];

}; //class GLC_Vector3d


#endif /*GLC_VECTOR3D_H_*/
