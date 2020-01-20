/*
 *  glc_triangle.cpp
 *
 *  Created on: 07
 *      Author: Laurent Ribon
 */

#include "glc_geomtools.h"

#include "glc_triangle.h"

GLC_Triangle::GLC_Triangle()
    : m_Points()
    , m_Normals()
    , m_SharpEdge()
    , m_Mutex()
{
    m_SharpEdge[0]= -1;
    m_SharpEdge[1]= -1;
    m_SharpEdge[2]= -1;
}

GLC_Triangle::GLC_Triangle(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3
                           , const GLC_Vector3d& n1, const GLC_Vector3d& n2, const GLC_Vector3d& n3)
    : m_Points()
    , m_Normals()
    , m_SharpEdge()
    , m_Mutex()
{
    m_Points[0]= p1;
    m_Points[1]= p2;
    m_Points[2]= p3;

    m_Normals[0]= n1;
    m_Normals[1]= n2;
    m_Normals[2]= n3;

    m_SharpEdge[0]= -1;
    m_SharpEdge[1]= -1;
    m_SharpEdge[2]= -1;
}

GLC_Triangle::GLC_Triangle(const GLC_Triangle& other)
    : m_Points()
    , m_Normals()
    , m_SharpEdge()
    , m_Mutex()
{
    memcpy(m_Points, other.m_Points, sizeof(GLC_Point3d) * 3);
    memcpy(m_Normals, other.m_Normals, sizeof(GLC_Vector3d) * 3);
    memcpy(m_SharpEdge, other.m_SharpEdge, sizeof(int) * 3);
}

bool GLC_Triangle::operator ==(const GLC_Triangle& other) const
{
    bool subject= true;
    if (this != &other)
    {
        for (int i= 0; subject && (i < 3); ++i)
        {
            subject= subject && (m_Points[i] == other.m_Points[i]);
            subject= subject && (m_Normals[i] == other.m_Normals[i]);
        }
    }

    return subject;
}

GLC_BoundingBox GLC_Triangle::boundingBox() const
{
    GLC_BoundingBox subject;
    subject.combine(m_Points[0]);
    subject.combine(m_Points[1]);
    subject.combine(m_Points[2]);

    return subject;
}

QList<GLC_Point3d> GLC_Triangle::sharpEdges() const
{
    QList<GLC_Point3d> subject;
    if (m_SharpEdge[0] == 1)
    {
        if ((m_SharpEdge[1] == 0) && (m_SharpEdge[2] == 1))
        {
            subject << m_Points[2] << m_Points[0] << m_Points[1];
        }
        else
        {
            subject << m_Points[0] << m_Points[1];
            if (m_SharpEdge[1] == 1) subject << m_Points[2];
            if (m_SharpEdge[2] == 1) subject << m_Points[0];
        }
    }
    else if (m_SharpEdge[1] == 1)
    {
        subject << m_Points[1] << m_Points[2];
        if (m_SharpEdge[2] == 1) subject << m_Points[0];
    }
    else if (m_SharpEdge[2] == 1)
    {
        subject << m_Points[2] << m_Points[0];
    }

    return subject;
}

GLC_Triangle& GLC_Triangle::operator =(const GLC_Triangle& other)
{
    if (this != &other)
    {
        memcpy(m_Points, other.m_Points, sizeof(GLC_Point3d) * 3);
        memcpy(m_Normals, other.m_Normals, sizeof(GLC_Vector3d) * 3);
        memcpy(m_SharpEdge, other.m_SharpEdge, sizeof(int) * 3);
    }

    return *this;
}

void GLC_Triangle::setSharpEdge(GLC_Triangle* pOther, double angleThreshold)
{
    Q_ASSERT(this != pOther);
    if (boundingBox().fuzzyIntersect(pOther->boundingBox()))
    {
        bool edgeFound= false;
        for (int i= 0; (i < 3) && !edgeFound; ++i)
        {
            const GLC_Point3d& tri1V1(m_Points[i]);
            const GLC_Point3d& tri1N1(m_Normals[i]);

            const GLC_Point3d& tri1V2(m_Points[(i + 1) % 3]);
            const GLC_Point3d& tri1N2(m_Normals[(i + 1) % 3]);
            for (int j= 0; (j < 3) && !edgeFound; ++j)
            {
                const GLC_Point3d& tri2V1(pOther->m_Points[j]);
                const GLC_Point3d& tri2N1(pOther->m_Normals[j]);

                const GLC_Point3d& tri2V2(pOther->m_Points[(j + 1) % 3]);
                const GLC_Point3d& tri2N2(pOther->m_Normals[(j + 1) % 3]);

                edgeFound= glc::segmentsOverlap(tri1V1, tri1V2, tri2V1, tri2V2);
                if (edgeFound)
                {
                    const double delta1= (tri1V1 - tri2V1).squaredLength();
                    const double delta2= (tri1V1 - tri2V2).squaredLength();
                    const double delta3= (tri1V2 - tri2V1).squaredLength();
                    const double delta4= (tri1V2 - tri2V2).squaredLength();

                    double angle= 0;
                    if (glc::compare(delta1, 0.0) || ((delta1 < delta2) && (delta1 < delta3) && (delta1 < delta4)))
                    {
                        angle= tri1N1.angleWithVect(tri2N1);
                    }
                    else if (glc::compare(delta2, 0.0) || ((delta2 < delta1) && (delta2 < delta3) && (delta2 < delta4)))
                    {
                        angle= tri1N1.angleWithVect(tri2N2);
                    }
                    else if (glc::compare(delta3, 0.0) || ((delta3 < delta1) && (delta3 < delta2) && (delta3 < delta4)))
                    {
                        angle= tri1N2.angleWithVect(tri2N1);
                    }
                    else
                    {
                        angle= tri1N2.angleWithVect(tri2N2);
                    }

                    const bool isSharp= !(angle < angleThreshold);

                    QMutexLocker lock(&m_Mutex);
                    setSharpEdge(i, isSharp);
                    lock.unlock();
                    QMutexLocker otherLock(&(pOther->m_Mutex));
                    pOther->setSharpEdge(j, isSharp);
                }
            }
        }
    }
}

void GLC_Triangle::setSharpEdge(int index, bool sharp)
{
    if (m_SharpEdge[index] == -1)
    {
        m_SharpEdge[index]= ((sharp) ? 1 : 0);
    }
    else
    {
        m_SharpEdge[index]= ((sharp && (m_SharpEdge[index] == 1)) ? 1 : 0);
    }
}


bool compare(const GLC_Triangle& t1, const GLC_Triangle& t2)
{
    bool subject= true;
    if (&t1 != &t2)
    {
        for (int i= 0; subject && (i < 3); ++i)
        {
            subject= subject && glc::compare(t1.m_Points[i], t2.m_Points[i]);
            subject= subject && glc::compare(t1.m_Normals[i], t2.m_Normals[i]);
        }
    }

    return subject;
}

bool compare(const GLC_Triangle& t1, const GLC_Triangle& t2, double accuracy)
{
    bool subject= true;
    if (&t1 != &t2)
    {
        for (int i= 0; subject && (i < 3); ++i)
        {
            subject= subject && glc::compare(t1.m_Points[i], t2.m_Points[i], accuracy);
            subject= subject && glc::compare(t1.m_Normals[i], t2.m_Normals[i], accuracy);
        }
    }

    return subject;
}
