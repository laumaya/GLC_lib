/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_csgnode.h interface for the GLC_CsgNode class.

#ifndef GLC_CSGNODE_H
#define GLC_CSGNODE_H

#include "../glc_global.h"
#include "../maths/glc_matrix4x4.h"

#include "glc_3drep.h"
#include "glc_wiredata.h"

#include "../glc_config.h"

struct csgjs_model;
class GLC_Material;

//////////////////////////////////////////////////////////////////////
//! \class GLC_CsgNode
/*! \brief GLC_CsgNode : constructive geometry node*/

/*! An GLC_CsgNode is a node in a constructive geometry tree
*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_CsgNode
{
public:
    GLC_CsgNode();
    GLC_CsgNode(const GLC_Matrix4x4& matrix, const GLC_3DRep& rep);
    GLC_CsgNode(const GLC_CsgNode& other);

    virtual ~GLC_CsgNode();

public:
    virtual GLC_CsgNode* clone() const= 0;
    virtual bool update()= 0;
    virtual void createMesh()= 0;
    virtual QList<GLC_CsgNode*> chrildren() const= 0;

public:
    GLC_3DRep get3DRep() const
    {return m_3DRep;}

    const GLC_Matrix4x4& matrix() const
    {return m_Matrix;}

    GLC_Material* material() const
    {return m_pMaterial;}

    csgjs_model* csgjsModel() const
    {return m_pResultCsgModel;}

    virtual const GLC_WireData& wireData() const= 0;

public:
    void setMatrix(const GLC_Matrix4x4& matrix);

    void update(const GLC_Matrix4x4& matrix);

    void setMaterial(GLC_Material* pMaterial);

    void set3DRep(const GLC_3DRep& rep)
    {m_3DRep= rep;}

    QList<QList<GLC_CsgNode*> > LevelList() const;

    void multiThreadedUpdate();

protected:
    void createLvlList(QList<QList<GLC_CsgNode*> >* lvlList, int currentLevel) const;
    static GLC_CsgNode* mappedFutureFct(GLC_CsgNode* pNode);

protected:
    const GLC_uint m_Id;
    GLC_Matrix4x4 m_Matrix;
    GLC_3DRep m_3DRep;
    GLC_Material* m_pMaterial;

    csgjs_model* m_pResultCsgModel;

    int m_Level;
};

#endif // GLC_CSGNODE_H
