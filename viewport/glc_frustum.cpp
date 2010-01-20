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
//! \file glc_frustum.cpp Implementation of the GLC_Frustum class.

#include "glc_frustum.h"

// Default constructor
GLC_Frustum::GLC_Frustum(GLC_Viewport* pViewport)
: m_LeftPlane()
, m_RightPlane()
, m_TopPlane()
, m_BottomPlane()
, m_NearPlane()
, m_FarPlane()
{
	update(pViewport);
}

// Copy constructor
GLC_Frustum::GLC_Frustum(const GLC_Frustum& frustum)
: m_LeftPlane(frustum.m_LeftPlane)
, m_RightPlane(frustum.m_RightPlane)
, m_TopPlane(frustum.m_TopPlane)
, m_BottomPlane(frustum.m_BottomPlane)
, m_NearPlane(frustum.m_NearPlane)
, m_FarPlane(frustum.m_FarPlane)
{

}

GLC_Frustum::~GLC_Frustum()
{

}


// Update the frustum
void GLC_Frustum::update(GLC_Viewport* pViewport)
{
	// Get the viewport projection matrix
	GLC_Matrix4x4 projectionMatrix= pViewport->projectionMatrix();

	// Get the camera modelView matrix
	GLC_Matrix4x4 modelViewMatrix= pViewport->cameraHandle()->modelViewMatrix();

	// Composition matrix
	GLC_Matrix4x4 compMatrix= modelViewMatrix * projectionMatrix;

	// Left plane
	m_LeftPlane.setA(compMatrix.data()[12] + compMatrix.data()[0]);
	m_LeftPlane.setB(compMatrix.data()[13] + compMatrix.data()[1]);
	m_LeftPlane.setC(compMatrix.data()[14] + compMatrix.data()[2]);
	m_LeftPlane.setD(compMatrix.data()[15] + compMatrix.data()[3]);
	m_LeftPlane.normalize();

	// Right plane
	m_RightPlane.setA(compMatrix.data()[12] - compMatrix.data()[0]);
	m_RightPlane.setB(compMatrix.data()[13] - compMatrix.data()[1]);
	m_RightPlane.setC(compMatrix.data()[14] - compMatrix.data()[2]);
	m_RightPlane.setD(compMatrix.data()[15] - compMatrix.data()[3]);
	m_RightPlane.normalize();

	//Top plane
	m_TopPlane.setA(compMatrix.data()[12] + compMatrix.data()[4]);
	m_TopPlane.setB(compMatrix.data()[13] + compMatrix.data()[5]);
	m_TopPlane.setC(compMatrix.data()[14] + compMatrix.data()[6]);
	m_TopPlane.setD(compMatrix.data()[15] + compMatrix.data()[7]);
	m_TopPlane.normalize();

	//Bottom plane
	m_BottomPlane.setA(compMatrix.data()[12] - compMatrix.data()[4]);
	m_BottomPlane.setB(compMatrix.data()[13] - compMatrix.data()[5]);
	m_BottomPlane.setC(compMatrix.data()[14] - compMatrix.data()[6]);
	m_BottomPlane.setD(compMatrix.data()[15] - compMatrix.data()[7]);
	m_BottomPlane.normalize();

	//Near plane
	m_NearPlane.setA(compMatrix.data()[12] + compMatrix.data()[8]);
	m_NearPlane.setB(compMatrix.data()[13] + compMatrix.data()[9]);
	m_NearPlane.setC(compMatrix.data()[14] + compMatrix.data()[10]);
	m_NearPlane.setD(compMatrix.data()[15] + compMatrix.data()[11]);
	m_NearPlane.normalize();

	//Far plane
	m_FarPlane.setA(compMatrix.data()[12] - compMatrix.data()[8]);
	m_FarPlane.setB(compMatrix.data()[13] - compMatrix.data()[9]);
	m_FarPlane.setC(compMatrix.data()[14] - compMatrix.data()[10]);
	m_FarPlane.setD(compMatrix.data()[15] - compMatrix.data()[11]);
	m_FarPlane.normalize();
}
