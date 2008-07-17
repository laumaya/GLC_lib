/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.9, packaged on May, 2008.

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

//! \file glc_3dstoworld.h interface for the GLC_3dsToWorld class.

#ifndef GLC_3DSTOWORLD_H_
#define GLC_3DSTOWORLD_H_

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QString>
#include <QHash>
#include <QSet>

#include "glc_instance.h"

class GLC_World;
class QGLContext;
class GLC_Mesh2;
class GLC_Product;
class GLC_Material;

struct Lib3dsFile;
struct Lib3dsNode;
struct Lib3dsMesh;
struct Lib3dsMaterial;

//////////////////////////////////////////////////////////////////////
//! \class GLC_3dsToWorld
/*! \brief GLC_3dsToWorld : Create an GLC_World from 3ds file */

/*! An GLC_3dsToWorld extract meshs from an .3ds file \n
 * 	List of elements extracted from the 3ds
 * 		- Vertex
 * 		- Face
 * 		- Normal coordinate
 * 		- Material
 * 		- Meshes
  */
//////////////////////////////////////////////////////////////////////

class GLC_3dsToWorld : public QObject
{
	Q_OBJECT
	
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
	GLC_3dsToWorld(const QGLContext*);
	virtual ~GLC_3dsToWorld();
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an GLC_World from an input 3DS File
	GLC_World* CreateWorldFrom3ds(QFile &file);
//@}
	
//////////////////////////////////////////////////////////////////////
/*! @name Private services functions */
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! clear 3dsToWorld allocate memmory
	void clear();
	//! Create meshes from the 3ds File
	void createMeshes(GLC_Product*, Lib3dsNode*);
	//! Create Instance from a Lib3dsMesh
	GLC_Instance createInstance(Lib3dsMesh*);
	//! Load Material
	void loadMaterial(Lib3dsMaterial*);

//@}

//////////////////////////////////////////////////////////////////////
// Qt Signals
//////////////////////////////////////////////////////////////////////
	signals:
	void currentQuantum(int);

//////////////////////////////////////////////////////////////////////
	/* Private members */
//////////////////////////////////////////////////////////////////////
private:
	//! pointer to a GLC_World
	GLC_World* m_pWorld;
	//! The 3DS File name
	QString m_FileName;
	//! OpenGL Context
	const QGLContext* m_pQGLContext;	
	//! The current mesh
	GLC_Mesh2* m_pCurrentMesh;
	//! Index of the current vertex
	int m_CurVertexIndex;
	//! Index of the current normal
	int m_CurNormalIndex;
	//! The Lib3dsFile Structure
	Lib3dsFile* m_pLib3dsFile;
	//! The GLC_Material Hash Table
	QHash<QString, GLC_Material*> m_Materials;
	//! The Material index Hash Table
	QHash<QString, int> m_MaterialsIndex;
	//! The next material index
	int m_NextMaterialIndex;
	// The Current Material
	GLC_Material* m_pCurrentMaterial;
	// The set of loaded meshes
	QSet<QString> m_LoadedMeshes;


	

};

#endif /*GLC_3DSTOWORLD_H_*/
