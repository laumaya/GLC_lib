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

#include "glc_3drep.h"

// Default constructor
GLC_3DRep::GLC_3DRep()
: GLC_Rep()
, m_pGeomList(new QList<GLC_VboGeom*>)
, m_pType(new int(GLC_Rep::GLC_VBOGEOM))
, m_pDefaultLOD(new int(10))
{

}

// Construct a 3DRep with a geometry
GLC_3DRep::GLC_3DRep(GLC_VboGeom* pGeom)
: GLC_Rep()
, m_pGeomList(new QList<GLC_VboGeom*>)
, m_pType(new int(GLC_Rep::GLC_VBOGEOM))
, m_pDefaultLOD(new int(10))
{
	m_pGeomList->append(pGeom);
}

// Copy Constructor
GLC_3DRep::GLC_3DRep(const GLC_3DRep& rep)
: GLC_Rep(rep)
, m_pGeomList(rep.m_pGeomList)
, m_pType(rep.m_pType)
, m_pDefaultLOD(rep.m_pDefaultLOD)
{

}

// Assignement operator
GLC_3DRep& GLC_3DRep::operator=(const GLC_3DRep& rep)
{
	clear();
	GLC_Rep::operator=(rep);
	m_pGeomList= rep.m_pGeomList;
	m_pType= rep.m_pType;
	m_pDefaultLOD= rep.m_pDefaultLOD;

	return *this;
}

// Clone the representation
GLC_Rep* GLC_3DRep::clone() const
{
	return new GLC_3DRep(*this);
}

// Destructor
GLC_3DRep::~GLC_3DRep()
{
	clear();
}

//////////////////////////////////////////////////////////////////////
// Get functions
//////////////////////////////////////////////////////////////////////
// Return the type of representation
int GLC_3DRep::type() const
{
	return (*m_pType);
}

//////////////////////////////////////////////////////////////////////
// Opengl functions
//////////////////////////////////////////////////////////////////////
// Display the 3DRep
void GLC_3DRep::glExecute(bool isSelected, bool transparent, bool useLoad, double cameraCover)
{
	const int size= m_pGeomList->size();
	for (int i= 0; i < size; ++i)
	{
		const double diameter= m_pGeomList->at(i)->boundingBox().boundingSphereRadius() * 2.0;
		double ratio= diameter / cameraCover * 150.0;

		if (ratio > 100.0) ratio= 100.0;
		ratio= 100.0 - ratio;
		if (ratio > 98.0) ratio= 110.0;
		if (ratio < static_cast<double>(*m_pDefaultLOD)) ratio= static_cast<double>(*m_pDefaultLOD);
		//qDebug() << "RATIO = " << static_cast<int>(ratio);

		const int lodValue= static_cast<int>(ratio);
		if (useLoad)
		{
			if (lodValue <= 100)
			{
				m_pGeomList->at(i)->setCurrentLod(lodValue);
				m_pGeomList->at(i)->glExecute(isSelected, transparent);
			}
		}
		else
		{
			if (lodValue <= 100)
			{
				m_pGeomList->at(i)->setCurrentLod(*m_pDefaultLOD);
				m_pGeomList->at(i)->glExecute(isSelected, transparent);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////

// Clear the 3D representation
void GLC_3DRep::clear()
{
	if (isTheLast())
	{
		const int size= m_pGeomList->size();
		for (int i= 0; i < size; ++i)
		{
			delete (*m_pGeomList)[i];
		}
		delete m_pGeomList;
		m_pGeomList= NULL;
	}
	delete m_pType;
	m_pType= NULL;

	delete m_pDefaultLOD;
	m_pDefaultLOD= NULL;
}
