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
#include "../glc_factory.h"

GLC_CuttingPlane::GLC_CuttingPlane(const GLC_Point3d& center, const GLC_Vector3d& normal, double l1, double l2, GLC_3DWidgetManagerHandle*  pWidgetManagerHandle)
: GLC_3DWidget(pWidgetManagerHandle)
, m_Center(center)
, m_Normal(normal)
, m_L1(l1)
, m_L2(l2)
{
	if (NULL != pWidgetManagerHandle)
	{
		create3DviewInstance();
	}
}

GLC_CuttingPlane::~GLC_CuttingPlane()
{

}

void GLC_CuttingPlane::updateLenght(double l1, double l2)
{
	m_L1= l1;
	m_L2= l2;
}

void GLC_CuttingPlane::select(const GLC_Point3d&)
{
	GLC_3DViewInstance* pInstance= GLC_3DWidget::instanceHandle(0);
	GLC_Geometry* pGeom= pInstance->geomAt(0);
	pGeom->firstMaterial()->setDiffuseColor(Qt::darkGreen);

}

void GLC_CuttingPlane::unselect(const GLC_Point3d&)
{
	GLC_3DViewInstance* pInstance= GLC_3DWidget::instanceHandle(0);
	GLC_Geometry* pGeom= pInstance->geomAt(0);
	pGeom->firstMaterial()->setDiffuseColor(Qt::darkYellow);
}


void GLC_CuttingPlane::create3DviewInstance()
{
	Q_ASSERT(GLC_3DWidget::isEmpty());
	GLC_Material* pMaterial= new GLC_Material(Qt::darkYellow);
	pMaterial->setOpacity(0.3);
	GLC_3DViewInstance cuttingPlaneInstance= GLC_Factory::instance()->createCuttingPlane(m_Center, m_Normal, m_L1, m_L2, pMaterial);

	GLC_3DWidget::add3DViewInstance(cuttingPlaneInstance);
}



