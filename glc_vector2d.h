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

//! \file glc_vector2.h interface for the GLC_Vector2d class.

#ifndef GLC_VECTOR2D_H_
#define GLC_VECTOR2D_H_

#include "glc_utils_maths.h"
//////////////////////////////////////////////////////////////////////
// définition global
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//! \class GLC_Vector2d
/*! \brief GLC_Vector2d is a 2 dimensions Vector*/

/*! GLC_Vector2d is used to represent 2D position and vectors.
 * */
//////////////////////////////////////////////////////////////////////

class GLC_Vector2d  
{
	
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	GLC_Vector2d();
	/*! Standard constructor With x, y = 0.0*/
	GLC_Vector2d(const double &dX, const double &dY)
	{
		SetVect(dX, dY);
	}
	// Recopy constructor
	GLC_Vector2d(const GLC_Vector2d &Vect);
//@}

	
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	/*! X Composante*/
	GLC_Vector2d& SetX(const double &dX);
	
	/*! Y Composante*/
	GLC_Vector2d& SetY(const double &dY);
		
	/*! All Composante*/
	GLC_Vector2d& SetVect(const double &dX, const double &dY);
		
	/*! From another Vector*/
	GLC_Vector2d& SetVect(const GLC_Vector2d &Vect);
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
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
	/*! retourne un pointeur constant vers le tableau du vecteur.*/
	inline const double *Return_dVect(void) const
	{
		return dVecteur;
	}
	/*! Vector is null*/
	inline const bool IsNull(void) const
	{
		bool result;

		result= (fabs(dVecteur[0]) < EPSILON) && (fabs(dVecteur[1]) < EPSILON);
		
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
	*/
	double dVecteur[2];

}; //class GLC_Vector2d


#endif /*GLC_VECTOR2D_H_*/
