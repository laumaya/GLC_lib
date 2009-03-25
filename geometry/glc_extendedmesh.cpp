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

//! \file glc_extendedmesh.cpp Implementation for the GLC_ExtendedMesh class.

#include "glc_extendedmesh.h"
#include "../glc_state.h"
#include "../shading/glc_selectionmaterial.h"

GLC_ExtendedMesh::GLC_ExtendedMesh()
:GLC_VboGeom("ExtendedMesh", false)
, m_PrimitiveGroups()
, m_NumberOfFaces(0)
, m_NumberOfVertice(0)
, m_IsSelected(false)
, m_ColorPearVertex(false)
, m_ExtendedGeomEngine()
{

}

GLC_ExtendedMesh::GLC_ExtendedMesh(const GLC_ExtendedMesh& mesh)
:GLC_VboGeom(mesh)
, m_PrimitiveGroups(mesh.m_PrimitiveGroups)
, m_NumberOfFaces(mesh.m_NumberOfFaces)
, m_NumberOfVertice(mesh.m_NumberOfVertice)
, m_IsSelected(false)
, m_ColorPearVertex(mesh.m_ColorPearVertex)
, m_ExtendedGeomEngine(mesh.m_ExtendedGeomEngine)
{


}

GLC_ExtendedMesh::~GLC_ExtendedMesh()
{
	PrimitiveGroups::iterator iGroups= m_PrimitiveGroups.begin();
	while (iGroups != m_PrimitiveGroups.constEnd())
	{
		delete iGroups.value();
		++iGroups;
	}
}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get number of faces
unsigned int GLC_ExtendedMesh::numberOfFaces() const
{
	return m_NumberOfFaces;
}

// Get number of vertex
unsigned int GLC_ExtendedMesh::numberOfVertex() const
{
	return m_NumberOfVertice;
}

// return the mesh bounding box
GLC_BoundingBox& GLC_ExtendedMesh::boundingBox()
{
	if (NULL == m_pBoundingBox)
	{
		//qDebug() << "GLC_Mesh2::boundingBox create boundingBox";
		m_pBoundingBox= new GLC_BoundingBox();

		if (m_ExtendedGeomEngine.positionVectorHandle()->isEmpty())
		{
			qDebug() << "GLC_ExtendedMesh::getBoundingBox empty m_Positions";
		}
		else
		{
			GLfloatVector* pVertexVector= m_ExtendedGeomEngine.positionVectorHandle();
			const int max= pVertexVector->size();
			for (int i= 0; i < max; i= i + 3)
			{
				GLC_Vector3d vector((*pVertexVector)[i], (*pVertexVector)[i + 1], (*pVertexVector)[i + 2]);
				m_pBoundingBox->combine(vector);
			}
		}

	}
	return *m_pBoundingBox;

}

// Return a copy of the geometry
GLC_VboGeom* GLC_ExtendedMesh::clone() const
{
	return new GLC_ExtendedMesh(*this);
}
//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Add triangles
void GLC_ExtendedMesh::addTriangles(GLC_Material* pMaterial, const IndexList& indexList)
{
	GLC_uint groupId= setCurrentMaterial(pMaterial);
	Q_ASSERT(m_PrimitiveGroups.contains(groupId));
	m_PrimitiveGroups.value(groupId)->addTriangles(indexList);

	// Invalid the geometry
	m_GeometryIsValid = false;
	m_NumberOfFaces+= indexList.size() / 3;
}

// Add triangles Strip
void GLC_ExtendedMesh::addTrianglesStrip(GLC_Material* pMaterial, const IndexList& indexList)
{
	GLC_uint groupId= setCurrentMaterial(pMaterial);
	Q_ASSERT(m_PrimitiveGroups.contains(groupId));
	m_PrimitiveGroups.value(groupId)->addTrianglesStrip(indexList);

	// Invalid the geometry
	m_GeometryIsValid = false;
	m_NumberOfFaces+= indexList.size() - 2;
}
// Add triangles Fan
void GLC_ExtendedMesh::addTrianglesFan(GLC_Material* pMaterial, const IndexList& indexList)
{
	GLC_uint groupId= setCurrentMaterial(pMaterial);
	Q_ASSERT(m_PrimitiveGroups.contains(groupId));
	m_PrimitiveGroups.value(groupId)->addTrianglesFan(indexList);

	// Invalid the geometry
	m_GeometryIsValid = false;
	m_NumberOfFaces+= indexList.size() - 2;
}

//!Reverse mesh normal
void GLC_ExtendedMesh::reverseNormal()
{

}

// Copy index list in a vector for Vertex Array Use
void GLC_ExtendedMesh::finished()
{
	PrimitiveGroups::iterator iGroup= m_PrimitiveGroups.begin();
	while (iGroup != m_PrimitiveGroups.constEnd())
	{
		// Add group triangles index to engine triangles index vector
		if (iGroup.value()->containsTriangles())
		{
			iGroup.value()->setTrianglesOffset(BUFFER_OFFSET(m_ExtendedGeomEngine.trianglesIndexVectorHandle()->size() * sizeof(GLuint)));
			(*m_ExtendedGeomEngine.trianglesIndexVectorHandle())+= iGroup.value()->trianglesIndex().toVector();
		}

		// Add group strip index to engine strip index vector
		if (iGroup.value()->containsStrip())
		{
			iGroup.value()->setBaseTrianglesStripOffset(BUFFER_OFFSET(m_ExtendedGeomEngine.trianglesStripIndexVectorHandle()->size() * sizeof(GLuint)));
			(*m_ExtendedGeomEngine.trianglesStripIndexVectorHandle())+= iGroup.value()->stripsIndex().toVector();
		}

		// Add group fan index to engine fan index vector
		if (iGroup.value()->containsFan())
		{
			iGroup.value()->setBaseTrianglesFanOffset(BUFFER_OFFSET(m_ExtendedGeomEngine.trianglesFanIndexVectorHandle()->size() * sizeof(GLuint)));
			(*m_ExtendedGeomEngine.trianglesFanIndexVectorHandle())+= iGroup.value()->fansIndex().toVector();
		}

		++iGroup;
	}
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
// Specific glExecute method
void GLC_ExtendedMesh::glExecute(bool isSelected, bool transparent)
{
	m_IsSelected= isSelected;
	GLC_VboGeom::glExecute(isSelected, transparent);
	m_IsSelected= false;
}

// Virtual interface for OpenGL Geometry set up.
void GLC_ExtendedMesh::glDraw(bool transparent)
{
	Q_ASSERT(m_GeometryIsValid or not m_ExtendedGeomEngine.positionVectorHandle()->isEmpty());
	const bool vboIsUsed= GLC_State::vboUsed();
	if (vboIsUsed)
	{
		m_ExtendedGeomEngine.createVBOs();
	}
	// Create VBO and IBO
	if (!m_GeometryIsValid)
	{
		if (vboIsUsed)
		{
			// Create VBO of vertices
			{
				m_ExtendedGeomEngine.useVBO(true, GLC_ExtendedGeomEngine::GLC_Vertex);

				GLfloatVector* pPositionVector= m_ExtendedGeomEngine.positionVectorHandle();
				const GLsizei dataNbr= static_cast<GLsizei>(pPositionVector->size());
				const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
				glBufferData(GL_ARRAY_BUFFER, dataSize, pPositionVector->data(), GL_STATIC_DRAW);
			}

			// Create VBO of normals
			{
				m_ExtendedGeomEngine.useVBO(true, GLC_ExtendedGeomEngine::GLC_Normal);

				GLfloatVector* pNormalVector= m_ExtendedGeomEngine.normalVectorHandle();
				const GLsizei dataNbr= static_cast<GLsizei>(pNormalVector->size());
				const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
				glBufferData(GL_ARRAY_BUFFER, dataSize, pNormalVector->data(), GL_STATIC_DRAW);
			}
			// Create VBO of texel if needed
			if (m_ExtendedGeomEngine.useVBO(true, GLC_ExtendedGeomEngine::GLC_Texel))
			{
				GLfloatVector* pTexelVector= m_ExtendedGeomEngine.texelVectorHandle();
				const GLsizei dataNbr= static_cast<GLsizei>(pTexelVector->size());
				const GLsizeiptr dataSize= dataNbr * sizeof(GLfloat);
				glBufferData(GL_ARRAY_BUFFER, dataSize, pTexelVector->data(), GL_STATIC_DRAW);
			}

			//Create Triangle IBO
			if (m_ExtendedGeomEngine.containsTriangles())
			{
				QVector<GLuint>* pIndexVector= m_ExtendedGeomEngine.trianglesIndexVectorHandle();
				m_ExtendedGeomEngine.useIBO(true, GLC_ExtendedGeomEngine::GLC_Triangles);
				const GLsizei indexNbr= static_cast<GLsizei>(pIndexVector->size());
				const GLsizeiptr indexSize = indexNbr * sizeof(GLuint);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, pIndexVector->data(), GL_STATIC_DRAW);
			}
			//Create Triangle Strip IBO
			if (m_ExtendedGeomEngine.containsTrianglesStrip())
			{
				QVector<GLuint>* pIndexVector= m_ExtendedGeomEngine.trianglesStripIndexVectorHandle();
				m_ExtendedGeomEngine.useIBO(true, GLC_ExtendedGeomEngine::GLC_TrianglesStrip);
				const GLsizei indexNbr= static_cast<GLsizei>(pIndexVector->size());
				const GLsizeiptr indexSize = indexNbr * sizeof(GLuint);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, pIndexVector->data(), GL_STATIC_DRAW);
			}
			//Create Triangle Fan IBO
			if (m_ExtendedGeomEngine.containsTrianglesFan())
			{
				QVector<GLuint>* pIndexVector= m_ExtendedGeomEngine.trianglesFanIndexVectorHandle();
				m_ExtendedGeomEngine.useIBO(true, GLC_ExtendedGeomEngine::GLC_TrianglesFan);
				const GLsizei indexNbr= static_cast<GLsizei>(pIndexVector->size());
				const GLsizeiptr indexSize = indexNbr * sizeof(GLuint);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, pIndexVector->data(), GL_STATIC_DRAW);
			}
			//TODO clear engine
		}

	}

	if (vboIsUsed)
	{
		// Activate Vertices VBO
		m_ExtendedGeomEngine.useVBO(true, GLC_ExtendedGeomEngine::GLC_Vertex);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);

		// Activate Normals VBO
		m_ExtendedGeomEngine.useVBO(true, GLC_ExtendedGeomEngine::GLC_Normal);
		glNormalPointer(GL_FLOAT, 0, 0);
		glEnableClientState(GL_NORMAL_ARRAY);

		// Activate texel VBO if needed
		if (m_ExtendedGeomEngine.useVBO(true, GLC_ExtendedGeomEngine::GLC_Texel))
		{
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		//TODO test if color pear vertex is activated
	}

	PrimitiveGroups::iterator iGroup= m_PrimitiveGroups.begin();
	while (iGroup != m_PrimitiveGroups.constEnd())
	{
		GLC_PrimitiveGroup* pCurrentGroup= iGroup.value();
		GLC_Material* pCurrentMaterial= m_MaterialHash.value(pCurrentGroup->id());

   		if ((not GLC_State::selectionShaderUsed() or not m_IsSelected) and not GLC_State::isInSelectionMode())
    	{
			if (pCurrentMaterial->isTransparent() == transparent)
			{
				// Execute current material
				if (pCurrentMaterial->getAddRgbaTexture())
				{
					glEnable(GL_TEXTURE_2D);
				}
				else
				{
					glDisable(GL_TEXTURE_2D);
				}
				// Activate material
				pCurrentMaterial->glExecute();
				const GLfloat red= pCurrentMaterial->getDiffuseColor().redF();
				const GLfloat green= pCurrentMaterial->getDiffuseColor().greenF();
				const GLfloat blue= pCurrentMaterial->getDiffuseColor().blueF();
				const GLfloat alpha= pCurrentMaterial->getDiffuseColor().alphaF();

				glColor4f(red, green, blue, alpha);
				if (m_IsSelected) GLC_SelectionMaterial::glExecute();
			}
		}
		else if(not GLC_State::isInSelectionMode())
		{
			// Use Shader
			glDisable(GL_TEXTURE_2D);
		}


		if (m_IsSelected or GLC_State::isInSelectionMode() or (pCurrentMaterial->isTransparent() == transparent))
		{
			// Draw triangles
			if (pCurrentGroup->containsTriangles())
			{
				m_ExtendedGeomEngine.useIBO(true, GLC_ExtendedGeomEngine::GLC_Triangles);
				glDrawElements(GL_TRIANGLES, pCurrentGroup->trianglesIndex().size(), GL_UNSIGNED_INT, pCurrentGroup->trianglesIndexOffset());
				m_ExtendedGeomEngine.useIBO(false, GLC_ExtendedGeomEngine::GLC_Triangles);
			}

			// Draw Triangles strip
			if (pCurrentGroup->containsStrip())
			{
				m_ExtendedGeomEngine.useIBO(true, GLC_ExtendedGeomEngine::GLC_TrianglesStrip);
				const GLsizei stripsCount= static_cast<GLsizei>(pCurrentGroup->stripsOffset().size());
				glMultiDrawElements(GL_TRIANGLE_STRIP, pCurrentGroup->stripsSizes().data(), GL_UNSIGNED_INT, (const GLvoid**)pCurrentGroup->stripsOffset().data(), stripsCount);
				m_ExtendedGeomEngine.useIBO(false, GLC_ExtendedGeomEngine::GLC_TrianglesStrip);
			}

			// Draw Triangles fan
			if (pCurrentGroup->containsFan())
			{
				m_ExtendedGeomEngine.useIBO(true, GLC_ExtendedGeomEngine::GLC_TrianglesFan);
				const GLsizei fansCount= static_cast<GLsizei>(pCurrentGroup->fansOffset().size());
				glMultiDrawElements(GL_TRIANGLE_FAN, pCurrentGroup->fansSizes().data(), GL_UNSIGNED_INT, (const GLvoid**)pCurrentGroup->fansOffset().data(), fansCount);
				m_ExtendedGeomEngine.useIBO(false, GLC_ExtendedGeomEngine::GLC_TrianglesFan);
			}
		}

		++iGroup;
	}
	m_ExtendedGeomEngine.useVBO(false, GLC_ExtendedGeomEngine::GLC_Normal);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////

// Set the current material
GLC_uint GLC_ExtendedMesh::setCurrentMaterial(GLC_Material* pMaterial)
{
	GLC_uint returnId;
	if (NULL == pMaterial)
	{
		returnId= 0; // Default material id is 0
		if (not m_PrimitiveGroups.contains(returnId))
		{
			pMaterial= new GLC_Material();
			// Add the material to the mesh
			addMaterial(pMaterial);
			m_PrimitiveGroups.insert(returnId, new GLC_PrimitiveGroup(pMaterial->id()));
		}
	}
	else
	{
		returnId= pMaterial->id();
		if (not m_PrimitiveGroups.contains(returnId))
		{
			// Add the material to the mesh
			addMaterial(pMaterial);
			m_PrimitiveGroups.insert(returnId, new GLC_PrimitiveGroup(returnId));
		}
	}

	return returnId;
}
