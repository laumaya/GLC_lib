/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

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

//! \file glc_part.cpp implementation of the GLC_Part class.

#include "glc_part.h"
#include "glc_world.h"
#include "glc_mesh2.h"

GLC_Part::GLC_Part(GLC_Collection *pCollection, GLC_Instance& instance, GLuint shaderId)
: GLC_Node(pCollection)
, m_RepID(instance.id())
{
	m_pCollection->add(instance, shaderId);
	move(instance.getMatrix());
}

GLC_Part::~GLC_Part()
{
	if (not m_pCollection->isEmpty())
	{
		m_pCollection->remove(m_RepID);
	}
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Clone the part
GLC_Part* GLC_Part::clone(GLC_Collection * pCollection) const
{
	GLC_Instance instance(m_pCollection->getInstanceHandle(m_RepID)->instanciate());
	GLC_Part* pReturnPart;
	// Test if the instance is in a shading group
	if(m_pCollection->isInAShadingGroup(m_RepID))
	{
		GLuint shaderId= m_pCollection->shadingGroup(m_RepID);
		pCollection->bindShader(shaderId);
		pReturnPart= new GLC_Part(pCollection, instance, shaderId);
	}
	else
	{
		pReturnPart= new GLC_Part(pCollection, instance);
	}

	return pReturnPart;
}

//! Get number of Faces
int GLC_Part::numberOfFaces() const
{
	int number= 0;
	const GLC_Mesh2* pMesh= dynamic_cast<GLC_Mesh2*>(m_pCollection->getInstanceHandle(m_RepID)->getGeometry());
	if (NULL != pMesh)
	{
		number= pMesh->numberOfFaces();
	}
	return number;
}
//! Get number of vertex
int GLC_Part::numberOfVertex() const
{
	int number= 0;
	const GLC_Mesh2* pMesh= dynamic_cast<GLC_Mesh2*>(m_pCollection->getInstanceHandle(m_RepID)->getGeometry());
	if (NULL != pMesh)
	{
		number= pMesh->numberOfVertex();
	}
	return number;
}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Move the Part with specified matrix
GLC_Part* GLC_Part::move(const GLC_Matrix4x4 &matrix)
{
	m_RelativeMatrix= matrix * m_RelativeMatrix;
	m_AbsoluteMatrix= matrix * m_AbsoluteMatrix;
	m_pCollection->getInstanceHandle(m_RepID)->setMatrix(m_AbsoluteMatrix);
	return this;
}
// Update Part absolute matrix
void GLC_Part::updateAbsoluteMatrix()
{
	if (NULL != m_pParent)
	{
		m_AbsoluteMatrix= m_pParent->absoluteMatrix() * m_RelativeMatrix;
		m_pCollection->getInstanceHandle(m_RepID)->setMatrix(m_AbsoluteMatrix);
	}
}
//! Reverse representation normal
void GLC_Part::reverseMeshNormal()
{
	GLC_Mesh2* pMesh= dynamic_cast<GLC_Mesh2*>(m_pCollection->getInstanceHandle(m_RepID)->getGeometry());
	if (NULL != pMesh)
	{
		pMesh->reverseNormal();
	}
}

