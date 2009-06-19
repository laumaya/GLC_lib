/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.1.0, packaged on March, 2009.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/

#include "glc_distance.h"
#include "../geometry/glc_extendedmesh.h"

// Default constructor
GLC_Distance::GLC_Distance()
: GLC_Object("DistMin")
, m_ListOfInstances1()
, m_ListOfInstances2()
, m_Point1()
, m_Point2()
, m_DistanceMini(0.0)
{

}

// Construct a distmin with 2 GLC_Instance
GLC_Distance::GLC_Distance(const GLC_Instance& instance1, const GLC_Instance& instance2)
: GLC_Object("DistMin")
, m_ListOfInstances1()
, m_ListOfInstances2()
, m_Point1()
, m_Point2()
, m_DistanceMini(0.0)
{
	m_ListOfInstances1.append(instance1);
	m_ListOfInstances2.append(instance2);
}

// Copy Constructor
GLC_Distance::GLC_Distance(const GLC_Distance& distance)
: GLC_Object(distance)
, m_ListOfInstances1(distance.m_ListOfInstances1)
, m_ListOfInstances2(distance.m_ListOfInstances2)
, m_Point1(distance.m_Point1)
, m_Point2(distance.m_Point2)
, m_DistanceMini(distance.m_DistanceMini)
{

}

GLC_Distance::~GLC_Distance()
{

}

//////////////////////////////////////////////////////////////////////
// name Set Functions
//////////////////////////////////////////////////////////////////////

// Clear the 2 groups
void GLC_Distance::clear()
{
	m_ListOfInstances1.clear();
	m_ListOfInstances2.clear();
	m_Point1.setVect(GLC_Point4d());
	m_Point2.setVect(GLC_Point4d());
	m_DistanceMini= 0.0;
}

// Add instance in group 1
void GLC_Distance::addInstanceInGroup1(const GLC_Instance& instance)
{
	m_ListOfInstances1.append(instance);
}

// Add instances list in group 1
void GLC_Distance::addInstancesInGroup1(const QList<GLC_Instance>& instances)
{
	m_ListOfInstances1.append(instances);
}

// Add instance in group 1
void GLC_Distance::addInstanceInGroup2(const GLC_Instance& instance)
{
	m_ListOfInstances2.append(instance);
}

// Add instances list in group 1
void GLC_Distance::addInstancesInGroup2(const QList<GLC_Instance>& instances)
{
	m_ListOfInstances2.append(instances);
}

// Compute the minimum distance between the 2 groups
void GLC_Distance::computeMinimumDistance()
{
	DistanceResult distanceResult;
	distanceResult.m_Distance= MAXFLOAT;
	if (not m_ListOfInstances1.isEmpty() and not m_ListOfInstances2.isEmpty())
	{
		const int size1= m_ListOfInstances1.size();
		const int size2= m_ListOfInstances2.size();
		for (int index1= 0; index1 < size1; ++index1)
		{
			GLC_Instance instance1= m_ListOfInstances1.at(index1);
			for (int index2= 0; index2 < size2; ++index2)
			{
				GLC_Instance instance2= m_ListOfInstances2.at(index2);
				DistanceResult currentDistance= minimumDistance(instance1, instance2);
				qDebug() << "currentDistance " << currentDistance.m_Distance;
				if (currentDistance.m_Distance < distanceResult.m_Distance) distanceResult= currentDistance;
			}
		}
	}
	else
	{
		qDebug() << "a list is empty";
		distanceResult.m_Distance= -1.0;
		distanceResult.m_Point1= GLC_Point4d();
		distanceResult.m_Point2= GLC_Point4d();
	}
	m_DistanceMini= distanceResult.m_Distance;
	m_Point1= distanceResult.m_Point1;
	m_Point2= distanceResult.m_Point2;

}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////

// Return distance mini beween to instance
GLC_Distance::DistanceResult GLC_Distance::minimumDistance(GLC_Instance& instance1, GLC_Instance& instance2) const
{
	DistanceResult distanceResult;
	distanceResult.m_Distance= 0.0;

	//! Create the list ot the instance1 meshs
	QList<GLC_ExtendedMesh*> listOfMesh1;
	{
		const int size= instance1.numberOfGeometry();
		for (int i= 0; i < size; ++i)
		{
			GLC_ExtendedMesh* pMesh= dynamic_cast<GLC_ExtendedMesh*>(instance1.getGeometry(i));
			if (NULL != pMesh) listOfMesh1.append(pMesh);
		}
	}
	if (listOfMesh1.isEmpty())
	{
		qDebug() << "listOfMesh1 empty";
		return distanceResult;
	}

	//! Create the list ot the instance2 meshs
	QList<GLC_ExtendedMesh*> listOfMesh2;
	{
		const int size= instance2.numberOfGeometry();
		for (int i= 0; i < size; ++i)
		{
			GLC_ExtendedMesh* pMesh= dynamic_cast<GLC_ExtendedMesh*>(instance2.getGeometry(i));
			if (NULL != pMesh) listOfMesh2.append(pMesh);
		}
	}
	if (listOfMesh2.isEmpty())
	{
		qDebug() << "listOfMesh2 empty";
		return distanceResult;
	}


	// Create the list of instance1 positions
	QList<GLfloat> positions1;
	{
		const int size= listOfMesh1.size();
		for (int i= 0; i < size; ++i)
		{
			positions1.append(listOfMesh1.at(i)->positionVector().toList());
		}
	}
	// Create the list of instance2 positions
	QList<GLfloat> positions2;
	{
		const int size= listOfMesh2.size();
		for (int i= 0; i < size; ++i)
		{
			positions2.append(listOfMesh2.at(i)->positionVector().toList());
		}
	}

	// Update the position of instance1 vertice
	GLC_Matrix4x4 instance1Matrix(instance1.getMatrix());
	const int size1= positions1.size();
	if (instance1Matrix != GLC_Matrix4x4())
	{
		for (int i= 0; i < size1; i+= 3)
		{
			GLC_Point4d point(positions1.at(i), positions1.at(i + 1), positions1.at(i + 2));
			point= instance1Matrix * point;
			positions1[i]= static_cast<float>(point.X());
			positions1[i + 1]= static_cast<float>(point.Y());
			positions1[i + 2]= static_cast<float>(point.Z());
		}
	}

	// Update the position of instance1 vertice
	GLC_Matrix4x4 instance2Matrix(instance2.getMatrix());
	const int size2= positions2.size();
	if (instance2Matrix != GLC_Matrix4x4())
	{
		for (int i= 0; i < size2; i+= 3)
		{
			GLC_Point4d point(positions2.at(i), positions2.at(i + 1), positions2.at(i + 2));
			point= instance2Matrix * point;
			positions2[i]= static_cast<float>(point.X());
			positions2[i + 1]= static_cast<float>(point.Y());
			positions2[i + 2]= static_cast<float>(point.Z());
		}
	}

	// Comput the minimum distance
	distanceResult.m_Distance= HUGE_VAL;
	for (int index1= 0; index1 < size1; index1+= 3)
	{
		GLC_Point4d point1(positions1.at(index1), positions1.at(index1 + 1), positions1.at(index1 + 2));
		for (int index2= 0; index2 < size2; index2+= 3)
		{
			GLC_Point4d point2(positions2.at(index2), positions2.at(index2 + 1), positions2.at(index2 + 2));
			GLC_Vector4d delta(point1 - point2);
			double distance2= delta.norm();
			if (distance2 < distanceResult.m_Distance)
			{
				distanceResult.m_Distance= distance2;
				distanceResult.m_Point1= point1;
				distanceResult.m_Point2= point2;
			}
		}
	}

	qDebug() << "Distance " << distanceResult.m_Distance;
	return distanceResult;
 }

