/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Copyright (C) 2009 Pierre Soetewey
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
#include <stdio.h>
#include <math.h>
#include <../PQP/PQP.h>

#define PI 3.14159265359
#define LISTS 0

// Default constructor
GLC_Distance::GLC_Distance()
: GLC_Object("DistMin")
, m_ListOfInstances1()
, m_ListOfInstances2()
, m_Point1()
, m_Point2()
, m_DistanceMini(0.0)
, m_InstanceId1(0)
, m_InstanceId2(0)
, m_RelativeError(0.0)
, m_AbsoluteError(0.0)
{

}

// Construct a distmin with 2 GLC_3DViewInstance
GLC_Distance::GLC_Distance(const GLC_3DViewInstance& instance1, const GLC_3DViewInstance& instance2)
: GLC_Object("DistMin")
, m_ListOfInstances1()
, m_ListOfInstances2()
, m_Point1()
, m_Point2()
, m_DistanceMini(0.0)
, m_InstanceId1(0)
, m_InstanceId2(0)
, m_RelativeError(0.0)
, m_AbsoluteError(0.0)
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
, m_InstanceId1(distance.m_InstanceId1)
, m_InstanceId2(distance.m_InstanceId2)
, m_RelativeError(distance.m_RelativeError)
, m_AbsoluteError(distance.m_AbsoluteError)
{

}

GLC_Distance::~GLC_Distance()
{
	qDebug() << "GLC_Distance::~GLC_Distance()";

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
	m_InstanceId1= 0;
	m_InstanceId2= 0;
}

// Add instance in group 1
void GLC_Distance::addInstanceInGroup1(const GLC_3DViewInstance& instance)
{
	m_ListOfInstances1.append(instance);
}

// Add instances list in group 1
void GLC_Distance::addInstancesInGroup1(const QList<GLC_3DViewInstance>& instances)
{
	m_ListOfInstances1.append(instances);
}

// Add instance in group 1
void GLC_Distance::addInstanceInGroup2(const GLC_3DViewInstance& instance)
{
	m_ListOfInstances2.append(instance);
}

// Add instances list in group 1
void GLC_Distance::addInstancesInGroup2(const QList<GLC_3DViewInstance>& instances)
{
	m_ListOfInstances2.append(instances);
}

// Compute the minimum distance between the 2 groups
void GLC_Distance::computeMinimumDistance()
{
	DistanceResult distanceResult;
	if (not m_ListOfInstances1.isEmpty() and not m_ListOfInstances2.isEmpty())
	{
		const int size1= m_ListOfInstances1.size();
		const int size2= m_ListOfInstances2.size();
		GLC_3DViewInstance instance1;
		GLC_3DViewInstance instance2;
		DistanceResult currentDistance;
		double minimum= HUGE_VAL;
		double current;

		int index1= 0;
		int index2= 0;
		do
		{
			instance1= m_ListOfInstances1.at(index1);
			index2= 0;
			do
			{
				instance2= m_ListOfInstances2.at(index2);
				currentDistance= minimumDistance(instance1, instance2);
				current= currentDistance.m_Distance;

				if(current < minimum)
				{
					distanceResult= currentDistance;
					minimum= current;
					m_InstanceId1= instance1.id();
					m_InstanceId2= instance2.id();
				}
				++index2;

			} while(not qFuzzyCompare(current, 0.0) and (index2 < size2));
			++index1;

		} while(not qFuzzyCompare(current, 0.0) and (index1 < size1));

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
GLC_Distance::DistanceResult GLC_Distance::minimumDistance(GLC_3DViewInstance& instance1, GLC_3DViewInstance& instance2) const
{
	DistanceResult distanceResult;
	distanceResult.m_Distance= 0.0;

	//! Create the list ot the instance1 meshs
	QList<GLC_ExtendedMesh*> listOfMesh1;
	{
		const int size= instance1.numberOfGeometry();
		qDebug() << "Size = " << size;
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

	PQP_Model m1, m2;

	GLC_Matrix4x4 instance1Matrix(instance1.getMatrix());
	// List of axis scaling values
	QList<double> instance1Scale;
	instance1Scale << instance1Matrix.scalingX() << instance1Matrix.scalingY() << instance1Matrix.scalingZ();

	GLC_Matrix4x4 instance2Matrix(instance2.getMatrix());
	// List of axis scaling values
	QList<double> instance2Scale;
	instance2Scale << instance2Matrix.scalingX() << instance2Matrix.scalingY() << instance2Matrix.scalingZ();

	// Add first mesh's triangles to the PQP model
	addMeshTrianglesToPQP(&m1, listOfMesh1, instance1Scale);
	// Add second mesh's triangles to the PQP model
	addMeshTrianglesToPQP(&m2, listOfMesh2, instance2Scale);

	// PQP Translation
	PQP_REAL T1[3];
	PQP_REAL T2[3];
	// PQP rotation
	PQP_REAL R1[3][3];
	PQP_REAL R2[3][3];

	// Avoid scaling problem
	instance1Matrix= instance1Matrix.isometricMatrix();
	instance2Matrix= instance2Matrix.isometricMatrix();

	const double * pMat1= instance1Matrix.data();
	const double * pMat2= instance2Matrix.data();

	T1[0]= pMat1[12];
	T1[1]= pMat1[13];
	T1[2]= pMat1[14];

	T2[0]= pMat2[12];
	T2[1]= pMat2[13];
	T2[2]= pMat2[14];

	R1[0][0]= pMat1[0];
	R1[1][0]= pMat1[1];
	R1[2][0]= pMat1[2];
	R1[0][1]= pMat1[4];
	R1[1][1]= pMat1[5];
	R1[2][1]= pMat1[6];
	R1[0][2]= pMat1[8];
	R1[1][2]= pMat1[9];
	R1[2][2]= pMat1[10];

	R2[0][0]= pMat2[0];
	R2[1][0]= pMat2[1];
	R2[2][0]= pMat2[2];
	R2[0][1]= pMat2[4];
	R2[1][1]= pMat2[5];
	R2[2][1]= pMat2[6];
	R2[0][2]= pMat2[8];
	R2[1][2]= pMat2[9];
	R2[2][2]= pMat2[10];


	PQP_DistanceResult dres;

	PQP_Distance(&dres,R1,T1,&m1,R2,T2,&m2,m_RelativeError, m_AbsoluteError);
	double distance = dres.Distance();

	const PQP_REAL * p1 = dres.P1();
	const PQP_REAL * p2 = dres.P2();

	GLC_Point4d point1(p1[0],p1[1],p1[2]);
	GLC_Point4d point2(p2[0],p2[1],p2[2]);


	point1= instance1Matrix * point1;
	point2= instance2Matrix * point2;

	distanceResult.m_Distance= distance;
	distanceResult.m_Point1= point1;
	distanceResult.m_Point2= point2;

	//qDebug() << "Distance " << distanceResult.m_Distance;
	return distanceResult;
}

// Add mesh triangles to PQP model
void GLC_Distance::addMeshTrianglesToPQP(PQP_Model* pPQP_Model, const QList<GLC_ExtendedMesh*> listOfMeshes, const QList<double>& scaleFactors) const
{
	int generalCount=0;
	pPQP_Model->BeginModel();

	int i,j,k,l,size,size2,size3,size4, pos1, pos2, pos3;
	GLC_ExtendedMesh * pMesh=NULL;
	GLC_uint materialId;
	GLfloatVector positionVector;
	PQP_REAL p1[3], p2[3], p3[3];
	size= listOfMeshes.size();
	for (i= 0; i < size; ++i)
	{

		pMesh= listOfMeshes.at(i);
		positionVector= pMesh->positionVector();
		size2= pMesh->numberOfMaterials();
		for(j=0; j< size2;++j){

			materialId= pMesh->material(pMesh->materialIds().at(j))->id();
			if(pMesh->lodContainsMaterial(0, materialId)){

				if (pMesh->containsTriangles(0, materialId)){

					QVector<GLuint> index= pMesh->getTrianglesIndex(0, materialId);
					size3= index.size();
					for (k= 0; k < size3; k= k+3)
					{
						pos1= index.at(k)*3;
						pos2= index.at(k+1)*3;
						pos3= index.at(k+2)*3;
						p1[0]= positionVector.at(pos1) * scaleFactors.at(0);
						p1[1]= positionVector.at(pos1+1) * scaleFactors.at(1);
						p1[2]= positionVector.at(pos1+2) * scaleFactors.at(2);
						p2[0]= positionVector.at(pos2) * scaleFactors.at(0);
						p2[1]= positionVector.at(pos2+1) * scaleFactors.at(1);
						p2[2]= positionVector.at(pos2+2) * scaleFactors.at(2);
						p3[0]= positionVector.at(pos3) * scaleFactors.at(0);
						p3[1]= positionVector.at(pos3+1) * scaleFactors.at(1);
						p3[2]= positionVector.at(pos3+2) * scaleFactors.at(2);
						pPQP_Model->AddTri(p1, p2, p3, generalCount);
						++generalCount;
					}
				}

				if (pMesh->containsStrips(0, materialId)){

					QList<QVector<GLuint> > index= pMesh->getStripsIndex(0, materialId);
					size3= index.size();
					for (k= 0; k < size3; ++k)
					{
						pos1= index.at(k).at(0)*3;
						pos2= index.at(k).at(1)*3;
						pos3= index.at(k).at(2)*3;
						p1[0]= positionVector.at(pos1) * scaleFactors.at(0);
						p1[1]= positionVector.at(pos1+1) * scaleFactors.at(1);
						p1[2]= positionVector.at(pos1+2) * scaleFactors.at(2);
						p2[0]= positionVector.at(pos2) * scaleFactors.at(0);
						p2[1]= positionVector.at(pos2+1) * scaleFactors.at(1);
						p2[2]= positionVector.at(pos2+2) * scaleFactors.at(2);
						p3[0]= positionVector.at(pos3) * scaleFactors.at(0);
						p3[1]= positionVector.at(pos3+1) * scaleFactors.at(1);
						p3[2]= positionVector.at(pos3+2) * scaleFactors.at(2);
						pPQP_Model->AddTri(p1, p2, p3, generalCount);
						++generalCount;

						size4= index.at(k).size();
						for (l= 3; l < size4; ++l)
						{
							pos1= index.at(k).at(l)*3;
							pos2= index.at(k).at(l-1)*3;
							pos3= index.at(k).at(l-2)*3;
							p1[0]= positionVector.at(pos1) * scaleFactors.at(0);
							p1[1]= positionVector.at(pos1+1) * scaleFactors.at(1);
							p1[2]= positionVector.at(pos1+2) * scaleFactors.at(2);
							p2[0]= positionVector.at(pos2) * scaleFactors.at(0);
							p2[1]= positionVector.at(pos2+1) * scaleFactors.at(1);
							p2[2]= positionVector.at(pos2+2) * scaleFactors.at(2);
							p3[0]= positionVector.at(pos3) * scaleFactors.at(0);
							p3[1]= positionVector.at(pos3+1) * scaleFactors.at(1);
							p3[2]= positionVector.at(pos3+2) * scaleFactors.at(2);
							pPQP_Model->AddTri(p1, p2, p3, generalCount);
							++generalCount;
						}
					}
				}

				if (pMesh->containsFans(0, materialId)){

					QList<QVector<GLuint> > index= pMesh->getFansIndex(0, materialId);
					size3= index.size();
					for (k= 0; k < size3; ++k)
					{
						pos1= index.at(k).at(0)*3;
						pos2= index.at(k).at(1)*3;
						pos3= index.at(k).at(2)*3;
						p1[0]= positionVector.at(pos1) * scaleFactors.at(0);
						p1[1]= positionVector.at(pos1+1) * scaleFactors.at(1);
						p1[2]= positionVector.at(pos1+2) * scaleFactors.at(2);
						p2[0]= positionVector.at(pos2) * scaleFactors.at(0);
						p2[1]= positionVector.at(pos2+1) * scaleFactors.at(1);
						p2[2]= positionVector.at(pos2+2) * scaleFactors.at(2);
						p3[0]= positionVector.at(pos3) * scaleFactors.at(0);
						p3[1]= positionVector.at(pos3+1) * scaleFactors.at(1);
						p3[2]= positionVector.at(pos3+2) * scaleFactors.at(2);
						pPQP_Model->AddTri(p1, p2, p3, generalCount);
						++generalCount;

						size4= index.at(k).size();
						for (l= 3; l < size4; ++l)
						{
							pos2= index.at(k).at(l-1)*3;
							pos3= index.at(k).at(l)*3;
							p2[0]= positionVector.at(pos2) * scaleFactors.at(0);
							p2[1]= positionVector.at(pos2+1) * scaleFactors.at(1);
							p2[2]= positionVector.at(pos2+2) * scaleFactors.at(2);
							p3[0]= positionVector.at(pos3) * scaleFactors.at(0);
							p3[1]= positionVector.at(pos3+1) * scaleFactors.at(1);
							p3[2]= positionVector.at(pos3+2) * scaleFactors.at(2);
							pPQP_Model->AddTri(p1, p2, p3, generalCount);
							++generalCount;
						}
					}
				}
			}
		}
	}
	pPQP_Model->EndModel();
}
