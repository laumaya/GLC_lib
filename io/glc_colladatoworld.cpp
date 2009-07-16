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

#include "glc_colladatoworld.h"
#include "../sceneGraph/glc_world.h"
#include "../glc_fileformatexception.h"


// Default constructor
GLC_ColladaToWorld::GLC_ColladaToWorld(const QGLContext* pContext)
: QObject()
, m_pWorld(NULL)
, m_pQGLContext(pContext)
, m_pStreamReader(NULL)
, m_FileName()
, m_pFile()
, m_ImageFileHash()
, m_MaterialLibHash()
, m_SurfaceImageHash()
, m_MaterialEffectHash()
, m_pCurrentMaterial(NULL)
, m_TextureToMaterialHash()
, m_BulkDataHash()
, m_VerticesSourceHash()
, m_pMeshInfo(NULL)
{

}

// Destructor
GLC_ColladaToWorld::~GLC_ColladaToWorld()
{
	// Normal ends, wold has not to be deleted
	m_pWorld= NULL;
	clear();
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Create an GLC_World from an input Collada File
GLC_World* GLC_ColladaToWorld::CreateWorldFromCollada(QFile &file)
{
	m_pWorld= NULL;
	m_FileName= file.fileName();
	m_pFile= &file;

	//////////////////////////////////////////////////////////////////
	// Test if the file exist and can be opened
	//////////////////////////////////////////////////////////////////
	if (!m_pFile->open(QIODevice::ReadOnly))
	{
		QString message(QString("GLC_ColladaToWorld::CreateWorldFromCollada File ") + m_FileName + QString(" doesn't exist"));
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::FileNotFound);
		throw(fileFormatException);
	}
	m_pStreamReader= new QXmlStreamReader(m_pFile);

	// Go to the collada root Element
	goToElement("COLLADA");

	// Read the collada version
	QString version= readAttribute("version", true);

	while (endElementNotReached("COLLADA"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "library_images") loadLibraryImage();
			else if (currentElementName == "library_materials") loadLibraryMaterials();
			else if (currentElementName == "library_effects") loadLibraryEffects();
			else if (currentElementName == "library_geometries") loadLibraryGeometries();
			else if (currentElementName == "library_visual_scenes") loadVisualScenes();
			else if (currentElementName == "scene") loadScene();
		}

		m_pStreamReader->readNext();
	}
	// Link the textures to materials
	linkTexturesToMaterials();


	return m_pWorld;
}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
// Go to Element
void GLC_ColladaToWorld::goToElement(const QString& elementName)
{
	while(startElementNotReached(elementName))
	{
		m_pStreamReader->readNext();
	}
	checkForXmlError(QString("Element ") + elementName + QString(" Not Found"));
}

// Go to the end Element of a xml
void GLC_ColladaToWorld::goToEndElement(const QString& elementName)
{
	while(endElementNotReached(elementName))
	{
		m_pStreamReader->readNext();
	}
	checkForXmlError(QString("End Element ") + elementName + QString(" Not Found"));
}

// Return the content of an element
QString GLC_ColladaToWorld::getContent(const QString& element)
{
	QString Content;
	while(endElementNotReached(element))
	{
		m_pStreamReader->readNext();
		if (m_pStreamReader->isCharacters() and not m_pStreamReader->text().isEmpty())
		{
			Content+= m_pStreamReader->text().toString();
		}
	}

	return Content.trimmed();
}

// Read the specified attribute
QString GLC_ColladaToWorld::readAttribute(const QString& name, bool mandatory)
{
	QString attributeValue;
	if (mandatory and not m_pStreamReader->attributes().hasAttribute(name))
	{
		QString message(QString("Mandatory attribute ") + name + QString(" Not found"));
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}
	else
	{
		attributeValue= m_pStreamReader->attributes().value(name).toString();
	}
	return attributeValue;
}

// Check for XML error
void GLC_ColladaToWorld::checkForXmlError(const QString& info)
{
	if (m_pStreamReader->atEnd() or m_pStreamReader->hasError())
	{
		qDebug() << info << " " << m_FileName;
		GLC_FileFormatException fileFormatException(info, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}
}
// Throw an exception with the specified text
void GLC_ColladaToWorld::throwException(const QString& message)
{
	qDebug() << message;
	GLC_FileFormatException fileFormatException(message, m_FileName, GLC_FileFormatException::WrongFileFormat);
	clear();
	throw(fileFormatException);
}

// Clear memmory
void GLC_ColladaToWorld::clear()
{
	delete m_pWorld;
	m_pWorld= NULL;

	delete m_pStreamReader;
	m_pStreamReader= NULL;

	if (NULL != m_pFile) m_pFile->close();
	m_pFile= NULL;

	m_ImageFileHash.clear();
	m_MaterialLibHash.clear();
	m_SurfaceImageHash.clear();

	MaterialHash::iterator iMat= m_MaterialEffectHash.begin();
	while (iMat != m_MaterialEffectHash.constEnd())
	{
		if (iMat.value()->isUnused()) delete iMat.value();
		++iMat;
	}
	m_MaterialEffectHash.clear();

	delete m_pCurrentMaterial;
	m_pCurrentMaterial= NULL;

	m_TextureToMaterialHash.clear();

	m_BulkDataHash.clear();

	m_VerticesSourceHash.clear();

	delete m_pMeshInfo;
	m_pMeshInfo= NULL;
}

// Load library_images element
void GLC_ColladaToWorld::loadLibraryImage()
{
	while (endElementNotReached("library_images"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "image") loadImage();
		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : library_images");
}

// Load image element
void GLC_ColladaToWorld::loadImage()
{
	// load image id
	const QString id= readAttribute("id", true);
	QString fileName;
	// Trying to find external image fileName
	while (endElementNotReached("image"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "init_from")
			{
				fileName= getContent("init_from");
			}
		}
		m_pStreamReader->readNext();
	}

	checkForXmlError("Error occur while loading element : image");

	// Add the image in the image fileName Hash table
	if (not fileName.isEmpty())
	{
		m_ImageFileHash.insert(id, fileName);
	}
}

// Load library_materials element
void GLC_ColladaToWorld::loadLibraryMaterials()
{
	while (endElementNotReached("library_materials"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "material") loadMaterial();
		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : library_materials");

}

// Load a material
void GLC_ColladaToWorld::loadMaterial()
{
	// load material id
	const QString id= readAttribute("id", true);

	goToElement("instance_effect");

	// Load instance effect url
	const QString url= readAttribute("url", true);

	// Read instance effect parameters
	while (endElementNotReached("instance_effect"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "setparam")
			{
				qDebug() << "GLC_ColladaToWorld::loadMaterial : setparam found";
			}
		}
		m_pStreamReader->readNext();
	}

	checkForXmlError("Error occur while loading element : material");

	// Add the image in the image fileName Hash table
	if (not url.isEmpty())
	{
		m_MaterialLibHash.insert(id, url);
	}

}

// Load library_effects element
void GLC_ColladaToWorld::loadLibraryEffects()
{
	while (endElementNotReached("library_effects"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "effect") loadEffect();
		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : library_effects");

}

// Load an effect
void GLC_ColladaToWorld::loadEffect()
{
	// load effect id
	const QString id= readAttribute("id", true);

	delete m_pCurrentMaterial;
	m_pCurrentMaterial= new GLC_Material();
	m_pCurrentMaterial->setName(id);

	while (endElementNotReached("effect"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "profile_COMMON") loadProfileCommon();
		}
		m_pStreamReader->readNext();
	}

	checkForXmlError("Error occur while loading element : effect");


}

// Load profile_COMMON
void GLC_ColladaToWorld::loadProfileCommon()
{
	while (endElementNotReached("profile_COMMON"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "image") loadImage();
			else if (currentElementName == "newparam") loadNewParam();
			else if (currentElementName == "technique") loadTechnique();
		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : profile_COMMON");
}

// Load a new param of the common profile
void GLC_ColladaToWorld::loadNewParam()
{
	// load param sid
	const QString sid= readAttribute("sid", true);
	while (endElementNotReached("newparam"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "surface") loadSurface(sid);
			else if (currentElementName == "sampler2D") loadSampler2D(sid);
		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : profile_COMMON");
}

// Load a surface
void GLC_ColladaToWorld::loadSurface(const QString& sid)
{
	while (endElementNotReached("surface"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "init_from")
			{
				const QString imageId= getContent("init_from");
				m_SurfaceImageHash.insert(sid, imageId);
			}
		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : surface");
}

// Load Sampler 2D
void GLC_ColladaToWorld::loadSampler2D(const QString& sid)
{
	while (endElementNotReached("sampler2D"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "source")
			{
				const QString surfaceSid= getContent("source");
				m_Sampler2DSurfaceHash.insert(sid, surfaceSid);
			}
		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : sampler2D");
}

// Load technique
void GLC_ColladaToWorld::loadTechnique()
{
	while (endElementNotReached("technique"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "phong") loadPhong();
		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : technique");
}

// load phong material
void GLC_ColladaToWorld::loadPhong()
{
	while (endElementNotReached("phong"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if ((currentElementName == "emission")
					or (currentElementName == "ambient")
					or (currentElementName == "diffuse")
					or(currentElementName == "specular"))
				loadCommonColorOrTexture(currentElementName.toString());

		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : phong");
}

// load common color or texture
void GLC_ColladaToWorld::loadCommonColorOrTexture(const QString& name)
{
	Q_ASSERT(NULL != m_pCurrentMaterial);

	while (endElementNotReached(name))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "color")
			{
				if (name == "emission") m_pCurrentMaterial->setLightEmission(readXmlColor());
				else if (name == "ambient") m_pCurrentMaterial->setAmbientColor(readXmlColor());
				else if (name == "diffuse") m_pCurrentMaterial->setDiffuseColor(readXmlColor());
				else if (name == "specular") m_pCurrentMaterial->setSpecularColor(readXmlColor());
			}
			else if (currentElementName == "texture")
			{
				const QString sid = readAttribute("texture", true);
				m_TextureToMaterialHash.insert(sid, m_pCurrentMaterial);
			}
		}
		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : " + name);
}

// Read a xml Color
QColor GLC_ColladaToWorld::readXmlColor()
{
	QColor resultColor;

	QString colorString= getContent("color");
	QStringList colors= colorString.split(' ');
	if(colors.size() == 4)
	{
		bool okRed, okGreen, okBlue, okAlpha;
		const float red= colors.at(0).toFloat(&okRed);
		const float green= colors.at(1).toFloat(&okGreen);
		const float blue= colors.at(2).toFloat(&okBlue);
		const float alpha= colors.at(3).toFloat(&okAlpha);
		if (okRed and okGreen and okBlue and okAlpha)
		{
			resultColor.setRedF(red);
			resultColor.setGreenF(green);
			resultColor.setBlueF(blue);
			resultColor.setAlphaF(alpha);
		}
		else
		{
			QString info= "Error occur while reading xml color : " + colorString;
			qDebug() << info << " " << m_FileName;
			GLC_FileFormatException fileFormatException(info, m_FileName, GLC_FileFormatException::WrongFileFormat);
			clear();
			throw(fileFormatException);
		}
	}
	else
	{
		QString info= "Error occur while reading xml color : " + colorString;
		qDebug() << info << " " << m_FileName;
		GLC_FileFormatException fileFormatException(info, m_FileName, GLC_FileFormatException::WrongFileFormat);
		clear();
		throw(fileFormatException);
	}

	return resultColor;
}

// Load library_geometries element
void GLC_ColladaToWorld::loadLibraryGeometries()
{
	while (endElementNotReached("library_geometries"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "geometry") loadGeometry();
		}

		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : library_geometries");
}

// Load an geometry element
void GLC_ColladaToWorld::loadGeometry()
{
	delete m_pMeshInfo;
	m_pMeshInfo= new MeshInfo();
	m_pMeshInfo->m_pMesh= new GLC_ExtendedMesh;

	const QString name= readAttribute("name", false);
	if (not name.isEmpty())
	{
		m_pMeshInfo->m_pMesh->setName(name);
	}

	while (endElementNotReached("geometry"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "mesh") loadMesh();
		}

		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : geometry");
}

// Load a mesh
void GLC_ColladaToWorld::loadMesh()
{
	while (endElementNotReached("mesh"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if (currentElementName == "source") loadVertexBulkData();
			else if (currentElementName == "vertices") loadVertices();
			else if (currentElementName == "polylist") loadPolylist();
			//else if (currentElementName == "triangles") loadTriangles();
			//else if (currentElementName == "trifans") loadTriFans();
			//else if (currentElementName == "tristrips") loadTriStrip();
		}

		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : mesh");
}

// Load Vertex bulk data
void GLC_ColladaToWorld::loadVertexBulkData()
{
	qDebug() << "GLC_ColladaToWorld::loadVertexBulkData()";
	// load Vertex Bulk data id
	const QString id= readAttribute("id", true);
	qDebug() << "id=" << id;
	QList<float> vertices;

	while (endElementNotReached("source"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if ((currentElementName == "float_array"))
			{
				int count= readAttribute("count", true).toInt();
				QString array= getContent("float_array");
				QStringList list= array.split(' ');
				// Check the array size
				if (count != list.size()) throwException("float_array size not match");

				for (int i= 0; i < count; ++i)
				{
					vertices.append(list.at(i).toFloat());
				}
			}
			else if (currentElementName == "technique_common")
			{

			}
		}

		m_pStreamReader->readNext();
	}
	checkForXmlError("Error occur while loading element : source");
	m_BulkDataHash.insert(id, vertices);
}

// Load attributes and identity of mesh vertices
void GLC_ColladaToWorld::loadVertices()
{
	qDebug() << "GLC_ColladaToWorld::loadVertices()";
	// load Vertices id
	const QString id= readAttribute("id", true);

	goToElement("input");
	const QString source= readAttribute("source", true).remove('#');
	m_VerticesSourceHash.insert(id, source);
	checkForXmlError("Error occur while loading element : vertices");
}

// Load polygons or polylist
void GLC_ColladaToWorld::loadPolylist()
{
	qDebug() << "GLC_ColladaToWorld::loadPolylist()";
	// The number of polygon
	const int polygonCount= readAttribute("count", true).toInt();

	// The material id
	const QString materialId= readAttribute("material", false);

	// Offsets and data source list
	QList<InputData> inputDataList;

	// Polygon number of vertice list
	QList<int> vcountList;

	// Polygon index list
	QList<int> polyIndexList;

	while (endElementNotReached("polylist"))
	{
		if (QXmlStreamReader::StartElement == m_pStreamReader->tokenType())
		{
			const QStringRef currentElementName= m_pStreamReader->name();
			if ((currentElementName == "input") and vcountList.isEmpty())
			{
				InputData currentInput;
				// Get input data offset
				currentInput.m_Offset= readAttribute("offset", true).toInt();
				// Get input data semantic
				const QString semantic= readAttribute("semantic", true);
				if (semantic == "VERTEX") currentInput.m_Semantic= VERTEX;
				else if (semantic == "NORMAL") currentInput.m_Semantic= NORMAL;
				else if (semantic == "TEXCOORD") currentInput.m_Semantic= TEXCOORD;
				else throwException("Source semantic :" + semantic + "Not supported");
				// Get input data source id
				currentInput.m_Source= readAttribute("source", true).remove('#');

				// Bypasss vertices indirection
				if (m_VerticesSourceHash.contains(currentInput.m_Source))
				{
					currentInput.m_Source= m_VerticesSourceHash.value(currentInput.m_Source);
				}
				inputDataList.append(currentInput);
			}
			else if ((currentElementName == "vcount") and (inputDataList.size() > 0))
			{
				QString vcountString= getContent("vcount");
				QStringList vcountStringList= vcountString.split(' ');
				if (vcountStringList.size() != polygonCount) throwException("vcount size not match");
				bool toIntOK;
				for (int i= 0; i < polygonCount; ++i)
				{
					vcountList.append(vcountStringList.at(i).toInt(&toIntOK));
					if (not toIntOK) throwException("Unable to convert string :" + vcountStringList.at(i) + " To int");
				}
			}
			else if ((currentElementName == "p") and not vcountList.isEmpty() and polyIndexList.isEmpty())
			{

				{ // Fill index List
					QString pString= getContent("p");
					QStringList pStringList= pString.split(' ');
					bool toIntOK;
					const int size= pStringList.size();
					for (int i= 0; i < size; ++i)
					{
						polyIndexList.append(pStringList.at(i).toInt(&toIntOK));
						if (not toIntOK) throwException("Unable to convert string :" + pStringList.at(i) + " To int");
					}
				}

			}
		}
		m_pStreamReader->readNext();
	}
	// Add the polylist to the current mesh
	addPolylistToCurrentMesh(inputDataList, vcountList, polyIndexList);
}

// Add the polylist to the current mesh
void GLC_ColladaToWorld::addPolylistToCurrentMesh(const QList<InputData>& inputDataList, const QList<int>& vcountList, const QList<int>& polyIndexList)
{
	qDebug() << "GLC_ColladaToWorld::addPolylistToCurrentMesh";

	const int polygonCount= vcountList.size();
	const int inputDataCount= inputDataList.size();
	const int polyIndexCount= polyIndexList.size();

	// Flag to know if the polylist has normal
	bool hasNormals= false;

	// Check the existance of data source
	for (int dataIndex= 0; dataIndex < inputDataCount; ++dataIndex)
	{
		const QString source= inputDataList.at(dataIndex).m_Source;
		if ( not m_BulkDataHash.contains(source))
		{
			throwException(" Source : " + source + " Not found");
		}
		if (inputDataList.at(dataIndex).m_Semantic == NORMAL) hasNormals= true;
	}

	int maxOffset= 0;
	for (int i= 0; i < inputDataCount; ++i)
	{
		if (inputDataList.at(i).m_Offset > maxOffset)
		{
			maxOffset= inputDataList.at(i).m_Offset;
		}
	}
	qDebug() << " Max Offset :" << maxOffset;

	// the polygonIndex of the polylist
	QList<int> polygonIndex;

	// Fill the mapping, bulk data and index list of the current mesh info
	for (int i= 0; i < polyIndexCount; i+= maxOffset + 1)
	{
		// Create and set the current vertice
		ColladaVertice currentVertice;
		for (int dataIndex= 0; dataIndex < inputDataCount; ++dataIndex)
		{
			currentVertice.m_Values[inputDataList.at(dataIndex).m_Semantic]= polyIndexList.at(i + inputDataList.at(dataIndex).m_Offset);
		}
		if (m_pMeshInfo->m_Mapping.contains(currentVertice))
		{
			// Add the the index to the polygon index
			polygonIndex.append(m_pMeshInfo->m_Mapping.value(currentVertice));
		}
		else
		{
			// Add the current vertice to the current mesh info mapping hash table and increment the freeIndex
			m_pMeshInfo->m_Mapping.insert(currentVertice, (m_pMeshInfo->m_FreeIndex)++);
			// Add the the index to the polygon index
			polygonIndex.append(m_pMeshInfo->m_Mapping.value(currentVertice));

			// Add the bulk data associated to the current vertice to the current mesh info
			for (int dataIndex= 0; dataIndex < inputDataCount; ++dataIndex)
			{
				// The current input data
				InputData currentInputData= inputDataList.at(dataIndex);
				// QHash iterator on the right QList<float>
				BulkDataHash::const_iterator iBulkHash= m_BulkDataHash.find(currentInputData.m_Source);
				// Firts value
				m_pMeshInfo->m_Datas[currentInputData.m_Semantic].append(iBulkHash.value().at(polyIndexList.at(i + currentInputData.m_Offset)));
				// Second value
				m_pMeshInfo->m_Datas[currentInputData.m_Semantic].append(iBulkHash.value().at(polyIndexList.at(i + currentInputData.m_Offset + 1)));
				// Fird value
				if (currentInputData.m_Semantic != TEXCOORD)
				{
					m_pMeshInfo->m_Datas[currentInputData.m_Semantic].append(iBulkHash.value().at(polyIndexList.at(i + currentInputData.m_Offset + 2)));
				}
			}
		}
	}
	// Triangulate the polygons

	// Append the triangulated index list to the current mesh info

	// Check if normal computation is needed
	if (not hasNormals)
	{
		// Compute the normals and add them to the current mesh info
	}

}


// Load library_visual_scenes element
void GLC_ColladaToWorld::loadVisualScenes()
{

}

// Load scene element
void GLC_ColladaToWorld::loadScene()
{

}

// Link texture to material
void GLC_ColladaToWorld::linkTexturesToMaterials()
{

	// Iterat thru the the texture id to material hash
	MaterialHash::iterator iMat= m_TextureToMaterialHash.begin();
	while (iMat != m_TextureToMaterialHash.constEnd())
	{
		GLC_Material* pCurrentMaterial= iMat.value();
		const QString textureId= iMat.key();

		// Check that the texture is present
		if (m_Sampler2DSurfaceHash.contains(textureId) and m_SurfaceImageHash.contains(m_Sampler2DSurfaceHash.value(textureId))
				and m_ImageFileHash.contains(m_SurfaceImageHash.value(m_Sampler2DSurfaceHash.value(textureId))))
		{
			QString imageFileName= m_ImageFileHash.value(m_SurfaceImageHash.value(m_Sampler2DSurfaceHash.value(textureId)));
			imageFileName= QFileInfo(m_FileName).absolutePath() + QDir::separator() + imageFileName;
			if (QFileInfo(imageFileName).exists())
			{
				GLC_Texture* pTexture= new GLC_Texture(m_pQGLContext, imageFileName);
				pCurrentMaterial->setTexture(pTexture);
			}
			else
			{
				qDebug() << imageFileName << " Not found";
			}
		}
		else
		{
			qDebug() << "Texture : " << textureId << " Not found";
		}
		++iMat;
	}
}
