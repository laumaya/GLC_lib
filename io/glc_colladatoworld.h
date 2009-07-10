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
//! \file glc_colladatoworld.h interface for the GLC_ColladaToWorld class.

#ifndef GLC_COLLADATOWORLD_H_
#define GLC_COLLADATOWORLD_H_

#include <QObject>
#include <QString>
#include <QFile>
#include <QXmlStreamReader>
#include <QHash>
#include <QColor>

#include "../shading/glc_material.h"

class GLC_World;
class QGLContext;

//////////////////////////////////////////////////////////////////////
//! \class GLC_ColladaToWorld
/*! \brief GLC_ColladaToWorld : Create an GLC_World from dae file */
//////////////////////////////////////////////////////////////////////
class GLC_ColladaToWorld : public QObject
{
private:
	Q_OBJECT

	typedef QHash<const QString, GLC_Material*> MaterialHash;
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_ColladaToWorld(const QGLContext*);

	//! Destructor
	virtual ~GLC_ColladaToWorld();
//@}

//////////////////////////////////////////////////////////////////////
/*! @name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Create an GLC_World from an input Collada File
	GLC_World* CreateWorldFromCollada(QFile &);

//@}

//////////////////////////////////////////////////////////////////////
// Qt Signals
//////////////////////////////////////////////////////////////////////
	signals:
	void currentQuantum(int);

//////////////////////////////////////////////////////////////////////
/*! @name Private services functions */
//@{
//////////////////////////////////////////////////////////////////////
private:
	//! Go to an Element of a xml
	void goToElement(const QString&);

	//! Go to the end Element of a xml
	void goToEndElement(const QString&);

	// Return the content of an element
	QString getContent(const QString&);

	//! Read the specified attribute
	QString readAttribute(const QString&, bool mandatory= false);

	//! Check for XML error
	//! Throw ecxeption if error occur
	void checkForXmlError(const QString&);

	//! Throw an exception with the specified text
	void throwException(const QString&);

	//! Return true if the end of specified element is not reached
	inline bool endElementNotReached(const QString& element)
	{return not m_pStreamReader->atEnd() and not (m_pStreamReader->isEndElement() and (m_pStreamReader->name() == element));}

	//! Return true if the start of specified element is not reached
	inline bool startElementNotReached(const QString& element)
	{return not m_pStreamReader->atEnd() and not (m_pStreamReader->isStartElement() and (m_pStreamReader->name() == element));}

	//! Clear memmory
	void clear();

	//! Load library_images element
	void loadLibraryImage();

	//! Load image element
	void loadImage();

	//! Load library_materials element
	void loadLibraryMaterials();

	//! Load a material
	void loadMaterial();

	//! Load library_effects element
	void loadLibraryEffects();

	//! Load an effect
	void loadEffect();

	//! Load profile_COMMON
	void loadProfileCommon();

	//! Load a new param
	void loadNewParam();

	//! Load a surface
	void loadSurface(const QString&);

	//! Load Sampler 2D
	void loadSampler2D(const QString&);

	//! Load technique
	void loadTechnique();

	//! load phong material
	void loadPhong();

	//! load common color or texture
	void loadCommonColorOrTexture(const QString&);

	//! Read a xml Color
	QColor readXmlColor();

	//! Load library_geometries element
	void loadLibraryGeometries();

	//! Load an geometry element
	void loadGeometry();

	//! Load a mesh
	void loadMesh();

	//! Load Vertex bulk data
	void loadVertexBulkData();

	//! Load library_visual_scenes element
	void loadVisualScenes();

	//! Load scene element
	void loadScene();

	//! Link texture to material
	void linkTexturesToMaterials();


//@}
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! The world to built
	GLC_World* m_pWorld;

	//! OpenGL Context
	const QGLContext* m_pQGLContext;

	//! Xml Reader
	QXmlStreamReader* m_pStreamReader;

	//! The collada fileName
	QString m_FileName;

	//! The collada file
	QFile* m_pFile;

	//! Map image id to image file name
	QHash<QString, QString> m_ImageFileHash;

	//! Map materialInstance to material
	QHash<QString, QString> m_MaterialLibHash;

	//! Map surface sid to image id
	QHash<QString, QString> m_SurfaceImageHash;

	//! Map sampler2D sid to surface sid
	QHash<QString, QString> m_Sampler2DSurfaceHash;

	//! Material Effect hash table
	MaterialHash m_MaterialEffectHash;

	//! The current material
	GLC_Material* m_pCurrentMaterial;

	//! Texture to material link
	MaterialHash m_TextureToMaterialHash;

};

#endif /* GLC_COLLADATOWORLD_H_ */
