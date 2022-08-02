/*
 *  glc_lathemesh.cpp
 *
 *  Created on: 30/10/2018
 *      Author: Laurent Ribon
 */
#include "glc_lathemesh.h"

#include "../maths/glc_geomtools.h"

const quint32 GLC_LatheMesh::m_ChunkId= 0xA713;

using namespace glc;

GLC_LatheMesh::GLC_LatheMesh(const QList<GLC_Point3d>& profile)
    : GLC_Mesh()
    , m_Profile(profile)
    , m_SmothingPoints()
    , m_InvisibleEdgeIndex()
    , m_PhiStart(0.0)
    , m_PhiLength(2 * glc::PI)
    , m_Discret(GLC_DISCRET)
    , m_Step(0)
{

}

GLC_LatheMesh::GLC_LatheMesh(const GLC_LatheMesh& other)
    : GLC_Mesh(other)
    , m_Profile(other.m_Profile)
    , m_SmothingPoints(other.m_SmothingPoints)
    , m_InvisibleEdgeIndex(other.m_InvisibleEdgeIndex)
    , m_PhiStart(other.m_PhiStart)
    , m_PhiLength(other.m_PhiLength)
    , m_Discret(other.m_Discret)
    , m_Step(other.m_Step)
{

}

quint32 GLC_LatheMesh::chunckID()
{
    return m_ChunkId;
}

const GLC_BoundingBox&GLC_LatheMesh::boundingBox()
{
    if (GLC_Mesh::isEmpty())
    {
        createMeshAndWire();
    }
    return GLC_Mesh::boundingBox();
}

bool GLC_LatheMesh::update()
{
    bool subject= false;
    if (GLC_Mesh::isEmpty())
    {
        createMeshAndWire();
        subject= true;
    }

    return subject;
}

GLC_LatheMesh&GLC_LatheMesh::operator=(const GLC_LatheMesh& other)
{
    if (this != &other)
    {
        GLC_Mesh::operator =(other);
        m_Profile= other.m_Profile;
        m_SmothingPoints= other.m_SmothingPoints;
        m_InvisibleEdgeIndex= other.m_InvisibleEdgeIndex;
        m_PhiStart= other.m_PhiStart;
        m_PhiLength= other.m_PhiLength;
        m_Discret= other.m_Discret;
        m_Step= other.m_Step;
    }

    return *this;

}

void GLC_LatheMesh::setGeometry(const QList<GLC_Point3d>& profile, double phiStart, double phiLength)
{
    m_Profile= profile;
    m_PhiStart= phiStart;
    m_PhiLength= phiLength;
    GLC_Mesh::clearMeshWireAndBoundingBox();
}

void GLC_LatheMesh::setProfile(const QList<GLC_Point3d>& profile)
{
    const int count= profile.count();
    bool isEqual= false;
    if (count == m_Profile.count())
    {
        isEqual= true;
        for (int i= 0; i < count; ++i)
        {
            isEqual= glc::compare(profile.at(i), m_Profile.at(i));
            if (!isEqual) break;
        }
    }

    if (!isEqual)
    {
        m_Profile= profile;
        m_SmothingPoints.clear();
        m_InvisibleEdgeIndex.clear();

        GLC_Mesh::clearMeshWireAndBoundingBox();
    }
}

void GLC_LatheMesh::setDiscret(double discret)
{
    if (m_Discret != discret)
    {
        m_Discret= discret;
        GLC_Mesh::clearMeshWireAndBoundingBox();
    }
}

void GLC_LatheMesh::setSmoothingIndex(const QList<int>& smothingIndex)
{
    const int count= smothingIndex.count();
    bool isEqual= false;
    if (count == m_SmothingPoints.count())
    {
        for (int i= 0; i < count; ++i)
        {
            isEqual= smothingIndex.at(i) == m_SmothingPoints.at(i);
            if (!isEqual) break;
        }
    }

    if (!isEqual)
    {
        m_SmothingPoints= smothingIndex;
        GLC_Mesh::clearMeshWireAndBoundingBox();
    }
}

void GLC_LatheMesh::setInvisibleEdgeIndex(const QList<int>& invisibleEdgeIndex)
{
    const int count= invisibleEdgeIndex.count();
    bool isEqual= false;
    if (count == m_InvisibleEdgeIndex.count())
    {
        for (int i= 0; i < count; ++i)
        {
            isEqual= invisibleEdgeIndex.at(i) == m_InvisibleEdgeIndex.at(i);
            if (!isEqual) break;
        }
    }

    if (!isEqual)
    {
        m_InvisibleEdgeIndex= invisibleEdgeIndex;
        GLC_Mesh::clearMeshWireAndBoundingBox();
    }
}

void GLC_LatheMesh::glDraw(const GLC_RenderProperties& renderProperties)
{
    if (GLC_Mesh::isEmpty())
    {
        createMeshAndWire();
    }

    GLC_Mesh::glDraw(renderProperties);
}

void GLC_LatheMesh::createMeshAndWire()
{
    Q_ASSERT(GLC_Mesh::isEmpty());
    Q_ASSERT(m_WireData.isEmpty());

    createMesh();
    createWire();

    finish();
}

void GLC_LatheMesh::createMesh()
{
    QList<GLC_Vector3d> profileNormals(this->profileNormals());
    QList<GLC_Point3d> profileVertices(this->profileVertices());
    QList<double> profileV(this->profileV());

    Q_ASSERT(profileNormals.count() == profileVertices.count());
    Q_ASSERT(profileV.count() == profileNormals.count());

    const double angle= m_PhiLength - m_PhiStart;
    m_Step= static_cast<GLuint>(static_cast<double>(m_Discret) * (angle / (2 * glc::PI)));
    const int profileVerticeCount= profileNormals.count();
    // Bulk data
    GLfloatVector vertices(3 * m_Step * profileVerticeCount);
    GLfloatVector normals(3 * m_Step * profileVerticeCount);
    GLfloatVector texels(2 * m_Step* profileVerticeCount);

    if (m_Step < 2) m_Step= 2;
    const double angleOnStep= angle / static_cast<double>(m_Step);

    for(int i= 0; i < m_Step; ++i)
    {
        const double currentAngle= angleOnStep * i;
        const GLC_Matrix4x4 matrix(glc::Z_AXIS, currentAngle);

        const double u= static_cast<double>(i) / static_cast<double>(m_Step);

        for (int j= 0; j < profileVerticeCount; ++j)
        {
            const int baseIndex= (3 * i * profileVerticeCount) + (j * 3);
            GLC_Point3d point(matrix * profileVertices.at(j));
            vertices[baseIndex]= static_cast<GLfloat>(point.x());
            vertices[baseIndex + 1]= static_cast<GLfloat>(point.y());
            vertices[baseIndex + 2]= static_cast<GLfloat>(point.z());

            GLC_Vector3d normal(matrix * profileNormals.at(j));
            normals[baseIndex]= static_cast<GLfloat>(normal.x());
            normals[baseIndex + 1]= static_cast<GLfloat>(normal.y());
            normals[baseIndex + 2]= static_cast<GLfloat>(normal.z());

            const double v= profileV.at(j);
            const int baseUvIndex= (2 * i * profileVerticeCount) + j;
            texels[baseUvIndex]= u;
            texels[baseUvIndex + 1]= v;
        }
    }
    // Add bulk data in to the mesh
    GLC_Mesh::addVertice(vertices);
    GLC_Mesh::addNormals(normals);
    GLC_Mesh::addTexels(texels);

    // Set the material to use
    GLC_Material* pMaterial;
    if (hasMaterial())
    {
        pMaterial= this->firstMaterial();
    }
    else
    {
        pMaterial= new GLC_Material();
    }

    for (int i= 0; i < m_Step; ++i)
    {
        IndexList indexList;
        const int baseIndex1= i * profileVerticeCount;
        const int baseIndex2= ((i * profileVerticeCount) + profileVerticeCount) % ((m_Step) * profileVerticeCount);
        indexList << baseIndex1 << baseIndex2 + 1 << baseIndex1 + 1;
        for (int j= 2; j < profileVerticeCount - 2; j+=2)
        {
            const int index1= baseIndex1 + j;
            const int index2= baseIndex2 + j;

            indexList << index1 << index2 << index1 + 1;
            indexList << index2 << (index2 + 1) <<  index1 + 1;
        }

        const int lastIndex1= baseIndex1 + profileVerticeCount - 1;
        const int lastIndex2= baseIndex2 + profileVerticeCount - 1;
        indexList << lastIndex1 - 1 << lastIndex2 - 1 << lastIndex1;

        GLC_Mesh::addTriangles(pMaterial, indexList);
    }
}

void GLC_LatheMesh::createWire()
{
    Q_ASSERT(m_WireData.isEmpty());
    const int count= m_Profile.count();
    for (int i= 1; i < count - 1; ++i)
    {
        if (!m_SmothingPoints.contains(i))
        {
            createCircle(i);
        }
    }
}

void GLC_LatheMesh::createCircle(int index)
{
    const GLC_Point3d& point= m_Profile.at(index);
    const double radius= point.x();
    const double height= point.z();

    const double angle= m_PhiLength - m_PhiStart;

    m_Step= static_cast<int>(static_cast<double>(m_Discret) * (angle / (2 * glc::PI)));
    if (m_Step < 2) m_Step= 2;

    // Float vector
    GLfloatVector floatVector;

    // Resize the Vertex vector
    const int size= (m_Step + 1) * 3;
    floatVector.resize(size);
    // Fill Vertex Vector
    const double angleOnStep= angle / static_cast<double>(m_Step);
    for (int i= 0; i <= m_Step; ++i)
    {
        floatVector[(i * 3)]= static_cast<float>(radius * cos(static_cast<double>(i) * angleOnStep));
        floatVector[(i * 3) + 1]= static_cast<float>(radius * sin(static_cast<double>(i) * angleOnStep));
        floatVector[(i * 3) + 2]= static_cast<float>(height);
    }
    GLC_Geometry::addVerticeGroup(floatVector);
}

QList<GLC_Vector3d> GLC_LatheMesh::profileNormals() const
{
    QList<GLC_Vector3d> subject;
    const int count= m_Profile.count();
    for (int i= 0; i < (count - 1); ++i)
    {

        GLC_Vector3d vect(m_Profile.at(i + 1) - m_Profile.at(i));
        GLC_Vector3d normal= GLC_Vector3d(glc::Y_AXIS ^ vect).normalize();

        GLC_Vector3d normal1;
        GLC_Vector3d normal2;

        if (m_SmothingPoints.contains(i))
        {
            if (i > 0)
            {
                const int prevIndex= i - 1;
                GLC_Vector3d prevVect(m_Profile.at(i) - m_Profile.at(prevIndex));
                GLC_Vector3d prevNormal= GLC_Vector3d(glc::Y_AXIS ^ prevVect).normalize();
                normal1= (normal + prevNormal).normalize();

            }
            else
            {
                normal1= -glc::Z_AXIS;
            }
        }
        else
        {
            normal1= normal;
        }

        if (m_SmothingPoints.contains(i + 1))
        {
            if (i < (count - 1))
            {
                GLC_Vector3d nextVect(m_Profile.at((i + 2) % count) - m_Profile.at((i + 1) % count));
                GLC_Vector3d nextNormal= GLC_Vector3d(glc::Y_AXIS ^ nextVect).normalize();
                normal2= (normal + nextNormal).normalize();
            }
            else
            {
                normal2= glc::Z_AXIS;
            }
        }
        else
        {
            normal2= normal;
        }

        // First segment point
        subject << normal1 << normal2;
    }

    return subject;
}

QList<GLC_Point3d> GLC_LatheMesh::profileVertices() const
{
    QList<GLC_Point3d> subject;
    const int count= m_Profile.count() - 1;
    for (int i= 0; i < count; ++i)
    {

        GLC_Vector3d vertex1(m_Profile.at(i));
        GLC_Vector3d vertex2(m_Profile.at(i + 1));

        subject << vertex1 << vertex2;
    }

    return subject;
}

QList<double> GLC_LatheMesh::profileV()
{
    QList<double> subject;
    const double min= m_Profile.constFirst().z();
    const double range= m_Profile.last().z() - min;

    const int count= m_Profile.count() - 1;
    for (int i= 0; i < count; ++i)
    {

        const double v1= (m_Profile.at(i).z() - min) / range;
        const double v2= (m_Profile.at(i + 1).z() - min) / range;

        subject << v1 << v2;
    }

    return subject;
}
