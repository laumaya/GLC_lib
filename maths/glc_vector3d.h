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

//! \file glc_vector3d.h interface for the GLC_Vector3d class.

#ifndef GLC_VECTOR3D_H_
#define GLC_VECTOR3D_H_

#include <QDataStream>

#include "glc_utils_maths.h"
#include "glc_vector3df.h"
#include "glc_vector2d.h"
#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Vector3d
/*! \brief GLC_Vector3d is a 3 dimensions Vector*/

/*! GLC_Vector3d is used to represent 3D position and vectors.
 * */
//////////////////////////////////////////////////////////////////////

class GLC_LIB_EXPORT GLC_Vector3d
{
	friend class GLC_Vector4d;
	friend class GLC_Matrix4x4;
	//! Overload unary "-" operator
	inline friend GLC_Vector3d operator - (const GLC_Vector3d &Vect)
	{
		return GLC_Vector3d(-Vect.vector[0], -Vect.vector[1], -Vect.vector[2]);
	}

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	/*! Default constructor
	*  Value is set to
	* \n X = 0.0
	* \n Y =  0.0
	* \n Z =  0.0
	*/
	inline GLC_Vector3d()
	{
		vector[0]= 0.0;
		vector[1]= 0.0;
		vector[2]= 0.0;
	}

	/*! Standard constructor With x, y, z*/
	inline GLC_Vector3d(const double &dX, const double &dY, const double &dZ)
	{
		vector[0]= dX;
		vector[1]= dY;
		vector[2]= dZ;
	}

	/*! Copy constructor
	 * Sample use
	 * \code
	 * NewVect = new GLC_Vector3d(OldVect);
	 * \endcode
	 */
	inline GLC_Vector3d(const GLC_Vector3d &Vect)
	{
		vector[0]= Vect.vector[0];
		vector[1]= Vect.vector[1];
		vector[2]= Vect.vector[2];
	}
	/*! Copy constructor from a float vector
	 * Sample use
	 * \code
	 * NewVect = new GLC_Vector3d(OldVectf);
	 * \endcode
	 */
	inline GLC_Vector3d(const GLC_Vector3df &Vect)
	{
		vector[0]= static_cast<double>(Vect.dVector[0]);
		vector[1]= static_cast<double>(Vect.dVector[1]);
		vector[2]= static_cast<double>(Vect.dVector[2]);
	}

//@}

//////////////////////////////////////////////////////////////////////
/*! @name Operator Overload */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Overload binary "+" operator
	inline GLC_Vector3d operator + (const GLC_Vector3d &Vect) const
	{
		GLC_Vector3d VectResult(vector[0] + Vect.vector[0], vector[1] + Vect.vector[1],
			vector[2] + Vect.vector[2]);

		return VectResult;
	}


	//! Overload "=" operator
	inline GLC_Vector3d& operator = (const GLC_Vector3d &Vect)
	{
		vector[0]= Vect.vector[0];
		vector[1]= Vect.vector[1];
		vector[2]= Vect.vector[2];

		return *this;
	}

	//! Overload "=" operator
	inline GLC_Vector3d& operator = (const GLC_Vector3df &Vect)
	{
		vector[0]= static_cast<double>(Vect.dVector[0]);
		vector[1]= static_cast<double>(Vect.dVector[1]);
		vector[2]= static_cast<double>(Vect.dVector[2]);

		return *this;
	}

	//! Overload "+=" operator
	inline GLC_Vector3d* operator += (const GLC_Vector3d &Vect)
	{
		*this= *this + Vect;
		return this;
	}


	//! Overload binary "-" operator
	inline GLC_Vector3d operator - (const GLC_Vector3d &Vect) const
	{
		GLC_Vector3d VectResult(vector[0] - Vect.vector[0], vector[1] - Vect.vector[1],
			vector[2] - Vect.vector[2]);

		return VectResult;
	}

	//! Overload binary "-=" operator
	GLC_Vector3d* operator -= (const GLC_Vector3d &Vect)
	{
		*this= *this - Vect;
		return this;
	}

	//! Overload dot product "^" operator
	inline GLC_Vector3d operator ^ (const GLC_Vector3d &Vect) const
	{
		GLC_Vector3d VectResult;
		VectResult.vector[0]= vector[1] * Vect.vector[2] - vector[2] * Vect.vector[1];
		VectResult.vector[1]= vector[2] * Vect.vector[0] - vector[0] * Vect.vector[2];
		VectResult.vector[2]= vector[0] * Vect.vector[1] - vector[1] * Vect.vector[0];

		return VectResult;
	}

	//! Overload scalar product "*" operator between 2 vector
	inline double operator * (const GLC_Vector3d &Vect) const
	{
		return vector[0] * Vect.vector[0] + vector[1] * Vect.vector[1] +
			vector[2] * Vect.vector[2];
	}

	//! Overload scalar product "*" operator between 1 vector and one scalar
	inline GLC_Vector3d operator * (double Scalaire) const
	{
		return GLC_Vector3d(vector[0] * Scalaire, vector[1] * Scalaire, vector[2] * Scalaire);
	}


	//! Overload equality "==" operator
	bool operator == (const GLC_Vector3d &Vect) const
	{
		bool bResult= qFuzzyCompare(vector[0], Vect.vector[0]);
		bResult= bResult && qFuzzyCompare(vector[1], Vect.vector[1]);
		bResult= bResult && qFuzzyCompare(vector[2], Vect.vector[2]);

		return bResult;
	}

	//! Overload dot product "!=" operator
	inline bool operator != (const GLC_Vector3d &Vect) const
	{
		return !(*this == Vect);
	}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! X Compound
	inline GLC_Vector3d& setX(const double &dX)
	{
		vector[0]= dX;
		return *this;
	}

	//! Y Compound
	inline GLC_Vector3d& setY(const double &dY)
	{
		vector[1]= dY;
		return *this;
	}

	//! Z Compound
	inline GLC_Vector3d& setZ(const double &dZ)
	{
		vector[2]= dZ;
		return *this;
	}

	//! All Compound
	inline GLC_Vector3d& setVect(const double &dX, const double &dY, const double &dZ)
	{
		vector[0]= dX;
		vector[1]= dY;
		vector[2]= dZ;

		return *this;
	}

	//! From another Vector
	GLC_Vector3d& setVect(const GLC_Vector3d &Vect)
	{
		vector[0]= Vect.vector[0];
		vector[1]= Vect.vector[1];
		vector[2]= Vect.vector[2];
		return *this;
	}

	//! Vector Normal
	inline GLC_Vector3d& setNormal(const double &Norme)
	{
		const double normCur= sqrt( vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);

		if (normCur != 0)
		{
			const double Coef = Norme / normCur;

			vector[0] = vector[0] * Coef;
			vector[1] = vector[1] * Coef;
			vector[2] = vector[2] * Coef;
		}
		return *this;
	}

	/*! Invert Vector*/
	inline GLC_Vector3d& invert(void)
	{
		vector[0]= - vector[0];
		vector[1]= - vector[1];
		vector[2]= - vector[2];
		return *this;
	}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return X Compound
	inline double X() const
	{
		return vector[0];
	}
	//! Return Y Compound
	inline double Y() const
	{
		return vector[1];
	}
	//! Return Z Compound
	inline double Z() const
	{
		return vector[2];
	}
	//! Return a pointer to vector data
	inline const double *data() const
	{
		return vector;
	}
	//! Return true if the vector is null
	inline bool isNull() const
	{
		return qFuzzyCompare(vector[0], 0.0) && qFuzzyCompare(vector[1], 0.0)
		&& qFuzzyCompare(vector[2], 0.0);
	}

	//! Return Vector Norm
	inline double norm() const
	{
		return sqrt(vector[0] * vector[0] + vector[1] * vector[1]
			+ vector[2] * vector[2]);
	}

	//! Return the 2D vector specified by a mask vector
	/*! retrieve component corresponding to
	 * mask NULL component*/
	inline GLC_Vector2d toVector2d(const GLC_Vector3d& mask) const
	{
		double x;
		double y;
		if (mask.vector[0] == 0.0)
		{
			x= vector[0];
			if (mask.vector[1] == 0.0)
				y= vector[1];
			else
				y= vector[2];

		}
		else
		{
			x= vector[1];
			y= vector[2];

		}
		return GLC_Vector2d(x, y);
	}

	//! Return the Angle with another vector
	inline double getAngleWithVect(GLC_Vector3d Vect) const
	{
		GLC_Vector3d ThisVect(*this);
		ThisVect.setNormal(1);
		Vect.setNormal(1);
		// Rotation axis
		const GLC_Vector3d VectAxeRot(ThisVect ^ Vect);
		// Check if the rotation axis vector is null
		if (!VectAxeRot.isNull())
		{
			return acos(ThisVect * Vect);
		}
		else return 0.0;
	}

	inline GLC_Vector3df toVector3df() const
	{
		return GLC_Vector3df(static_cast<float>(vector[0]), static_cast<float>(vector[1]), static_cast<float>(vector[2]));
	}


	// return the vector string
	inline QString toString() const
	{
		QString result("[");

		result+= QString::number(vector[0]) + QString(" , ");
		result+= QString::number(vector[1]) + QString(" , ");
		result+= QString::number(vector[2]) + QString("]");

		return result;
	}

//@}

//////////////////////////////////////////////////////////////////////
//name Private attributes
//////////////////////////////////////////////////////////////////////
private:
	/*! Vector array definition \n
	*	vector[0]	X \n
	*	vector[1]	Y \n
	*	vector[2]	Z \n
	*/
	double vector[3];

}; //class GLC_Vector3d

// Vector constant in glc namespace
namespace glc
{
	// Axis definition
	/*! \var X_AXIS
	 *  \brief X axis Vector*/
	const GLC_Vector3d X_AXIS(1.0, 0.0, 0.0);

	/*! \var Y_AXIS
	 *  \brief Y axis Vector*/
	const GLC_Vector3d Y_AXIS(0.0, 1.0, 0.0);

	/*! \var Z_AXIS
	 *  \brief Z axis Vector*/
	const GLC_Vector3d Z_AXIS(0.0, 0.0, 1.0);
};

//! Define GLC_Point3D
typedef GLC_Vector3d GLC_Point3d;

//! Non-member stream operator
inline QDataStream &operator<<(QDataStream & stream, const GLC_Vector3d & vector)
{
	stream << vector.X() << vector.Y() << vector.Z();
	return stream;
}

inline QDataStream &operator>>(QDataStream &stream, GLC_Vector3d &vector)
{
	double x, y, z;
	stream >> x >> y >> z;
	vector.setVect(x, y, z);
	return stream;
}


#endif /*GLC_VECTOR3D_H_*/
