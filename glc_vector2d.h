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

//! \file glc_vector2d.h interface for the GLC_Vector2d class.

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
	friend class GLC_Vector4d;
	
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	/*! Default constructor
	*  Value is set to 
	* \n X = 0.0
	* \n Y =  0.0
	*/
	inline GLC_Vector2d()
	{
		dVecteur[0]= 0.0;
		dVecteur[1]= 0.0;
	}
	
	/*! Standard constructor With x, y = 0.0*/
	inline GLC_Vector2d(const double &dX, const double &dY)
	{
		dVecteur[0]= dX;
		dVecteur[1]= dY;
	}
	
	/*! Recopy constructor
	 * Sample use
	 * \code
	 * NewVect = new GLC_Vector2d(OldVect);
	 * \endcode
	 */
	inline GLC_Vector2d(const GLC_Vector2d &Vect)
	{
		dVecteur[0]= Vect.dVecteur[0];
		dVecteur[1]= Vect.dVecteur[1];
	}
//@}
	
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	/*! X Composante*/
	inline GLC_Vector2d& setX(const double &dX)
	{
		dVecteur[0]= dX;
		return *this;
	}
	
	/*! Y Composante*/
	inline GLC_Vector2d& setY(const double &dY)
	{
		dVecteur[1]= dY;
		return *this;
	}
		
	/*! All Composante*/
	inline GLC_Vector2d& setVect(const double &dX, const double &dY)
	{
		dVecteur[0]= dX;
		dVecteur[1]= dY;
		return *this;
	}
		
	/*! From another Vector*/
	inline GLC_Vector2d& setVect(const GLC_Vector2d &Vect)
	{
		dVecteur[0]= Vect.dVecteur[0];
		dVecteur[1]= Vect.dVecteur[1];
		return *this;
	}
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	/*! X Composante*/
	inline double getX(void) const
	{
		return dVecteur[0];
	}
	/*! Y Composante*/
	inline double getY(void) const
	{
		return dVecteur[1];
	}
	/*! retourne un pointeur constant vers le tableau du vecteur.*/
	inline const double *return_dVect(void) const
	{
		return dVecteur;
	}
	/*! Vector is null*/
	inline bool isNull(void) const
	{
		return (fabs(dVecteur[0]) < EPSILON) && (fabs(dVecteur[1]) < EPSILON);
	}

//@}

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
