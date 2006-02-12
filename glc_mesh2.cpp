/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.4, packaged on January, 2006.

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

//! \file glc_mesh.cpp implementation of the GLC_Mesh2 class.

#include <QtDebug>
#include <assert.h>
#include "glc_mesh2.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Mesh2::GLC_Mesh2(const GLfloat *pColor)
:GLC_Geometry("Mesh", pColor)
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

//! Get material from mesh
void GLC_Mesh2::GetMaterial(int Index, GLC_Material &Material)
{
	MaterialHash::const_iterator iMaterial= m_MaterialHash.find(Index);
	// Check if the key is find
	assert(iMaterial != m_MaterialHash.end());
	
	// Retrieve the material
	Material= m_MaterialHash[Index];

}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////


//! Add material to mesh
void GLC_Mesh2::AddMaterial(int Index, GLC_Material &Material)
{
		
	// Add or replace the Material to Material hash table
	m_MaterialHash.insert(Index, Material);

}

//! Modify material from mesh
void GLC_Mesh2::ModifyMaterial(int Index, GLC_Material &Material)
{
	//! \todo GLC_Material should have copy constructor
	MaterialHash::const_iterator iMaterial= m_MaterialHash.find(Index);
	// Check if the key is find
	assert(iMaterial != m_MaterialHash.end());
	
	// Modify the material
	m_MaterialHash.insert(Index, Material);

}


// Add a vertex to mesh
void GLC_Mesh2::AddCoordinate(int Index, GLC_Vector3d Coordinate)
{
	Vector3dHash::const_iterator iVector= m_CoordinateHash.find(Index);
	// Check if the key is already use
	
	assert(iVector == m_CoordinateHash.end());
	
	// Add the coordinate to coordinate hash table
	m_CoordinateHash.insert(Index, Coordinate);
	
}

// Add Normal
void GLC_Mesh2::AddNormal(int Index, GLC_Vector3d Normal)
{
	Vector3dHash::const_iterator iVector= m_NormalHash.find(Index);
	// Check if the key is already use
	
	assert(iVector == m_NormalHash.end());
	
	// Add the coordinate to coordinate hash table
	m_NormalHash.insert(Index, Normal);
	
}

// Add texture coordinate
void GLC_Mesh2::AddTextureCoordinate(int Index, GLC_Vector2d TextureCoordinate)
{
	Vector2dHash::const_iterator iVector= m_TextCoordinateHash.find(Index);
	
	// Check if the key is already in use	
	assert(iVector == m_TextCoordinateHash.end());
	
	// Add the coordinate to coordinate hash table
	m_TextCoordinateHash.insert(Index, TextureCoordinate);
	
}


//! Add a face without texture coordinate
void GLC_Mesh2::AddFace(const QVector<int> &Material, const QVector<int> &Coordinate, const QVector<int> &Normal)
{
	
	AddMaterialIndex(Material);
	AddCoordAndNormIndex(Coordinate, Normal);
	
	// The testture index should be empty
	//assert(m_TextureIndex.isEmpty());
	
	// Increment number of faces
	m_NumberOfFaces++;
	
}

// Add a face with texture coordinate
void GLC_Mesh2::AddFace(const QVector<int> &Material, const QVector<int> &Coordinate, const QVector<int> &Normal,
						const QVector<int> &TextureCoordinate)
{
	AddMaterialIndex(Material);
	AddCoordAndNormIndex(Coordinate, Normal);
	AddTextureIndex(TextureCoordinate);
		
	// Increment number of faces
	m_NumberOfFaces++;
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Virtual interface for OpenGL Geometry set up.
void GLC_Mesh2::GlDraw()
{
	qDebug() << "GLC_Mesh2::GlDraw ENTER";
	
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
					//qDebug() << "GLC_Mesh2::GlDraw : CurrentMaterialIndex" << CurrentMaterialIndex;
					
					MaterialHash::const_iterator iMaterial= m_MaterialHash.find(CurrentMaterialIndex);
					// Check if the key is already use
					if (iMaterial != m_MaterialHash.end())
					{
						if (m_MaterialHash[CurrentMaterialIndex].GetAddRgbaTexture())
						{
							glEnable(GL_TEXTURE_2D);
							//qDebug() << "GLC_Mesh2::GlDraw : Texture enabled";
						}
						else
						{
							glDisable(GL_TEXTURE_2D);
						}
						m_MaterialHash[CurrentMaterialIndex].GlExecute();
					}
					else
					{
						m_pMaterial->GlExecute();
					}
				}
				IsNewFace= false;
				// New polygon creation
				glBegin(GL_POLYGON);
			}
				// Vertex texture coordinate if necessary
				if (i < m_TextureIndex.size())
				{
					glTexCoord2dv(m_TextCoordinateHash[m_TextureIndex.at(i)].Return_dVect());
				}
					
				// Vertex Normal
				if (i < m_NormalIndex.size())
				{
					glNormal3dv(m_NormalHash[m_NormalIndex.at(i)].Return_dVect());
				}
				else
				{
					qDebug() << "GLC_Mesh2::GlDraw : m_NormalIndex out of bound";
				}
								
				// Vertex 3D coordinate
				if (i < m_CoordinateIndex.size())
				{
					glVertex3dv(m_CoordinateHash[m_CoordinateIndex.at(i)].Return_dVect());
				}
				else
				{
					qDebug() << "GLC_Mesh2::GlDraw : m_CoordinateIndex out of bound";
				}

		}		
	}
}

// Virtual interface for OpenGL Geomtry properties.
void GLC_Mesh2::GlPropGeom()
{
		//! Change the current matrix
		glMultMatrixd(m_MatPos.Return_dMat());
		
		// Polygons display mode
		glPolygonMode(m_PolyFace, m_PolyMode);
		
		if (m_IsBlended)
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
			glLineWidth(GetThickness());	// is thikness
			glColor4fv(GetfRGBA());			// is color
		}
		else if (m_pMaterial->GetAddRgbaTexture())
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			m_pMaterial->GlExecute();
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			m_pMaterial->GlExecute();
		}
		
}

// Add coordinate and normal index of a face
void GLC_Mesh2::AddCoordAndNormIndex(const QVector<int> &Coordinate, const QVector<int> &Normal)
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
void GLC_Mesh2::AddTextureIndex(const QVector<int> &TextureCoordinate)
{
	for (int i= 0; i < TextureCoordinate.size(); ++i)
	{
		m_TextureIndex.append(TextureCoordinate[i]);		
	}
	m_TextureIndex.append(-1); // End of the face's texture coordinate index
	
}

// Add Material index of a face
void GLC_Mesh2::AddMaterialIndex(const QVector<int> &Material)
{
	for (int i= 0; i < Material.size(); ++i)
	{
		m_MaterialIndex.append(Material[i]);		
	}
	m_MaterialIndex.append(-1); // End of the face's material index
}
