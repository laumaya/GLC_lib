/*
 *  glc_triangle.h
 *
 *  Created on: 07
 *      Author: Laurent Ribon
 */
#ifndef GLC_TRIANGLE_H
#define GLC_TRIANGLE_H

#include <QMutex>
#include <QMutexLocker>

#include "../glc_config.h"

#include "../glc_boundingbox.h"

#include "glc_vector3d.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Triangle
/*! \brief GLC_Triangle : Math triangle representation */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_Triangle
{
    friend GLC_LIB_EXPORT bool compare(const GLC_Triangle& t1, const GLC_Triangle& t2);
    friend GLC_LIB_EXPORT bool compare(const GLC_Triangle& t1, const GLC_Triangle& t2, double accuracy);

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
    GLC_Triangle();

    GLC_Triangle(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3,
                 const GLC_Vector3d& n1, const GLC_Vector3d& n2, const GLC_Vector3d& n3);

    GLC_Triangle(const GLC_Triangle& other);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

    bool operator == (const GLC_Triangle& other) const;

    bool operator != (const GLC_Triangle& other) const
    {return !(this->operator ==(other));}

    const GLC_Point3d& point(int i) const
    {return m_Points[i];}

    const GLC_Vector3d normal(int i) const
    {return m_Normals[i];}

    const GLC_Point3d& p1() const
    {return m_Points[0];}

    const GLC_Point3d& p2() const
    {return m_Points[1];}

    const GLC_Point3d& p3() const
    {return m_Points[2];}

    const GLC_Vector3d& n1() const
    {return m_Normals[0];}

    const GLC_Vector3d& n2() const
    {return m_Normals[1];}

    const GLC_Vector3d& n3() const
    {return m_Normals[2];}

    const GLC_BoundingBox& boundingBox() const
    {return m_Bbox;}

    bool hasSharpEdge() const
    {return ((m_SharpEdge[0] == 1) || (m_SharpEdge[1] == 1) || (m_SharpEdge[2] == 1));}

    QList<GLC_Point3d> sharpEdges() const;

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

    GLC_Triangle& operator =(const GLC_Triangle& other);

    void setP1(const GLC_Point3d& p1)
    {m_Points[0]= p1;}

    void setP2(const GLC_Point3d& p2)
    {m_Points[1]= p2;}

    void setP3(const GLC_Point3d& p3)
    {m_Points[2]= p3;}

    void setN1(const GLC_Vector3d& n1)
    {m_Normals[0]= n1;}

    void setN2(const GLC_Vector3d& n2)
    {m_Normals[1]= n2;}

    void setN3(const GLC_Vector3d& n3)
    {m_Normals[2]= n3;}

    void setPoints(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3)
    {m_Points[0]= p1; m_Points[1]= p2; m_Points[2]= p3; computeBbox();}

    void setNormal(const GLC_Vector3d& n1, const GLC_Vector3d& n2, const GLC_Vector3d& n3)
    {m_Normals[0]= n1; m_Normals[1]= n2; m_Normals[2]= n3;}

    void setSharpEdge(GLC_Triangle* pOther, double angleThreshold);

    void setSharpEdge(int index, bool sharp);

    void computeBbox();

//@}

//////////////////////////////////////////////////////////////////////
// Private Member
//////////////////////////////////////////////////////////////////////

private:
    GLC_Point3d m_Points[3];
    GLC_Vector3d m_Normals[3];
    int m_SharpEdge[3];
    QMutex m_Mutex;
    GLC_BoundingBox m_Bbox;
};

GLC_LIB_EXPORT bool compare(const GLC_Triangle& t1, const GLC_Triangle& t2);
GLC_LIB_EXPORT bool compare(const GLC_Triangle& t1, const GLC_Triangle& t2, double accuracy);

#endif // GLC_TRIANGLE_H
