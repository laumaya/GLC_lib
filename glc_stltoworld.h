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

//! \file glc_stltoworld.h interface for the GLC_StlToWorld class.

#ifndef GLC_STLTOWORLD_H_
#define GLC_STLTOWORLD_H_

#include <QString>
#include <QObject>
#include <QFile>
#include <QTextStream>

#include "glc_vector3df.h"

class GLC_World;
class QGLContext;
class GLC_Mesh2;

//////////////////////////////////////////////////////////////////////
//! \class GLC_StlToWorld
/*! \brief GLC_StlToWorld : Create an GLC_World from stl file */

/*! An GLC_StlToWorld extract the only mesh from an .stl file \n
 * 	List of elements extracted from the STL
 * 		- Vertex
 * 		- Face
 * 		- Normal coordinate
  */
//////////////////////////////////////////////////////////////////////

class GLC_StlToWorld : public QObject
{
	Q_OBJECT
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	GLC_StlToWorld(const QGLContext*);
	virtual ~GLC_StlToWorld();
//@}
//////////////////////////////////////////////////////////////////////
/*! @name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an GLC_World from an input OBJ File
	GLC_World* CreateWorldFromStl(QFile &file);
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Private services functions */
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! clear stlToWorld allocate memmory
	void clear();
	//! Scan a line previously extracted from STL file
	void scanFacet();
	//! Extract a 3D Vector from a string
	GLC_Vector3df extract3dVect(QString &);
	//! Load Binarie STL File
	void LoadBinariStl(QFile &);
	
	

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
	//! The Stl File name
	QString m_FileName;
	//! OpenGL Context
	const QGLContext* m_pQGLContext;	
	//! The current line number
	int m_CurrentLineNumber;
	//! The Text Stream
	QTextStream m_StlStream;
	//! The current mesh
	GLC_Mesh2* m_pCurrentMesh;
	//! Index of the current vertex
	int m_CurVertexIndex;
	//! Index of the current normal
	int m_CurNormalIndex;
};

#endif /*GLC_STLTOWORLD_H_*/
