/*
 *  glc_worldtocollada.h
 *
 *  Created on: 22/12/2020
 *      Author: Laurent Ribon
 */
#ifndef GLC_WORLDTOCOLLADA_H
#define GLC_WORLDTOCOLLADA_H

#include <QString>
#include <QXmlStreamWriter>
#include <QSet>
#include <QHash>
#include <QMultiHash>

#include "../sceneGraph/glc_world.h"

class GLC_Mesh;

class GLC_WorldToCollada
{
public:
    GLC_WorldToCollada(const GLC_World& world);

public:
    void setAuthoringTool(const QString& value)
    {m_AuthoringTool= value;}

    bool exportToCollada(const QString& absoluteFileName);

private:
    void writeHeaderAsset();
    void writeMaterials();
    void writeGeometries();
    QString imageId();
    QString indexToString(int index);
    void init();
    void writeLibraryImages();
    QString writeImage(GLC_Texture* pTexture, const QString& source);
    void writeLibraryMaterials();
    void writeMaterial(const QString& materialId);
    QString materialId();
    QString materialFxId(const QString& materialId);
    QString meshId();
    void writeLibraryEffects();
    void writeMaterialEffect(GLC_Material* pMat);
    void writeMaterialProfileCommon(GLC_Material* pMat);
    void writeTextureProfile(GLC_Texture* pTexture);
    void writeMaterialTechnique(GLC_Material* pMat);
    void writeColor(const QColor& color);
    void writeMesh(GLC_Mesh* pMesh);
    void writeMeshPosition(GLC_Mesh* pMesh, const QString& meshId);
    void writeMeshNormal(GLC_Mesh* pMesh, const QString& meshId);
    void writeMeshTexel(GLC_Mesh* pMesh, const QString& meshId);
    void writeVertices(GLC_Mesh* pMesh, const QString& meshId, unsigned int wireOffset);
    void writeMeshTriangle(const IndexList& index, const QString& meshId, GLC_Material* pMat);
    void writeLineStrips(const GLC_Mesh* pMesh, const QString& meshId, unsigned int offset);
    void writeLibraryNode();
    void writeLibraryVisualScenes();
    void writeVisualScene();
    void writeNode(GLC_StructOccurrence* pOcc);
    void writeMatrix(const GLC_Matrix4x4& matrix);
    void writeMeshesNode(const QList<GLC_Mesh*> meshes);
    void writeBindMaterial(GLC_Mesh* pMesh);
    void writeInstanceMaterial(GLC_Material* pMat);
    void writeScene();
    QString meshNodeId(GLC_uint id) const;

private:
    GLC_World m_World;
    QString m_AuthoringTool;
    QString m_AbsoluteFileName;
    QXmlStreamWriter m_Writer;
    int m_ImageIndex;
    QHash<QString, QString> m_ImageSourceFileNameToTarget;
    QList<GLC_Material*> m_MaterialList;
    QHash<GLC_Material*, QString> m_MaterialToIdHash;
    QHash<GLC_Texture*, QString> m_TextureToImageId;
    QHash<GLC_Mesh*, QString> m_MeshIdHash;
    int m_MaterialIndex;
    int m_MeshIndex;
};



#endif // GLC_WORLDTOCOLLADA_H
