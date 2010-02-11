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
//! \file glc_cuttingplane.cpp Implementation of the GLC_CuttingPlane class.

#include "glc_cuttingplane.h"
#include "../geometry/glc_rectangle.h"
#include "../shading/glc_material.h"

GLC_CuttingPlane::GLC_CuttingPlane(const GLC_Point3d& center, const GLC_Vector3d& normal, double l1, double l2, GLC_3DWidgetManagerHandle*  pWidgetManagerHandle)
: GLC_3DWidget(pWidgetManagerHandle)
, m_pMaterial(new GLC_Material(Qt::darkYellow))
, m_pRectangle(new GLC_Rectangle(l1, l2))
{
	m_pMaterial->setOpacity(0.3);
	m_pRectangle->replaceMasterMaterial(m_pMaterial);

	// Create the rectangle to (0,0) and  z normal
	GLC_3DRep rectangleRep(m_pRectangle);

}

GLC_CuttingPlane::~GLC_CuttingPlane()
{

}

void GLC_CuttingPlane::updateLenght(double l1, double l2)
{
	m_pRectangle->setLength1(l1);
	m_pRectangle->setLength2(l2);
}
