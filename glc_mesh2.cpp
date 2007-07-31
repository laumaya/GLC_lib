/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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

#include <assert.h>
#include "glc_mesh2.h"
#include "glc_openglexception.h"
#include "glc_selectionmaterial.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Mesh2::GLC_Mesh2(const QColor& color)
:GLC_Geometry("Mesh", color)
, m_NumberOfFaces(0)
{

}

GLC_Mesh2::GLC_Mesh2(const GLC_Mesh2 &meshToCopy)
: GLC_Geometry(meshToCopy)
, m_CoordinateHash(meshToCopy.m_CoordinateHash)
, m_CoordinateIndex(meshToCopy.m_CoordinateIndex)
, m_MaterialHash(meshToCopy.m_MaterialHash)
, m_MaterialIndex(meshToCopy.m_MaterialIndex)
, m_NormalHash(meshToCopy.m_NormalHash)
, m_NormalIndex(meshToCopy.m_NormalIndex)
, m_TextCoordinateHash(meshToCopy.m_TextCoordinateHash)
, m_TextureIndex(meshToCopy.m_TextureIndex)

, m_NumberOfFaces(meshToCopy.m_NumberOfFaces)
{
	
}


GLC_Mesh2::~GLC_Mesh2(void)
{
	m_CoordinateHash.clear();
	m_CoordinateIndex.clear();

	m_NormalHash.clear();
	m_NormalIndex.clear();

	m_TextCoordinateHash.clear();
	m_TextureIndex.clear();

	m_MaterialHash.clear();
	m_MaterialIndex.clear();
}
/////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get material from mesh
void GLC_Mesh2::getOneMaterial(int Index, GLC_Material &Material)
{
	MaterialHash::const_iterator iMaterial= m_MaterialHash.find(Index);
	// Check if the key is find
	assert(iMaterial != m_MaterialHash.end());
	
	// Retrieve the material
	Material= m_MaterialHash[Index];

}
// return the mesh bounding box
GLC_BoundingBox* GLC_Mesh2::getBoundingBox(void) const
{
	GLC_BoundingBox* pBoundingBox= new GLC_BoundingBox();
	
	Vector3dHash::const_iterator iEntry= m_CoordinateHash.begin();
	
    while (iEntry != m_CoordinateHash.constEnd())
    {
        // Combine the vertex with the bounding box        
        pBoundingBox->combine(iEntry.value());
        ++iEntry;
    }
    
    pBoundingBox->transform(m_MatPos);	
	return pBoundingBox;	
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////


// Add material to mesh
void GLC_Mesh2::addMaterial(int Index, GLC_Material &Material)
{
		
	// Add or replace the Material to Material hash table
	m_MaterialHash.insert(Index, Material);

}

// Modify material from mesh
void GLC_Mesh2::modifyMaterial(int Index, GLC_Material &Material)
{
	MaterialHash::const_iterator iMaterial= m_MaterialHash.find(Index);
	// Check if the key is find
	assert(iMaterial != m_MaterialHash.end());
	
	// Modify the material
	m_MaterialHash.insert(Index, Material);

}


// Add a vertex to mesh
void GLC_Mesh2::addVertex(int Index, GLC_Vector3d Coordinate)
{
	Vector3dHash::const_iterator iVector= m_CoordinateHash.find(Index);
	// Check if the key is already use
	
	assert(iVector == m_CoordinateHash.end());
	
	// Add the coordinate to coordinate hash table
	m_CoordinateHash.insert(Index, Coordinate);
	
}

// Add Normal
void GLC_Mesh2::addNormal(int Index, GLC_Vector3d Normal)
{
	Vector3dHash::const_iterator iVector= m_NormalHash.find(Index);
	// Check if the key is already use
	
	assert(iVector == m_NormalHash.end());
	
	// Add the coordinate to coordinate hash table
	m_NormalHash.insert(Index, Normal);
	
}

// Add texture coordinate
void GLC_Mesh2::addTextureCoordinate(int Index, GLC_Vector2d TextureCoordinate)
{
	Vector2dHash::const_iterator iVector= m_TextCoordinateHash.find(Index);
	
	// Check if the key is already in use	
	assert(iVector == m_TextCoordinateHash.end());
	
	// Add the coordinate to coordinate hash table
	m_TextCoordinateHash.insert(Index, TextureCoordinate);
	
}


//! Add a face without texture coordinate
void GLC_Mesh2::addFace(const QVector<int> &Material, const QVector<int> &Coordinate, const QVector<int> &Normal)
{
	
	addMaterialIndex(Material);
	addCoordAndNormIndex(Coordinate, Normal);
	
	// The testture index should be empty
	//assert(m_TextureIndex.isEmpty());
	
	// Increment number of faces
	m_NumberOfFaces++;
	
}

// Add a face with texture coordinate
void GLC_Mesh2::addFace(const QVector<int> &Material, const QVector<int> &Coordinate, const QVector<int> &Normal,
						const QVector<int> &TextureCoordinate)
{
	addMaterialIndex(Material);
	addCoordAndNormIndex(Coordinate, Normal);
	addTextureIndex(TextureCoordinate);
		
	// Increment number of faces
	m_NumberOfFaces++;
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Virtual interface for OpenGL Geometry set up.
void GLC_Mesh2::glDraw()
{
	//qDebug() << "GLC_Mesh2::glDraw ENTER";
	
	// If the mesh is empty there is noting to do
	if (m_CoordinateIndex.isEmpty()) return;
		
	// Index face separator
	const int separator= -1;
	
	int CurrentMaterialIndex= -1;
	bool IsNewFace= true;
	
	for(int i=0; i < m_CoordinateIndex.size(); ++i)
	{
		if (m_CoordinateIndex.at(i) == separator)
		{			
			assert((i != 0) && (!IsNewFace));	// the first index couldn't be a separator
			// End of current face
			glEnd();
			// At the next round a new face will be create
			IsNewFace=true;
		}
		else
		{
			if (IsNewFace)
			{
				if (CurrentMaterialIndex != m_MaterialIndex.at(i))
				{	// If the material change, make it current
					CurrentMaterialIndex= m_MaterialIndex.at(i);
					//qDebug() << "GLC_Mesh2::glDraw : CurrentMaterialIndex" << CurrentMaterialIndex;
					
					MaterialHash::const_iterator iMaterial= m_MaterialHash.find(CurrentMaterialIndex);
					// Check if the key is already use
					if (iMaterial != m_MaterialHash.end() && !m_IsSelected)
					{
						if (m_MaterialHash[CurrentMaterialIndex].getAddRgbaTexture())
						{
							glEnable(GL_TEXTURE_2D);
							//qDebug() << "GLC_Mesh2::glDraw : Texture enabled";
						}
						else
						{
							glDisable(GL_TEXTURE_2D);
						}
						m_MaterialHash[CurrentMaterialIndex].glExecute();
					}
					else if (m_IsSelected)
					{
						GLC_SelectionMaterial::glExecute();
					}
					else
					{
						m_pMaterial->glExecute();
					}
				}
				IsNewFace= false;
				// New polygon creation
				glBegin(GL_POLYGON);
			} // end if isNewFace
			
			// Vertex texture coordinate if necessary
			if (i < m_TextureIndex.size())
			{
				glTexCoord2dv(m_TextCoordinateHash[m_TextureIndex.at(i)].return_dVect());
			}
				
			// Vertex Normal
			assert(i < m_NormalIndex.size());
			glNormal3dv(m_NormalHash[m_NormalIndex.at(i)].return_dVect());
			
			// Vertex 3D coordinate
			assert(i < m_CoordinateIndex.size());
			glVertex3dv(m_CoordinateHash[m_CoordinateIndex.at(i)].return_dVect());
		}		
	}
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Mesh2::GlDraw ", error);
		throw(OpenGlException);
	}
	
}

// Virtual interface for OpenGL Geometry properties.
void GLC_Mesh2::glPropGeom()
{
	//! Change the current matrix
	glMultMatrixd(m_MatPos.return_dMat());
	
	// Polygons display mode
	glPolygonMode(m_PolyFace, m_PolyMode);
	if (m_IsBlended && !m_IsSelected)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
	}
	else
	{
		glDisable(GL_BLEND);
	}		
	
	if(!m_pMaterial || (m_PolyMode != GL_FILL))
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glLineWidth(getThickness());	// is thikness
		
		if (m_IsSelected) GLC_SelectionMaterial::glExecute();
		else glColor4d(getdRed(), getdGreen(), getdBlue(), getdAlpha());			// is color
	}
	else if (m_pMaterial->getAddRgbaTexture() && !m_IsSelected)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		m_pMaterial->glExecute();
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		if (m_IsSelected) GLC_SelectionMaterial::glExecute();
		else m_pMaterial->glExecute();
	}
		
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_Mesh2::GlPropGeom ", error);
		throw(OpenGlException);
	}
		
}

// Add coordinate and normal index of a face
void GLC_Mesh2::addCoordAndNormIndex(const QVector<int> &Coordinate, const QVector<int> &Normal)
{	
	// Number of coordinate to add
	const int size= Coordinate.size();
	
	// Add Coordinate index of the face
	for (int i= 0; i < size; ++i)
	{
		m_CoordinateIndex.append(Coordinate[i]);		
	}
	m_CoordinateIndex.append(-1); // End of the face's coordinate index

	// Add Normal index of the face
	for (int i= 0; i < size; ++i)
	{
		m_NormalIndex.append(Normal[i]);		
	}
	m_NormalIndex.append(-1); // End of the face's normal index
	
	// Check if indexed list have the same size
	assert(m_NormalIndex.size() == m_CoordinateIndex.size());
	
}

// Add Texture coordinate index of a face
void GLC_Mesh2::addTextureIndex(const QVector<int> &TextureCoordinate)
{
	for (int i= 0; i < TextureCoordinate.size(); ++i)
	{
		m_TextureIndex.append(TextureCoordinate[i]);		
	}
	m_TextureIndex.append(-1); // End of the face's texture coordinate index
	
}

// Add Material index of a face
void GLC_Mesh2::addMaterialIndex(const QVector<int> &Material)
{
	for (int i= 0; i < Material.size(); ++i)
	{
		m_MaterialIndex.append(Material[i]);		
	}
	m_MaterialIndex.append(-1); // End of the face's material index
}
