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

//! \file glc_mesh2.cpp implementation of the GLC_Mesh2 class.

#include "glc_mesh2.h"
#include "../glc_openglexception.h"
#include "../shading/glc_selectionmaterial.h"
#include "../glc_state.h"

#include <QtDebug>

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Mesh2::GLC_Mesh2()
:GLC_VboGeom("Mesh", false)
, m_Vertex()
, m_MaterialGroup()
, m_NumberOfFaces(0)
, m_IsSelected(false)
, m_ColorPearVertex(false)
, m_SimpleGeomEngine()
{
	//qDebug() << "GLC_Mesh2::GLC_Mesh2" << id();
	// Index group with default material
	IndexList* pIndexList= new IndexList;
	m_MaterialGroup.insert(0, pIndexList);
}

GLC_Mesh2::GLC_Mesh2(const GLC_Mesh2 &meshToCopy)
: GLC_VboGeom(meshToCopy)
, m_Vertex(meshToCopy.m_Vertex)
, m_MaterialGroup()
, m_NumberOfFaces(meshToCopy.m_NumberOfFaces)
, m_IsSelected(false)
, m_ColorPearVertex(meshToCopy.m_ColorPearVertex)
, m_SimpleGeomEngine(meshToCopy.m_SimpleGeomEngine)
{
	//qDebug() << "GLC_Mesh2::GLC_Mesh2" << id();

	// Copy inner material hash
	MaterialHash::const_iterator i= m_MaterialHash.begin();
	MaterialHash newMaterialHash;
	QHash<GLC_uint, GLC_uint> materialMap;
    while (i != m_MaterialHash.constEnd())
    {
        // update inner material use table
    	i.value()->delGLC_Geom(id());
    	GLC_Material* pNewMaterial= new GLC_Material(*(i.value()));
    	newMaterialHash.insert(pNewMaterial->id(), pNewMaterial);
    	pNewMaterial->addGLC_Geom(this);
    	materialMap.insert(i.key(), pNewMaterial->id());
        ++i;
    }
    m_MaterialHash= newMaterialHash;

	// Copy Material group IBO
	MaterialGroupHash::const_iterator j= meshToCopy.m_MaterialGroup.begin();
    while (j != meshToCopy.m_MaterialGroup.constEnd())
    {
    	IndexList* pIndexList= new IndexList(*j.value());
    	m_MaterialGroup.insert(materialMap.value(j.key()), pIndexList);
        ++j;
    }
}


GLC_Mesh2::~GLC_Mesh2(void)
{
    // delete mesh inner index material group
	{
		MaterialGroupHash::const_iterator i= m_MaterialGroup.begin();
	    while (i != m_MaterialGroup.constEnd())
	    {
	    	// Delete index vector
	        delete i.value();
	        ++i;
	    }
	}

	m_Vertex.clear();
	m_MaterialGroup.clear();
}
/////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get number of faces
unsigned int GLC_Mesh2::numberOfFaces() const
{
	return m_NumberOfFaces;
}

// Get number of vertex
unsigned int GLC_Mesh2::numberOfVertex() const
{
	return m_NumberOfFaces * 3;
}

// return the mesh bounding box
GLC_BoundingBox& GLC_Mesh2::boundingBox(void)
{

	if (NULL == m_pBoundingBox)
	{
		//qDebug() << "GLC_Mesh2::boundingBox create boundingBox";
		m_pBoundingBox= new GLC_BoundingBox();

		if (m_SimpleGeomEngine.vertexVectorHandle()->isEmpty())
		{
			qDebug() << "GLC_Mesh2::getBoundingBox empty m_VertexVector";
		}
		else
		{
			VertexVector* pVertexVector= m_SimpleGeomEngine.vertexVectorHandle();
			const int max= pVertexVector->size();

			for (int i= 0; i < max; ++i)
			{
				GLC_Vector3d vector((*pVertexVector)[i].x, (*pVertexVector)[i].y, (*pVertexVector)[i].z);
				m_pBoundingBox->combine(vector);
			}
		}

	}
	return *m_pBoundingBox;
}

// Return a copy of the current geometry
GLC_VboGeom* GLC_Mesh2::clone() const
{
	return new GLC_Mesh2(*this);
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Add triangles with the same material to the mesh
void GLC_Mesh2::addTriangles(const VertexList &triangles, GLC_Material* pMaterial)
{
	// test if the material is already in the mesh
	GLC_uint materialID;
	if (NULL != pMaterial)
	{
		materialID= pMaterial->id();
	}
	else
	{
		materialID= 0;
	}
	IndexList* pCurIndexList= NULL;
	if ((materialID == 0) or m_MaterialHash.contains(materialID))
	{
		pCurIndexList= m_MaterialGroup.value(materialID);
	}
	else
	{
		// Test if the mesh have already an equivalent material
		GLC_uint index=  materialIndex(*pMaterial);
		if (0 == index)
		{
			addMaterial(pMaterial);
			pCurIndexList= new IndexList;
			m_MaterialGroup.insert(materialID, pCurIndexList);
		}
		else
		{
			pCurIndexList= m_MaterialGroup.value(index);
		}

	}
	const int startVertexIndex= m_Vertex.size();
	const int delta= triangles.size();
	// Add triangles vertex to the mesh
	m_Vertex+= triangles;

	for (int i= 0; i < delta; ++i)
	{
		pCurIndexList->append(startVertexIndex + static_cast<GLuint>(i));
	}
	// Invalid the geometry
	m_GeometryIsValid = false;
	m_NumberOfFaces+= delta / 3;
}

// Reverse mesh normal
void GLC_Mesh2::reverseNormals()
{
	VertexVector* pVertexVector= m_SimpleGeomEngine.vertexVectorHandle();
	// Copy Vertex and index data if necessary
	if (pVertexVector->isEmpty())
	{
		(*m_SimpleGeomEngine.vertexVectorHandle())= m_SimpleGeomEngine.vertexVector();
		(*m_SimpleGeomEngine.indexVectorHandle())= m_SimpleGeomEngine.indexVector();
	}

	const int max= pVertexVector->size();
	for (int i= 0; i < max; ++i)
	{
		(*pVertexVector)[i].nx= (*pVertexVector)[i].nx * -1.0f;
		(*pVertexVector)[i].ny= (*pVertexVector)[i].ny * -1.0f;
		(*pVertexVector)[i].nz= (*pVertexVector)[i].nz * -1.0f;
	}
	// Invalid the geometry
	m_GeometryIsValid = false;
}

// Specific glExecute method
void GLC_Mesh2::glExecute(bool isSelected, bool transparent)
{
	m_IsSelected= isSelected;
	GLC_VboGeom::glExecute(isSelected, transparent);
	m_IsSelected= false;
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Virtual interface for OpenGL Geometry set up.
void GLC_Mesh2::glDraw(bool transparent)
{
	Q_ASSERT(m_GeometryIsValid or not m_SimpleGeomEngine.vertexVectorHandle()->isEmpty());

	const bool vboIsUsed= GLC_State::vboUsed();
	if (vboIsUsed)
	{
		m_SimpleGeomEngine.createVBOs();
		m_SimpleGeomEngine.useVBOs(true);
	}
	IndexList iboList;
	MaterialGroupHash::iterator iMaterialGroup;

	// Create VBO and IBO
	if (!m_GeometryIsValid)
	{
		// Fill index
		iMaterialGroup= m_MaterialGroup.begin();
	    while (iMaterialGroup != m_MaterialGroup.constEnd())
	    {
	    	if (!iMaterialGroup.value()->isEmpty())
	    	{
	    		iboList+= *(iMaterialGroup.value());
	    	}
	    	++iMaterialGroup;
	    }
		// Set index vector
	    QVector<GLuint>* pIndexVector= m_SimpleGeomEngine.indexVectorHandle();
		(*pIndexVector)= iboList.toVector();

		if (vboIsUsed)
		{
			// Create VBO
			VertexVector* pVertexVector= m_SimpleGeomEngine.vertexVectorHandle();
			const GLsizei dataNbr= static_cast<GLsizei>(pVertexVector->size());
			const GLsizeiptr dataSize= dataNbr * sizeof(GLC_Vertex);
			glBufferData(GL_ARRAY_BUFFER, dataSize, pVertexVector->data(), GL_STATIC_DRAW);
			// Create IBO
			const GLsizei indexNbr= static_cast<GLsizei>(iboList.size());
			const GLsizeiptr indexSize = indexNbr * sizeof(GLuint);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, pIndexVector->data(), GL_STATIC_DRAW);
			m_SimpleGeomEngine.clear();
		}
	}

	if (vboIsUsed)
	{
		// Use VBO
		glVertexPointer(3, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(0));
		glNormalPointer(GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(12));
		glTexCoordPointer(2, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(24));

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// test if color pear vertex is activated
		if (m_ColorPearVertex and not m_IsSelected and not GLC_State::isInSelectionMode())
		{
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(32));
		}
	}
	else
	{
		// Use Vertex Array
		float* pVertexData= (float *) m_SimpleGeomEngine.vertexVectorHandle()->data();
		glVertexPointer(3, GL_FLOAT, sizeof(GLC_Vertex), pVertexData);
		glNormalPointer(GL_FLOAT, sizeof(GLC_Vertex), &pVertexData[3]);
		glTexCoordPointer(2, GL_FLOAT, sizeof(GLC_Vertex), &pVertexData[6]);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// test if color pear vertex is activated
		if (m_ColorPearVertex and not m_IsSelected and not GLC_State::isInSelectionMode())
		{
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, sizeof(GLC_Vertex), &pVertexData[8]);
		}
	}

	GLC_Material* pCurrentMaterial= NULL;
	GLuint max;
	GLuint cur= 0;
	iMaterialGroup= m_MaterialGroup.begin();
    while (iMaterialGroup != m_MaterialGroup.constEnd())
    {
    	if (not iMaterialGroup.value()->isEmpty())
    	{
    		if ((not GLC_State::selectionShaderUsed() or not m_IsSelected) and not GLC_State::isInSelectionMode())
    		{
        		// Set current material
        		if (iMaterialGroup.key() == 0)
        		{
        			// Use default material
        			pCurrentMaterial= firstMaterial();
        		}
        		else
        		{
        			pCurrentMaterial= m_MaterialHash.value(iMaterialGroup.key());
        			Q_ASSERT(pCurrentMaterial != NULL);
        		}

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

			max= static_cast<GLuint>(iMaterialGroup.value()->size());
    		if (m_IsSelected or GLC_State::isInSelectionMode() or (pCurrentMaterial->isTransparent() == transparent))
    		{
    			// Draw Mesh
    			if (vboIsUsed)
    			{
    				// Use VBO
    				//glDrawRangeElements(GL_TRIANGLES, 0, max, max, GL_UNSIGNED_INT, BUFFER_OFFSET((cur) * sizeof(unsigned int)));
    				glDrawElements(GL_TRIANGLES, max, GL_UNSIGNED_INT, BUFFER_OFFSET((cur) * sizeof(unsigned int)));
    			}
    			else
    			{
    				// Use Vertex Array
    				glDrawElements(GL_TRIANGLES, max, GL_UNSIGNED_INT, &m_SimpleGeomEngine.indexVectorHandle()->data()[cur]);
    			}
    		}

			cur+= max;
    	}
    	++iMaterialGroup;
    }
	if (m_ColorPearVertex and not m_IsSelected and not GLC_State::isInSelectionMode())
	{
		glDisableClientState(GL_COLOR_ARRAY);
		glDisable(GL_COLOR_MATERIAL);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (vboIsUsed)
	{
		m_SimpleGeomEngine.useVBOs(false);
	}

	// OpenGL error handler
	GLenum error= glGetError();
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Mesh2::GlDraw ", error);
		throw(OpenGlException);
	}

}
