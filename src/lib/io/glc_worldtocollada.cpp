/*
 *  glc_worldtocollada.cpp
 *
 *  Created on: 22/12/2020
 *      Author: Laurent Ribon
 */
#include "glc_worldtocollada.h"
#include "glc_worldtocollada.h"

#include <QtDebug>
#include <QFile>
#include <QDateTime>

#include "../shading/glc_material.h"
#include "../sceneGraph/glc_structreference.h"
#include "../sceneGraph/glc_structoccurrence.h"
#include "../geometry/glc_3drep.h"
#include "../geometry/glc_mesh.h"
#include "../geometry/glc_geometry.h"

#include "glc_colladaxmlelement.h"

GLC_WorldToCollada::GLC_WorldToCollada(const GLC_World& world)
    : m_World(world)
    , m_AuthoringTool("GLC_lib")
    , m_AbsoluteFileName()
    , m_Writer()
    , m_ImageIndex(0)
    , m_ImageSourceFileNameToTarget()
    , m_MaterialList()
    , m_MaterialUidToStringIdHash()
    , m_TextureToImageId()
    , m_MeshIdHash()
    , m_MaterialIndex(0)
    , m_MeshIndex(0)
    , m_UseInstanciation(true)
    , m_CopiedMesh()
{

}

GLC_WorldToCollada::~GLC_WorldToCollada()
{
    qDeleteAll(m_CopiedMesh);
}

bool GLC_WorldToCollada::exportToCollada(const QString& absoluteFileName)
{
    init();
    bool subject= false;
    m_AbsoluteFileName= absoluteFileName;
    QFile exportFile(m_AbsoluteFileName);
    if (exportFile.open(QIODevice::WriteOnly))
    {
        m_Writer.setDevice(&exportFile);
        m_Writer.setAutoFormatting(true);
        m_Writer.writeStartDocument();
        m_Writer.writeStartElement(ColladaElement::colladaTopElement);
        m_Writer.writeAttribute("xlmns", "http://www.collada.org/2005/11/COLLADASchema");
        m_Writer.writeAttribute("version", "1.4.1");

        writeHeaderAsset();
        if (m_UseInstanciation)
        {
            writeMaterials(m_World.listOfMaterials());
            writeInstanciateGeometries();
        }
        else
        {
            fillCopiedMeshList();
            QSet<GLC_Material*> materialSet;
            for (GLC_Mesh* pMesh : m_CopiedMesh)
            {
                materialSet.unite(pMesh->materialSet());
            }
            writeMaterials(materialSet.values());
            writeNotInstanciateGeometries();
        }

        writeLibraryNode();
        writeLibraryVisualScenes();
        writeScene();

        m_Writer.writeEndElement(); // Collada top element
        m_Writer.writeEndDocument();
        subject= true;
    }

    return subject;
}

void GLC_WorldToCollada::writeHeaderAsset()
{
    m_Writer.writeStartElement(ColladaElement::assetElement);

    m_Writer.writeStartElement(ColladaElement::contributorElement);
    m_Writer.writeTextElement(ColladaElement::authoringToolElement, m_AuthoringTool);
    m_Writer.writeEndElement(); // contributor

    const QString date(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    m_Writer.writeTextElement(ColladaElement::createdElement, date);
    m_Writer.writeTextElement(ColladaElement::modifiedElement, date);

    m_Writer.writeStartElement(ColladaElement::unitElement);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, "mm");
    m_Writer.writeAttribute(ColladaElement::meterAttribute, "0.001");
    m_Writer.writeEndElement();

    m_Writer.writeTextElement(ColladaElement::upAxisElement, "Z_UP");

    m_Writer.writeEndElement(); // asset element
}

void GLC_WorldToCollada::writeMaterials(const QList<GLC_Material*> materialList)
{
    m_MaterialList= materialList;
    writeLibraryImages();
    writeLibraryMaterials();
    writeLibraryEffects();
}

void GLC_WorldToCollada::writeInstanciateGeometries()
{
    m_Writer.writeStartElement(ColladaElement::libraryGeometriesElement);

    QList<GLC_StructReference*> referencies(m_World.references());
    for (GLC_StructReference* pRef : referencies)
    {
        if (pRef->hasRepresentation())
        {
            GLC_3DRep* pRep= dynamic_cast<GLC_3DRep*>(pRef->representationHandle());
            if (nullptr != pRep)
            {
                const int count= pRep->numberOfBody();
                for (int i= 0 ; i < count; ++i)
                {
                    GLC_Mesh* pMesh= dynamic_cast<GLC_Mesh*>(pRep->geomAt(i));
                    if (nullptr != pMesh)
                    {
                        writeMesh(pMesh);
                    }
                }
            }
        }
    }

    m_Writer.writeEndElement();
}

void GLC_WorldToCollada::writeNotInstanciateGeometries()
{
    m_Writer.writeStartElement(ColladaElement::libraryGeometriesElement);

    for (GLC_Mesh* pMesh : m_CopiedMesh)
    {
        writeMesh(pMesh);
    }
    m_Writer.writeEndElement();
}

QString GLC_WorldToCollada::imageId()
{
    const QString subject("image" + indexToString(m_ImageIndex++));

    return subject;
}

QString GLC_WorldToCollada::indexToString(int index)
{
    QString subject(QString::number(index));
    const int length= 4 - subject.length();
    for (int i= 0; i < length; ++i)
    {
        subject.prepend("0");
    }

    return subject;
}

void GLC_WorldToCollada::init()
{
    m_ImageIndex= 0;
    m_ImageSourceFileNameToTarget.clear();
    m_MaterialUidToStringIdHash.clear();
    m_MaterialList.clear();
    m_TextureToImageId.clear();
    m_MeshIdHash.clear();
    m_MaterialIndex= 0;
    m_MeshIndex= 0;
}

void GLC_WorldToCollada::writeLibraryImages()
{
    const QString filePath(QFileInfo(m_AbsoluteFileName).path());
    m_Writer.writeStartElement(ColladaElement::libraryImagesElement);
    for (GLC_Material* pMat : m_MaterialList)
    {
        if (pMat->hasTexture())
        {
            GLC_Texture* pTexture= pMat->textureHandle();
            const QString fileName(pTexture->fileName());
            QFile file(fileName);
            if (!m_ImageSourceFileNameToTarget.contains(fileName))
            {
                QString destFileName(filePath + '/' + QFileInfo(fileName).fileName());
                if (QFileInfo(fileName).isReadable())
                {
                    QFile file(fileName);
                    if (QFileInfo(destFileName).exists())
                    {
                        QFile::remove(destFileName);
                    }
                    if (file.copy(destFileName))
                    {
                        const QString targetFileName(writeImage(pTexture, QDir(filePath).relativeFilePath(destFileName)));
                        m_ImageSourceFileNameToTarget.insert(fileName, targetFileName);
                    }
                    else
                    {
                        // raise error
                    }
                }
                else
                {
                    if (glc::isArchiveString(fileName))
                    {
                        destFileName= filePath + '/' + glc::archiveEntryFileName(fileName);
                    }
                    QImage textureImage= pTexture->imageOfTexture();
                    if (textureImage.save(destFileName, QFileInfo(destFileName).suffix().toLatin1().data()))
                    {
                        const QString targetFileName(writeImage(pTexture, QFileInfo(destFileName).fileName()));
                        m_ImageSourceFileNameToTarget.insert(fileName, targetFileName);
                    }
                }
            }
            else
            {
                m_TextureToImageId.insert(pTexture, m_ImageSourceFileNameToTarget.value(fileName));
            }
        }
    }
    m_Writer.writeEndElement();
}

QString GLC_WorldToCollada::writeImage(GLC_Texture* pTexture, const QString& source)
{
    m_Writer.writeStartElement(ColladaElement::imageElement);
    const QString subject(imageId());
    m_TextureToImageId.insert(pTexture, subject);
    m_Writer.writeAttribute(ColladaElement::idAttribute, subject);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, subject);
    m_Writer.writeTextElement(ColladaElement::initFromElement, source);
    m_Writer.writeEndElement();

    return subject;
}

void GLC_WorldToCollada::writeLibraryMaterials()
{
    m_Writer.writeStartElement(ColladaElement::libraryMaterialsElement);
    for (GLC_Material* pMat : m_MaterialList)
    {
        const QString materialId(this->materialId());
        m_MaterialUidToStringIdHash.insert(pMat->id(), materialId);
        writeMaterial(materialId);
    }

    m_Writer.writeEndElement();
}

void GLC_WorldToCollada::writeMaterial(const QString& materialId)
{
    m_Writer.writeStartElement(ColladaElement::materialElement);
    const QString materialFxId(this->materialFxId(materialId));
    m_Writer.writeAttribute(ColladaElement::idAttribute, materialId);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, materialId);

    m_Writer.writeStartElement(ColladaElement::instanceEffectElement);
    m_Writer.writeAttribute(ColladaElement::urlAttribute, materialFxId);
    m_Writer.writeEndElement();

    m_Writer.writeEndElement();
}

QString GLC_WorldToCollada::materialId()
{
    const QString subject("phong" + indexToString(m_MaterialIndex++));

    return subject;
}

QString GLC_WorldToCollada::materialFxId(const QString& materialId)
{
    const QString subject("#" + materialId + "-fx");

    return subject;
}

QString GLC_WorldToCollada::meshId()
{
    const QString subject("mesh" + indexToString(m_MeshIndex++));

    return subject;
}

void GLC_WorldToCollada::writeLibraryEffects()
{
    m_Writer.writeStartElement(ColladaElement::libraryEffectsElement);
    for (GLC_Material* pMaterial : m_MaterialList)
    {
        writeMaterialEffect(pMaterial);
    }

    m_Writer.writeEndElement();
}

void GLC_WorldToCollada::writeMaterialEffect(GLC_Material* pMat)
{
    m_Writer.writeStartElement(ColladaElement::effectElement);
    const QString effectId(m_MaterialUidToStringIdHash.value(pMat->id()) + "-fx");
    m_Writer.writeAttribute(ColladaElement::idAttribute, effectId);
    writeMaterialProfileCommon(pMat);
    m_Writer.writeEndElement();
}

void GLC_WorldToCollada::writeMaterialProfileCommon(GLC_Material* pMat)
{
    m_Writer.writeStartElement(ColladaElement::profileCommonElement);
    if (pMat->hasTexture())
    {
        writeTextureProfile(pMat->textureHandle());
    }
    writeMaterialTechnique(pMat);
    m_Writer.writeEndElement();
}

void GLC_WorldToCollada::writeTextureProfile(GLC_Texture* pTexture)
{
    m_Writer.writeStartElement(ColladaElement::newParamElement);
    const QString& imageId= m_TextureToImageId.value(pTexture);
    const QString surfaceId(imageId + "-surface");
    m_Writer.writeAttribute(ColladaElement::sidAttribute, surfaceId);
    m_Writer.writeStartElement(ColladaElement::surfaceElement);
    m_Writer.writeAttribute(ColladaElement::typeAttribute, "2D");
    m_Writer.writeTextElement(ColladaElement::initFromElement, imageId);
    m_Writer.writeTextElement(ColladaElement::formatElement, "A8R8G8B8");
    m_Writer.writeEndElement(); // Surface element
    m_Writer.writeEndElement(); // New param


    m_Writer.writeStartElement(ColladaElement::newParamElement);
    const QString samplerId(imageId + "-sampler");
    m_Writer.writeAttribute(ColladaElement::sidAttribute, samplerId);
    m_Writer.writeStartElement(ColladaElement::sampler2DElement);
    m_Writer.writeTextElement(ColladaElement::sourceElement, surfaceId);
    m_Writer.writeTextElement(ColladaElement::minFilterElement, "LINEAR_MIPMAP_LINEAR");
    m_Writer.writeTextElement(ColladaElement::magFilterElement, "LINEAR");
    m_Writer.writeEndElement(); // Sampler 2D
    m_Writer.writeEndElement(); // new param
}

void GLC_WorldToCollada::writeMaterialTechnique(GLC_Material* pMat)
{
    m_Writer.writeStartElement(ColladaElement::techniqueElement);
    m_Writer.writeAttribute(ColladaElement::sidAttribute, "COMMON");

    m_Writer.writeStartElement(ColladaElement::phongElement);

    m_Writer.writeStartElement(ColladaElement::emissionElement);
    writeColor(pMat->emissiveColor());
    m_Writer.writeEndElement();

    m_Writer.writeStartElement(ColladaElement::ambientElement);
    writeColor(pMat->ambientColor());
    m_Writer.writeEndElement();

    m_Writer.writeStartElement(ColladaElement::diffuseElement);
    if (pMat->hasTexture() && m_TextureToImageId.contains(pMat->textureHandle()))
    {
        const QString samplerId(m_TextureToImageId.value(pMat->textureHandle()) + "-sampler");
        m_Writer.writeStartElement(ColladaElement::textureElement);
        m_Writer.writeAttribute(ColladaElement::textureAttribute, samplerId);
        m_Writer.writeAttribute(ColladaElement::textureCoordAttribute, "UVSET0");
        m_Writer.writeEndElement();
    }
    else
    {
        writeColor(pMat->diffuseColor());
    }
    m_Writer.writeEndElement();

    if (pMat->isTransparent())
    {
        m_Writer.writeStartElement(ColladaElement::transparentElement);
        writeColor(pMat->diffuseColor()); // Use diffuse color for the transparent color
        m_Writer.writeEndElement();

        m_Writer.writeStartElement(ColladaElement::transparencyElement);
        m_Writer.writeTextElement(ColladaElement::floatElement, QString::number(pMat->opacity()));
        m_Writer.writeEndElement();
   }

    m_Writer.writeStartElement(ColladaElement::shininessElement);
    m_Writer.writeTextElement(ColladaElement::floatElement, QString::number(pMat->shininess()));
    m_Writer.writeEndElement();

    m_Writer.writeEndElement(); // Phong element
    m_Writer.writeEndElement(); // Technique common
}

void GLC_WorldToCollada::writeColor(const QColor& color)
{
    QString value(QString::number(color.redF()) + ' ' + QString::number(color.greenF())
                  + ' ' + QString::number(color.blueF()) + " 1");
    m_Writer.writeTextElement(ColladaElement::colorElement, value);
}

void GLC_WorldToCollada::writeMesh(GLC_Mesh* pMesh)
{
    if (!pMesh->isEmpty())
    {
        const QString meshId(this->meshId());
        m_MeshIdHash.insert(pMesh, meshId);
        m_Writer.writeStartElement(ColladaElement::geometryElement);
        m_Writer.writeAttribute(ColladaElement::idAttribute, meshId);
        m_Writer.writeAttribute(ColladaElement::nameAttribute, meshId);
        m_Writer.writeStartElement(ColladaElement::meshElement);
        writeMeshPosition(pMesh, meshId);
        writeMeshNormal(pMesh, meshId);
        writeMeshTexel(pMesh, meshId);
        writeVertices(pMesh, meshId, pMesh->vertexCount());

        m_Writer.writeEndElement(); // Mesh element

        m_Writer.writeEndElement(); // Geometry element
    }
}

void GLC_WorldToCollada::writeMeshPosition(GLC_Mesh* pMesh, const QString& meshId)
{
    m_Writer.writeStartElement(ColladaElement::sourceElement);
    const QString sourceId(meshId + "-positions");
    m_Writer.writeAttribute(ColladaElement::idAttribute, sourceId);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, sourceId);

    m_Writer.writeStartElement(ColladaElement::floatArrayElement);
    const GLfloatVector positionVector(pMesh->positionVector() + pMesh->wirePositionVector());

    const QString arrayId(sourceId + "-array");
    m_Writer.writeAttribute(ColladaElement::idAttribute, arrayId);
    m_Writer.writeAttribute(ColladaElement::countAttribute, QString::number(positionVector.count()));

    QString positions;
    for (const GLfloat& value : positionVector)
    {
        positions.append(QString::number(value) + ' ');
    }
    positions.resize(positions.size() - 1);
    m_Writer.writeCharacters(positions);
    m_Writer.writeEndElement(); // Float array

    m_Writer.writeStartElement(ColladaElement::techniqueCommonElement);
    m_Writer.writeStartElement(ColladaElement::accessorElement);
    const int stride= 3;
    m_Writer.writeAttribute(ColladaElement::countAttribute, QString::number(positionVector.count() / stride));
    m_Writer.writeAttribute(ColladaElement::sourceAttribute, '#' + arrayId);
    m_Writer.writeAttribute(ColladaElement::strideAttribute, QString::number(stride));

    m_Writer.writeStartElement(ColladaElement::paramElement);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, "X");
    m_Writer.writeAttribute(ColladaElement::typeAttribute, "float");
    m_Writer.writeEndElement(); // paramElement

    m_Writer.writeStartElement(ColladaElement::paramElement);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, "Y");
    m_Writer.writeAttribute(ColladaElement::typeAttribute, "float");
    m_Writer.writeEndElement(); // paramElement

    m_Writer.writeStartElement(ColladaElement::paramElement);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, "Z");
    m_Writer.writeAttribute(ColladaElement::typeAttribute, "float");
    m_Writer.writeEndElement(); // paramElement

    m_Writer.writeEndElement(); // Accessor

    m_Writer.writeEndElement(); // Technique common

    m_Writer.writeEndElement(); // source element
}

void GLC_WorldToCollada::writeMeshNormal(GLC_Mesh* pMesh, const QString& meshId)
{
    m_Writer.writeStartElement(ColladaElement::sourceElement);
    const QString sourceId(meshId + "-normals");
    m_Writer.writeAttribute(ColladaElement::idAttribute, sourceId);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, sourceId);

    m_Writer.writeStartElement(ColladaElement::floatArrayElement);
    GLfloatVector normalVector(pMesh->normalVector());

    const QString arrayId(sourceId + "-array");
    m_Writer.writeAttribute(ColladaElement::idAttribute, arrayId);
    m_Writer.writeAttribute(ColladaElement::countAttribute, QString::number(normalVector.count()));

    QString normals;
    for (const GLfloat& value : normalVector)
    {
        normals.append(QString::number(value) + ' ');
    }
    normals.resize(normals.size() - 1);
    m_Writer.writeCharacters(normals);
    m_Writer.writeEndElement(); // Float array

    m_Writer.writeStartElement(ColladaElement::techniqueCommonElement);
    m_Writer.writeStartElement(ColladaElement::accessorElement);
    const int stride= 3;
    m_Writer.writeAttribute(ColladaElement::countAttribute, QString::number(normalVector.count() / stride));
    m_Writer.writeAttribute(ColladaElement::sourceAttribute, '#' + arrayId);
    m_Writer.writeAttribute(ColladaElement::strideAttribute, QString::number(stride));

    m_Writer.writeStartElement(ColladaElement::paramElement);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, "X");
    m_Writer.writeAttribute(ColladaElement::typeAttribute, "float");
    m_Writer.writeEndElement(); // paramElement

    m_Writer.writeStartElement(ColladaElement::paramElement);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, "Y");
    m_Writer.writeAttribute(ColladaElement::typeAttribute, "float");
    m_Writer.writeEndElement(); // paramElement

    m_Writer.writeStartElement(ColladaElement::paramElement);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, "Z");
    m_Writer.writeAttribute(ColladaElement::typeAttribute, "float");
    m_Writer.writeEndElement(); // paramElement

    m_Writer.writeEndElement(); // Accessor

    m_Writer.writeEndElement(); // Technique common

    m_Writer.writeEndElement(); // source element
}

void GLC_WorldToCollada::writeMeshTexel(GLC_Mesh* pMesh, const QString& meshId)
{
    GLfloatVector texelVector(pMesh->texelVector());
    if (!texelVector.isEmpty())
    {
        m_Writer.writeStartElement(ColladaElement::sourceElement);
        const QString sourceId(meshId + "-map");
        m_Writer.writeAttribute(ColladaElement::idAttribute, sourceId);
        m_Writer.writeAttribute(ColladaElement::nameAttribute, sourceId);

        m_Writer.writeStartElement(ColladaElement::floatArrayElement);

        const QString arrayId(sourceId + "-array");
        m_Writer.writeAttribute(ColladaElement::idAttribute, arrayId);
        m_Writer.writeAttribute(ColladaElement::countAttribute, QString::number(texelVector.count()));

        QString texel;
        for (const GLfloat& value : texelVector)
        {
            texel.append(QString::number(value) + ' ');
        }
        texel.resize(texel.size() - 1);
        m_Writer.writeCharacters(texel);
        m_Writer.writeEndElement(); // Float array

        m_Writer.writeStartElement(ColladaElement::techniqueCommonElement);
        m_Writer.writeStartElement(ColladaElement::accessorElement);
        const int stride= 2;
        m_Writer.writeAttribute(ColladaElement::countAttribute, QString::number(texelVector.count() / stride));
        m_Writer.writeAttribute(ColladaElement::sourceAttribute, '#' + arrayId);
        m_Writer.writeAttribute(ColladaElement::strideAttribute, QString::number(stride));

        m_Writer.writeStartElement(ColladaElement::paramElement);
        m_Writer.writeAttribute(ColladaElement::nameAttribute, "S");
        m_Writer.writeAttribute(ColladaElement::typeAttribute, "float");
        m_Writer.writeEndElement(); // paramElement

        m_Writer.writeStartElement(ColladaElement::paramElement);
        m_Writer.writeAttribute(ColladaElement::nameAttribute, "T");
        m_Writer.writeAttribute(ColladaElement::typeAttribute, "float");
        m_Writer.writeEndElement(); // paramElement

        m_Writer.writeEndElement(); // Accessor

        m_Writer.writeEndElement(); // Technique common

        m_Writer.writeEndElement(); // source element
    }
}

void GLC_WorldToCollada::writeVertices(GLC_Mesh* pMesh, const QString& meshId, unsigned int wireOffset)
{
    m_Writer.writeStartElement(ColladaElement::verticesElement);
    const QString verticesId(meshId + "-vertices");
    m_Writer.writeAttribute(ColladaElement::idAttribute, verticesId);
    const QString positionId('#' + meshId + "-positions");
    m_Writer.writeStartElement(ColladaElement::inputElement);
    m_Writer.writeAttribute(ColladaElement::semanticAttribute, "POSITION");
    m_Writer.writeAttribute(ColladaElement::sourceAttribute, positionId);
    m_Writer.writeEndElement(); // input element

    m_Writer.writeEndElement(); // vertices element
    QSet<GLC_Material*> materialSet(pMesh->materialSet());
    QSet<GLC_Material*>::const_iterator iMat= materialSet.constBegin();
    while (iMat != materialSet.constEnd())
    {
        GLC_Material* pMat= *iMat;
        if (m_MaterialUidToStringIdHash.contains(pMat->id()))
        {
            const IndexList index(pMesh->getEquivalentTrianglesStripsFansIndex(0, pMat->id()));
            writeMeshTriangle(index, meshId, pMat);
        }
        ++iMat;
    }

    writeLineStrips(pMesh, meshId, wireOffset);
}

void GLC_WorldToCollada::writeMeshTriangle(const IndexList& index, const QString& meshId, GLC_Material* pMat)
{
    if (!index.isEmpty())
    {
        // GLC_lib index are not interleaved (VBO index is used)
        const QString materialId(m_MaterialUidToStringIdHash.value(pMat->id()) + "SG");
        m_Writer.writeStartElement(ColladaElement::trianglesElement);
        m_Writer.writeAttribute(ColladaElement::countAttribute, QString::number(index.count() / 3));
        m_Writer.writeAttribute(ColladaElement::materialAttribute, materialId);

        m_Writer.writeStartElement(ColladaElement::inputElement);
        m_Writer.writeAttribute(ColladaElement::offsetAttribute, "0");
        m_Writer.writeAttribute(ColladaElement::semanticAttribute, "VERTEX");
        m_Writer.writeAttribute(ColladaElement::sourceAttribute, QString('#' + meshId + "-vertices"));
        m_Writer.writeEndElement(); // input element

        m_Writer.writeStartElement(ColladaElement::inputElement);
        m_Writer.writeAttribute(ColladaElement::offsetAttribute, "0");
        m_Writer.writeAttribute(ColladaElement::semanticAttribute, "NORMAL");
        m_Writer.writeAttribute(ColladaElement::sourceAttribute, QString('#' + meshId + "-normals"));
        m_Writer.writeEndElement(); // input element

        if (pMat->hasTexture())
        {
            m_Writer.writeStartElement(ColladaElement::inputElement);
            m_Writer.writeAttribute(ColladaElement::offsetAttribute, "0");
            m_Writer.writeAttribute(ColladaElement::semanticAttribute, "TEXCOORD");
            m_Writer.writeAttribute(ColladaElement::sourceAttribute, QString('#' + meshId + "-map"));
            m_Writer.writeAttribute(ColladaElement::setAttribute, "0");
            m_Writer.writeEndElement(); // input element
        }

        QString indexString;
        for (GLuint currentIndex : index)
        {
            const QString current(QString::number(currentIndex));
            indexString.append(current + ' ');
        }
        indexString.resize(indexString.size() - 1);
        m_Writer.writeTextElement(ColladaElement::primitiveElement, indexString);
        m_Writer.writeEndElement(); // triangles element
    }
}

void GLC_WorldToCollada::writeLineStrips(const GLC_Mesh* pMesh, const QString& meshId, unsigned int offset)
{
    if (!pMesh->wireDataIsEmpty())
    {
        const GLC_WireData& wireData= pMesh->wireData();
        const int count= wireData.verticeGroupCount();
        for (int i= 0; i < count; ++i)
        {
            const int currentSize= wireData.verticeGroupSize(i);
            m_Writer.writeStartElement(ColladaElement::linestripsElement);
            m_Writer.writeAttribute(ColladaElement::countAttribute, QString::number(currentSize));

            m_Writer.writeStartElement(ColladaElement::inputElement);
            m_Writer.writeAttribute(ColladaElement::offsetAttribute, "0");
            m_Writer.writeAttribute(ColladaElement::semanticAttribute, "VERTEX");
            m_Writer.writeAttribute(ColladaElement::sourceAttribute, QString('#' + meshId + "-vertices"));
            m_Writer.writeEndElement(); // input element

            QString indexString;
            unsigned int index= wireData.verticeGroupOffset(i) + offset;
            for (int j= 0; j < currentSize; ++j)
            {
                indexString.append(QString::number(index++) + ' ');
            }
            indexString.resize(indexString.size() - 1);
            m_Writer.writeTextElement(ColladaElement::primitiveElement, indexString);

            m_Writer.writeEndElement(); // line strips
        }
    }
}

void GLC_WorldToCollada::writeLibraryNode()
{
    m_Writer.writeStartElement(ColladaElement::libraryNodesElement);
    QHash<GLC_Mesh*, QString>::const_iterator iMesh= m_MeshIdHash.constBegin();
    while (iMesh != m_MeshIdHash.constEnd())
    {
        GLC_Mesh* pMesh= iMesh.key();
        const QString meshId(iMesh.value());

        m_Writer.writeStartElement(ColladaElement::nodeElement);
        m_Writer.writeAttribute(ColladaElement::idAttribute, meshNodeId(pMesh->id()));
        m_Writer.writeAttribute(ColladaElement::nameAttribute, pMesh->name());

        m_Writer.writeStartElement(ColladaElement::instanceGeometryElement);
        QString url('#' + meshId);
        m_Writer.writeAttribute(ColladaElement::urlAttribute, url);

        writeBindMaterial(pMesh);

        m_Writer.writeEndElement(); // instance geometry

        m_Writer.writeEndElement(); // node element

        ++iMesh;
    }
    m_Writer.writeEndElement(); // Library node
}

void GLC_WorldToCollada::writeLibraryVisualScenes()
{
    m_Writer.writeStartElement(ColladaElement::libraryVisualScenesElement);
    if (m_UseInstanciation)
    {
        writeInstanciateVisualScene();
    }
    else
    {
        writeNotInstanciateVisualScene();
    }
    m_Writer.writeEndElement();
}

void GLC_WorldToCollada::writeInstanciateVisualScene()
{
    m_Writer.writeStartElement(ColladaElement::visualSceneElement);
    m_Writer.writeAttribute(ColladaElement::idAttribute, "visualScene");
    m_Writer.writeAttribute(ColladaElement::nameAttribute, "visualScene");
    QList<GLC_StructOccurrence*> occurences(m_World.listOfOccurrence());
    for (GLC_StructOccurrence* pOcc : occurences)
    {
        if (pOcc->structReference()->hasRepresentation())
        {
            writeNode(pOcc);
        }
    }
    m_Writer.writeEndElement();
}

void GLC_WorldToCollada::writeNotInstanciateVisualScene()
{
    m_Writer.writeStartElement(ColladaElement::visualSceneElement);
    m_Writer.writeAttribute(ColladaElement::idAttribute, "visualScene");
    m_Writer.writeAttribute(ColladaElement::nameAttribute, "visualScene");

    writeMeshesNode(m_CopiedMesh);

    m_Writer.writeEndElement();
}

void GLC_WorldToCollada::writeNode(GLC_StructOccurrence* pOcc)
{
    m_Writer.writeStartElement(ColladaElement::nodeElement);
    const QString id("Node_" + QString::number(pOcc->id()));
    m_Writer.writeAttribute(ColladaElement::idAttribute, id);
    m_Writer.writeAttribute(ColladaElement::nameAttribute, pOcc->name());
    writeMatrix(pOcc->absoluteMatrix());
    if (pOcc->structReference()->hasRepresentation())
    {
        GLC_3DRep* pRep= dynamic_cast<GLC_3DRep*>(pOcc->structReference()->representationHandle());
        if (nullptr != pRep)
        {
            QList<GLC_Mesh*> meshList;
            const int count= pRep->numberOfBody();
            for (int i= 0; i < count; ++i)
            {
                GLC_Mesh* pCurrent= dynamic_cast<GLC_Mesh*>(pRep->geomAt(i));
                if (nullptr != pCurrent) meshList.append(pCurrent);
            }
            writeMeshesNode(meshList);
        }
    }
    m_Writer.writeEndElement(); // node element
}

void GLC_WorldToCollada::writeMatrix(const GLC_Matrix4x4& matrix)
{
    QString subject;
    const QChar spaceChar(' ');
    // Rotation
    subject+= QString::number(matrix.getData()[0], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[4], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[8], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[12], 'g', 16) + spaceChar;

    subject+= QString::number(matrix.getData()[1], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[5], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[9], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[13], 'g', 16) + spaceChar;

    subject+= QString::number(matrix.getData()[2], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[6], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[10], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[14], 'g', 16) + spaceChar;

    // Translation
    subject+= QString::number(matrix.getData()[3], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[7], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[11], 'g', 16) + spaceChar;
    subject+= QString::number(matrix.getData()[15], 'g', 16);


    m_Writer.writeTextElement(ColladaElement::matrixElement, subject);
}

void GLC_WorldToCollada::writeMeshesNode(const QList<GLC_Mesh*> meshes)
{
    for (GLC_Mesh* pMesh : meshes)
    {
        if (m_MeshIdHash.contains(pMesh))
        {
            m_Writer.writeStartElement(ColladaElement::nodeElement);
            const QString id(meshNodeId(pMesh->id()) + "_1");
            m_Writer.writeAttribute(ColladaElement::idAttribute, id);
            m_Writer.writeAttribute(ColladaElement::nameAttribute, id);
            m_Writer.writeStartElement(ColladaElement::instanceNodeElement);
            m_Writer.writeAttribute(ColladaElement::urlAttribute, QString('#') + meshNodeId(pMesh->id()));
            m_Writer.writeEndElement(); // Instance node
            m_Writer.writeEndElement(); // node element
        }
    }
}

void GLC_WorldToCollada::writeBindMaterial(GLC_Mesh* pMesh)
{
    m_Writer.writeStartElement(ColladaElement::bindMaterialElement);
    m_Writer.writeStartElement(ColladaElement::techniqueCommonElement);
    QList<GLC_Material*> materialList= pMesh->materialSet().values();
    for (GLC_Material* pMat : materialList)
    {
        writeInstanceMaterial(pMat);
    }
    m_Writer.writeEndElement();
    m_Writer.writeEndElement();

}

void GLC_WorldToCollada::writeInstanceMaterial(GLC_Material* pMat)
{
    if (m_MaterialUidToStringIdHash.contains(pMat->id()))
    {
        m_Writer.writeStartElement(ColladaElement::instanceMaterialElement);
        const QString materialId(m_MaterialUidToStringIdHash.value(pMat->id()));
        m_Writer.writeAttribute(ColladaElement::symbolAttribute, materialId + "SG");
        m_Writer.writeAttribute(ColladaElement::targetAttribute, '#' + materialId);
        if (pMat->hasTexture() && m_TextureToImageId.contains(pMat->textureHandle()))
        {
            m_Writer.writeStartElement(ColladaElement::bindVertexInputElement);
            m_Writer.writeAttribute(ColladaElement::semanticAttribute, "UVSET0");
            m_Writer.writeAttribute(ColladaElement::inputSemanticAttribute, "TEXTCOORD");
            m_Writer.writeAttribute(ColladaElement::inputSetAttribute, "0");
            m_Writer.writeEndElement();
        }
        m_Writer.writeEndElement();
    }
}

void GLC_WorldToCollada::writeScene()
{
    m_Writer.writeStartElement(ColladaElement::sceneElement);
    m_Writer.writeStartElement(ColladaElement::instanceVisualSceneElement);
    m_Writer.writeAttribute(ColladaElement::urlAttribute, "#visualScene");
    m_Writer.writeEndElement(); // instance visual scene
    m_Writer.writeEndElement(); // scene element
}

QString GLC_WorldToCollada::meshNodeId(GLC_uint id) const
{
    const QString subject("Mesh_" + QString::number(id));

    return subject;
}

void GLC_WorldToCollada::fillCopiedMeshList()
{
    Q_ASSERT(!m_UseInstanciation);

    QList<GLC_StructOccurrence*> occurences(m_World.listOfOccurrence());
    for (GLC_StructOccurrence* pOcc : occurences)
    {
        if (pOcc->hasRepresentation())
        {
            GLC_3DRep* pRep= dynamic_cast<GLC_3DRep*>(pOcc->structReference()->representationHandle());
            if (nullptr != pRep)
            {
                const int count= pRep->numberOfBody();
                for (int i= 0 ; i < count; ++i)
                {
                    GLC_Mesh* pMesh= dynamic_cast<GLC_Mesh*>(pRep->geomAt(i));
                    if (nullptr != pMesh)
                    {
                        pMesh= new GLC_Mesh(*(pMesh));
                        pMesh->transformVertice(pOcc->absoluteMatrix());
                        m_CopiedMesh.append(pMesh);
                    }
                }
            }
        }
    }
}
