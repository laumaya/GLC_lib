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
#include "../maths/glc_line3d.h"
#include "../maths/glc_geomtools.h"

GLC_CuttingPlane::GLC_CuttingPlane(const GLC_Point3d& center, const GLC_Vector3d& normal, double l1, double l2, GLC_3DWidgetManagerHandle*  pWidgetManagerHandle)
: GLC_3DWidget(pWidgetManagerHandle)
, m_Center(center)
, m_Normal(normal)
, m_L1(l1)
, m_L2(l2)
, m_Previous()
, m_SlidingPlane()
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

glc::WidgetEventFlag GLC_CuttingPlane::select(const GLC_Point3d& pos)
{
	qDebug() << "GLC_CuttingPlane::select";

	qDebug() << "cutting plane center : " << m_Center.toString();
	qDebug() << "Selection coord " << pos.toString();

	GLC_3DViewInstance* pInstance= GLC_3DWidget::instanceHandle(0);
	GLC_Geometry* pGeom= pInstance->geomAt(0);
	pGeom->firstMaterial()->setDiffuseColor(Qt::darkGreen);

	// Update previous point
	m_Previous= pos;
	prepareToSlide();
	return glc::AcceptEvent;

}

glc::WidgetEventFlag GLC_CuttingPlane::unselect(const GLC_Point3d&)
{
	qDebug() << "GLC_CuttingPlane::unselect";
	GLC_3DViewInstance* pInstance= GLC_3DWidget::instanceHandle(0);
	GLC_Geometry* pGeom= pInstance->geomAt(0);
	pGeom->firstMaterial()->setDiffuseColor(Qt::darkYellow);
	return glc::AcceptEvent;
}

glc::WidgetEventFlag GLC_CuttingPlane::mouseOver(const GLC_Point3d&)
{
	qDebug() << "GLC_CuttingPlane::mouseOver";
	GLC_3DViewInstance* pInstance= GLC_3DWidget::instanceHandle(0);
	GLC_Geometry* pGeom= pInstance->geomAt(0);
	pGeom->firstMaterial()->setDiffuseColor(Qt::green);
	return glc::AcceptEvent;
}

glc::WidgetEventFlag GLC_CuttingPlane::mouseMove(const GLC_Point3d& pos, Qt::MouseButtons)
{
	// Projection of intersection point on slidding plane
	GLC_Vector3d forward(GLC_3DWidget::widgetManagerHandle()->cameraHandle()->forward());
	GLC_Point3d sliddingPoint;
	GLC_Line3d projectionLine(pos, forward);
	glc::lineIntersectPlane(projectionLine, m_SlidingPlane, &sliddingPoint);

	// Projection of the slidding point on cutting plane normal
	sliddingPoint= glc::project(sliddingPoint, GLC_Line3d(GLC_Point3d(), m_Normal));

	GLC_3DWidget::instanceHandle(0)->translate(sliddingPoint - m_Previous);
	m_Previous= sliddingPoint;

	m_Center= GLC_Matrix4x4(sliddingPoint - m_Previous) * m_Center;

	return glc::IgnoreEvent;
}

void GLC_CuttingPlane::create3DviewInstance()
{
	Q_ASSERT(GLC_3DWidget::isEmpty());
	GLC_Material* pMaterial= new GLC_Material(Qt::darkYellow);
	pMaterial->setOpacity(0.3);
	GLC_3DViewInstance cuttingPlaneInstance= GLC_Factory::instance()->createCuttingPlane(m_Center, m_Normal, m_L1, m_L2, pMaterial);

	GLC_3DWidget::add3DViewInstance(cuttingPlaneInstance);
}

void GLC_CuttingPlane::prepareToSlide()
{
	/*
	// Projection of the point on cutting plane
	GLC_Line3d projectionLine(m_Previous, m_Normal);
	GLC_Plane cuttingPlane(m_Normal, m_Center);
	glc::lineIntersectPlane(projectionLine, cuttingPlane, &m_Previous);
*/
	// Plane throw intersection and plane normal and camera up vector
	GLC_Point3d p0= m_Previous;
	GLC_Point3d p1= m_Previous + GLC_3DWidget::widgetManagerHandle()->cameraHandle()->upVector();
	GLC_Point3d p2= m_Previous + m_Normal;
	GLC_Plane sliddingPlane(p0, p1, p2);
	m_SlidingPlane= sliddingPlane;
	qDebug() << "Sliding plane normal : " << m_SlidingPlane.normal().toString();

	// Projection of intersection point on slidding plane
	GLC_Vector3d forward(GLC_3DWidget::widgetManagerHandle()->cameraHandle()->forward());
	GLC_Line3d projectionLine(m_Previous, forward);
	glc::lineIntersectPlane(projectionLine, m_SlidingPlane, &m_Previous);

	m_Previous= glc::project(m_Previous, GLC_Line3d(GLC_Point3d(), m_Normal));

}


