/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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


//! \file glc_vector3d.cpp implementation of the GLC_Vector3d class.
#include <QtDebug>
#include "glc_vector3d.h"


//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

/*! Default constructor
*  Value is set to 
* \n X = 0.0
* \n Y =  0.0
* \n Z =  0.0
*/

GLC_Vector3d::GLC_Vector3d()
{
	dVecteur[0]= 0.0;
	dVecteur[1]= 0.0;
	dVecteur[2]= 0.0;

}
/*! Recopy constructor
 * Sample use
 * \code
 * NewVect = new GLC_Vector3d(OldVect);
 * \endcode
 */
GLC_Vector3d::GLC_Vector3d(const GLC_Vector3d &Vect)
{
	dVecteur[0]= Vect.dVecteur[0];
	dVecteur[1]= Vect.dVecteur[1];
	dVecteur[2]= Vect.dVecteur[2];

}


//////////////////////////////////////////////////////////////////////
// Fonction Set
//////////////////////////////////////////////////////////////////////
GLC_Vector3d& GLC_Vector3d::setX(const double &dX)
{
	dVecteur[0]= dX;
	return *this;
}

GLC_Vector3d& GLC_Vector3d::setY(const double &dY)
{
	dVecteur[1]= dY;
	return *this;
}

GLC_Vector3d& GLC_Vector3d::setZ(const double &dZ)
{
	dVecteur[2]= dZ;
	return *this;
}


GLC_Vector3d& GLC_Vector3d::setVect(const double &dX, const double &dY, const double &dZ)
{
	dVecteur[0]= dX;
	dVecteur[1]= dY;
	dVecteur[2]= dZ;

	return *this;
}
GLC_Vector3d& GLC_Vector3d::setVect(const GLC_Vector3d &Vect)
{
	dVecteur[0]= Vect.dVecteur[0];
	dVecteur[1]= Vect.dVecteur[1];
	dVecteur[2]= Vect.dVecteur[2];
	return *this;
}


