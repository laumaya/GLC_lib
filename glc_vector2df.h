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

//! \file glc_vector2df.h interface for the GLC_Vector2df class.

#ifndef GLC_VECTOR2DF_H_
#define GLC_VECTOR2DF_H_

#include "glc_utils_maths.h"
//////////////////////////////////////////////////////////////////////
// definition global
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//! \class GLC_Vector2df
/*! \brief GLC_Vector2df is a 2 dimensions Vector*/

/*! GLC_Vector2df is used to represent 2D position and vectors.
 * */
//////////////////////////////////////////////////////////////////////

class GLC_Vector2df  
{
	friend class GLC_Vector2d;
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
	inline GLC_Vector2df()
	{
		dVecteur[0]= 0.0f;
		dVecteur[1]= 0.0f;
	}
	
	/*! Standard constructor With x, y = 0.0*/
	inline GLC_Vector2df(const float &dX, const float &dY)
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
	inline GLC_Vector2df(const GLC_Vector2df &Vect)
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
	inline GLC_Vector2df& setX(const float &dX)
	{
		dVecteur[0]= dX;
		return *this;
	}
	
	/*! Y Composante*/
	inline GLC_Vector2df& setY(const float &dY)
	{
		dVecteur[1]= dY;
		return *this;
	}
		
	/*! All Composante*/
	inline GLC_Vector2df& setVect(const float &dX, const float &dY)
	{
		dVecteur[0]= dX;
		dVecteur[1]= dY;
		return *this;
	}
		
	/*! From another Vector*/
	inline GLC_Vector2df& setVect(const GLC_Vector2df &Vect)
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
	inline float getX(void) const
	{
		return dVecteur[0];
	}
	/*! Y Composante*/
	inline float getY(void) const
	{
		return dVecteur[1];
	}
	/*! retourne un pointeur constant vers le tableau du vecteur.*/
	inline const float *return_dVect(void) const
	{
		return dVecteur;
	}
	/*! Vector is null*/
	inline bool isNull(void) const
	{
		return (fabs(dVecteur[0]) < glc::EPSILON) && (fabs(dVecteur[1]) < glc::EPSILON);
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
	float dVecteur[2];

}; //class GLC_Vector2df

//! Define GLC_Point2D
typedef GLC_Vector2df GLC_Point2df;

#endif /*GLC_VECTOR2DF_H_*/
