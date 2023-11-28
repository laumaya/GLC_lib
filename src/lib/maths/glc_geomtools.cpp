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

//! \file glc_geomtools.cpp implementation of geometry function.

#include "glc_geomtools.h"
#include "glc_matrix4x4.h"
#include "../3rdparty/clip2tri/clip2tri/clip2tri.h"

#include <QtGlobal>


double glc::comparedPrecision= glc::defaultPrecision;

//////////////////////////////////////////////////////////////////////
//Tools Functions
//////////////////////////////////////////////////////////////////////

// Test if a polygon is convex
bool glc::polygon2DIsConvex(const QList<GLC_Point2d>& vertices)
{
	bool isConvex= true;
	const int size= vertices.size();
	if (vertices.size() > 3)
	{
		GLC_Point2d s0(vertices.at(0));
		GLC_Point2d s1(vertices.at(1));
		GLC_Point2d s2(vertices.at(2));
		const bool openAngle= ((s1.x() - s0.x()) * (s2.y() - s0.y()) - (s2.x() - s0.x()) * (s1.y() - s0.y())) < 0.0;

		int i= 3;
		while ((i < size) && isConvex)
		{
			s0= s1;
			s1= s2;
			s2= vertices.at(i);
			isConvex= openAngle == (((s1.x() - s0.x()) * (s2.y() - s0.y()) - (s2.x() - s0.x()) * (s1.y() - s0.y())) < 0.0);
			++i;
		}
	}

	return isConvex;
}

bool glc::polygonIsConvex(QList<GLuint>* pIndexList, const QList<float>& bulkList)
{
	bool isConvex= true;
	const int size= pIndexList->size();
	GLuint currentIndex;
	GLC_Vector3d v0;
	GLC_Vector3d v1;
	int i= 0;
	while ((i < size) && isConvex)
	{
		currentIndex= pIndexList->at(i);
		v0.setVect(bulkList.at(currentIndex * 3), bulkList.at(currentIndex * 3 + 1), bulkList.at(currentIndex * 3 + 2));
		currentIndex= pIndexList->at((i + 1) % size);
		v1.setVect(bulkList.at(currentIndex * 3), bulkList.at(currentIndex * 3 + 1), bulkList.at(currentIndex * 3 + 2));
        isConvex= (v0.angleWithVect2(v1) < glc::PI);
		++i;
	}
	return isConvex;
}
// find intersection between two 2D segments
QVector<GLC_Point2d> glc::findIntersection(const GLC_Point2d& s1p1, const GLC_Point2d& s1p2, const GLC_Point2d& s2p1, const GLC_Point2d& s2p2)
{
	const GLC_Vector2d D0= s1p2 - s1p1;
	const GLC_Vector2d D1= s2p2 - s2p1;
	// The QVector of result points
	QVector<GLC_Point2d> result;

	const GLC_Vector2d E(s2p1 - s1p1);
	double kross= D0 ^ D1;
	double sqrKross= kross * kross;
	const double sqrLen0= D0.x() * D0.x() + D0.y() * D0.y();
	const double sqrLen1= D1.x() * D1.x() + D1.y() * D1.y();
	// Test if the line are nor parallel
	if (sqrKross > (EPSILON * sqrLen0 * sqrLen1))
	{
		const double s= (E ^ D1) / kross;
		if ((s < 0.0) || (s > 1.0))
		{
			// Intersection of lines is not a point on segment s1p1 + s * DO
			return result; // Return empty QVector
		}
		const double t= (E ^ D0) / kross;

		if ((t < 0.0) || (t > 1.0))
		{
			// Intersection of lines is not a point on segment s2p1 + t * D1
			return result; // Return empty QVector
		}

		// Intersection of lines is a point on each segment
		result << (s1p1 + (D0 * s));
		return result; // Return a QVector of One Point
	}

	// Lines of the segments are parallel
	const double sqrLenE= E.x() * E.x() + E.y() * E.y();
	kross= E ^ D0;
	sqrKross= kross * kross;
	if (sqrKross > (EPSILON * sqrLen0 * sqrLenE))
	{
		// Lines of the segments are different
		return result; // Return empty QVector
	}

	// Lines of the segments are the same. Need to test for overlap of segments.
	const double s0= (D0 * E) / sqrLen0;
	const double s1= (D0 * D1) / sqrLen0;
	const double sMin= qMin(s0, s1);
	const double sMax= qMax(s0, s1);
	QVector<double> overlaps= findIntersection(0.0, 1.0, sMin, sMax);
	const int iMax= overlaps.size();
	for (int i= 0; i < iMax; ++i)
	{
		result.append(s1p1 + (D0 * overlaps[i]));
	}
	return result;
}

// return true if there is an intersection between 2 segments
bool glc::isIntersected(const GLC_Point2d& s1p1, const GLC_Point2d& s1p2, const GLC_Point2d& s2p1, const GLC_Point2d& s2p2)
{
	const GLC_Vector2d D0= s1p2 - s1p1;
	const GLC_Vector2d D1= s2p2 - s2p1;

	const GLC_Vector2d E(s2p1 - s1p1);
	double kross= D0 ^ D1;
	double sqrKross= kross * kross;
	const double sqrLen0= D0.x() * D0.x() + D0.y() * D0.y();
	const double sqrLen1= D1.x() * D1.x() + D1.y() * D1.y();
	// Test if the line are nor parallel
	if (sqrKross > (EPSILON * sqrLen0 * sqrLen1))
	{
		const double s= (E ^ D1) / kross;
		if ((s < 0.0) || (s > 1.0))
		{
			// Intersection of lines is not a point on segment s1p1 + s * DO
			return false;
		}
		const double t= (E ^ D0) / kross;

		if ((t < 0.0) || (t > 1.0))
		{
			// Intersection of lines is not a point on segment s2p1 + t * D1
			return false;
		}

		// Intersection of lines is a point on each segment
		return true;
	}

	// Lines of the segments are parallel
	const double sqrLenE= E.x() * E.x() + E.y() * E.y();
	kross= E ^ D0;
	sqrKross= kross * kross;
	if (sqrKross > (EPSILON * sqrLen0 * sqrLenE))
	{
		// Lines of the segments are different
		return false;
	}

	// Lines of the segments are the same. Need to test for overlap of segments.
	const double s0= (D0 * E) / sqrLen0;
	const double s1= s0 + (D0 * D1) / sqrLen0;
	const double sMin= qMin(s0, s1);
	const double sMax= qMax(s0, s1);
	if (findIntersection(0.0, 1.0, sMin, sMax).size() == 0) return false; else return true;

}

// return true if there is an intersection between a ray and a segment
bool glc::isIntersectedRaySegment(const GLC_Point2d& s1p1, const GLC_Vector2d& s1p2, const GLC_Point2d& s2p1, const GLC_Point2d& s2p2)
{
	const GLC_Vector2d D0= s1p2 - s1p1;
	const GLC_Vector2d D1= s2p2 - s2p1;

	const GLC_Vector2d E(s2p1 - s1p1);
	double kross= D0 ^ D1;
	double sqrKross= kross * kross;
	const double sqrLen0= D0.x() * D0.x() + D0.y() * D0.y();
	const double sqrLen1= D1.x() * D1.x() + D1.y() * D1.y();
	// Test if the line are nor parallel
	if (sqrKross > (EPSILON * sqrLen0 * sqrLen1))
	{
		const double s= (E ^ D1) / kross;
		if ((s < 0.0))
		{
			// Intersection of lines is not a point on segment s1p1 + s * DO
			return false;
		}
		const double t= (E ^ D0) / kross;

		if ((t < 0.0) || (t > 1.0))
		{
			// Intersection of lines is not a point on segment s2p1 + t * D1
			return false;
		}

		// Intersection of lines is a point on each segment
		return true;
	}

	// Lines of the segments are parallel
	const double sqrLenE= E.x() * E.x() + E.y() * E.y();
	kross= E ^ D0;
	sqrKross= kross * kross;
	if (sqrKross > (EPSILON * sqrLen0 * sqrLenE))
	{
		// Lines of are different
		return false;
	}
	else return true;

}

// find intersection of two intervals [u0, u1] and [v0, v1]
QVector<double> glc::findIntersection(const double& u0, const double& u1, const double& v0, const double& v1)
{
	//Q_ASSERT((u0 < u1) and (v0 < v1));
	QVector<double> result;
	if (u1 < v0 || u0 > v1) return result; // Return empty QVector

	if (u1 > v0)
	{
		if (u0 < v1)
		{
			if (u0 < v0) result.append(v0); else result.append(u0);
			if (u1 > v1) result.append(v1); else result.append(u1);
			return result;
		}
		else // u0 == v1
		{
			result.append(u0);
			return result;
		}
	}
	else // u1 == v0
	{
		result.append(u1);
		return result;
	}
}

// return true if the segment is in polygon cone
bool glc::segmentInCone(const GLC_Point2d& V0, const GLC_Point2d& V1, const GLC_Point2d& VM, const GLC_Point2d& VP)
{
	// assert: VM, V0, VP are not collinear
	const GLC_Vector2d diff(V1 - V0);
	const GLC_Vector2d edgeL(VM - V0);
	const GLC_Vector2d edgeR(VP - V0);
	if ((edgeR ^ edgeL) > 0)
	{
		// Vertex is convex
		return (((diff ^ edgeR) < 0.0) && ((diff ^ edgeL) > 0.0));
	}
	else
	{
		// Vertex is reflex
		return (((diff ^ edgeR) < 0.0) || ((diff ^ edgeL) > 0.0));
	}
}

// Return true if the segment is a polygon diagonal
bool glc::isDiagonal(const QList<GLC_Point2d>& polygon, const int i0, const int i1)
{
	const int size= polygon.size();
	int iM= (i0 - 1) % size;
	if (iM < 0) iM= size - 1;
	int iP= (i0 + 1) % size;

	if (!segmentInCone(polygon[i0], polygon[i1], polygon[iM], polygon[iP]))
	{
		return false;
	}

	int j0= 0;
	int j1= size - 1;
	// test segment <polygon[i0], polygon[i1]> to see if it is a diagonal
	while (j0 < size)
	{
		if (j0 != i0 && j0 != i1 && j1 != i0 && j1 != i1)
		{
			if (isIntersected(polygon[i0], polygon[i1], polygon[j0], polygon[j1]))
				return false;
		}

		j1= j0;
		++j0;
	}

	return true;
}

// Triangulate a polygon
void glc::triangulate(QList<GLC_Point2d>& polygon, QList<int>& index, QList<int>& tList)
{
	const int size= polygon.size();
	if (size == 3)
	{
		tList << index[0] << index[1] << index[2];
		return;
	}
	int i0= 0;
	int i1= 1;
	int i2= 2;
	while(i0 < size)
	{
		if (isDiagonal(polygon, i0, i2))
		{
			// Add the triangle before removing the ear.
			tList << index[i0] << index[i1] << index[i2];
			// Remove the ear from polygon and index lists
			polygon.removeAt(i1);
			index.removeAt(i1);
			// recurse to the new polygon
			triangulate(polygon, index, tList);
			return;
		}
		++i0;
		i1= (i1 + 1) % size;
		i2= (i2 + 1) % size;
	}
}

// return true if the polygon is couterclockwise ordered
bool glc::isCounterclockwiseOrdered(const QList<GLC_Point2d>& polygon)
{
	const int size= polygon.size();
	int j0= 0;
	int j1= size - 1;
	// test segment <polygon[i0], polygon[i1]> to see if it is a diagonal
	while (j0 < size)
	{
		GLC_Vector2d perp((polygon[j0] - polygon[j1]).perp());
		int j2= 0;
		int j3= size - 1;
		bool isIntersect= false;
		// Application of perp vector
		GLC_Point2d moy((polygon[j0] + polygon[j1]) * 0.5);
		while (j2 < size && !isIntersect)
		{
			if(j2 != j0 && j3 != j1)
			{
				if (isIntersectedRaySegment(moy, (perp + moy), polygon[j2], polygon[j3]))
					isIntersect= true;
			}
			j3= j2;
			++j2;
		}
		if(!isIntersect) return false;
		j1= j0;
		++j0;
	}

	return true;

}

// Triangulate a no convex polygon
void glc::triangulatePolygon(QList<GLuint>* pIndexList, const QList<float>& bulkList)
{
	int size= pIndexList->size();
	if (polygonIsConvex(pIndexList, bulkList))
	{
		QList<GLuint> indexList(*pIndexList);
		pIndexList->clear();
		for (int i= 0; i < size - 2; ++i)
		{
			pIndexList->append(indexList.at(0));
			pIndexList->append(indexList.at(i + 1));
			pIndexList->append(indexList.at(i + 2));
		}
	}
	else
	{
		// Get the polygon vertice
		QList<GLC_Point3d> originPoints;
		QHash<int, int> indexMap;

		QList<int> face;
		GLC_Point3d currentPoint;
		int delta= 0;
		for (int i= 0; i < size; ++i)
		{
			const int currentIndex= pIndexList->at(i);
			currentPoint= GLC_Point3d(bulkList.at(currentIndex * 3), bulkList.at(currentIndex * 3 + 1), bulkList.at(currentIndex * 3 + 2));
			if (!originPoints.contains(currentPoint))
			{
				originPoints.append(GLC_Point3d(bulkList.at(currentIndex * 3), bulkList.at(currentIndex * 3 + 1), bulkList.at(currentIndex * 3 + 2)));
				indexMap.insert(i - delta, currentIndex);
				face.append(i - delta);
			}
			else
			{
				qDebug() << "Multi points";
				++delta;
			}
		}
		// Values of PindexList must be reset
		pIndexList->clear();

		// Update size
		size= size - delta;

		// Check new size
		if (size < 3) return;

		//-------------- Change frame to mach polygon plane
			// Compute face normal
			const GLC_Point3d point1(originPoints[0]);
			const GLC_Point3d point2(originPoints[1]);
			const GLC_Point3d point3(originPoints[2]);

			const GLC_Vector3d edge1(point2 - point1);
			const GLC_Vector3d edge2(point3 - point2);

			GLC_Vector3d polygonPlaneNormal(edge1 ^ edge2);
			polygonPlaneNormal.normalize();

			// Create the transformation matrix
			GLC_Matrix4x4 transformation;

			GLC_Vector3d rotationAxis(polygonPlaneNormal ^ Z_AXIS);
			if (!rotationAxis.isNull())
			{
				const double angle= acos(polygonPlaneNormal * Z_AXIS);
				transformation.setMatRot(rotationAxis, angle);
			}

			QList<GLC_Point2d> polygon;
			// Transform polygon vertexs
			for (int i=0; i < size; ++i)
			{
				originPoints[i]= transformation * originPoints[i];
				// Create 2d vector
				polygon << originPoints[i].toVector2d(Z_AXIS);
			}
			// Create the index
			QList<int> index= face;

			const bool faceIsCounterclockwise= isCounterclockwiseOrdered(polygon);

			if(!faceIsCounterclockwise)
			{
				//qDebug() << "face Is Not Counterclockwise";
				const int max= size / 2;
				for (int i= 0; i < max; ++i)
				{
                    polygon.swapItemsAt(i, size - 1 -i);
					int temp= face[i];
					face[i]= face[size - 1 - i];
					face[size - 1 - i]= temp;
				}
			}

            QList<int> tList;
			triangulate(polygon, index, tList);
			size= tList.size();
			for (int i= 0; i < size; i+= 3)
			{
				// Avoid normal problem
				if (faceIsCounterclockwise)
				{
					pIndexList->append(indexMap.value(face[tList[i]]));
					pIndexList->append(indexMap.value(face[tList[i + 1]]));
					pIndexList->append(indexMap.value(face[tList[i + 2]]));
				}
				else
				{
					pIndexList->append(indexMap.value(face[tList[i + 2]]));
					pIndexList->append(indexMap.value(face[tList[i + 1]]));
					pIndexList->append(indexMap.value(face[tList[i]]));
				}
			}
			Q_ASSERT(size == pIndexList->size());
	}

}

bool glc::lineIntersectPlane(const GLC_Line3d& line, const GLC_Plane& plane, GLC_Point3d* pPoint)
{
	const GLC_Vector3d n= plane.normal();
	const GLC_Point3d p= line.startingPoint();
	const GLC_Vector3d d= line.direction();

	const double denominator= d * n;
	if (glc::fuzzyCompare(fabs(denominator), 0.0))
	{
		qDebug() << " glc::lineIntersectPlane : Line parallel to the plane";
		// The line is parallel to the plane
		return false;
	}
	else
	{
		// The line intersect the plane by one point
		const double t= -((n * p) + plane.coefD()) / denominator;
		(*pPoint)= p + (t * d);

		return true;
	}
}

bool glc::segmentIntersectPlane(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Plane& plane, GLC_Point3d* pPoint)
{
    bool subject= false;
    GLC_Line3d line(p1, p2 - p1);
    GLC_Point3d intersection;
    if (lineIntersectPlane(line, plane, &intersection))
    {
        if (pointIsOnSegment(p1, p2, intersection))
        {
            pPoint->operator =(intersection);
            subject= true;
        }
    }

    return subject;
}

GLC_Point3d glc::project(const GLC_Point3d& point, const GLC_Line3d& line)
{
	const GLC_Vector3d lineDirection(line.direction().normalize());
	double t= lineDirection * (point - line.startingPoint());
	GLC_Point3d projectedPoint= line.startingPoint() + (t * lineDirection);
	return projectedPoint;
}

double glc::pointLineDistance(const GLC_Point3d& point, const GLC_Line3d& line)
{
	const GLC_Vector3d lineDirection(line.direction().normalize());
	double t= lineDirection * (point - line.startingPoint());
	GLC_Point3d projectedPoint= line.startingPoint() + (t * lineDirection);
	return (point - projectedPoint).length();
}

bool glc::pointsAreCollinear(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3)
{
    bool subject= false;
    const double p1p2Length= (p1 - p2).length();
    const double p1p3Length= (p1 - p3).length();
    const double p2p3Length= (p2 - p3).length();
    QList<double> lengthList;
    lengthList << p1p2Length << p1p3Length << p2p3Length;
    std::sort(lengthList.begin(), lengthList.end());
    subject= compare(lengthList.at(2), (lengthList.at(0) + lengthList.at(1)));
    return subject;
}

bool glc::pointIsIncludeInSegment(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3)
{
    bool subject= false;
    const GLC_Vector3d p1p2((p2 - p1).setLength(1.0));
    const GLC_Vector3d p2p3((p3 - p2).setLength(1.0));
    if (compare(p1p2, p2p3)  || compare(p1p2, p2p3.inverted()))
    {
        const double p1p2Length= (p1 -p2).length();
        const double p1p3Length= (p1 -p3).length();
        if ((p1p2Length > p1p3Length) && !glc::compare(p1p2Length, p1p3Length))
        {
            const double p2p3Length= (p2 -p3).length();
            subject= (p1p2Length > p2p3Length) && !glc::compare(p1p2Length, p2p3Length);
        }
    }
    return subject;
}

bool glc::pointIsOnSegment(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3)
{
    bool subject= false;
    const bool p1EqualP3= compare(p1, p3);
    const bool p2EqualP3= compare(p2, p3);
    if (p1EqualP3 || p2EqualP3)
    {
        subject= true;
    }
    else
    {
        const GLC_Vector3d p1p2((p2 - p1).setLength(1.0));
        const GLC_Vector3d p2p3((p3 - p2).setLength(1.0));
        if (compare(p1p2, p2p3)  || compare(p1p2, p2p3.inverted()))
        {
            const double p1p2Length= (p1 -p2).length();
            const double p1p3Length= (p1 -p3).length();
            if (p1p2Length > p1p3Length)
            {
                const double p2p3Length= (p2 -p3).length();
                subject= (p1p2Length > p2p3Length);
            }
        }
    }
    return subject;
}

bool glc::segmentsOverlap(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3, const GLC_Point3d& p4)
{
    bool subject= false;
    const bool p1EqualP3= compare(p1, p3);
    const bool p2EqualP3= compare(p2, p3);
    const bool p1EqualP4= compare(p1, p4);
    const bool p2EqualP4= compare(p2, p4);
    if ((p1EqualP3 && p2EqualP4) || (p2EqualP3 && p1EqualP4))
    {
        subject= true;
    }
    else if (glc::pointsAreCollinear(p1, p2, p3) && glc::pointsAreCollinear(p1, p2, p4))
    {
        const double squaredLength1= GLC_Vector3d(p1 - p2).squaredLength();
        const double squaredLength2= GLC_Vector3d(p3 - p4).squaredLength();
        if (squaredLength1 > squaredLength2)
        {
            subject= (pointIsIncludeInSegment(p1, p2, p3) || pointIsIncludeInSegment(p1, p2, p4));
        }
        else
        {
            subject= (pointIsIncludeInSegment(p3, p4, p1) || pointIsIncludeInSegment(p3, p4, p2));
        }
    }
    return subject;
}

double glc::round(double value)
{
    return round(value, comparedPrecision);
}

double glc::round(double value, double accuracy)
{
    if (!qFuzzyIsNull(accuracy))
    {
        value= value / accuracy;
        value= (value >= 0.0 ? floor(value + 0.5) : ceil(value - 0.5));
        value= value * accuracy;
    }
    else value= 0.0;

    return value;
}

QPointF glc::round(const QPointF& point)
{
	QPointF subject(glc::round(static_cast<double>(point.x())), glc::round(static_cast<double>(point.y())));
	return subject;
}

QPointF glc::round(const QPointF& point, double accuracy)
{
    QPointF subject(glc::round(static_cast<double>(point.x()), accuracy), glc::round(static_cast<double>(point.y()), accuracy));
    return subject;
}

GLC_Vector2d glc::round(const GLC_Vector2d& vector)
{
	GLC_Vector2d subject(glc::round(vector.x()), glc::round(vector.y()));

	return subject;
}

GLC_Vector2d glc::round(const GLC_Vector2d& vector, double accuracy)
{
    GLC_Vector2d subject(glc::round(vector.x(), accuracy), glc::round(vector.y(), accuracy));

    return subject;
}

GLC_Vector3d glc::round(const GLC_Vector3d& vector)
{
	GLC_Vector3d subject(glc::round(vector.x()), glc::round(vector.y()), glc::round(vector.z()));

	return subject;
}

GLC_Vector3d glc::round(const GLC_Vector3d& vector, double accuracy)
{
    GLC_Vector3d subject(glc::round(vector.x(), accuracy), glc::round(vector.y(), accuracy), glc::round(vector.z(), accuracy));

    return subject;
}

bool glc::pointInPolygon(const GLC_Point2d& point, const QList<GLC_Point2d>& polygon)
{
	const int polygonSize= polygon.size();
	bool inside= false;
	int i= 0;
	int j= polygonSize - 1;

	while (i < polygonSize)
	{
		const GLC_Point2d point0= polygon.at(i);
		const GLC_Point2d point1= polygon.at(j);
		if (point.y() < point1.y())
		{
			//point1 above ray
			if (point0.y() <= point.y())
			{
				//point2 on or below ray
				const double val1= (point.y() - point0.y()) * (point1.x() - point0.x());
				const double val2= (point.x() - point0.x()) * (point1.y() - point0.y());
				if (val1 > val2) inside= !inside;
			}
		}
		else if (point.y() < point0.y())
		{
			// point 1 on or below ray, point0 above ray
			const double val1= (point.y() - point0.y()) * (point1.x() - point0.x());
			const double val2= (point.x() - point0.x()) * (point1.y() - point0.y());
			if (val1 < val2) inside= !inside;
		}
		j= i;
		++i;
	}
	return inside;
}

double glc::zeroTo2PIAngle(double angle)
{
	if (glc::fuzzyCompare(fabs(angle), glc::PI))
	{
		angle= glc::PI;
	}
	else if (angle < 0)
	{
		angle= (2.0 * glc::PI) + angle;
	}
	return angle;
}

QList<GLC_Point2d> glc::polygonIn2d(QList<GLC_Point3d> polygon3d)
{
    const int count= polygon3d.count();
    Q_ASSERT(count > 2);

    // Compute face normal
    const GLC_Point3d point1(polygon3d[0]);
    const GLC_Point3d point2(polygon3d[1]);
    const GLC_Point3d point3(polygon3d[2]);

    const GLC_Vector3d edge1(point2 - point1);
    const GLC_Vector3d edge2(point3 - point2);

    GLC_Vector3d polygonPlaneNormal(edge1 ^ edge2);
    polygonPlaneNormal.normalize();

    // Create the transformation matrix
    GLC_Matrix4x4 transformation;

    GLC_Vector3d rotationAxis(polygonPlaneNormal ^ Z_AXIS);
    if (!rotationAxis.isNull())
    {
        const double angle= acos(polygonPlaneNormal * Z_AXIS);
        transformation.setMatRot(rotationAxis, angle);
    }

    for (GLC_Point3d& point : polygon3d)
    {
        point= transformation * point;
    }

    GLC_Matrix4x4 transformation1;
    if (!rotationAxis.isNull())
    {
        transformation1.setMatRot(rotationAxis, glc::X_AXIS);
    }

    QList<GLC_Point2d> subject;
    for (GLC_Point3d& point : polygon3d)
    {
        point= transformation1 * point;
        subject << point.toVector2d(Z_AXIS);
    }

    return subject;
}

QList<GLC_Point2d> glc::normalyzePolygon(const QList<GLC_Point2d>& polygon)
{
    QList<GLC_Point2d> subject;
    const int count= polygon.count();
    Q_ASSERT(count > 2);

    GLC_Point2d minPoint= polygon.constFirst();
    GLC_Point2d maxPoint= minPoint;
    for (int i= 1; i < count; ++i)
    {
        GLC_Point2d point= polygon.at(i);
        minPoint.setX(qMin(point.x(), minPoint.x()));
        minPoint.setY(qMin(point.y(), minPoint.y()));

        maxPoint.setX(qMax(point.x(), maxPoint.x()));
        maxPoint.setY(qMax(point.y(), maxPoint.y()));
    }
    const GLC_Vector2d range= maxPoint - minPoint;
    Q_ASSERT(range.x() != 0.0);
    Q_ASSERT(range.y() != 0.0);

    for (int i= 0; i < count; ++i)
    {
        const GLC_Point2d point= polygon.at(i);
        const GLC_Point2d temp= (point - minPoint);

        const GLC_Point2d result(temp.x() / range.x(), temp.y() / range.y());
        subject.append(result);
    }

    return subject;
}

GLC_Vector3d glc::triangulatePolygonClip2TRi(QList<GLuint> *pIndexList, const QList<float> &bulkList)
{
    // Get the polygon vertice
    QList<GLC_Point3d> originPoints;
    const int size= pIndexList->size();
    for (int i= 0; i < size; ++i)
    {
        const int currentIndex= pIndexList->at(i);
        const GLC_Point3d currentPoint= GLC_Point3d(bulkList.at(currentIndex * 3), bulkList.at(currentIndex * 3 + 1), bulkList.at(currentIndex * 3 + 2));
        originPoints.append(currentPoint);
    }

    //-------------- Change frame to mach polygon plane
    // Compute face normal
    const GLC_Point3d p1(originPoints[0]);
    const GLC_Point3d p2(originPoints[1]);
    const GLC_Point3d p3(originPoints[2]);
    GLC_Vector3d subject(triangleNormal(p1, p2, p3));

    // Check if the vertice lies on the plane
    GLC_Plane plane(p1, p2, p3);
    bool isLieOnPlane= true;
    int i= 3;
    while (isLieOnPlane && (i < size))
    {
        GLC_Point3d point= originPoints.at(i);
        isLieOnPlane= plane.lieOnThisPlane(point);
        ++i;
    }
    if (isLieOnPlane)
    {
        // Create the transformation matrix
        GLC_Matrix4x4 transformation;

        GLC_Vector3d rotationAxis(subject ^ glc::Z_AXIS);
        if (!rotationAxis.isNull())
        {
            const double angle= acos(subject * glc::Z_AXIS);
            transformation.setMatRot(rotationAxis, angle);
        }
        else if (glc::compare(Z_AXIS.inverted(), subject))
        {
            transformation.setMatRot(glc::X_AXIS, glc::PI);
        }

        QList<GLC_Point2d> polygon;
        vector<c2t::Point> boundingPolygon(size);
        // Transform polygon vertexs
        for (int i=0; i < size; ++i)
        {
            originPoints[i]= transformation * originPoints[i];
            // Create 2d vector

            GLC_Point2d currentPoint= originPoints[i].toVector2d(Z_AXIS);
            polygon.append(currentPoint);
            c2t::Point point;
            point.x= currentPoint.x();
            point.y= currentPoint.y();
            boundingPolygon[i]= point;
        }

        if (polygonCompatibleWithClip2TRi(polygon))
        {
            vector<vector<c2t::Point> > inputPolygons;
            vector<c2t::Point> outputTriangles;  // Every 3 points is a triangle

            c2t::clip2tri clip2tri;
            clip2tri.triangulate(inputPolygons, outputTriangles, boundingPolygon);

            QList<GLuint> oldIndex(*pIndexList);
            pIndexList->clear();
            const int count= outputTriangles.size();
            for (int i= 0; i < count; ++i)
            {
                const GLC_Point2d point(outputTriangles.at(i).x, outputTriangles.at(i).y);
                for (int j= 0; j < size ; ++j)
                {
                    if (glc::compare(point, (polygon.at(j))))
                    {
                        pIndexList->append(oldIndex.at(j));
                    }
                }
            }
            if(pIndexList->size() != count)
            {
                qWarning() << "glc::triangulatePolygonClip2TRi failed";
                // Somethings got wrong
                // Use the old method wich preserve index integrety
                pIndexList->operator =(oldIndex);
                triangulatePolygon(pIndexList, bulkList);
            }
        }
        else
        {
            qWarning() << "Polygon with NULL edge found glc::triangulatePolygonClip2TRi failed";
            qWarning() << "Use old method";
            triangulatePolygon(pIndexList, bulkList);
        }
    }
    else
    {
        qWarning() << "Non planar polygon found glc::triangulatePolygonClip2TRi failed";
        qWarning() << "Use old method";
        triangulatePolygon(pIndexList, bulkList);
    }

    return subject;
}

bool glc::triangleIsCCW(const GLC_Point3d& p1, const GLC_Point3d& p2, const GLC_Point3d& p3, const GLC_Vector3d& normal)
{
    const GLC_Vector3d computedNormal(triangleNormal(p1, p2, p3));

    bool subject= glc::compare(computedNormal, normal);

    return subject;
}

GLC_Vector3d glc::triangleNormal(const GLC_Point3d &p1, const GLC_Point3d &p2, const GLC_Point3d &p3)
{
    const GLC_Vector3d edge1(p2 - p1);
    const GLC_Vector3d edge2(p3 - p2);

    GLC_Vector3d subject(edge1 ^ edge2);
    subject.normalize();

    return subject;
}

QList<GLC_uint> glc::reverseTriangleIndexWindingOrder(const QList<GLC_uint>& index)
{
    QList<GLC_uint> subject;
    const int count= index.count();
    for (int i= 0; i < count; i+=3)
    {
        subject.append(index.at(i));
        subject.append(index.at(i + 2));
        subject.append(index.at(i + 1));
    }

    return subject;
}

bool glc::polygonCompatibleWithClip2TRi(const QList<GLC_Point2d> polygon)
{
    bool subject= true;
    const int count= polygon.count();
    int i= 0;
    while (subject && (i < count))
    {
        GLC_Point2d p1= polygon.at(i);
        int j= i + 1;
        while (subject && (j < count))
        {
            GLC_Point2d p2= polygon.at(j);
            subject= !glc::compare(p1, p2, glc::EPSILON);
            ++j;
        }
        ++i;
    }

    return subject;
}

QList<GLC_Point3d> glc::AddCorner(const QList<GLC_Point3d>& segments, double radius, int count, GLC_Point3d* pAxisPos)
{
    Q_ASSERT(segments.count() == 3);
    Q_ASSERT(radius > 0.0);
    Q_ASSERT(count > 1);

    QList<GLC_Point3d> subject;

    // Compute face normal to compute angle between segments
    const GLC_Point3d p0(segments[0]);
    const GLC_Point3d p1(segments[1]);
    const GLC_Point3d p2(segments[2]);

    const GLC_Vector3d edge0(p1 - p0);
    const GLC_Vector3d edge1(p1 - p2);

    GLC_Vector3d direction(edge1 ^ edge0);
    direction.normalize();

    const double angle = (edge1.signedAngleWithVect(edge0, direction));
    const double deltaLength= radius * (cos(angle / 2.0) / sin(angle / 2.0));

    // Compute new segment end before corner
    GLC_Vector3d p1OffsetOnEdge0(p0 - p1);
    p1OffsetOnEdge0.setLength(deltaLength);
    const GLC_Point3d newP1OnEdge0(p1 + p1OffsetOnEdge0);

    GLC_Vector3d p1OffsetOnEdge1(p2 - p1);
    p1OffsetOnEdge1.setLength(deltaLength);
    const GLC_Point3d newP1OnEdge1(p1 + p1OffsetOnEdge1);


    // Compute corner axis position
    const GLC_Matrix4x4 m1(p1OffsetOnEdge0, glc::PI / 2.0);
    GLC_Vector3d centerOffset= m1 * direction;
    centerOffset.setLength(radius);
    const GLC_Point3d axisPos(newP1OnEdge0 + centerOffset);

    if (nullptr != pAxisPos) pAxisPos->operator=(axisPos);

    // Add first segment
    subject << p0;
    if (p0 != newP1OnEdge0)
    {
        subject << newP1OnEdge0;
    }

    const double deltaAngle= (glc::PI - angle) / count; // Complementary angle

    // Add Corner
    const GLC_Point3d relativeStartingPoint(newP1OnEdge0 - axisPos);
    for (int i= 1; i < count; ++i)
    {
        const double currentAngle= deltaAngle * i;
        const GLC_Matrix4x4 transformation(direction, currentAngle);
        const GLC_Point3d currentPoint(transformation * relativeStartingPoint);
        subject.append(currentPoint + axisPos);
    }

    // Add last segment
    subject << newP1OnEdge1;
    if (newP1OnEdge1 != p2)
    {
        subject << p2;
    }

    return subject;
}

QList<GLC_Point2d> glc::findIntersectionBetwen2Circle(const GLC_Point2d& c0, double r0, const GLC_Point2d& c1, double r1)
{
    QList<GLC_Point2d> subject;

    const GLC_Vector2d u(c1 - c0);
    const double uSqrLen= u * u;
    const double deltaRadius= r0 - r1;
    if (!qFuzzyIsNull(uSqrLen) || !qFuzzyIsNull(deltaRadius))
    {
        const double sQrDeltaRadius= deltaRadius * deltaRadius;
        if (uSqrLen >= sQrDeltaRadius)
        {
            const double r0AddR1= r0 + r1;
            const double sqrR0AddR1= r0AddR1 * r0AddR1;
            if (uSqrLen < sqrR0AddR1)
            {
                if (sQrDeltaRadius < uSqrLen)
                {
                    const double invUSqrLen= 1.0 / uSqrLen;
                    const double s= 0.5 * (((r0 * r0) - (r1 * r1)) * invUSqrLen + 1.0);
                    const GLC_Vector2d tmp(c0 + (u * s));

                    // In theory, discr is nonnegative.  However, numerical round-off
                    // errors can make it slightly negative.  Clamp it to zero.
                    double discr= ((r0 * r0) * invUSqrLen) - (s * s);
                    if (discr < 0.0)
                    {
                        discr= 0.0;
                    }

                    const double t= sqrt(discr);
                    const GLC_Vector2d v(u.y(), -u.x());

                    const GLC_Point2d int1(tmp - (v * t));
                    subject.append(int1);

                    const GLC_Point2d int2(tmp + (v * t));
                    if (int1 != int2)
                    {
                        subject.append(int2);
                    }
                }
                else
                {
                    // tangent
                    GLC_Point2d intersect(c0 + (u * (r0 / deltaRadius)));
                    subject.append(intersect);
                }
            }
            else
            {
                // tangent
                GLC_Point2d intersect(c0 + (u * (r0 / r0AddR1)));
                subject.append(intersect);
            }
        }
    }

    return subject;
}

QList<GLC_Point3d> glc::circleFromCenterAndTwoPoint(const GLC_Point3d& center, const GLC_Point3d& start
                                                    , const GLC_Point3d& end, int count, const GLC_Vector3d& direction)
{
    // Compute face normal to compute angle between segments
    const GLC_Vector3d edge0(start - center);
    const GLC_Vector3d edge1(end - center);

    const double angle = (edge0.signedAngleWithVect(edge1, direction));

    QList<GLC_Point3d> subject;

    // Add first segment
    subject << start;

    const double deltaAngle= (angle) / count;

    // Add Corner
    const GLC_Point3d relativeStartingPoint(edge0);
    for (int i= 1; i < count; ++i)
    {
        const double currentAngle= deltaAngle * i;
        const GLC_Matrix4x4 transformation(direction, currentAngle);
        const GLC_Point3d currentPoint(transformation * relativeStartingPoint);
        subject.append(currentPoint + center);
    }

    // Add last segment
    subject << end;

    return subject;
}

QList<double> glc::line2DImplicitCoefs(const GLC_Point2d& p, const GLC_Vector2d& v)
{
    // Normal vector
    const GLC_Vector2d n(perpVector(v));

    // Get coeffs
    double a= n.x();
    double b= n.y();
    double c= -(n * p);

    QList<double> subject;
    subject << a << b << c;

    return subject;
}

QList<GLC_Point2d> glc::circleCenterTangentToLineAndCircleWithGivenRadius(const GLC_Point2d& linePoint, const GLC_Vector2d& lineVect
                                                                          , const GLC_Point2d& circleCenter, double circleRadius
                                                                          , double radius)
{
    QList<GLC_Point2d> subject;
    QList<double> lineCoef= line2DImplicitCoefs(linePoint, lineVect);
    double a= lineCoef.at(0);
    double b= lineCoef.at(1);
    double c= lineCoef.at(2);

    const double normalizeFactor= sqrt((a * a) + (b * b));

    const double circleCenterToLineDistance= qAbs(a * circleCenter.x() + b * circleCenter.y() + c) / normalizeFactor;
    if (!(circleCenterToLineDistance > ((2.0 * radius) + circleRadius)))
    {
        const GLC_Vector2d n(perpVector(lineVect).setLength(radius));
        const GLC_Point2d& linePointp1= linePoint + n;
        const GLC_Point2d& linePointp2= linePoint - n;
        const double radiusPlusCircleRadius= circleRadius + radius;
        const double radiusSubstCircleRadius= circleRadius - radius;
        subject.append(line2CircleIntersection(linePointp1, lineVect, circleCenter, radiusPlusCircleRadius));
        subject.append(line2CircleIntersection(linePointp2, lineVect, circleCenter, radiusPlusCircleRadius));
        subject.append(line2CircleIntersection(linePointp1, lineVect, circleCenter, radiusSubstCircleRadius));
        subject.append(line2CircleIntersection(linePointp2, lineVect, circleCenter, radiusSubstCircleRadius));
    }

    return subject;
}

QList<GLC_Point2d> glc::removeDuplicate(const QList<GLC_Point2d>& points)
{
    QList<GLC_Point2d> subject;
    const int count= points.count();
    for (int i= 0; i < count; ++i)
    {
        const GLC_Point2d point(points.at(i));
        const int subjectCount= subject.count();
        bool duplicateFound= false;
        for (int j= 0; j < subjectCount; ++j)
        {
            duplicateFound= (compare(subject.at(j), point));
            if (duplicateFound) break;
        }
        if (!duplicateFound) subject.append(point);
    }
    return subject;
}

QList<GLC_Point2d> glc::line2CircleIntersection(const GLC_Point2d& lineOrigin, const GLC_Vector2d& lineDir, const GLC_Point2d& circleCenter, double radius)
{
    QList<GLC_Point2d> subject;
    const GLC_Vector2d diff(lineOrigin - circleCenter);
    const double a0= (diff * diff) - (radius * radius);
    const double a1= (lineDir * diff);
    const double discr= (a1 * a1) - a0;
    if (discr > 0.0)
    {
        double root= sqrt(discr);
        const GLC_Point2d p1(lineOrigin + (lineDir * (-a1 - root)));
        const GLC_Point2d p2(lineOrigin + (lineDir * (-a1 + root)));
        subject << p1 << p2;
    }
    else if (qFuzzyIsNull(discr))
    {
        const GLC_Point2d p(lineOrigin + (lineDir * (-a1)));
        subject << p;
    }

    return subject;
}

GLC_Point3d glc::project(const GLC_Point3d& point, const GLC_Plane& plane)
{
    Q_ASSERT(!plane.isNull());

    const GLC_Vector3d normal(plane.normal());

    GLC_Point3d subject(point - (((point * normal) + plane.coefD()) * normal));

    return subject;
}

bool glc::pointsAreCollinear(const QPointF& p1, const QPointF& p2, const QPointF& p3, double accuracy)
{
    return glc::compare(0.0, ((((p2.x() - p1.x()) * (p3.y() - p1.y()) - (p3.x() - p1.x()) * (p2.y() - p1.y())))), accuracy);
}

double glc::area(const QPolygonF& polygon)
{
    double subject= 0;
    const int count= polygon.count();
    for (int i= 0; i < count -1; ++i)
    {
        int j= (i + 1) % count;
        subject+= polygon.at(i).x() * polygon.at(j).y();
        subject-= polygon.at(i).y() * polygon.at(j).x();
    }

    return qAbs(subject);
}
