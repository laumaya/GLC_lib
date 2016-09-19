/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2010 Laurent Bauer
 Copyright (C) 2010 Laurent Ribon (laumaya@users.sourceforge.net)
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
//! \file glc_sphere.cpp implementation of the GLC_Sphere class.

#include "glc_sphere.h"

// Class chunk id
quint32 GLC_Sphere::m_ChunkId= 0xA710;

GLC_Sphere::GLC_Sphere(double radius, int discretization)
: GLC_Mesh()
, m_Radius (radius)
, m_Discret(discretization)
, m_ThetaMin (0.0)
, m_ThetaMax(2 * glc::PI)
, m_PhiMin(-glc::PI / 2.0)
, m_PhiMax(glc::PI / 2.0)
, m_Center()
{
	createMesh();
}

GLC_Sphere::GLC_Sphere(double radius, const GLC_Point3d& center, int discretization)
: GLC_Mesh()
, m_Radius (radius)
, m_Discret(discretization)
, m_ThetaMin (0.0)
, m_ThetaMax(2 * glc::PI)
, m_PhiMin(-glc::PI / 2.0)
, m_PhiMax(glc::PI / 2.0)
, m_Center(center)
{
	createMesh();
}


GLC_Sphere::GLC_Sphere(const GLC_Sphere & sphere)
:GLC_Mesh(sphere)
, m_Radius (sphere.m_Radius)
, m_Discret(sphere.m_Discret)
, m_ThetaMin (sphere.m_ThetaMin)
, m_ThetaMax(sphere.m_ThetaMax)
, m_PhiMin(sphere.m_PhiMin)
, m_PhiMax(sphere.m_PhiMax)
, m_Center(sphere.m_Center)
{
    if (isEmpty()) createMesh();
}

GLC_Sphere::~GLC_Sphere()
{

}

GLC_Geometry* GLC_Sphere::clone() const
{
	return new GLC_Sphere (*this);
}

const GLC_BoundingBox& GLC_Sphere::boundingBox()
{
	if ( GLC_Mesh::isEmpty() )
	{
		createMesh();
	}
    return GLC_Mesh::boundingBox();
}

bool GLC_Sphere::update()
{
    bool subject;
    if ( GLC_Mesh::isEmpty() )
    {
        createMesh();
        subject= true;
    }
    else
    {
        subject= false;
    }

    return subject;
}

void GLC_Sphere::setRadius(double Radius)
{
	Q_ASSERT(Radius > 0.0);
	m_Radius= Radius;

	GLC_Mesh::clearMeshWireAndBoundingBox();
}


void GLC_Sphere::setDiscretion(int TargetDiscret)
{
	Q_ASSERT(TargetDiscret > 0);
	if (TargetDiscret != m_Discret)
	{
		m_Discret= TargetDiscret;
		if (m_Discret < 6) m_Discret= 6;

		GLC_Mesh::clearMeshWireAndBoundingBox();
	}
}

void GLC_Sphere::setCenter(const GLC_Point3d& pos)
{
	if (pos != m_Center)
	{
		m_Center= pos;

		GLC_Mesh::clearMeshWireAndBoundingBox();
	}
}

void GLC_Sphere::glDraw(const GLC_RenderProperties& renderProperties)
{
	if (GLC_Mesh::isEmpty())
	{
		createMesh();
	}

	GLC_Mesh::glDraw(renderProperties);
}

void GLC_Sphere::createMesh()
{
	Q_ASSERT(GLC_Mesh::isEmpty());

	GLfloatVector verticeFloat;
	GLfloatVector normalsFloat;
	GLfloatVector texelVector;

	int currentIndex=0;

    // Theta
    const double wishedThetaStep= glc::PI / static_cast<double>(m_Discret);
    const double thetaRange= m_ThetaMax - m_ThetaMin;
    const int nbThetaSteps= static_cast<int>(thetaRange / wishedThetaStep) + 1 ;
    const double thetaStep= thetaRange / static_cast<double>(nbThetaSteps);

    // Phi
    const double wishedPhiStep= wishedThetaStep;
    const double phiRange= m_PhiMax - m_PhiMin;
    const int nbPhiSteps= static_cast<int>(phiRange / wishedPhiStep) + 1 ;
    const double phiStep= phiRange / nbPhiSteps;
    double phi= m_PhiMin;

    // Set sphere material
	GLC_Material* pMaterial;
	if (hasMaterial())
		pMaterial= this->firstMaterial();
	else
		pMaterial= new GLC_Material();

    // Set center delta
	const double dx= m_Center.x();
	const double dy= m_Center.y();
	const double dz= m_Center.z();

	// shaded face
	for (int p= 0; p < nbPhiSteps; ++p)
	{
        const double cosp= cos (phi);
        const double sinp= sin (phi);
        const double cospp= cos (phi + phiStep);
        const double sinpp= sin (phi + phiStep);

        const double zi = m_Radius * sinp;
        const double zf = m_Radius * sinpp;

		IndexList indexFace;

        float theta = m_ThetaMin;
		int t;
		for (t= 0; t <= nbThetaSteps; ++t)
		{
            const double cost= cos( theta );
            const double sint= sin( theta );

            const double xi= m_Radius * cost * cosp;
            const double yi= m_Radius * sint * cosp;
            const double xf= m_Radius * cost * cospp;
            const double yf= m_Radius * sint * cospp;

            verticeFloat << static_cast<float>(xf + dx) << static_cast<float>(yf + dy) << static_cast<float>(zf + dz);
            verticeFloat << static_cast<float>(xi + dx) << static_cast<float>(yi + dy) << static_cast<float>(zi + dz);

            normalsFloat << static_cast<float>(cost * cospp) << static_cast<float>(sint * cospp) << static_cast<float>(sinpp);
            normalsFloat << static_cast<float>(cost * cosp) << static_cast<float>(sint * cosp) << static_cast<float>(sinp);

            texelVector << static_cast<float>(t) * 1.0f / static_cast<float>(nbThetaSteps);
            texelVector << static_cast<float>(p) * 1.0f / static_cast<float>(nbPhiSteps);
            texelVector << static_cast<float>(t) * 1.0f / static_cast<float>(nbThetaSteps);
            texelVector << static_cast<float>(p+1) * 1.0f / static_cast<float>(nbPhiSteps);

            indexFace << (currentIndex + 2 * t) << (currentIndex + 2 * t + 1);

			theta+= thetaStep;
		}

		currentIndex+= 2 * t;
		addTrianglesStrip(pMaterial, indexFace);
		phi+= phiStep;
	}

	addVertice(verticeFloat);
	addNormals(normalsFloat);
	addTexels(texelVector);

	finish();
}


