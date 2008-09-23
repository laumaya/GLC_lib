/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

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

//! \file glc_orbitCircle.h interface for the GLC_OrbitCircle class.

#ifndef GLC_ORBITCIRCLE_H_
#define GLC_ORBITCIRCLE_H_


#include "glc_circle.h"
#include "glc_instance.h"

class GLC_Factory;
class QGLContext;
//////////////////////////////////////////////////////////////////////
//! \class GLC_OrbitCircle
/*! \brief GLC_OrbitCircle : OpenGL 3D orbit Circle*/

/*! An GLC_OrbitCircle is a wire geometry composed of 3d lines \n
 * It's as a circle (GLC_Circle) with 2 arcs (GLC_Circle) of an angle of #ARCANGLE\n
 * The orbit Circle represent camera orientation.
 * */
//////////////////////////////////////////////////////////////////////

class GLC_OrbitCircle
{

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an orbit Circle with a given radius, name and color.
	GLC_OrbitCircle(const double &, const QGLContext*);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
	//! Return Color Red component
	GLdouble getdRed(void)
	{return m_MainCircle.getdRed();}

	//! Return Color Green component
	GLdouble getdGreen(void)
	{return m_MainCircle.getdGreen();}
	
	//! Return Color blue component
	GLdouble getdBlue(void)
	{return m_MainCircle.getdBlue();}
	
	//! Return Color Alpha component
	GLdouble getdAlpha(void)
	{return m_MainCircle.getdAlpha();}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Change the radius of the orbit circle
	void setRadius(double);

	//! Set Arcs orientation and position in concordance with mouse position
	void setOrientArcs(GLC_Vector4d VectAngle, const GLC_Matrix4x4 &Matrice);

	//! Set Arcs position in concordance with mouse position
	void mapArcs(const GLC_Matrix4x4 &Matrice);
	
	//! overload function setColor(color);
	void setRGBAColor(const QColor&);
	
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:	
	//! orbit circle OpenGL Execution
	void glExecute(double Profondeur);
	
//@}

//////////////////////////////////////////////////////////////////////
// Private Members
//////////////////////////////////////////////////////////////////////
private:
	//! OpenGl context
	GLC_Factory* m_pFactory;
	//! Main Circle
	GLC_Circle m_MainCircle;
	//! Arc 1 showing orbit sphere orientation	
	GLC_Instance m_Arc1;
	//! Arc 1 positionning Matrix
	GLC_Matrix4x4 m_MatArc1;
	//! Arc 2 showing orbit sphere orientation
	GLC_Instance m_Arc2;
	//! Arc 2 positionning Matrix
	GLC_Matrix4x4 m_MatArc2;


};

#endif //GLC_ORBITCIRCLE_H_
