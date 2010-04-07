/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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
//! \file glc_worldto3dxml.h interface for the GLC_WorldTo3dxml class.

#ifndef GLC_WORLDTO3DXML_H_
#define GLC_WORLDTO3DXML_H_
#include <QObject>
#include <QXmlStreamWriter>

#include "../sceneGraph/glc_world.h"
#include "../glc_config.h"

class QuaZip;
class QuaZipFile;
class QFile;
class GLC_Mesh;

//////////////////////////////////////////////////////////////////////
//! \class GLC_WorldTo3dxml
/*! \brief GLC_WorldTo3dxml : Export a GLC_World to a 3dxml file */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_WorldTo3dxml : public QObject
{
	Q_OBJECT

public:
	enum ExportType
	{
		Compressed3dxml,
		Exploded3dxml
	};
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	GLC_WorldTo3dxml(const GLC_World& world);
	virtual ~GLC_WorldTo3dxml();
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Save the world to the specified file name
	bool exportToFile(const QString& filename, GLC_WorldTo3dxml::ExportType exportType);

	//! Set the name of the 3dxml generator default is GLC_LIB
	inline void setGeneratorName(const QString& generator)
	{m_Generator= generator;}
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Private services functions */
//@{
//////////////////////////////////////////////////////////////////////
private:

	//! Write 3DXML Header
	void writeHeader();

	//! Write 3DXML reference 3D element
	void writeReference3D(const GLC_StructReference* pRef);

	//! Write 3DXML reference representation
	void writeReferenceRep(const GLC_3DRep* p3DRep);

	//! Write 3DXML instance 3D element
	void writeInstance3D(const GLC_StructInstance* pInstance, unsigned int parentId);

	//! Write 3DXML instance 3D element
	void writeInstanceRep(const GLC_3DRep* p3DRep, unsigned int parentId);

	//! Set the streamwriter to the specified file and return true if success
	void setStreamWriterToFile(const QString& fileName);

	//! Add the manifest to 3DXML compressed file
	void addManifest();

	//! Export the assembly structure from the list of reference
	void exportAssemblyStructure();

	//! Export assembly from the given occurence
	void exportAssemblyFromOccurence(const GLC_StructOccurence* pOccurence);

	//! Return the 3DXML string of the given matrix
	QString matrixString(const GLC_Matrix4x4& matrix);

	//! Write the given 3DRep to 3DXML 3DRep
	void write3DRep(const GLC_3DRep* pRep);

	//! Return the file name of the given 3DRep
	QString representationFileName(const GLC_3DRep* pRep);

	//! Write the given mesh to 3DXML 3DRep
	void writeGeometry(const GLC_Mesh* pMesh);

	//! Write the geometry face from the given lod and material
	void writeGeometryFace(const GLC_Mesh* pMesh, int lod, GLC_uint materialId);

	//! Write surface attributes
	void writeSurfaceAttributes(const GLC_Material* pMaterial);
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
	//! The world to export
	GLC_World m_World;

	//! The file name in which the world is exported
	QString m_FileName;

	//! The Stream writer
	QXmlStreamWriter* m_pOutStream;

	//! QString the 3DXML Generator
	QString m_Generator;

	//! The current 3DXML id
	unsigned int m_CurrentId;

	//! The 3DXML Archive
	QuaZip* m_p3dxmlArchive;

	//! The current quazp file
	QuaZipFile* m_pCurrentZipFile;

	//! The current file
	QFile* m_pCurrentFile;

	//! the 3dxml absolute path
	QString m_AbsolutePath;

	//! Map reference to 3dxml id
	QHash<const GLC_StructReference*, unsigned int> m_ReferenceToIdHash;

	//! Map instance to 3dxml id
	QHash<const GLC_StructInstance*, unsigned int> m_InstanceToIdHash;

	//! Map reference rep to 3dxml id
	QHash<const GLC_3DRep*, unsigned int> m_ReferenceRepToIdHash;

};

#endif /* GLC_WORLDTO3DXML_H_ */
