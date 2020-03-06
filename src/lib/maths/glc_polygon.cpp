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
//! \file glc_polygon.cpp Implementation of the GLC_Polygon class.

#include "glc_polygon.h"

#include <GLC_Matrix4x4>

GLC_Polygon::GLC_Polygon()
    : m_Points()
{

}

GLC_Polygon::GLC_Polygon(const QList<GLC_Point3d>& points)
    : m_Points(points)
{

}

GLC_Polygon::GLC_Polygon(const GLC_Polygon& other)
    : m_Points(other.m_Points)
{

}

bool GLC_Polygon::operator==(const GLC_Polygon& other) const
{
    bool subject= m_Points == other.m_Points;

    return subject;
}

GLC_Point3d GLC_Polygon::centroid() const
{
    GLC_Point3d subject;
    if (!isEmpty())
    {
        for (const GLC_Point3d& point : m_Points)
        {
            subject+= point;
        }
    }
    subject= subject * (1.0 / m_Points.count());
    return subject;
}

GLC_Vector3d GLC_Polygon::normalCCW() const
{
    GLC_Vector3d subject;
    if (m_Points.count() >= 3)
    {
        const GLC_Point3d& p0(m_Points.at(0));
        const GLC_Point3d& p1(m_Points.at(1));
        const GLC_Point3d& p2(m_Points.at(2));
        subject= (p2 - p0) ^ (p1 - p0);
        subject.normalize();
    }

    return subject;
}

GLC_Vector3d GLC_Polygon::normalCW() const
{
    GLC_Vector3d subject;
    if (m_Points.count() >= 3)
    {
        const GLC_Point3d& p0(m_Points.at(0));
        const GLC_Point3d& p1(m_Points.at(1));
        const GLC_Point3d& p2(m_Points.at(2));
        subject= (p0 - p2) ^ (p1 - p0);
        subject.normalize();
    }

    return subject;
}

GLC_Polygon& GLC_Polygon::operator=(const GLC_Polygon& other)
{
    if (this != &other)
    {
        m_Points= other.m_Points;
    }
    return *this;
}

void GLC_Polygon::transform(const GLC_Matrix4x4& matrix)
{
    for (GLC_Point3d& point : m_Points)
    {
        point= matrix * point;
    }
}
