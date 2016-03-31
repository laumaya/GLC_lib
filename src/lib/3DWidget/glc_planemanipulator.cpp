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
//! \file glc_planemanipulator.cpp Implementation of the GLC_PlaneManipulator class.

#include "../maths/glc_line3d.h"
#include "../maths/glc_geomtools.h"
#include "../viewport/glc_viewport.h"

#include "glc_planemanipulator.h"

GLC_PlaneManipulator::GLC_PlaneManipulator(GLC_Viewport* pViewport, const GLC_Plane& slidingPlane)
    : GLC_AbstractManipulator(pViewport)
    , m_SlidingPlane(slidingPlane)
{

}

GLC_PlaneManipulator::GLC_PlaneManipulator(const GLC_PlaneManipulator &other)
    : GLC_AbstractManipulator(other)
    , m_SlidingPlane(other.m_SliddingPlane)
{

}

GLC_PlaneManipulator::~GLC_PlaneManipulator()
{

}

GLC_AbstractManipulator *GLC_PlaneManipulator::clone() const
{
    return new GLC_PlaneManipulator(*this);
}

void GLC_PlaneManipulator::setSlidingPlane(const GLC_Plane &slidingPlane)
{
    Q_ASSERT(!GLC_AbstractManipulator::isInManipulateState());
    m_SlidingPlane= slidingPlane;
}

GLC_Matrix4x4 GLC_PlaneManipulator::doManipulate(const GLC_Point3d &newPoint, const GLC_Vector3d &projectionDirection)
{
    // Project the given point on the sliding plane with the given direction
    GLC_Point3d projectedPoint;
    GLC_Line3d projectionLine1(GLC_AbstractManipulator::m_PreviousPosition, projectionDirection);
    GLC_Line3d projectionLine(newPoint, projectionDirection);

    // Project the point on the previous computed plane
    glc::lineIntersectPlane(projectionLine1, m_SlidingPlane, &(GLC_AbstractManipulator::m_PreviousPosition));
    glc::lineIntersectPlane(projectionLine, m_SlidingPlane, &projectedPoint);

    // Create the transformation matrix
    GLC_Matrix4x4 subject(projectedPoint - GLC_AbstractManipulator::m_PreviousPosition);

    // Update previous position to this position
    GLC_AbstractManipulator::m_PreviousPosition= projectedPoint;

    return subject;
}
