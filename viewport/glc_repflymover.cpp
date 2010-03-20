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

GLC_RepFlyMover::GLC_RepFlyMover(GLC_Viewport* pViewport)
: GLC_RepMover(pViewport)
, m_Radius(0.05)
, m_CenterCircle(m_Radius)
, m_Opacity(100)
{
	m_MainColor.setAlpha(m_Opacity);

	m_CenterCircle.setLineWidth(1.5);
	m_CenterCircle.setWireColor(m_MainColor);
}

GLC_RepFlyMover::GLC_RepFlyMover(const GLC_RepFlyMover& repFlyMover)
: GLC_RepMover(repFlyMover)
, m_Radius(repFlyMover.m_Radius)
, m_CenterCircle(repFlyMover.m_CenterCircle)
, m_Opacity(repFlyMover.m_Opacity)

{

}

GLC_RepFlyMover::~GLC_RepFlyMover()
{

}

GLC_RepMover* GLC_RepFlyMover::clone() const
{
	return new GLC_RepFlyMover(*this);
}

void GLC_RepFlyMover::init(const GLC_Vector3d& vector, const GLC_Matrix4x4& matrix)
{

}

void GLC_RepFlyMover::update(const GLC_Matrix4x4& matrix)
{

}
void GLC_RepFlyMover::setMainColor(const QColor& color)
{
	GLC_RepMover::setMainColor(color);
	m_MainColor.setAlpha(m_Opacity);
	m_CenterCircle.setWireColor(GLC_RepMover::m_MainColor);

}

void GLC_RepFlyMover::glDraw()
{
	const double aspectRatio= static_cast<double>(m_pViewport->viewHSize())/static_cast<double>(m_pViewport->viewVSize());

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(aspectRatio * -1.0 ,aspectRatio * 1.0 ,-1.0 ,1.0 ,-1.0 ,1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_RenderProperties.setRenderingFlag(glc::TransparentRenderFlag);
	m_CenterCircle.render(m_RenderProperties);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);

}
