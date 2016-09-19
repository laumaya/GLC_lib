/*
 *  glc_csghelper.h
 *
 *  Created on: 15/07/2016
 *      Author: Laurent Ribon
 */
#ifndef GLC_CSGHELPER_H
#define GLC_CSGHELPER_H

#include "../glc_config.h"

class GLC_Mesh;
class GLC_Material;
class GLC_Matrix4x4;

struct csgjs_model;

class GLC_LIB_EXPORT GLC_CsgHelper
{
private:
    GLC_CsgHelper();

public:
    static GLC_Mesh* intersection(const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2, GLC_Material* pMaterial);
    static void intersection(GLC_Mesh* pResultMesh, const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2, GLC_Material* pMaterial);

    static GLC_Mesh* add(const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2, GLC_Material* pMaterial);
    static void add(GLC_Mesh* pResultMesh, const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2, GLC_Material* pMaterial);

    static GLC_Mesh* soustract(const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2, GLC_Material* pMaterial);
    static void soustract(GLC_Mesh* pResultMesh, const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2, GLC_Material* pMaterial);

    static csgjs_model* csgModelFromMesh(const GLC_Mesh* pMesh, const GLC_Matrix4x4& matrix);
    static GLC_Mesh* meshFromCsgModel(const csgjs_model& model, GLC_Material *pMaterial);
    static void meshFromCsgModel(const csgjs_model& model, GLC_Material *pMaterial, GLC_Mesh* pMesh);
};

#endif // GLC_CSGHELPER_H
