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
	clear();
	const int size1= m_ListOfInstances1.size();
	const int size2= m_ListOfInstances2.size();
	for (int index1= 0; index1 < size1; ++index1)
	{
		for (int index2= 0; index2 < size2; ++index2)
		{

		}
	}

}

// Return the minimum distance between the 2 groups
double GLC_Distance::distMin()
{

}


//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////

// Return distance mini beween to instance
double GLC_Distance::minimumDistance(GLC_Instance& instance1, GLC_Instance& instance2) const
{
	double distance= 0.0;

	QList<GLC_ExtendedMesh*> listOfMesh1;
	{
		const int size= instance1.numberOfGeometry();
		for (int i= 0; i < size; ++i)
		{
			GLC_ExtendedMesh* pMesh= dynamic_cast<GLC_ExtendedMesh*>(instance1.getGeometry(i));
			if (NULL != pMesh) listOfMesh1.append(pMesh);
		}
	}
	if (listOfMesh1.isEmpty()) return distance;

	QList<GLC_ExtendedMesh*> listOfMesh2;
	{
		const int size= instance2.numberOfGeometry();
		for (int i= 0; i < size; ++i)
		{
			GLC_ExtendedMesh* pMesh= dynamic_cast<GLC_ExtendedMesh*>(instance2.getGeometry(i));
			if (NULL != pMesh) listOfMesh2.append(pMesh);
		}
	}
	if (listOfMesh2.isEmpty()) return distance;

	int minIndexList1= 0;
	int minIndexList2= 0;
	const int sizeOfList1= listOfMesh1.size();
	const int sizeOfList2= listOfMesh2.size();
	GLC_Matrix4x4 instance1Matrix= instance1.getMatrix();
	GLC_Matrix4x4 instance2Matrix= instance2.getMatrix();
	distance= (instance1.getBoundingBox().getCenter() - instance2.getBoundingBox().getCenter()).norm();
	distance+= instance1.getBoundingBox().boundingSphereRadius() + instance2.getBoundingBox().boundingSphereRadius();
	qDebug() << "Distance " << distance;

	for (int indexList1= 0; indexList1 < sizeOfList1; ++indexList1)
	{
		GLC_Point4d point1= listOfMesh1.at(indexList1)->boundingBox().transform(instance1Matrix).getCenter();
		for (int indexList2= 0; indexList2 < sizeOfList2; ++indexList2)
		{

			GLC_Point4d point2= listOfMesh2.at(indexList2)->boundingBox().transform(instance2Matrix).getCenter();
			GLC_Vector4d delta(point1 - point2);
			double distance2= delta.norm() + listOfMesh2.at(indexList2)->boundingBox().boundingSphereRadius();
			if (distance2 < distance)
			{
				minIndexList1= indexList1;
				minIndexList2= indexList2;
				distance= distance2;
			}
		}

	}

	return distance;
 }

