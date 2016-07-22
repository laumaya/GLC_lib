/*
 *  glc_csghelper.cpp
 *
 *  Created on: 15/07/2016
 *      Author: Laurent Ribon
 */
#include "glc_csghelper.h"


#include "../3rdparty/csgjs/csgjs.h"

#include "glc_mesh.h"

GLC_CsgHelper::GLC_CsgHelper()
{

}

GLC_Mesh *GLC_CsgHelper::soustract(const GLC_Mesh *pMesh1, const GLC_Mesh *pMesh2)
{
    csgjs_model* pCsgModel1= csgModelFromMesh(pMesh1);
    csgjs_model* pCsgModel2= csgModelFromMesh(pMesh2);

    csgjs_model result= csgjs_difference(*pCsgModel1, *pCsgModel2);

    GLC_Mesh* pSubject= meshFromCsgModel(result, new GLC_Material);

    delete pCsgModel1;
    delete pCsgModel2;

    return pSubject;
}

csgjs_model *GLC_CsgHelper::csgModelFromMesh(const GLC_Mesh *pMesh)
{
    csgjs_model* pSubject= new csgjs_model;

    const GLfloatVector positionVector= pMesh->positionVector();
    const GLfloatVector normalVector= pMesh->normalVector();
    const GLfloatVector texelVector= pMesh->texelVector();

    QList<GLC_uint> materialIdList= pMesh->materialIds();
    const int materialCount= materialIdList.count();
    for (int iMat= 0; iMat < materialCount; ++iMat)
    {
        const GLC_uint materialId= materialIdList.at(iMat);
        const IndexList indexList= pMesh->getEquivalentTrianglesStripsFansIndex(0, materialId);
        const int count= indexList.count();
        for (int i= 0; i < count; ++i)
        {
            const int index= indexList.at(i);
            csgjs_vertex vertex;
            vertex.pos.x= positionVector.at(index * 3);
            vertex.pos.y= positionVector.at((index * 3) + 1);
            vertex.pos.z= positionVector.at((index * 3) + 2);

            vertex.normal.x= normalVector.at(index * 3);
            vertex.normal.y= normalVector.at((index * 3) + 1);
            vertex.normal.z= normalVector.at((index * 3) + 2);

            if (!texelVector.isEmpty())
            {
                vertex.uv.x= texelVector.at(index * 2);
                vertex.uv.y= texelVector.at((index * 2) + 1);
            }

            pSubject->vertices.push_back(vertex);
            pSubject->indices.push_back(i);
        }
    }

    return pSubject;
}

GLC_Mesh *GLC_CsgHelper::meshFromCsgModel(const csgjs_model &model, GLC_Material* pMaterial)
{
    GLC_Mesh* pSubject= new GLC_Mesh;

    GLfloatVector positionVector;
    GLfloatVector normalVector;
    GLfloatVector texelVector;

    const size_t verticeCount= model.vertices.size();
    for (size_t i= 0; i < verticeCount; ++i)
    {
        csgjs_vertex vertex= model.vertices.at(i);
        positionVector << vertex.pos.x << vertex.pos.y << vertex.pos.z;
        normalVector << vertex.normal.x << vertex.normal.y << vertex.normal.z;
        texelVector << vertex.uv.x << vertex.uv.y;
    }
    pSubject->addVertice(positionVector);
    pSubject->addNormals(normalVector);
    pSubject->addTexels(texelVector);

    IndexList indexList;
    const size_t indexCount= model.indices.size();
    for (size_t i= 0; i < indexCount; ++i)
    {
        indexList.append(model.indices.at(i));
    }
    pSubject->addTriangles(pMaterial, indexList);
    pSubject->finish();

    return pSubject;
}
