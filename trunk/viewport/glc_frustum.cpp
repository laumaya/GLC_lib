/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 2.0.0 Beta 1, packaged on April 2010.

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
//! \file glc_frustum.cpp Implementation of the GLC_Frustum class.

#include "glc_frustum.h"
#include "glc_viewport.h"

// Default constructor
GLC_Frustum::GLC_Frustum()
: m_PlaneList()
, m_PreviousMatrix()
{
	for (int i= 0; i < 6; ++i)
	{
		m_PlaneList.append(GLC_Plane());
	}
}

// Copy constructor
GLC_Frustum::GLC_Frustum(const GLC_Frustum& frustum)
: m_PlaneList(frustum.m_PlaneList)
, m_PreviousMatrix(frustum.m_PreviousMatrix)
{

}

GLC_Frustum::~GLC_Frustum()
{

}

// Localize bounding box
GLC_Frustum::Localisation GLC_Frustum::localizeBoundingBox(const GLC_BoundingBox& box) const
{
	const GLC_Point3d center= box.center();
	const double radius= box.boundingSphereRadius();

	return localizeSphere(center, radius);
}

// Localize sphere
GLC_Frustum::Localisation GLC_Frustum::localizeSphere(const GLC_Point3d& center, double radius) const
{
	GLC_Frustum::Localisation localisationResult= InFrustum;

	int i= 0;
	bool continu= true;
	while (continu && (i < 6))
	{
		//qDebug() << "Localisation of plane " << i;
		localisationResult= static_cast<GLC_Frustum::Localisation>(localisationResult | localizeSphereToPlane(center, radius, m_PlaneList.at(i)));
		continu= (localisationResult != GLC_Frustum::OutFrustum);
		++i;
	}

	return localisationResult;
}

// localize a bounding box to a plane
GLC_Frustum::Localisation GLC_Frustum::localizeSphereToPlane(const GLC_Point3d& center, double radius, const GLC_Plane& plane) const
{
	GLC_Frustum::Localisation localisationResult;
	const double signedDistance= plane.distanceToPoint(center);
	const double distance= fabs(signedDistance);
	//qDebug() << "radius " << radius;
	//qDebug() << "signed Distance " << signedDistance;
	if (distance > radius)
	{
		if (signedDistance > 0) localisationResult= GLC_Frustum::InFrustum;
		else localisationResult= GLC_Frustum::OutFrustum;
	}
	else
	{
		localisationResult= GLC_Frustum::IntersectFrustum;
	}

	return localisationResult;
}

// Update the frustum
bool GLC_Frustum::update(const GLC_Matrix4x4& compMatrix)
{

	// Test if the frustum change
	if (compMatrix == m_PreviousMatrix)
	{
		//qDebug() << "No change in frustum";
		return false;
	}
	else
	{
		//qDebug() << "-------------CHANGE---------------";
		m_PreviousMatrix= compMatrix;
		// Left plane
		m_PlaneList[LeftPlane].setA(compMatrix.data()[3] + compMatrix.data()[0]);
		m_PlaneList[LeftPlane].setB(compMatrix.data()[7] + compMatrix.data()[4]);
		m_PlaneList[LeftPlane].setC(compMatrix.data()[11] + compMatrix.data()[8]);
		m_PlaneList[LeftPlane].setD(compMatrix.data()[15] + compMatrix.data()[12]);
		m_PlaneList[LeftPlane].normalize();
		//qDebug() << "Left Plane : x=" << m_PlaneList[LeftPlane].coefA() << " y=" << m_PlaneList[LeftPlane].coefB() << " z=" << m_PlaneList[LeftPlane].coefC();
		//qDebug() << "Left plane coef d=" << m_PlaneList[LeftPlane].coefD();

		// Right plane
		m_PlaneList[RightPlane].setA(compMatrix.data()[3] - compMatrix.data()[0]);
		m_PlaneList[RightPlane].setB(compMatrix.data()[7] - compMatrix.data()[4]);
		m_PlaneList[RightPlane].setC(compMatrix.data()[11] - compMatrix.data()[8]);
		m_PlaneList[RightPlane].setD(compMatrix.data()[15] - compMatrix.data()[12]);
		m_PlaneList[RightPlane].normalize();
		//qDebug() << "Right Plane : x=" << m_PlaneList[RightPlane].coefA() << " y=" << m_PlaneList[RightPlane].coefB() << " z=" << m_PlaneList[RightPlane].coefC();
		//qDebug() << "Right plane coef d=" << m_PlaneList[RightPlane].coefD();

		//Top plane
		m_PlaneList[TopPlane].setA(compMatrix.data()[3] + compMatrix.data()[1]);
		m_PlaneList[TopPlane].setB(compMatrix.data()[7] + compMatrix.data()[5]);
		m_PlaneList[TopPlane].setC(compMatrix.data()[11] + compMatrix.data()[9]);
		m_PlaneList[TopPlane].setD(compMatrix.data()[15] + compMatrix.data()[13]);
		m_PlaneList[TopPlane].normalize();
		//qDebug() << "Top Plane : x=" << m_PlaneList[TopPlane].coefA() << " y=" << m_PlaneList[TopPlane].coefB() << " z=" << m_PlaneList[TopPlane].coefC();
		//qDebug() << "Top plane coef d=" << m_PlaneList[TopPlane].coefD();

		//Bottom plane
		m_PlaneList[BottomPlane].setA(compMatrix.data()[3] - compMatrix.data()[1]);
		m_PlaneList[BottomPlane].setB(compMatrix.data()[7] - compMatrix.data()[5]);
		m_PlaneList[BottomPlane].setC(compMatrix.data()[11] - compMatrix.data()[9]);
		m_PlaneList[BottomPlane].setD(compMatrix.data()[15] - compMatrix.data()[13]);
		m_PlaneList[BottomPlane].normalize();
		//qDebug() << "Bottom Plane : x=" << m_PlaneList[BottomPlane].coefA() << " y=" << m_PlaneList[BottomPlane].coefB() << " z=" << m_PlaneList[BottomPlane].coefC();
		//qDebug() << "Bottom plane coef d=" << m_PlaneList[BottomPlane].coefD();

		//Near plane
		m_PlaneList[NearPlane].setA(compMatrix.data()[3] + compMatrix.data()[2]);
		m_PlaneList[NearPlane].setB(compMatrix.data()[7] + compMatrix.data()[6]);
		m_PlaneList[NearPlane].setC(compMatrix.data()[11] + compMatrix.data()[10]);
		m_PlaneList[NearPlane].setD(compMatrix.data()[15] + compMatrix.data()[14]);
		m_PlaneList[NearPlane].normalize();

		//Far plane
		m_PlaneList[FarPlane].setA(compMatrix.data()[3] - compMatrix.data()[2]);
		m_PlaneList[FarPlane].setB(compMatrix.data()[7] - compMatrix.data()[6]);
		m_PlaneList[FarPlane].setC(compMatrix.data()[11] - compMatrix.data()[10]);
		m_PlaneList[FarPlane].setD(compMatrix.data()[15] - compMatrix.data()[14]);
		m_PlaneList[FarPlane].normalize();
		return true;
	}

}
