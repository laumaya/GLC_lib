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

//! \file glc_repflymover.cpp Implementation of the GLC_RepFlyMover class.

#include "glc_repflymover.h"
#include "glc_viewport.h"
#include "../geometry/glc_polylines.h"
#include <QFontMetrics>

GLC_RepFlyMover::GLC_RepFlyMover(GLC_Viewport* pViewport)
: GLC_RepMover(pViewport)
, m_Radius(0.06)
, m_CenterCircle(m_Radius)
, m_Opacity(220)
, m_Plane()
, m_Hud()
, m_HudOffset(m_Radius * 5.0, m_Radius * 5.3)
{
	m_MainColor.setAlpha(m_Opacity);

	//m_CenterCircle.setLineWidth(1.5);
	m_CenterCircle.setWireColor(m_MainColor);

	createRepresentation();
}

GLC_RepFlyMover::GLC_RepFlyMover(const GLC_RepFlyMover& repFlyMover)
: GLC_RepMover(repFlyMover)
, m_Radius(repFlyMover.m_Radius)
, m_CenterCircle(repFlyMover.m_CenterCircle)
, m_Opacity(repFlyMover.m_Opacity)
, m_Plane(repFlyMover.m_Plane)
, m_Hud(repFlyMover.m_Hud)
, m_HudOffset(repFlyMover.m_HudOffset)
{

}

GLC_RepFlyMover::~GLC_RepFlyMover()
{

}

GLC_RepMover* GLC_RepFlyMover::clone() const
{
	return new GLC_RepFlyMover(*this);
}

void GLC_RepFlyMover::update()
{
	Q_ASSERT(NULL != m_pRepMoverInfo);
	Q_ASSERT(!m_pRepMoverInfo->m_VectorInfo.isEmpty());
	Q_ASSERT(!m_pRepMoverInfo->m_DoubleInfo.isEmpty());

	GLC_Vector3d vector(m_pRepMoverInfo->m_VectorInfo.first());

	// Rotation
	double deltaX= vector.x();
	double angle= - deltaX;
	GLC_Matrix4x4 rotation(glc::Z_AXIS, angle);

	// Translation
	vector.setX(vector.x() * m_Radius * 4.0);
	vector.setY(vector.y() * m_Radius * 4.0);
	GLC_Matrix4x4 translation(vector);

	m_Plane.setMatrix(translation * rotation);
}
void GLC_RepFlyMover::setMainColor(const QColor& color)
{
	GLC_RepMover::setMainColor(color);
	m_MainColor.setAlpha(m_Opacity);
	m_CenterCircle.setWireColor(GLC_RepMover::m_MainColor);
	m_Plane.geomAt(0)->setWireColor(GLC_RepMover::m_MainColor);
	m_Hud.geomAt(0)->setWireColor(GLC_RepMover::m_MainColor);

}

void GLC_RepFlyMover::glDraw()
{
	Q_ASSERT(NULL != m_pRepMoverInfo);
	Q_ASSERT(!m_pRepMoverInfo->m_DoubleInfo.isEmpty());

	// Get viewport informations
	const double calibre= 800.0;
	const double hRatio= static_cast<double>(m_pViewport->viewHSize()) / calibre;
	const double vRatio= static_cast<double>(m_pViewport->viewVSize()) / calibre;

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(hRatio * -1.0 ,hRatio * 1.0 ,vRatio * -1.0 ,vRatio * 1.0 ,-1.0 ,1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_RenderProperties.setRenderingFlag(glc::TransparentRenderFlag);
	m_CenterCircle.render(m_RenderProperties);

	m_Hud.render(glc::TransparentRenderFlag);
	m_Plane.render(glc::TransparentRenderFlag);

	// Render velocity value + text
	QString velocity(QChar(' ') + QString::number(static_cast<int>(100.0 * m_pRepMoverInfo->m_DoubleInfo.first())));
	QFont myFont;
	myFont.setBold(true);
	QFontMetrics fontmetrics(myFont);
	int txtHeight= fontmetrics.boundingRect(velocity).height();
	double posy= 2.0 * static_cast<double>(txtHeight) / calibre;
	m_pViewport->qGLWidgetHandle()->renderText(- m_HudOffset.getX(), m_HudOffset.getY() - posy, 0.0, velocity, myFont);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
}

void GLC_RepFlyMover::createRepresentation()
{
	// HUD creation
	GLC_Polylines* pPolylines= new GLC_Polylines();
	GLfloatVector  points;
	const double hudx= m_HudOffset.getX();
	const double hudy= m_HudOffset.getY();
	points << -hudx << -hudy << 0.0;
	points << -hudx << hudy << 0.0;
	pPolylines->addPolyline(points);
	points.clear();
	points << hudx << -hudy << 0.0;
	points << hudx << hudy << 0.0;
	pPolylines->addPolyline(points);
	pPolylines->setWireColor(GLC_RepMover::m_MainColor);
	m_Hud.setGeometry(pPolylines);

	// Plane creation
	pPolylines= new GLC_Polylines();
	points.clear();
	const double l1= m_Radius * 1.5;
	points << (-m_Radius - l1) << -m_Radius << 0.0;
	points << -m_Radius << -m_Radius << 0.0;
	points << 0.0 << 0.0 << 0.0;
	points << m_Radius << -m_Radius << 0.0;
	points << (m_Radius + l1) << -m_Radius << 0.0;
	pPolylines->addPolyline(points);
	pPolylines->setWireColor(GLC_RepMover::m_MainColor);
	m_Plane.setGeometry(pPolylines);
}
