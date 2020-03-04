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
#include "../maths/glc_plane.h"
#include "../maths/glc_geomtools.h"

// Class chunk id
quint32 GLC_ExtrudedMesh::m_ChunkId= 0xA712;

GLC_ExtrudedMesh::GLC_ExtrudedMesh(const QList<GLC_Point3d> &points, const GLC_Vector3d &dir, double lenght, bool mirroredExtend)
    :GLC_Mesh()
    , m_Points(points)
    , m_SmothingPoints()
    , m_InvisibleEdgeIndex()
    , m_ExtrusionVector(GLC_Vector3d(dir).normalize())
    , m_ExtrusionLenght(lenght)
    , m_GivenFaceNormal()
    , m_MirroredExtend(mirroredExtend)
    , m_MasterMaterialId(0)
    , m_EdgeMaterialId()
    , m_EdgeToMaterialIndex()
    , m_TextureRangeFactor(1.0f)
    , m_Polygons()
{

}

GLC_ExtrudedMesh::GLC_ExtrudedMesh(const GLC_ExtrudedMesh &other)
    :GLC_Mesh(other)
    , m_Points(other.m_Points)
    , m_SmothingPoints(other.m_SmothingPoints)
    , m_InvisibleEdgeIndex(other.m_InvisibleEdgeIndex)
    , m_ExtrusionVector(other.m_ExtrusionVector)
    , m_ExtrusionLenght(other.m_ExtrusionLenght)
    , m_GivenFaceNormal(other.m_GivenFaceNormal)
    , m_MirroredExtend(other.m_MirroredExtend)
    , m_MasterMaterialId(GLC_Mesh::newMaterialIdFromOldMaterialId(other.m_MasterMaterialId))
    , m_EdgeMaterialId()
    , m_EdgeToMaterialIndex(other.m_EdgeToMaterialIndex)
    , m_TextureRangeFactor(other.m_TextureRangeFactor)
    , m_Polygons(other.m_Polygons)
{
    copyEdgeMaterialId(other);
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

GLC_Material* GLC_ExtrudedMesh::masterMaterial() const
{
    GLC_Material* pSubject;
    if (GLC_Mesh::containsMaterial(m_MasterMaterialId))
    {
        pSubject= GLC_Mesh::material(m_MasterMaterialId);
    }
    else
    {
        pSubject= nullptr;
    }

    return pSubject;
}

QList<GLC_Polygon> GLC_ExtrudedMesh::polygons() const
{
    if (m_Polygons.isEmpty())
    {
        const_cast<GLC_ExtrudedMesh*>(this)->createPolygons();
    }

    return m_Polygons;
}

bool GLC_ExtrudedMesh::update()
{
    bool subject;
    if (GLC_Mesh::isEmpty())
    {
        createMeshAndWire();
        subject= true;
    }
    else
    {
        subject= false;
    }

    return subject;
}

GLC_ExtrudedMesh &GLC_ExtrudedMesh::operator =(const GLC_ExtrudedMesh &other)
{
    if (this != &other)
    {
        GLC_Mesh::operator =(other);
        m_Points= other.m_Points;
        m_SmothingPoints= other.m_SmothingPoints;
        m_InvisibleEdgeIndex= other.m_InvisibleEdgeIndex;
        m_ExtrusionVector= other.m_ExtrusionVector;
        m_ExtrusionLenght= other.m_ExtrusionLenght;
        m_GivenFaceNormal= other.m_GivenFaceNormal;
        m_MirroredExtend= other.m_MirroredExtend;
        m_MasterMaterialId= GLC_Mesh::newMaterialIdFromOldMaterialId(other.m_MasterMaterialId);
        copyEdgeMaterialId(other);
        m_EdgeToMaterialIndex= other.m_EdgeToMaterialIndex;
    }

    return *this;
}

void GLC_ExtrudedMesh::setGeometry(const QList<GLC_Point3d> &points, const GLC_Vector3d &extrudedVector, double lenght)
{
    m_Points= points;
    m_SmothingPoints.clear();
    m_InvisibleEdgeIndex.clear();

    m_ExtrusionVector= extrudedVector;
    m_ExtrusionLenght= lenght;

    GLC_Mesh::clearMeshWireAndBoundingBox();
    m_Polygons.clear();
}

void GLC_ExtrudedMesh::setPoints(const QList<GLC_Point3d> &points)
{
    const int count= points.count();
    bool isEqual;
    if (count == m_Points.count())
    {
        isEqual= true;
        for (int i= 0; i < count; ++i)
        {
            isEqual= glc::compare(points.at(i), m_Points.at(i));
            if (!isEqual) break;
        }
    }
    else
    {
        isEqual= false;
    }
    if (!isEqual)
    {
        m_Points= points;
        m_SmothingPoints.clear();
        m_InvisibleEdgeIndex.clear();

        GLC_Mesh::clearMeshWireAndBoundingBox();
        m_Polygons.clear();
    }
}

void GLC_ExtrudedMesh::setExtrudedVector(const GLC_Vector3d &extrudedVector)
{
    if (!glc::compare(extrudedVector, m_ExtrusionVector))
    {
        m_ExtrusionVector= extrudedVector;
        GLC_Mesh::clearMeshWireAndBoundingBox();
        m_Polygons.clear();
    }
}

void GLC_ExtrudedMesh::setExtrudedLenght(double length)
{
    if (!glc::compare(length, m_ExtrusionLenght))
    {
        m_ExtrusionLenght= length;
        GLC_Mesh::clearMeshWireAndBoundingBox();
        m_Polygons.clear();
    }
}

void GLC_ExtrudedMesh::setSmoothingIndex(const QList<int> &smothingIndex)
{
    const int count= smothingIndex.count();
    bool isEqual= true;
    if (count == m_SmothingPoints.count())
    {
        for (int i= 0; i < count; ++i)
        {
            isEqual= smothingIndex.at(i) == m_SmothingPoints.at(i);
            if (!isEqual) break;
        }
    }
    else
    {
        isEqual= false;
    }

    if (!isEqual)
    {
        m_SmothingPoints= smothingIndex;
        GLC_Mesh::clearMeshWireAndBoundingBox();
        m_Polygons.clear();
    }
}

void GLC_ExtrudedMesh::setInvisibleEdgeIndex(const QList<int>& invisibleEdgeIndex)
{
    const int count= invisibleEdgeIndex.count();
    bool isEqual= true;
    if (count == m_InvisibleEdgeIndex.count())
    {
        for (int i= 0; i < count; ++i)
        {
            isEqual= invisibleEdgeIndex.at(i) == m_InvisibleEdgeIndex.at(i);
            if (!isEqual) break;
        }
    }
    else
    {
        isEqual= false;
    }

    if (!isEqual)
    {
        m_InvisibleEdgeIndex= invisibleEdgeIndex;
        GLC_Mesh::clearMeshWireAndBoundingBox();
        m_Polygons.clear();
    }
}

void GLC_ExtrudedMesh::setMasterMaterial(GLC_Material* pMaterial)
{
    Q_ASSERT(nullptr != pMaterial);
    Q_ASSERT(m_MasterMaterialId == 0);

    GLC_Material* pMasterMaterial= new GLC_Material(*pMaterial);
    pMasterMaterial->setId(glc::GLC_GenID());
    GLC_Mesh::addMaterial(pMasterMaterial);
    m_MasterMaterialId= pMasterMaterial->id();
}

void GLC_ExtrudedMesh::setEdgeMaterialAndMapping(const QHash<int, int>& mapping)
{
    if (mapping != m_EdgeToMaterialIndex)
    {
        GLC_Mesh::clearMeshWireAndBoundingBox();
        m_Polygons.clear();
        QHash<int, GLC_uint>::iterator iMaterialId= m_EdgeMaterialId.begin();
        while (iMaterialId != m_EdgeMaterialId.end())
        {

            GLC_Geometry::removeMaterial(iMaterialId.value());
            ++iMaterialId;
        }
        m_EdgeMaterialId.clear();
        m_EdgeToMaterialIndex= mapping;
        QHash<int, int>::iterator iMaterialIndex= m_EdgeToMaterialIndex.begin();
        while (iMaterialIndex != m_EdgeToMaterialIndex.end())
        {
            GLC_Material* pMaterial= new GLC_Material;
            GLC_Mesh::addMaterial(pMaterial);
            m_EdgeMaterialId.insert(iMaterialIndex.value(), pMaterial->id());

            ++iMaterialIndex;
        }
    }
}

void GLC_ExtrudedMesh::setTextureRangeFactor(double value)
{
    if (!glc::compare(value, m_TextureRangeFactor))
    {
        m_TextureRangeFactor= static_cast<float>(value);
        GLC_Mesh::clearMeshWireAndBoundingBox();
        m_Polygons.clear();
    }
}

void GLC_ExtrudedMesh::createMeshAndWire()
{
    Q_ASSERT(GLC_Mesh::isEmpty());
    Q_ASSERT(m_WireData.isEmpty());

    Q_ASSERT(m_Points.size() > 2);
    Q_ASSERT(pointsLieOnAPlane());
    Q_ASSERT(!glc::compare(m_Points.first(), m_Points.last(), glc::EPSILON));

    computeGivenFaceNormal();

    double angleWithVect= m_GivenFaceNormal.angleWithVect2(m_ExtrusionVector);
    const bool normalEquals= glc::compare(m_GivenFaceNormal, m_ExtrusionVector);
    const bool normalsInverted= glc::compare(m_GivenFaceNormal, m_ExtrusionVector.inverted());

    if (normalEquals || (!normalsInverted && (angleWithVect < glc::PI / 2)))
    {
        qDebug() << "Invert extruded profile";
        std::reverse(m_Points.begin(), m_Points.end());
        std::reverse(m_SmothingPoints.begin(), m_SmothingPoints.end());
        std::reverse(m_InvisibleEdgeIndex.begin(), m_InvisibleEdgeIndex.end());
        m_GivenFaceNormal.invert();
    }

    createMesh();
    createWire();

    finish();
}

void GLC_ExtrudedMesh::createMesh()
{
    // Bulk data
    GLfloatVector vertices;
    GLfloatVector normals;
    GLfloatVector texels= baseFaceTexels() + createdFaceTexels() + basedOutlineFacesTexels() + createdOutlineFacesTexels();
    GLC_Mesh::addTexels(texels);

    // Set the material to use
    GLC_Material* pMasterMaterial;
    if (GLC_Geometry::containsMaterial(m_MasterMaterialId))
    {
        pMasterMaterial= GLC_Geometry::material(m_MasterMaterialId);
    }
    else
    {
        pMasterMaterial= new GLC_Material();
        m_MasterMaterialId= pMasterMaterial->id();
        GLC_Geometry::addMaterial(pMasterMaterial);
    }

    {
        // Given face (Face 1)
        GLfloatVector face1Vertices= baseFaceVertices();
        GLfloatVector face1Normals= baseFaceNormals();
        IndexList face1Index;
        const GLuint count= static_cast<GLuint>(m_Points.count());
        for (GLuint i= 0; i < count; ++i)
        {
            face1Index.append(i);
        }
        vertices+= face1Vertices;
        normals+= face1Normals;
        Q_ASSERT(vertices.size() == normals.size());
        glc::triangulatePolygonClip2TRi(&face1Index, vertices.toList());
        addTriangles(faceOutlineMaterial(-2), face1Index);
    }

    {
        // Created face (Face2)
        GLfloatVector face2Vertices= createdFaceVertices();
        GLfloatVector face2Normals= createdFaceNormals();
        IndexList face2Index;
        const GLuint offset= static_cast<GLuint>(vertices.size() / 3);
        const GLuint count= static_cast<GLuint>(m_Points.count());
        for (GLuint i= 0; i < count; ++i)
        {
            face2Index.append(offset + i);
        }
        vertices+= face2Vertices;
        normals+= face2Normals;
        Q_ASSERT(vertices.size() == normals.size());
        glc::triangulatePolygonClip2TRi(&face2Index, vertices.toList());
        addTriangles(faceOutlineMaterial(-1), face2Index);
    }


    {
        // Add outline faces
        const GLuint count= static_cast<GLuint>(m_Points.count());
        const GLuint offset1= static_cast<GLuint>(vertices.size() / 3);
        GLfloatVector facesNormals= baseOutlineNormals();
        GLuint indexLenght= static_cast<GLuint>(facesNormals.size() / 3);
        GLuint offset2= offset1 + indexLenght;
        facesNormals= facesNormals + createdOutlineNormals();
        GLfloatVector facesVertices= baseOutlineFacesVertices() + createdOutlineFacesVertices();
        vertices+= facesVertices;
        normals+= facesNormals;

        for (GLuint face= 0; face < count; ++face)
        {
            IndexList faceIndex;
            GLuint startIndex1= offset1 + (face * 2);
            GLuint startIndex2= offset2 + (indexLenght -1) - (face * 2);
            faceIndex << startIndex1 << startIndex2 << (startIndex1 + 1) << (startIndex2 - 1);
            addTrianglesStrip(faceOutlineMaterial(static_cast<int>(face)), faceIndex);
        }
    }

    // Add bulk data in to the mesh
    GLC_Mesh::addVertice(vertices);
    GLC_Mesh::addNormals(normals);
}

void GLC_ExtrudedMesh::createWire()
{
    GLfloatVector baseFaceWire= baseFaceVertices();
    baseFaceWire << baseFaceWire.at(0) << baseFaceWire.at(1) << baseFaceWire.at(2);
    GLC_Geometry::addVerticeGroup(baseFaceWire);

    GLfloatVector createdFaceWire= createdFaceVertices();
    createdFaceWire << createdFaceWire.at(0) << createdFaceWire.at(1) << createdFaceWire.at(2);
    GLC_Geometry::addVerticeGroup(createdFaceWire);

    // Outline edges
    const int count= m_Points.count();
    for (int i= 0; i < count; ++i)
    {
        if (!m_InvisibleEdgeIndex.contains(i))
        {
            GLfloatVector edge(6); // the edge is a line
            edge[0]= baseFaceWire.at(i * 3);
            edge[1]= baseFaceWire.at(i * 3 + 1);
            edge[2]= baseFaceWire.at(i * 3 + 2);

            edge[3]= createdFaceWire.at(((count - i) % count) * 3);
            edge[4]= createdFaceWire.at(((count - i) % count) * 3 + 1);
            edge[5]= createdFaceWire.at(((count - i) % count) * 3 + 2);

            GLC_Geometry::addVerticeGroup(edge);
        }
    }
}

void GLC_ExtrudedMesh::computeGivenFaceNormal()
{
    Q_ASSERT(m_Points.count() > 2);

    const GLC_Vector3d edge1(GLC_Vector3d(m_Points.at(1) - m_Points.at(0)).normalize());
    const GLC_Vector3d edge2(GLC_Vector3d(m_Points.at(2) - m_Points.at(1)).normalize());

    m_GivenFaceNormal= edge1 ^ edge2;
    m_GivenFaceNormal.normalize();
}

bool GLC_ExtrudedMesh::pointsLieOnAPlane() const
{
    bool subject= true;
    const int count= m_Points.count();
    Q_ASSERT(count > 2);
    if (count > 3)
    {
        // Create a plane on the first 3 points
        GLC_Plane plane(m_Points.at(0), m_Points.at(1), m_Points.at(2));

        // Check if others points lie on the plane
        for (int i= 3; i < count; ++i)
        {
            subject= plane.lieOnThisPlane(m_Points.at(i));
            if (!subject) break;
        }
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::baseOutlineNormals() const
{
    const int count= m_Points.count();
    GLfloatVector subject(count * 6); // 2 normals by vertex and 3 components pear normal => 6
    for (int i= 0; i < count; ++i)
    {
        GLC_Vector3d vect(m_Points.at((i + 1) % count) - m_Points.at(i));
        GLC_Vector3d normal= GLC_Vector3d(m_ExtrusionVector ^ vect).normalize();

        GLC_Vector3d normal1;
        GLC_Vector3d normal2;

        if (m_SmothingPoints.contains(i))
        {
            const int prevIndex= ((i - 1) < 0) ? (count + (i - 1)) : (i - 1);
            GLC_Vector3d prevVect(m_Points.at(i) - m_Points.at(prevIndex));
            GLC_Vector3d prevNormal= GLC_Vector3d(m_ExtrusionVector ^ prevVect).normalize();
            normal1= (normal + prevNormal).normalize();
        }
        else
        {
            normal1= normal;
        }

        if (m_SmothingPoints.contains(((i + 1) % count)))
        {
            GLC_Vector3d nextVect(m_Points.at((i + 2) % count) - m_Points.at((i + 1) % count));
            GLC_Vector3d nextNormal= GLC_Vector3d(m_ExtrusionVector ^ nextVect).normalize();
            normal2= (normal + nextNormal).normalize();
        }
        else
        {
            normal2= normal;
        }

        // First segment point
        subject[(6 * i)]= static_cast<GLfloat>(normal1.x());
        subject[(6 * i) + 1]= static_cast<GLfloat>(normal1.y());
        subject[(6 * i) + 2]= static_cast<GLfloat>(normal1.z());

        // Second segment point
        subject[(6 * i) + 3]= static_cast<GLfloat>(normal2.x());
        subject[(6 * i) + 4]= static_cast<GLfloat>(normal2.y());
        subject[(6 * i) + 5]= static_cast<GLfloat>(normal2.z());
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::createdOutlineNormals() const
{
    const int count= m_Points.count();
    GLfloatVector subject(count * 6); // 2 normals by vertex and 3 components pear normal => 6
    int index= 0.0;
    for (int i= count; i > 0; --i)
    {
        GLC_Vector3d vect(m_Points.at(i % count) - m_Points.at(i - 1));
        GLC_Vector3d normal= GLC_Vector3d(m_ExtrusionVector ^ vect).normalize();

        GLC_Vector3d normal1;
        GLC_Vector3d normal2;

        if (m_SmothingPoints.contains((i % count)))
        {
            GLC_Vector3d nextVect(m_Points.at((i + 1) % count) - m_Points.at(i % count));
            GLC_Vector3d nextNormal= GLC_Vector3d(m_ExtrusionVector ^ nextVect).normalize();
            normal1= (normal + nextNormal).normalize();
        }
        else
        {
            normal1= normal;
        }

        if (m_SmothingPoints.contains(i - 1))
        {
            const int prevPrevIndex= ((i - 2) < 0) ? (count + (i - 2)) : (i - 2);
            GLC_Vector3d prevVect(m_Points.at(i - 1) - m_Points.at(prevPrevIndex));
            GLC_Vector3d prevNormal= GLC_Vector3d(m_ExtrusionVector ^ prevVect).normalize();
            normal2= (normal + prevNormal).normalize();
        }
        else
        {
            normal2= normal;
        }

        // First segment point
        subject[(6 * index)]= static_cast<GLfloat>(normal1.x());
        subject[(6 * index) + 1]= static_cast<GLfloat>(normal1.y());
        subject[(6 * index) + 2]= static_cast<GLfloat>(normal1.z());

        // Second segment point
        subject[(6 * index) + 3]= static_cast<GLfloat>(normal2.x());
        subject[(6 * index) + 4]= static_cast<GLfloat>(normal2.y());
        subject[(6 * index) + 5]= static_cast<GLfloat>(normal2.z());
        ++index;
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::baseFaceVertices() const
{
    QList<GLC_Point3d> points= baseFacePoints();
    const int count= points.count();
    GLfloatVector subject(count * 3); // 3 components pear vertice
    for (int i= 0; i < count; ++i)
    {
        GLC_Point3d point= points.at(i);
        subject[(3 * i)]= static_cast<GLfloat>(point.x());
        subject[(3 * i) + 1]= static_cast<GLfloat>(point.y());
        subject[(3 * i) + 2]= static_cast<GLfloat>(point.z());
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::baseFaceTexels() const
{
    QList<GLC_Point3d> points= baseFacePoints();
    const int count= points.count();
    GLfloatVector subject(count * 2);

    QList<GLC_Point2d> baseFace2DPolygon= glc::polygonIn2d(points);

    for (int i= 0; i < count; ++i)
    {
        GLC_Point2d texel= baseFace2DPolygon.at(i) * m_TextureRangeFactor;
        subject[i * 2]= static_cast<GLfloat>(texel.x());
        subject[i * 2 + 1]= static_cast<GLfloat>(texel.y());
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::baseOutlineFacesVertices() const
{
    QList<GLC_Point3d> points= baseFacePoints();
    const int count= points.count();
    GLfloatVector subject(count * 6); // 3 components pear vertice * 2
    for (int i= 0; i < count; ++i)
    {
        GLC_Point3d point1= points.at(i);
        subject[(6 * i)]= static_cast<GLfloat>(point1.x());
        subject[(6 * i) + 1]= static_cast<GLfloat>(point1.y());
        subject[(6 * i) + 2]= static_cast<GLfloat>(point1.z());

        GLC_Point3d point2= points.at((i + 1) % count);
        subject[(6 * i) + 3]= static_cast<GLfloat>(point2.x());
        subject[(6 * i) + 4]= static_cast<GLfloat>(point2.y());
        subject[(6 * i) + 5]= static_cast<GLfloat>(point2.z());
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::basedOutlineFacesTexels() const
{
    QList<GLC_Point3d> points= baseFacePoints();
    const int count= points.count();
    GLfloatVector subject(count * 4);

    QList<double> distances;
    for (int i= 0; i < count; ++i)
    {
        distances.append((points.at((i + 1) % count) - points.at(i)).length());
    }

    for (int i= 0; i < count; ++i)
    {
        subject[i * 4]= static_cast<GLfloat>(0.0);
        subject[i * 4 + 1]= static_cast<GLfloat>(0.0);

        double value= distances.at(i) * m_TextureRangeFactor;
        subject[i * 4 + 2]= static_cast<GLfloat>(0.0);
        subject[i * 4 + 3]= static_cast<GLfloat>(value);
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::baseFaceNormals() const
{
    const int count= m_Points.count();
    GLfloatVector subject(count * 3); // 3 components pear normal
    for (int i= 0; i < count; ++i)
    {
        subject[(3 * i)]= static_cast<GLfloat>(m_GivenFaceNormal.x());
        subject[(3 * i) + 1]= static_cast<GLfloat>(m_GivenFaceNormal.y());
        subject[(3 * i) + 2]= static_cast<GLfloat>(m_GivenFaceNormal.z());
    }

    return subject;
}

QList<GLC_Point3d> GLC_ExtrudedMesh::baseFacePoints() const
{
    double offsetLength;
    if (m_MirroredExtend)
    {
        offsetLength= -(m_ExtrusionLenght / 2.0);
    }
    else
    {
        offsetLength= 0.0;
    }
    const GLC_Vector3d offset(m_ExtrusionVector * offsetLength);
    const int count= m_Points.count();
    QList<GLC_Point3d> subject;
    for (int i= 0; i < count; ++i)
    {
        subject.append(m_Points.at(i) + offset);
    }

    return subject;
}

QList<GLC_Point3d> GLC_ExtrudedMesh::createdFacePoints() const
{
    // The order is inverted to preserve face counterclockwise order

    double offsetLength;
    if (m_MirroredExtend)
    {
        offsetLength= (m_ExtrusionLenght / 2.0);
    }
    else
    {
        offsetLength= m_ExtrusionLenght;
    }

    const GLC_Vector3d offset(m_ExtrusionVector * offsetLength);
    const int count= m_Points.count();
    QList<GLC_Point3d> subject;
    for (int i= 0; i < count; ++i)
    {
        subject.append(m_Points.at((count -i) % count) + offset);
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::createdFaceNormals() const
{
    const GLC_Vector3d normal= m_GivenFaceNormal.inverted();

    const int count= m_Points.count();
    GLfloatVector subject(count * 3); // 3 components pear normal
    for (int i= 0; i < count; ++i)
    {
        subject[(3 * i)]= static_cast<GLfloat>(normal.x());
        subject[(3 * i) + 1]= static_cast<GLfloat>(normal.y());
        subject[(3 * i) + 2]= static_cast<GLfloat>(normal.z());
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::createdFaceVertices() const
{
    QList<GLC_Point3d> points= createdFacePoints();

    const int count= points.count();
    GLfloatVector subject(count * 3); // 3 components pear normal
    for (int i= 0; i < count; ++i)
    {
        GLC_Point3d point= points.at(i);
        subject[(3 * i)]= static_cast<GLfloat>(point.x());
        subject[(3 * i) + 1]= static_cast<GLfloat>(point.y());
        subject[(3 * i) + 2]= static_cast<GLfloat>(point.z());
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::createdFaceTexels() const
{
    const int count= m_Points.count();
    GLfloatVector subject(count * 2);

    QList<GLC_Point2d> baseFace2DPolygon= glc::polygonIn2d(createdFacePoints());
    for (int i= 0; i < count; ++i)
    {
        GLC_Point2d texel= baseFace2DPolygon.at(i) * m_TextureRangeFactor;
        subject[i * 2]= static_cast<GLfloat>(texel.x());
        subject[i * 2 + 1]= static_cast<GLfloat>(texel.y());
    }

    return subject;
}

GLfloatVector GLC_ExtrudedMesh::createdOutlineFacesVertices() const
{
    QList<GLC_Point3d> points= createdFacePoints();
    const int count= points.count();
    GLfloatVector subject(count * 6); // 3 components pear vertice * 2
    for (int i= 0; i < count; ++i)
    {
        GLC_Point3d point1= points.at(i);
        subject[(6 * i)]= static_cast<GLfloat>(point1.x());
        subject[(6 * i) + 1]= static_cast<GLfloat>(point1.y());
        subject[(6 * i) + 2]= static_cast<GLfloat>(point1.z());

        GLC_Point3d point2= points.at((i + 1) % count);
        subject[(6 * i) + 3]= static_cast<GLfloat>(point2.x());
        subject[(6 * i) + 4]= static_cast<GLfloat>(point2.y());
        subject[(6 * i) + 5]= static_cast<GLfloat>(point2.z());
    }

    return subject;

}

GLfloatVector GLC_ExtrudedMesh::createdOutlineFacesTexels() const
{
    QList<GLC_Point3d> points= createdFacePoints();
    const int count= points.count();
    GLfloatVector subject(count * 4);

    QList<double> distances;
    for (int i= 0; i < count; ++i)
    {
        distances.append((points.at((i + 1) % count) - points.at(i)).length());
    }

    for (int i= 0; i < count; ++i)
    {
        double value= distances.at(i) * m_TextureRangeFactor;
        subject[i * 4]= static_cast<GLfloat>(m_ExtrusionLenght * m_TextureRangeFactor);
        subject[i * 4 + 1]= static_cast<GLfloat>(value);

        subject[i * 4 + 2]= static_cast<GLfloat>(m_ExtrusionLenght * m_TextureRangeFactor);
        subject[i * 4 + 3]= static_cast<GLfloat>(0.0);
    }

    return subject;

    return subject;
}

void GLC_ExtrudedMesh::copyEdgeMaterialId(const GLC_ExtrudedMesh& other)
{
    QHash<int, GLC_uint>::const_iterator iMaterialId= other.m_EdgeMaterialId.constBegin();
    while (iMaterialId != other.m_EdgeMaterialId.constEnd())
    {
        const int key= iMaterialId.key();
        m_EdgeMaterialId.insert(key, GLC_Mesh::newMaterialIdFromOldMaterialId(other.m_EdgeMaterialId.value(key)));
        ++iMaterialId;
    }
}

GLC_Material* GLC_ExtrudedMesh::faceOutlineMaterial(int face) const
{
    GLC_Material* pSubject;
    if (!m_EdgeMaterialId.isEmpty())
    {
        Q_ASSERT(m_EdgeToMaterialIndex.contains(face));
        int index= m_EdgeToMaterialIndex.value(face);
        Q_ASSERT(m_EdgeMaterialId.contains(index));
        GLC_uint materialId= m_EdgeMaterialId.value(index);
        Q_ASSERT(containsMaterial(materialId));
        pSubject= material(materialId);
    }
    else
    {
        Q_ASSERT(containsMaterial(m_MasterMaterialId));
        pSubject= material(m_MasterMaterialId);
    }

    return pSubject;
}

void GLC_ExtrudedMesh::createPolygons()
{
    Q_ASSERT(m_Polygons.isEmpty());

    const GLC_Polygon baseFacePolygon(baseFacePoints());
    const GLC_Polygon createdFacePolygon(createdFacePoints());

    m_Polygons.append(baseFacePolygon);
    m_Polygons.append(createdFacePolygon);

    const GLC_Vector3d offset(m_ExtrusionVector * m_ExtrusionLenght);

    const int count= m_Points.count();
    for (int i= 0; i < count; ++i)
    {
        GLC_Polygon polygon;
        const GLC_Point3d p1(baseFacePolygon.pointAt(i));
        const GLC_Point3d p2(p1 + offset);
        const GLC_Point3d p4(baseFacePolygon.pointAt((i + 1) % count));
        const GLC_Point3d p3(p4 + offset);

        polygon.addPoint(p1);
        polygon.addPoint(p2);
        polygon.addPoint(p3);
        polygon.addPoint(p4);
        m_Polygons.append(polygon);
    }
}

void GLC_ExtrudedMesh::glDraw(const GLC_RenderProperties& renderProperties)
{
    if (GLC_Mesh::isEmpty())
    {
        createMeshAndWire();
    }

    GLC_Mesh::glDraw(renderProperties);
}
