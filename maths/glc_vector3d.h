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
	{return GLC_Vector3d(-Vect.m_Vector[0], -Vect.m_Vector[1], -Vect.m_Vector[2]);}

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
	inline GLC_Vector3d();

	//! Standard constructor With x, y, z
	inline GLC_Vector3d(double, double , double);

	//! Copy constructor
	inline GLC_Vector3d(const GLC_Vector3d &vector)
	{memcpy(m_Vector, vector.m_Vector, sizeof(double) * 3);}

	//! Copy constructor from a float vector
	inline GLC_Vector3d(const GLC_Vector3df &Vect);

//@}

//////////////////////////////////////////////////////////////////////
/*! @name Operator Overload */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Overload binary "+" operator
	inline GLC_Vector3d operator + (const GLC_Vector3d &Vect) const
	{return GLC_Vector3d(m_Vector[0] + Vect.m_Vector[0], m_Vector[1] + Vect.m_Vector[1], m_Vector[2] + Vect.m_Vector[2]);}

	//! Overload "=" operator
	inline GLC_Vector3d& operator = (const GLC_Vector3d &vector)
	{
		memcpy(m_Vector, vector.m_Vector, sizeof(double) * 3);
		return *this;
	}

	//! Overload "=" operator
	inline GLC_Vector3d& operator = (const GLC_Vector3df &);

	//! Overload "+=" operator
	inline GLC_Vector3d& operator += (const GLC_Vector3d &vector)
	{
		*this= *this + vector;
		return *this;
	}

	//! Overload binary "-" operator
	inline GLC_Vector3d operator - (const GLC_Vector3d &Vect) const
	{return GLC_Vector3d(m_Vector[0] - Vect.m_Vector[0], m_Vector[1] - Vect.m_Vector[1], m_Vector[2] - Vect.m_Vector[2]);}

	//! Overload binary "-=" operator
	GLC_Vector3d& operator -= (const GLC_Vector3d &Vect)
	{
		*this= *this - Vect;
		return *this;
	}

	//! Overload dot product "^" operator
	inline GLC_Vector3d operator ^ (const GLC_Vector3d &vector) const;

	//! Overload scalar product "*" operator between 2 vector
	inline double operator * (const GLC_Vector3d &Vect) const
	{return m_Vector[0] * Vect.m_Vector[0] + m_Vector[1] * Vect.m_Vector[1] + m_Vector[2] * Vect.m_Vector[2];}

	//! Overload scalar product "*" operator between 1 vector and one scalar
	inline GLC_Vector3d operator * (double Scalaire) const
	{return GLC_Vector3d(m_Vector[0] * Scalaire, m_Vector[1] * Scalaire, m_Vector[2] * Scalaire);}


	//! Overload equality "==" operator
	inline bool operator == (const GLC_Vector3d &vector) const;

	//! Overload dot product "!=" operator
	inline bool operator != (const GLC_Vector3d &Vect) const
	{return !(*this == Vect);}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! X Compound
	inline GLC_Vector3d& setX(const double &dX)
	{
		m_Vector[0]= dX;
		return *this;
	}

	//! Y Compound
	inline GLC_Vector3d& setY(const double &dY)
	{
		m_Vector[1]= dY;
		return *this;
	}

	//! Z Compound
	inline GLC_Vector3d& setZ(const double &dZ)
	{
		m_Vector[2]= dZ;
		return *this;
	}

	//! All Compound
	inline GLC_Vector3d& setVect(double, double, double);

	//! From another Vector
	GLC_Vector3d& setVect(const GLC_Vector3d &vector)
	{
		memcpy(m_Vector, vector.m_Vector, sizeof(double) * 3);
		return *this;
	}

	//! Set vector lenght
	inline GLC_Vector3d& setLenght(double);

	//! Normalize the vector
	inline GLC_Vector3d& normalize()
	{return setLenght(1.0);}

	//! Invert Vector
	inline GLC_Vector3d& invert();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return X Compound
	inline double x() const
	{return m_Vector[0];}

	//! Return Y Compound
	inline double y() const
	{return m_Vector[1];}

	//! Return Z Compound
	inline double z() const
	{return m_Vector[2];}

	//! Return a pointer to vector data
	inline const double *data() const
	{return m_Vector;}

	//! Return true if the vector is null
	inline bool isNull() const
	{return (m_Vector[0] == 0.0f) && (m_Vector[1] == 0.0f) && (m_Vector[2] == 0.0f);}

	//! Return Vector Lenght
	inline double lenght() const
	{return sqrt(m_Vector[0] * m_Vector[0] + m_Vector[1] * m_Vector[1] + m_Vector[2] * m_Vector[2]);}

	//! Return the 2D vector specified by a mask vector
	/*! retrieve component corresponding to
	 * mask NULL component*/
	inline GLC_Vector2d toVector2d(const GLC_Vector3d& mask) const;

	//! Return the Angle with another vector
	inline double angleWithVect(GLC_Vector3d Vect) const;

	inline GLC_Vector3df toVector3df() const
	{return GLC_Vector3df(static_cast<float>(m_Vector[0]), static_cast<float>(m_Vector[1]), static_cast<float>(m_Vector[2]));}

	//! Return the vector string
	inline QString toString() const;

	//! Return the inverted vector
	inline GLC_Vector3d inverted() const
	{return GLC_Vector3d(*this).invert();}

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
	double m_Vector[3];

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
	stream << vector.x() << vector.y() << vector.z();
	return stream;
}

inline QDataStream &operator>>(QDataStream &stream, GLC_Vector3d &vector)
{
	double x, y, z;
	stream >> x >> y >> z;
	vector.setVect(x, y, z);
	return stream;
}

// Default constructor
GLC_Vector3d::GLC_Vector3d()
{
	m_Vector[0]= 0.0;
	m_Vector[1]= 0.0;
	m_Vector[2]= 0.0;
}

// Standard constructor With x, y, z
GLC_Vector3d::GLC_Vector3d(double x, double y, double z)
{
	m_Vector[0]= x;
	m_Vector[1]= y;
	m_Vector[2]= z;
}

// Copy constructor from a float vector
GLC_Vector3d::GLC_Vector3d(const GLC_Vector3df &vector)
{
	m_Vector[0]= static_cast<double>(vector.m_Vector[0]);
	m_Vector[1]= static_cast<double>(vector.m_Vector[1]);
	m_Vector[2]= static_cast<double>(vector.m_Vector[2]);
}

// Overload "=" operator
GLC_Vector3d& GLC_Vector3d::operator = (const GLC_Vector3df &Vect)
{
	m_Vector[0]= static_cast<double>(Vect.m_Vector[0]);
	m_Vector[1]= static_cast<double>(Vect.m_Vector[1]);
	m_Vector[2]= static_cast<double>(Vect.m_Vector[2]);

	return *this;
}

//  Overload dot product "^" operator
GLC_Vector3d GLC_Vector3d::operator ^ (const GLC_Vector3d &vector) const
{
	GLC_Vector3d vectResult;
	vectResult.m_Vector[0]= m_Vector[1] * vector.m_Vector[2] - m_Vector[2] * vector.m_Vector[1];
	vectResult.m_Vector[1]= m_Vector[2] * vector.m_Vector[0] - m_Vector[0] * vector.m_Vector[2];
	vectResult.m_Vector[2]= m_Vector[0] * vector.m_Vector[1] - m_Vector[1] * vector.m_Vector[0];

	return vectResult;
}

// Overload equality "==" operator
bool GLC_Vector3d::operator == (const GLC_Vector3d &vector) const
{
	bool bResult= qFuzzyCompare(m_Vector[0], vector.m_Vector[0]);
	bResult= bResult && qFuzzyCompare(m_Vector[1], vector.m_Vector[1]);
	bResult= bResult && qFuzzyCompare(m_Vector[2], vector.m_Vector[2]);

	return bResult;
}

// All Compound
GLC_Vector3d& GLC_Vector3d::setVect(double x, double y, double z)
{
	m_Vector[0]= x;
	m_Vector[1]= y;
	m_Vector[2]= z;

	return *this;
}

// Vector Normal
inline GLC_Vector3d& GLC_Vector3d::setLenght(double norme)
{
	const double normCur= sqrt( m_Vector[0] * m_Vector[0] + m_Vector[1] * m_Vector[1] + m_Vector[2] * m_Vector[2]);

	if (normCur != 0.0f)
	{
		const double Coef = norme / normCur;

		m_Vector[0] = m_Vector[0] * Coef;
		m_Vector[1] = m_Vector[1] * Coef;
		m_Vector[2] = m_Vector[2] * Coef;
	}
	return *this;
}

// Invert Vector
GLC_Vector3d& GLC_Vector3d::invert()
{
	m_Vector[0]= - m_Vector[0];
	m_Vector[1]= - m_Vector[1];
	m_Vector[2]= - m_Vector[2];
	return *this;
}

// Return the 2D vector specified by a mask vector
GLC_Vector2d GLC_Vector3d::toVector2d(const GLC_Vector3d& mask) const
{
	GLC_Vector2d resultVect;
	if (mask.m_Vector[0] == 0.0)
	{
		resultVect.setX(m_Vector[0]);
		if (mask.m_Vector[1] == 0.0) resultVect.setY(m_Vector[1]);
		else resultVect.setY(m_Vector[2]);
	}
	else resultVect.setVect(m_Vector[1], m_Vector[2]);

	return resultVect;
}

// Return the Angle with another vector
double GLC_Vector3d::angleWithVect(GLC_Vector3d Vect) const
{
	GLC_Vector3d ThisVect(*this);
	ThisVect.normalize();
	Vect.normalize();
	// Rotation axis
	const GLC_Vector3d VectAxeRot(ThisVect ^ Vect);
	// Check if the rotation axis vector is null
	if (!VectAxeRot.isNull())
	{
		return acos(ThisVect * Vect);
	}
	else return 0.0;
}
// Return the vector string
QString GLC_Vector3d::toString() const
{
	QString result("[");

	result+= QString::number(m_Vector[0]) + QString(" , ");
	result+= QString::number(m_Vector[1]) + QString(" , ");
	result+= QString::number(m_Vector[2]) + QString("]");

	return result;
}

#endif /*GLC_VECTOR3D_H_*/
