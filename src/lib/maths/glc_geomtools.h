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

//! \file glc_geomtools.h declaration of geometry tools functions

#ifndef GLC_GEOMTOOLS_H_
#define GLC_GEOMTOOLS_H_

#include <QVector>
#include <QList>
#include <QPointF>

#include "../glc_global.h"

#include "glc_vector3d.h"
#include "glc_line3d.h"
#include "glc_plane.h"

#include "glc_utils_maths.h"

#include "../glc_config.h"

namespace glc
{
    const double defaultPrecision= 0.01;
    GLC_LIB_EXPORT extern double comparedPrecision;
//////////////////////////////////////////////////////////////////////
/*! \name Tools Functions*/
//@{
//////////////////////////////////////////////////////////////////////
    //! test if the given 2D polygon is convex
    GLC_LIB_EXPORT bool polygon2DIsConvex(const QList<GLC_Point2d>& vertices);

    //! Test if the given 3d polygon is convex
    GLC_LIB_EXPORT bool polygonIsConvex(QList<GLuint>* pIndexList, const QList<float>& bulkList);

    //! find intersection between two 2D segments
    /*! Return the intersection as QVector of GLC_Point2d
     *  - Empty QVector if there is no intersection
     *  - Qvector size 1 if there is a unique intersection
     *  - Qvector size 2 if the segement overlap*/
    GLC_LIB_EXPORT QVector<GLC_Point2d> findIntersection(const GLC_Point2d&, const GLC_Point2d&, const GLC_Point2d&, const GLC_Point2d&);

    //! Return true if there is an intersection between 2 segments
    GLC_LIB_EXPORT bool isIntersected(const GLC_Point2d&, const GLC_Point2d&, const GLC_Point2d&, const GLC_Point2d&);

    //! return true if there is an intersection between a ray and a segment
    GLC_LIB_EXPORT bool isIntersectedRaySegment(const GLC_Point2d&, const GLC_Vector2d&, const GLC_Point2d&, const GLC_Point2d&);

    //! Find intersection of two intervals [u0, u1] and [v0, v1]
    /*! Return the intersection as QVector of GLC_Vector2d
     *  - Empty QVector if there is no intersection
     *  - Qvector size 1 if there is a unique intersection
     *  - Qvector size 2 if the segement overlap*/
    GLC_LIB_EXPORT QVector<double> findIntersection(const double&, const double&, const double&, const double&);

    //! Return true if the segment is in polygon cone
    GLC_LIB_EXPORT bool segmentInCone(const GLC_Point2d&, const GLC_Point2d&, const GLC_Point2d&, const GLC_Point2d&);

    //! Return true if the segment is a polygon diagonal
    GLC_LIB_EXPORT bool isDiagonal(const QList<GLC_Point2d>&, const int, const int);

    //! Triangulate a polygon
    GLC_LIB_EXPORT void triangulate(QList<GLC_Point2d>&, QList<int>&, QList<int>&);

    //! Return true if the polygon is couterclockwise ordered
    GLC_LIB_EXPORT bool isCounterclockwiseOrdered(const QList<GLC_Point2d>&);

    //! Triangulate a polygon
    /*! If the polygon is convex the returned index is a fan*/
    GLC_LIB_EXPORT void triangulatePolygon(QList<GLuint>*, const QList<float>&);

    //! Triangulate a polygon and return the computed normal of triangles
    /*! If the polygon is convex the returned index is a fan*/
    GLC_LIB_EXPORT GLC_Vector3d triangulatePolygonClip2TRi(QList<GLuint>*, const QList<float>&);

    GLC_LIB_EXPORT bool triangleIsCCW(const GLC_Point3d &p1, const GLC_Point3d &p2, const GLC_Point3d &p3, const GLC_Vector3d& normal);

    GLC_LIB_EXPORT GLC_Vector3d triangleNormal(const GLC_Point3d &p1, const GLC_Point3d &p2, const GLC_Point3d &p3);

    GLC_LIB_EXPORT QList<GLC_uint> reverseTriangleIndexWindingOrder(const QList<GLC_uint>& index);

    //! Return true if the given 3d line is intersected with the given plane
    /*! If there is an intersection point is set to the given 3d point
     *  If the line lie on the plane this method return false*/
    GLC_LIB_EXPORT bool lineIntersectPlane(const GLC_Line3d& line, const GLC_Plane& plane, GLC_Point3d* pPoint);

    GLC_LIB_EXPORT bool segmentIntersectPlane(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Plane& plane, GLC_Point3d* pPoint);

    //! Return the projected point on the given line from the given point
    GLC_LIB_EXPORT GLC_Point3d project(const GLC_Point3d& point, const GLC_Line3d& line);

    //! Return the projected point on the given normalysed plane from the given point
    GLC_LIB_EXPORT GLC_Point3d project(const GLC_Point3d& point, const GLC_Plane& plane);

    //! Return the midpoint of the two given points
    static inline GLC_Point3d midPoint(const GLC_Point3d& point1, const GLC_Point3d& point2)
    {return point1 + (point2 - point1) * 0.5;}

    //! Return the perpendicular 2D vector of the given 2D vector
    static inline GLC_Vector2d perpVector(const GLC_Vector2d& vect)
    {return GLC_Vector2d(-vect.y(), vect.x());}

    //! Return the distance between the given point and line
    GLC_LIB_EXPORT double pointLineDistance(const GLC_Point3d& point, const GLC_Line3d& line);

    //! Return true if the given 3 points are collinear
    GLC_LIB_EXPORT bool pointsAreCollinear(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3);

    GLC_LIB_EXPORT bool pointsAreCollinear(const QPointF& p1, const QPointF& p2, const QPointF& p3, double accuracy);

    //! Return true if the given 3 points are collinear and p3 is on ]p1 p2[ segment
    GLC_LIB_EXPORT bool pointIsIncludeInSegment(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3);

    //! Return true if the given 3 points are collinear and p3 is on [p1 p2] segment
    GLC_LIB_EXPORT bool pointIsOnSegment(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3);

    GLC_LIB_EXPORT bool segmentsOverlap(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3, const GLC_Point3d& p4);

    static inline bool compare(double p1, double p2)
    {return qAbs(p1 - p2) <= comparedPrecision;}

    static inline bool compare(double p1, double p2, double accuracy)
    {return qAbs(p1 - p2) <= accuracy;}

    static inline bool compareAngle(double p1, double p2)
    {return qAbs(p1 - p2) <= toRadian(comparedPrecision);}

    static inline bool compare(const GLC_Vector3d& v1, const GLC_Vector3d& v2)
    {return glc::compare(v1.x(), v2.x()) && glc::compare(v1.y(), v2.y()) && glc::compare(v1.z(), v2.z());}

    static inline bool compare(const GLC_Vector3d& v1, const GLC_Vector3d& v2, double accuracy)
    {return glc::compare(v1.x(), v2.x(), accuracy) && glc::compare(v1.y(), v2.y(), accuracy) && glc::compare(v1.z(), v2.z(), accuracy);}

    static inline bool compare(const GLC_Vector2d& v1, const GLC_Vector2d& v2)
    {return glc::compare(v1.x(), v2.x()) && glc::compare(v1.y(), v2.y());}

    static inline bool compare(const GLC_Vector2d& v1, const GLC_Vector2d& v2, double accuracy)
    {return glc::compare(v1.x(), v2.x(), accuracy) && glc::compare(v1.y(), v2.y(), accuracy);}

    static inline bool compare(const QPointF& v1, const QPointF& v2)
    {return glc::compare(v1.x(), v2.x()) && glc::compare(v1.y(), v2.y());}

    static inline bool compare(const QPointF& v1, const QPointF& v2, double accuracy)
    {return glc::compare(v1.x(), v2.x(), accuracy) && glc::compare(v1.y(), v2.y(), accuracy);}

    GLC_LIB_EXPORT double round(double value);

    GLC_LIB_EXPORT double round(double value, double accuracy);

    GLC_LIB_EXPORT QPointF round(const QPointF& point);

    GLC_LIB_EXPORT QPointF round(const QPointF& point, double accuracy);

    GLC_LIB_EXPORT GLC_Vector2d round(const GLC_Vector2d& vector);

    GLC_LIB_EXPORT GLC_Vector2d round(const GLC_Vector2d& vector, double accuracy);

    GLC_LIB_EXPORT GLC_Vector3d round(const GLC_Vector3d& vector);

    GLC_LIB_EXPORT GLC_Vector3d round(const GLC_Vector3d& vector, double accuracy);

    //! Return true if the given 2d point is inside the given polygon
    GLC_LIB_EXPORT bool pointInPolygon(const GLC_Point2d& point, const QList<GLC_Point2d>& polygon);

    //! Return the angle from 0 to 2PI from an given angle from -PI to PI
    GLC_LIB_EXPORT double zeroTo2PIAngle(double angle);

    //! Return the 2D polygon from the given plane 3D polygon
    GLC_LIB_EXPORT QList<GLC_Point2d> polygonIn2d(QList<GLC_Point3d> polygon3d);

    //! Return 2D polygon with normalyze coordinate
    GLC_LIB_EXPORT QList<GLC_Point2d> normalyzePolygon(const QList<GLC_Point2d>& polygon);

    GLC_LIB_EXPORT bool polygonCompatibleWithClip2TRi(const QList<GLC_Point2d> polygon);

    //! Segments composed of 3 points
    GLC_LIB_EXPORT QList<GLC_Point3d> AddCorner(const QList<GLC_Point3d>& segments, double radius, int count);

    GLC_LIB_EXPORT QList<GLC_Point2d> findIntersectionBetwen2Circle(const GLC_Point2d& c0, double r0, const GLC_Point2d& c1 , double r1);

    GLC_LIB_EXPORT QList<GLC_Point3d> circleFromCenterAndTwoPoint(const GLC_Point3d& center, const GLC_Point3d& start
                                                                  , const GLC_Point3d& end, int count, const GLC_Vector3d& direction);

    GLC_LIB_EXPORT QList<double> line2DImplicitCoefs(const GLC_Point2d& p, const GLC_Vector2d& v);

    GLC_LIB_EXPORT QList<GLC_Point2d> circleCenterTangentToLineAndCircleWithGivenRadius(const GLC_Point2d& linePoint, const GLC_Vector2d& lineVect
                                                                                        , const GLC_Point2d& circleCenter, double circleRadius
                                                                                        , double radius);

    GLC_LIB_EXPORT QList<GLC_Point2d> removeDuplicate(const QList<GLC_Point2d>& points);

    GLC_LIB_EXPORT QList<GLC_Point2d> line2CircleIntersection(const GLC_Point2d& lineOrigin, const GLC_Vector2d& lineDir
                                                              , const GLC_Point2d& circleCenter, double radius);

    GLC_LIB_EXPORT double area(const QPolygonF& polygon);


//@}

}

#endif /*GLC_GEOMTOOLS_H_*/
