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
#include "../geometry/glc_arrow.h"
#include "../geometry/glc_disc.h"

GLC_CuttingPlane::GLC_CuttingPlane(const GLC_Point3d& center, const GLC_Vector3d& normal, double l1, double l2, GLC_3DWidgetManagerHandle*  pWidgetManagerHandle)
: GLC_3DWidget(pWidgetManagerHandle)
, m_Center(center)
, m_Normal(normal)
, m_L1(l1)
, m_L2(l2)
, m_Previous()
, m_SlidingPlane()
, m_Color(Qt::darkGreen)
, m_Opacity(0.3)
{
	if (NULL != pWidgetManagerHandle)
	{
		create3DviewInstance();
	}
}

GLC_CuttingPlane::GLC_CuttingPlane(const GLC_CuttingPlane& cuttingPlane)
: GLC_3DWidget(cuttingPlane)
, m_Center(cuttingPlane.m_Center)
, m_Normal(cuttingPlane.m_Normal)
, m_L1(cuttingPlane.m_L1)
, m_L2(cuttingPlane.m_L2)
, m_Previous()
, m_SlidingPlane()
, m_Color(cuttingPlane.m_Color)
, m_Opacity(cuttingPlane.m_Opacity)
{

}

GLC_CuttingPlane::~GLC_CuttingPlane()
{

}

GLC_CuttingPlane& GLC_CuttingPlane::operator=(const GLC_CuttingPlane& cuttingPlane)
{
	GLC_3DWidget::operator=(cuttingPlane);

	m_Center= cuttingPlane.m_Center;
	m_Normal= cuttingPlane.m_Normal;
	m_L1= cuttingPlane.m_L1;
	m_L2= cuttingPlane.m_L2;
	m_Color= cuttingPlane.m_Color;
	m_Opacity= cuttingPlane.m_Opacity;

	return *this;
}

void GLC_CuttingPlane::updateLength(double l1, double l2)
{
	m_L1= l1;
	m_L2= l2;

	if (GLC_3DWidget::has3DWidgetManager())
	{
		GLC_3DWidget::remove3DViewInstance();
		create3DviewInstance();
	}
}

void GLC_CuttingPlane::viewportAsChanged()
{
	const double viewTangent= GLC_3DWidget::widgetManagerHandle()->viewportTangent();
	const GLC_Point3d eye(GLC_3DWidget::widgetManagerHandle()->cameraHandle()->eye());
	const double distanceToNormal= (m_Previous - eye).length();
	const double viewWidth= distanceToNormal * viewTangent;
	updateArrow(viewWidth * 0.06);
}

glc::WidgetEventFlag GLC_CuttingPlane::select(const GLC_Point3d& pos)
{
	// Update previous point
	m_Previous= pos;
	prepareToSlide();

	GLC_3DWidget::set3DViewInstanceVisibility(1, true);
	GLC_3DWidget::set3DViewInstanceVisibility(2, true);

	GLC_Matrix4x4 translationMatrix(pos);
	GLC_3DWidget::instanceHandle(1)->setMatrix(translationMatrix);
	GLC_3DWidget::instanceHandle(2)->setMatrix(translationMatrix);

	return glc::BlockedEvent;
}

glc::WidgetEventFlag GLC_CuttingPlane::mousePressed(const GLC_Point3d& pos, Qt::MouseButton button)
{
	glc::WidgetEventFlag returnFlag= glc::IgnoreEvent;
	if (button == Qt::LeftButton)
	{
		m_Previous= pos;
		prepareToSlide();
		returnFlag= glc::BlockedEvent;

		GLC_3DWidget::set3DViewInstanceVisibility(1, true);
		GLC_3DWidget::set3DViewInstanceVisibility(2, true);

		GLC_Matrix4x4 translationMatrix(pos);
		GLC_3DWidget::instanceHandle(1)->setMatrix(translationMatrix);
		GLC_3DWidget::instanceHandle(2)->setMatrix(translationMatrix);
	}

	return returnFlag;
}

glc::WidgetEventFlag GLC_CuttingPlane::unselect(const GLC_Point3d&)
{
	GLC_3DWidget::set3DViewInstanceVisibility(1, false);
	GLC_3DWidget::set3DViewInstanceVisibility(2, false);

	return glc::AcceptEvent;
}

glc::WidgetEventFlag GLC_CuttingPlane::mouseOver(const GLC_Point3d&)
{
	return glc::AcceptEvent;
}

glc::WidgetEventFlag GLC_CuttingPlane::mouseMove(const GLC_Point3d& pos, Qt::MouseButtons)
{
	// Projection of intersection point on slidding plane
	const GLC_Point3d eye(GLC_3DWidget::widgetManagerHandle()->cameraHandle()->eye());
	GLC_Vector3d direction;
	if (GLC_3DWidget::useOrtho())
	{
		direction= GLC_3DWidget::widgetManagerHandle()->cameraHandle()->forward().normalize();
	}
	else
	{
		direction= (pos - eye);
	}

	GLC_Point3d sliddingPoint;
	GLC_Line3d projectionLine(pos, direction);
	glc::lineIntersectPlane(projectionLine, m_SlidingPlane, &sliddingPoint);

	GLC_Vector3d projNormal= (GLC_3DWidget::widgetManagerHandle()->cameraHandle()->sideVector() ^ m_Normal).normalize();
	GLC_Plane projPlane(projNormal, m_Previous);
	glc::lineIntersectPlane(projectionLine, projPlane, &sliddingPoint);

	// Projection of the slidding point on cutting plane normal
	sliddingPoint= glc::project(sliddingPoint, GLC_Line3d(m_Previous, m_Normal));

	GLC_Matrix4x4 translationMatrix(sliddingPoint - m_Previous);

	// Update the instance
	GLC_3DWidget::instanceHandle(0)->multMatrix(translationMatrix);
	GLC_3DWidget::instanceHandle(1)->multMatrix(translationMatrix);
	GLC_3DWidget::instanceHandle(2)->multMatrix(translationMatrix);

	// Update plane center
	m_Center= translationMatrix * m_Center;

	m_Previous= sliddingPoint;

	// Plane throw intersection and plane normal and camera up vector
	GLC_Plane sliddingPlane(GLC_3DWidget::widgetManagerHandle()->cameraHandle()->forward().normalize(), m_Previous);
	m_SlidingPlane= sliddingPlane;

	emit asChanged();

	return glc::AcceptEvent;
}

void GLC_CuttingPlane::create3DviewInstance()
{
	Q_ASSERT(GLC_3DWidget::isEmpty());
	// The cutting plane material
	GLC_Material* pMaterial= new GLC_Material(m_Color);
	pMaterial->setOpacity(m_Opacity);

	// Cutting plane 3Dview instance
	GLC_3DViewInstance cuttingPlaneInstance= GLC_Factory::instance()->createCuttingPlane(m_Center, m_Normal, m_L1, m_L2, pMaterial);
	GLC_3DWidget::add3DViewInstance(cuttingPlaneInstance);

	// Normal arrow geometry
	GLC_Arrow* pArrow= new GLC_Arrow(GLC_Point3d(), -m_Normal, GLC_3DWidget::widgetManagerHandle()->cameraHandle()->forward().normalize());
	pArrow->setLineWidth(2.5);
	QColor arrowColor(Qt::red);
	arrowColor.setAlphaF(0.4);
	pArrow->setWireColor(arrowColor);

	if (pArrow->isTransparent())
	{
		qDebug() << "Arrow is transparent";
	}
	else
	{
		qDebug() << "Arrow is NOT transparent";
	}
	//Base arrow disc
	pMaterial= new GLC_Material(Qt::red);
	pMaterial->setOpacity(m_Opacity);
	GLC_Disc* pDisc= new GLC_Disc(1.0);
	pDisc->replaceMasterMaterial(pMaterial);

	// Normal arrow + base instance
	GLC_3DViewInstance normalLine(pArrow);
	GLC_3DWidget::add3DViewInstance(normalLine);
	GLC_3DWidget::set3DViewInstanceVisibility(1, false);

	GLC_3DViewInstance normalBase(pDisc);
	GLC_3DWidget::add3DViewInstance(normalBase);
	GLC_3DWidget::set3DViewInstanceVisibility(2, false);

}

void GLC_CuttingPlane::prepareToSlide()
{
	// Plane throw intersection and plane normal and camera up vector
	GLC_Plane sliddingPlane(GLC_3DWidget::widgetManagerHandle()->cameraHandle()->forward().normalize(), m_Previous);
	m_SlidingPlane= sliddingPlane;

	// Projection of intersection point on slidding plane
	const GLC_Point3d eye(GLC_3DWidget::widgetManagerHandle()->cameraHandle()->eye());
	const GLC_Vector3d direction((m_Previous - eye).normalize());

	GLC_Line3d projectionLine(m_Previous, direction);
	glc::lineIntersectPlane(projectionLine, m_SlidingPlane, &m_Previous);

	//m_Previous= glc::project(m_Previous, GLC_Line3d(m_Previous, m_Normal));
}

void GLC_CuttingPlane::updateArrow(double length)
{
	GLC_Arrow* pArrow= dynamic_cast<GLC_Arrow*>(GLC_3DWidget::instanceHandle(1)->geomAt(0));
	Q_ASSERT(NULL != pArrow);

	pArrow->setEndPoint(-m_Normal * length);
	pArrow->setHeadLength(length * 0.15);
	pArrow->setViewDir(GLC_3DWidget::widgetManagerHandle()->cameraHandle()->forward().normalize());

	GLC_Disc* pDisc= dynamic_cast<GLC_Disc*>(GLC_3DWidget::instanceHandle(2)->geomAt(0));
	Q_ASSERT(NULL != pDisc);
	pDisc->setRadius(length * 0.3);
}
