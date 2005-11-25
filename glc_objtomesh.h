/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9, packaged on Novemeber, 2005.

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

//! \file glc_objToMesh.h interface for the GLC_ObjToMesh class.

#ifndef GLC_OBJ_TO_MESH_H_
#define GLC_OBJ_TO_MESH_H_

#include "glc_mesh.h"
#include <string>



#define GLC_OBJ_LIGNE_LENGHT 100

//////////////////////////////////////////////////////////////////////
//! \class GLC_ObjToMesh
/*! \brief GLC_ObjToMesh : Create an GLC_Mesh from obj file */

/*! An GLC_ObjToMesh extract a single mesh from an .obj file \n
 * 	List of elements extracted from the OBJ
 * 		- Vertex
 * 		- Face
 * 		- Texture coordinate
 * 		- Normal coordinate
  */
//////////////////////////////////////////////////////////////////////

class GLC_ObjToMesh
{
//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	/*! Create an empty ObjToMesh object */
	GLC_ObjToMesh(void);
	
/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
public:
	//! Create an GLC_Mesh from an input OBJ File
	GLC_Mesh* CreateMeshFromObj(std::string sFile);

//////////////////////////////////////////////////////////////////////
// Private services functions
//////////////////////////////////////////////////////////////////////
private:
	//! Scan a line previously extracted from OBJ file
	void ScanLigne(const char* c_strLigne);

	//! Extract a Vector from a string
	GLC_Vector4d ExtractVect(const char* c_strLigne);

	//! Extract a face from a string
	GLC_Face* ExtractFace(const char* c_strLigne);

	//! Extract a vertex from a string
	GLC_Vertex* ExtractVertex(std::string sLigne);


//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////

private:
	//! pointer to a GLC_Mesh
	GLC_Mesh* m_pMesh;
	
//! @name Array of Vertex Vector
//@{
	//! Pointer to a Vector
	GLC_Vector4d* m_pArrayPos;
	//! Number of Vertex Vector to extract
	int m_nNbrVectPos;
	//! Index of current vertex vector
	int m_nCurVectPos;
//@} End of Array of Vertex Vector

//! @name Array of Nomal Vector
//@{
	//! Pointer to a Vector
	GLC_Vector4d* m_pArrayNorm;
	//! Number of normal vector to extract
	int m_nNbrVectNorm;
	//! Index of current normal vector
	int m_nCurVectNorm;
//@} End of array of normal vector

//! @name Array of texture coordinate vector
//@{
	//! Pointer to a Vector
	GLC_Vector4d* m_pArrayTexture;
	//! Number of Texture coordinate vector to extract
	int m_nNbrVectTexture;
	//! Index of current Texture coordinate vector
	int m_nCurVectTexture;
//@} End of array of texture coordinate vector

	//! OBJ File name
	std::string m_sFile;

};
#endif //GLC_OBJ_TO_MESH_H_
