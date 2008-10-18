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
	//qDebug() << "GLC_Mesh2::GLC_Mesh2" << getID();
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
	//qDebug() << "GLC_Mesh2::GLC_Mesh2" << getID();
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
	        i.value()->delGLC_Geom(getID());
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
GLC_BoundingBox* GLC_Mesh2::getBoundingBox(void) const
{
	GLC_BoundingBox* pBoundingBox= new GLC_BoundingBox();
	
	const int max= m_Vertex.size();
	for (int i= 0; i < max; ++i)
	{
		GLC_Vector3d vector(m_Vertex[i].x, m_Vertex[i].y, m_Vertex[i].z);
		pBoundingBox->combine(vector);
	}
    
	return pBoundingBox;
}

// Return a copy of the current geometry
GLC_VboGeom* GLC_Mesh2::clone() const
{
	return new GLC_Mesh2(*this);
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Add material to mesh
void GLC_Mesh2::addMaterial(GLC_Material* pMaterial)
{
	if (pMaterial != NULL)
	{
		const GLC_uint materialID= pMaterial->getID(); 
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
		materialID= pMaterial->getID();
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
		addMaterial(pMaterial);
		pCurIndexList= new IndexList;
		m_MaterialGroup.insert(materialID, pCurIndexList);
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
	const int max= m_Vertex.size();
	for (int i= 0; i < max; ++i)
	{
		m_Vertex[i].nx= m_Vertex[i].nx * -1.0f;
		m_Vertex[i].ny= m_Vertex[i].ny * -1.0f;
		m_Vertex[i].nz= m_Vertex[i].nz * -1.0f;
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
	IndexList iboList;
	MaterialGroupHash::iterator iMaterialGroup;

	// Create VBO and IBO
	if (!m_GeometryIsValid)
	{
		// Create VBO
		const GLsizei dataNbr= static_cast<GLsizei>(m_Vertex.size());
		const GLsizeiptr dataSize= dataNbr * sizeof(GLC_Vertex);
		QVector<GLC_Vertex> vectorVertex(m_Vertex.toVector());
		const GLC_Vertex* pPositionData= vectorVertex.data();
		glBufferData(GL_ARRAY_BUFFER, dataSize, pPositionData, GL_STATIC_DRAW);
		
		// Create IBO
		iMaterialGroup= m_MaterialGroup.begin();
	    while (iMaterialGroup != m_MaterialGroup.constEnd())
	    {
	    	if (!iMaterialGroup.value()->isEmpty())
	    	{
	    		iboList+= *(iMaterialGroup.value());
	    	}
	    	++iMaterialGroup;
	    }
		const GLsizei indexNbr= static_cast<GLsizei>(iboList.size());
		const GLsizeiptr indexSize = indexNbr * sizeof(GLuint);
		QVector<GLuint> vectorIndex(iboList.toVector());
		const GLuint* pIndexData= vectorIndex.data();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, pIndexData, GL_STATIC_DRAW);	    	
	}
	
	glVertexPointer(3, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(12));
	glTexCoordPointer(2, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(24));
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// test if color pear vertex is acivated
	if (m_ColorPearVertex and !m_IsSelected)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, sizeof(GLC_Vertex), BUFFER_OFFSET(32));
	}
	GLC_Material* pCurrentMaterial;
	GLuint max;
	GLuint cur= 0;
	iMaterialGroup= m_MaterialGroup.begin();
    while (iMaterialGroup != m_MaterialGroup.constEnd())
    {
    	if (!iMaterialGroup.value()->isEmpty())
    	{
    		if (not GLC_State::selectionShaderUsed() or not m_IsSelected)
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
    		else
    		{
    			// Use Shader
        		glDisable(GL_TEXTURE_2D);
    		}
    		
			max= static_cast<GLuint>(iMaterialGroup.value()->size());
			// Draw cylinder 
			glDrawRangeElements(GL_TRIANGLES, 0, max, max, GL_UNSIGNED_INT, BUFFER_OFFSET((cur) * sizeof(unsigned int)));
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
