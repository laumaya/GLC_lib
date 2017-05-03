/*
 *  glc_csghelper.h
 *
 *  Created on: 15/07/2016
 *      Author: Laurent Ribon
 */
#ifndef GLC_CSGHELPER_H
#define GLC_CSGHELPER_H

#include <QHash>

#include "../glc_config.h"
#include "../glc_global.h"

class GLC_Mesh;
class GLC_Material;
class GLC_Matrix4x4;

struct csgjs_model;

class GLC_LIB_EXPORT GLC_CsgHelper
{
private:
    GLC_CsgHelper();

public:
    static GLC_Mesh* intersection(const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2);
    static void intersection(GLC_Mesh* pResultMesh, const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2);

    static GLC_Mesh* add(const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2);
    static void add(GLC_Mesh* pResultMesh, const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2);

    static GLC_Mesh* soustract(const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2);
    static void soustract(GLC_Mesh* pResultMesh, const GLC_Mesh* pMesh1, const GLC_Matrix4x4& m1, const GLC_Mesh* pMesh2, const GLC_Matrix4x4& m2);

    static csgjs_model* csgModelFromMesh(const GLC_Mesh* pMesh, const GLC_Matrix4x4& matrix);
    static GLC_Mesh* meshFromCsgModel(const csgjs_model& model, const QHash<GLC_uint, GLC_Material*>& materialHash);
    static void meshFromCsgModel(const csgjs_model& model, const QHash<GLC_uint, GLC_Material*>& materialHash, GLC_Mesh* pMesh);

private:
    static QHash<GLC_uint, GLC_Material*> materialHash(const GLC_Mesh* pMesh1, const GLC_Mesh* pMesh2);
};

#endif // GLC_CSGHELPER_H
