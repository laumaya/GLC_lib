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

//! \file glc_mesh2.cpp implementation of the GLC_Mesh2 class.

#include "glc_mesh2.h"
#include "glc_openglexception.h"
#include "glc_selectionmaterial.h"
#include "glc_state.h"

#include <QtDebug>

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Mesh2::GLC_Mesh2()
:GLC_VboGeom("Mesh", false)
, m_Vertex()
, m_MaterialGroup()
, m_MaterialHash()
, m_NumberOfFaces(0)
, m_IsSelected(false)
, m_ColorPearVertex(false)
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
, m_MaterialHash(meshToCopy.m_MaterialHash)
, m_NumberOfFaces(meshToCopy.m_NumberOfFaces)
, m_IsSelected(false)
, m_ColorPearVertex(meshToCopy.m_ColorPearVertex)
{
	// Copy Vertex and index data if necessary
	if (m_VertexVector.isEmpty())
	{
		m_VertexVector= meshToCopy.getVertexVector();
		m_IndexVector= meshToCopy.getIndexVector();
	}
	//qDebug() << "GLC_Mesh2::GLC_Mesh2" << id();
	// Add this mesh to inner material
	MaterialHash::const_iterator i= m_MaterialHash.begin();
    while (i != m_MaterialHash.constEnd())
    {
        // update inner material use table
        i.value()->addGLC_Geom(this);
        ++i;
    }
    // Copy Material group IBO
	MaterialGroupHash::const_iterator j= meshToCopy.m_MaterialGroup.begin();
    while (j != meshToCopy.m_MaterialGroup.constEnd())
    {
    	IndexList* pIndexList= new IndexList(*j.value());
    	m_MaterialGroup.insert(j.key(), pIndexList);
        ++j;
    }
}


GLC_Mesh2::~GLC_Mesh2(void)
{
	// delete mesh inner material
	{
		MaterialHash::const_iterator i= m_MaterialHash.begin();
	    while (i != m_MaterialHash.constEnd())
	    {
	        // delete the material if necessary
	        i.value()->delGLC_Geom(id());
	        if (i.value()->isUnused()) delete i.value();
	        ++i;
	    }
	}
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

	m_MaterialHash.clear();
	m_Vertex.clear();
	m_MaterialGroup.clear();
}
/////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

//! Return material index if Material is the same than a material already in the mesh
GLC_uint GLC_Mesh2::materialIndex(const GLC_Material& mat) const
{
	int index= 0;
	MaterialHash::const_iterator iEntry= m_MaterialHash.begin();

    while ((iEntry != m_MaterialHash.constEnd()) and !(*(iEntry.value()) == mat))
    {
        ++iEntry;
    }
    if (iEntry != m_MaterialHash.constEnd())
    {
    	index= iEntry.key();
    }
	return index;
}

// return the mesh bounding box
GLC_BoundingBox& GLC_Mesh2::boundingBox(void)
{

	if (NULL == m_pBoundingBox)
	{
		//qDebug() << "GLC_Mesh2::boundingBox create boundingBox";
		m_pBoundingBox= new GLC_BoundingBox();

		if (m_VertexVector.isEmpty())
		{
			qDebug() << "GLC_Mesh2::getBoundingBox empty m_VertexVector";
		}
		else
		{
			const int max= m_VertexVector.size();
			for (int i= 0; i < max; ++i)
			{
				GLC_Vector3d vector(m_VertexVector[i].x, m_VertexVector[i].y, m_VertexVector[i].z);
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

// Return the Vertex Vector
VertexVector GLC_Mesh2::getVertexVector() const
{

	if (0 != m_VboId)
	{
		// VBO created get data from VBO
		const int sizeOfVbo= numberOfVertex();
		const GLsizeiptr dataSize= sizeOfVbo * sizeof(GLC_Vertex);
		VertexVector vertexVector(sizeOfVbo);

		glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
		GLvoid* pVbo = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		memcpy(vertexVector.data(), pVbo, dataSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return vertexVector;
	}
	else
	{
		return m_VertexVector;
	}
}

// Return the Index Vector
QVector<GLuint> GLC_Mesh2::getIndexVector() const
{
	if (0 != m_IboId)
	{
		// IBO created get data from IBO
		const int sizeOfVbo= numberOfVertex();
		const GLsizeiptr indexSize = sizeOfVbo * sizeof(GLuint);
		QVector<GLuint> indexVector(sizeOfVbo);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IboId);
		GLvoid* pIbo = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
		memcpy(indexVector.data(), pIbo, indexSize);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return indexVector;
	}
	else
	{
		return m_IndexVector;
	}

}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Add material to mesh
void GLC_Mesh2::addMaterial(GLC_Material* pMaterial)
{
	if (pMaterial != NULL)
	{
		const GLC_uint materialID= pMaterial->id();
		MaterialHash::const_iterator iMaterial= m_MaterialHash.find(materialID);
		// Check if there is a material at specified index
		Q_ASSERT(iMaterial == m_MaterialHash.end());

		// Add this geometry in the material use table
		pMaterial->addGLC_Geom(this);
		// Add the Material to Material hash table
		m_MaterialHash.insert(materialID, pMaterial);
		// Test if the material is transparent
		if (pMaterial->isTransparent() && (m_MaterialHash.size() == 1))
		{
			setTransparency(true);
		}
		else if (isTransparent() && !pMaterial->isTransparent())
		{
			setTransparency(false);
		}
		// Invalid the geometry
		m_GeometryIsValid = false;
	}
}

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
void GLC_Mesh2::reverseNormal()
{
	const int max= m_VertexVector.size();
	for (int i= 0; i < max; ++i)
	{
		m_VertexVector[i].nx= m_VertexVector[i].nx * -1.0f;
		m_VertexVector[i].ny= m_VertexVector[i].ny * -1.0f;
		m_VertexVector[i].nz= m_VertexVector[i].nz * -1.0f;
	}
	// Invalid the geometry
	m_GeometryIsValid = false;
}

// Specific glExecute method
void GLC_Mesh2::glExecute(bool isSelected, bool forceWire)
{
	m_IsSelected= isSelected;
	GLC_VboGeom::glExecute(isSelected, forceWire);
	m_IsSelected= false;
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
// if the geometry have a texture, load it
void GLC_Mesh2::glLoadTexture(void)
{
	// Load texture of the master material
	m_pMaterial->glLoadTexture();

	MaterialHash::iterator iMaterial= m_MaterialHash.begin();

    while (iMaterial != m_MaterialHash.constEnd())
    {
        // Load texture of mesh materials
        iMaterial.value()->glLoadTexture();
        ++iMaterial;
    }
}

// Virtual interface for OpenGL Geometry set up.
void GLC_Mesh2::glDraw()
{
	Q_ASSERT(m_GeometryIsValid or not m_VertexVector.isEmpty());

	const bool vboIsSupported= GLC_State::vboIsSupported();

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
		m_IndexVector= iboList.toVector();

		if (vboIsSupported)
		{
			// Create VBO
			const GLsizei dataNbr= static_cast<GLsizei>(m_VertexVector.size());
			const GLsizeiptr dataSize= dataNbr * sizeof(GLC_Vertex);
			glBufferData(GL_ARRAY_BUFFER, dataSize, m_VertexVector.data(), GL_STATIC_DRAW);
			m_VertexVector.clear();
			// Create IBO
			const GLsizei indexNbr= static_cast<GLsizei>(iboList.size());
			const GLsizeiptr indexSize = indexNbr * sizeof(GLuint);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, m_IndexVector.data(), GL_STATIC_DRAW);
			m_IndexVector.clear();
		}
	}

	if (vboIsSupported)
	{
		// Use VBO
		glVertexPointer(3, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(0));
		glNormalPointer(GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(12));
		glTexCoordPointer(2, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(24));

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// test if color pear vertex is activated
		if (m_ColorPearVertex and !m_IsSelected)
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
		float* pVertexData= (float *) m_VertexVector.data();
		glVertexPointer(3, GL_FLOAT, sizeof(GLC_Vertex), pVertexData);
		glNormalPointer(GL_FLOAT, sizeof(GLC_Vertex), &pVertexData[3]);
		glTexCoordPointer(2, GL_FLOAT, sizeof(GLC_Vertex), &pVertexData[6]);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// test if color pear vertex is activated
		if (m_ColorPearVertex and !m_IsSelected)
		{
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, sizeof(GLC_Vertex), &pVertexData[8]);
		}
	}

	GLC_Material* pCurrentMaterial;
	GLuint max;
	GLuint cur= 0;
	iMaterialGroup= m_MaterialGroup.begin();
    while (iMaterialGroup != m_MaterialGroup.constEnd())
    {
    	if (!iMaterialGroup.value()->isEmpty())
    	{
    		if ((not GLC_State::selectionShaderUsed() or not m_IsSelected) and not GLC_State::isInSelectionMode())
    		{
        		// Set current material
        		if (iMaterialGroup.key() == 0)
        		{
        			// Use default material
        			pCurrentMaterial= m_pMaterial;
        		}
        		else
        		{
        			pCurrentMaterial= m_MaterialHash.value(iMaterialGroup.key());
        			Q_ASSERT(pCurrentMaterial != NULL);
        		}

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
    		else if(not GLC_State::isInSelectionMode())
    		{
    			// Use Shader
        		glDisable(GL_TEXTURE_2D);
    		}

			max= static_cast<GLuint>(iMaterialGroup.value()->size());
			// Draw Mesh
			if (vboIsSupported)
			{
				// Use VBO
				//glDrawRangeElements(GL_TRIANGLES, 0, max, max, GL_UNSIGNED_INT, BUFFER_OFFSET((cur) * sizeof(unsigned int)));
				glDrawElements(GL_TRIANGLES, max, GL_UNSIGNED_INT, BUFFER_OFFSET((cur) * sizeof(unsigned int)));
			}
			else
			{
				// Use Vertex Array
				glDrawElements(GL_TRIANGLES, max, GL_UNSIGNED_INT, &m_IndexVector.data()[cur]);
			}
			cur+= max;
    	}
    	++iMaterialGroup;
    }
	if (m_ColorPearVertex and !m_IsSelected)
	{
		glDisableClientState(GL_COLOR_ARRAY);
		glDisable(GL_COLOR_MATERIAL);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// OpenGL error handler
	GLenum error= glGetError();
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Mesh2::GlDraw ", error);
		throw(OpenGlException);
	}

}
