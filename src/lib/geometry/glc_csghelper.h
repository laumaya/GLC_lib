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

struct csgjs_model;

class GLC_LIB_EXPORT GLC_CsgHelper
{
private:
    GLC_CsgHelper();

public:
    static GLC_Mesh* soustract(const GLC_Mesh* pMesh1, const GLC_Mesh* pMesh2);
private:
    static csgjs_model* csgModelFromMesh(const GLC_Mesh* pMesh);
    static GLC_Mesh* meshFromCsgModel(const csgjs_model& model, GLC_Material *pMaterial);
};

#endif // GLC_CSGHELPER_H
