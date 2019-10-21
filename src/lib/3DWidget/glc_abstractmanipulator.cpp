/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/

#include "glc_abstractmanipulator.h"
#include "../viewport/glc_viewport.h"
#include "../maths/glc_geomtools.h"

#include <QtGlobal>

GLC_AbstractManipulator::GLC_AbstractManipulator(GLC_Viewport* pViewport)
: m_pViewport(pViewport)
, m_SlidingPlane()
, m_PreviousPosition()
, m_IsInManipulateState(false)
{
	Q_ASSERT(NULL != m_pViewport);
}

GLC_AbstractManipulator::GLC_AbstractManipulator(const GLC_AbstractManipulator& abstractManipulator)
: m_pViewport(abstractManipulator.m_pViewport)
, m_SlidingPlane(abstractManipulator.m_SlidingPlane)
, m_PreviousPosition(abstractManipulator.m_PreviousPosition)
, m_IsInManipulateState(abstractManipulator.m_IsInManipulateState)
{

}

GLC_AbstractManipulator::~GLC_AbstractManipulator()
{
}

void GLC_AbstractManipulator::enterManipulateState(const GLC_Point3d& startPoint)
{
    m_SlidingPlane= GLC_Plane(projectionDirection(startPoint), startPoint);

	m_PreviousPosition = startPoint;
    m_IsInManipulateState= true;
}

void GLC_AbstractManipulator::enterManipulateState(const GLC_Point3d& startPoint, const GLC_Plane& slidingPlane)
{
    m_SlidingPlane= slidingPlane;

    GLC_Line3d projectionLine(startPoint, projectionDirection(startPoint));
    glc::lineIntersectPlane(projectionLine, m_SlidingPlane, &m_PreviousPosition);
    m_IsInManipulateState= true;
}

GLC_Matrix4x4 GLC_AbstractManipulator::manipulate(const GLC_Point3d& newPoint)
{
	Q_ASSERT(m_IsInManipulateState);

    GLC_Matrix4x4 transformation(doManipulate(newPoint, projectionDirection(newPoint)));

    return transformation;
}

GLC_Vector3d GLC_AbstractManipulator::projectionDirection(const GLC_Point3d& point)
{
    GLC_Vector3d subject;
    if (m_pViewport->useOrtho())
    {
        subject= m_pViewport->cameraHandle()->forward();
    }
    else
    {
        subject= (point - m_pViewport->cameraHandle()->eye());
    }

    subject.normalize();

    return subject;
}
