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
//! \file glc_polygon.h Interface for the GLC_Polygon class.

#ifndef GLC_POLYGON_H
#define GLC_POLYGON_H

#include <QList>

#include "glc_vector3d.h"

#include "../glc_config.h"

class GLC_Matrix4x4;

//////////////////////////////////////////////////////////////////////
//! \class GLC_Polygon
/*! \brief GLC_Polygon : Maths convex and planar polygon */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_Polygon
{
public:
    GLC_Polygon();

    explicit GLC_Polygon(const QList<GLC_Point3d>& points);

    GLC_Polygon(const GLC_Polygon& other);

public:
    bool operator==(const GLC_Polygon& other) const;

    bool operator!=(const GLC_Polygon& other) const
    {return !(this->operator==(other));}

    bool isEmpty() const
    {return m_Points.isEmpty();}

    const GLC_Point3d& pointAt(int index) const
    {return m_Points.at(index);}

    const QList<GLC_Point3d>& points() const
    {return m_Points;}

    GLC_Point3d centroid() const;

    GLC_Vector3d normalCCW() const;

    GLC_Vector3d normalCW() const;

    int count() const
    {return m_Points.count();}

public:
    GLC_Polygon& operator=(const GLC_Polygon& other);

    void setPoints(const QList<GLC_Point3d>& points)
    {m_Points= points;}

    void addPoint(const GLC_Point3d& point)
    {m_Points.append(point);}

    void clear()
    {m_Points.clear();}

    void transform(const GLC_Matrix4x4& matrix);

private:
    QList<GLC_Point3d> m_Points;
};

#endif // GLC_POLYGON_H
