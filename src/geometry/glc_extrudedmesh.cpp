/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2013 Laurent Ribon (laumaya@users.sourceforge.net)
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

//! \file glc_extrudedmesh.cpp Implementation of the GLC_ExtrudedMesh class.

#include <QtDebug>

#include "glc_extrudedmesh.h"

// Class chunk id
quint32 GLC_ExtrudedMesh::m_ChunkId= 0xA712;

GLC_ExtrudedMesh::GLC_ExtrudedMesh(QList<GLC_Point3d> points, GLC_Vector3d dir, double lenght)
    :GLC_Mesh()
    , m_Points(points)
    , m_ExtrusionVector(dir)
    , m_ExtrusionLenght(lenght)
{
    Q_ASSERT(m_Points.size() > 2);

    createMeshAndWire();
}

GLC_ExtrudedMesh::GLC_ExtrudedMesh(const GLC_ExtrudedMesh &other)
    :GLC_Mesh(other)
    , m_Points(other.m_Points)
    , m_ExtrusionVector(other.m_ExtrusionVector)
    , m_ExtrusionLenght(other.m_ExtrusionLenght)
{
    if (GLC_Mesh::isEmpty())
    {
        createMeshAndWire();
    }
}

GLC_ExtrudedMesh::~GLC_ExtrudedMesh()
{

}

quint32 GLC_ExtrudedMesh::chunckID()
{
    return m_ChunkId;
}

GLC_Geometry *GLC_ExtrudedMesh::clone() const
{
    return new GLC_ExtrudedMesh(*this);
}

const GLC_BoundingBox &GLC_ExtrudedMesh::boundingBox()
{
    if (GLC_Mesh::isEmpty())
    {
        createMeshAndWire();
    }
    return GLC_Mesh::boundingBox();
}

void GLC_ExtrudedMesh::createMeshAndWire()
{

}

void GLC_ExtrudedMesh::createMesh()
{

}

void GLC_ExtrudedMesh::createWire()
{

}

void GLC_ExtrudedMesh::glDraw(const GLC_RenderProperties& renderProperties)
{

}
