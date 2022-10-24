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

//! \file glc_boundingbox.h interface for the GLC_BoundingBox class.

#ifndef GLC_BOUNDINGBOX_
#define GLC_BOUNDINGBOX_

#include "maths/glc_vector3d.h"
#include "maths/glc_utils_maths.h"
#include "maths/glc_matrix4x4.h"
#include "maths/glc_geomtools.h"

#include <QtDebug>
#include "glc_config.h"

class GLC_Plane;

class GLC_LIB_EXPORT GLC_Obb
{
public:
    bool getCollision(const GLC_Obb& other) const;

public:
    const GLC_Vector3d& position() const
    {return m_Position;}

    const GLC_Vector3d& xAxis() const
    {return m_XAxis;}

    const GLC_Vector3d& yAxis() const
    {return m_YAxis;}

    const GLC_Vector3d& zAxis() const
    {return m_ZAxis;}

    const GLC_Vector3d& halfSize() const
    {return m_HalfSize;}

public:
    void setPosition(const GLC_Vector3d& value)
    {m_Position= value;}

    void setXAxis(const GLC_Vector3d& value)
    {m_XAxis= value;}

    void setYAxis(const GLC_Vector3d& value)
    {m_YAxis= value;}

    void setZAxis(const GLC_Vector3d& value)
    {m_ZAxis= value;}

    void setHalfSize(const GLC_Vector3d& value)
    {m_HalfSize= value;}

private:
    bool getSeparatingPlane(const GLC_Vector3d& rPos, const GLC_Vector3d& plane, const GLC_Obb& other) const;

private:
    GLC_Vector3d m_Position;
    GLC_Vector3d m_XAxis;
    GLC_Vector3d m_YAxis;
    GLC_Vector3d m_ZAxis;
    GLC_Vector3d m_HalfSize;
};

inline bool GLC_Obb::getCollision(const GLC_Obb& other) const
{
    const GLC_Vector3d rPos(other.position() - position());

    return !(getSeparatingPlane(rPos, xAxis(), other) ||
             getSeparatingPlane(rPos, yAxis(), other) ||
             getSeparatingPlane(rPos, zAxis(), other) ||
             getSeparatingPlane(rPos, other.xAxis(), other) ||
             getSeparatingPlane(rPos, other.yAxis(), other) ||
             getSeparatingPlane(rPos, other.zAxis(), other) ||
             getSeparatingPlane(rPos, xAxis() ^ other.xAxis(), other) ||
             getSeparatingPlane(rPos, xAxis() ^ other.yAxis(), other) ||
             getSeparatingPlane(rPos, xAxis() ^ other.zAxis(), other) ||
             getSeparatingPlane(rPos, yAxis() ^ other.xAxis(), other) ||
             getSeparatingPlane(rPos, yAxis() ^ other.yAxis(), other) ||
             getSeparatingPlane(rPos, yAxis() ^ other.zAxis(), other) ||
             getSeparatingPlane(rPos, zAxis() ^ other.xAxis(), other) ||
             getSeparatingPlane(rPos, zAxis() ^ other.yAxis(), other) ||
             getSeparatingPlane(rPos, zAxis() ^ other.zAxis(), other));
}

inline bool GLC_Obb::getSeparatingPlane(const GLC_Vector3d& rPos, const GLC_Vector3d& plane, const GLC_Obb& other) const
{
    return (fabs(rPos * plane) >
            (fabs((xAxis() * halfSize().x()) * plane) +
             fabs((yAxis() * halfSize().y()) * plane) +
             fabs((zAxis() * halfSize().z()) * plane) +
             fabs((other.xAxis() * other.halfSize().x()) * plane) +
             fabs((other.yAxis() * other.halfSize().y()) * plane) +
             fabs((other.zAxis() * other.halfSize().z()) * plane)));
}

//////////////////////////////////////////////////////////////////////
//! \class GLC_BoundingBox
/*! \brief GLC_BoundingBox : Geometry bounding box*/

/*! An GLC_BoundingBox is a axis oriented bounding box
*/

//////////////////////////////////////////////////////////////////////

class GLC_LIB_EXPORT GLC_BoundingBox
{
    friend GLC_LIB_EXPORT QDataStream &operator<<(QDataStream &, const GLC_BoundingBox &);
    friend GLC_LIB_EXPORT QDataStream &operator>>(QDataStream &, GLC_BoundingBox &);

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Construct an empty bounding box
    GLC_BoundingBox();

    //! Construct a bounding box from  the given bounding box
    GLC_BoundingBox(const GLC_BoundingBox& other);

    //! Construct a bounding box from the given 3d point
    GLC_BoundingBox(const GLC_Point3d& lower, const GLC_Point3d& upper);

    virtual ~GLC_BoundingBox();

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Return this class Chunk ID
    static quint32 chunckID();

    //! Return true if this bounding box is empty
    bool isEmpty(void) const
    {
        return m_IsEmpty;
    }

    QList<GLC_Point3d> points() const;

    //! Return true if the given 3d point intersect this bounding box
    bool intersect(const GLC_Point3d& point) const;

    //! Return true if the given plane intersect this boundingBox
    bool intersect(const GLC_Plane& plane) const;

    //! Return true if the given bounding box intersect this bounding box
    inline bool intersect(const GLC_BoundingBox& boundingBox, double epsilon= glc::EPSILON) const;

    inline bool intersectObb(const GLC_BoundingBox& other) const;

    inline bool fuzzyIntersect(const GLC_BoundingBox& boundingBox) const;

    //! Return true if the given 3d point intersect this bounding sphere of bounding box
    inline bool intersectBoundingSphere(const GLC_Point3d& point) const;

    //! Return true if the given bounding sphere of bounding box intersect the bounding sphere of box bounding box
    inline bool intersectBoundingSphere(const GLC_BoundingBox& boundingSphere) const;

    //! Return the lower corner of this bounding box
    inline const GLC_Point3d& lowerCorner() const
    {return m_Lower;}

    //! Return the upper corner of this bounding box
    inline const GLC_Point3d& upperCorner() const
    {return m_Upper;}

    //! Return the center of this bounding box
    inline GLC_Point3d center() const;

    //! Return the radius of this bounding sphere of bounding box
    inline double boundingSphereRadius() const
    {return GLC_Vector3d(m_Lower - m_Upper).length() / 2.0;}

    //! Return true if this bounding box is equal of the given bounding box
    inline bool operator == (const GLC_BoundingBox& other) const;

    //! Return true if this bounding box is not equal of the given bounding box
    inline bool operator != (const GLC_BoundingBox& boundingBox)
    {return !(*this == boundingBox);}

    //! Return the length of this bounding box on x axis
    inline double xLength() const
    {return fabs(m_Upper.x() - m_Lower.x());}

    //! Return the length of this bounding box on y axis
    inline double yLength() const
    {return fabs(m_Upper.y() - m_Lower.y());}

    //! Return the length of this bounding box on z axis
    inline double zLength() const
    {return fabs(m_Upper.z() - m_Lower.z());}

    //! Return the size of this bounding box
    GLC_Vector3d size() const;


//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Combine this bounding Box with the given 3d point and return a reference of this bounding box
    GLC_BoundingBox& combine(const GLC_Point3d& point);

    //! Combine this bounding Box with the given 3d point and return a reference of this bounding box
    GLC_BoundingBox& combine(const GLC_Point3df& point);

    //! Combine this bounding Box with the given bounding box and return a reference of this bounding box
    GLC_BoundingBox& combine(const GLC_BoundingBox& box);

    //! Transform this bounding Box with the given matrix and return a reference of this bounding box
    GLC_BoundingBox& transform(const GLC_Matrix4x4& matrix);

    virtual void clear();

//@}

private:
    inline void setObb();
    inline void updateObb(const GLC_Matrix4x4& matrix);

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
    //! Lower corner 3d point
    GLC_Point3d m_Lower;

    //! Upper corner 3d point
    GLC_Point3d m_Upper;

    //! Flag to know if this bounding box is empty
    bool m_IsEmpty;

    GLC_Obb m_Obb;

    //! This class chunk id
    static quint32 m_ChunkId;
};

//! Non-member stream operator
GLC_LIB_EXPORT QDataStream &operator<<(QDataStream &, const GLC_BoundingBox &);
GLC_LIB_EXPORT QDataStream &operator>>(QDataStream &, GLC_BoundingBox &);

// Return true if the given bounding box intersect this bounding box
bool GLC_BoundingBox::intersect(const GLC_BoundingBox& boundingBox, double epsilon) const
{
    bool subject= false;
    // Distance between bounding box center
    const GLC_Point3d thisCenter= center();
    const GLC_Point3d otherCenter= boundingBox.center();

    if (this->intersect(otherCenter) || (boundingBox.intersect(thisCenter)))
    {
        subject= true;
    }
    else
    {
        const double distanceX= fabs(thisCenter.x() - otherCenter.x());
        const double distanceY= fabs(thisCenter.y() - otherCenter.y());
        const double distanceZ= fabs(thisCenter.z() - otherCenter.z());

        const double deltaX= ((xLength() + boundingBox.xLength()) * 0.5);
        const double deltaY= ((yLength() + boundingBox.yLength()) * 0.5);
        const double deltaZ= ((zLength() + boundingBox.zLength()) * 0.5);

        subject= (distanceX < deltaX) && !glc::compare(distanceX, deltaX, epsilon);
        subject= subject && ((distanceY < deltaY) && !glc::compare(distanceY, deltaY, epsilon));
        subject= subject && ((distanceZ < deltaZ) && !glc::compare(distanceZ, deltaZ, epsilon));
    }
    return subject;
}

inline bool GLC_BoundingBox::intersectObb(const GLC_BoundingBox& other) const
{
    return m_Obb.getCollision(other.m_Obb);
}

bool GLC_BoundingBox::fuzzyIntersect(const GLC_BoundingBox& boundingBox) const
{
    bool subject= false;
    // Distance between bounding box center
    const GLC_Point3d thisCenter= center();
    const GLC_Point3d otherCenter= boundingBox.center();

    if (this->intersect(otherCenter) || (boundingBox.intersect(thisCenter)))
    {
        subject= true;
    }
    else
    {
        const double distanceX= fabs(thisCenter.x() - otherCenter.x());
        const double distanceY= fabs(thisCenter.y() - otherCenter.y());
        const double distanceZ= fabs(thisCenter.z() - otherCenter.z());

        const double deltaX= ((xLength() + boundingBox.xLength()) * 0.5);
        const double deltaY= ((yLength() + boundingBox.yLength()) * 0.5);
        const double deltaZ= ((zLength() + boundingBox.zLength()) * 0.5);

        subject= (distanceX < deltaX) || glc::fuzzyCompare(distanceX, deltaX);
        subject= subject && ((distanceY < deltaY) || glc::fuzzyCompare(distanceY, deltaY));
        subject= subject && ((distanceZ < deltaZ) || glc::fuzzyCompare(distanceZ, deltaZ));
    }
    return subject;
}

bool GLC_BoundingBox::intersectBoundingSphere(const GLC_Point3d& point) const
{
    const double distance= (center() - point).length();
    return distance < boundingSphereRadius();
}

bool GLC_BoundingBox::intersectBoundingSphere(const GLC_BoundingBox& boundingSphere) const
{
    const double distance= (center() - boundingSphere.center()).length();
    return distance < (boundingSphereRadius() + boundingSphere.boundingSphereRadius());
}

bool GLC_BoundingBox::operator == (const GLC_BoundingBox& other) const
{
    bool subject= true;
    if (this != &other)
    {
        subject= (m_Lower == other.m_Lower) && (m_Upper == other.m_Upper);
    }

    return subject;
}

inline void GLC_BoundingBox::setObb()
{
    m_Obb.setPosition(this->center());
    m_Obb.setXAxis(glc::X_AXIS);
    m_Obb.setYAxis(glc::Y_AXIS);
    m_Obb.setZAxis(glc::Z_AXIS);
    m_Obb.setHalfSize(size() * 0.5);

}



GLC_Point3d GLC_BoundingBox::center(void) const
{
    GLC_Vector3d vectResult = (m_Lower + m_Upper) * 0.5;
    return vectResult;
}

#endif /*GLC_BOUNDINGBOX_*/
