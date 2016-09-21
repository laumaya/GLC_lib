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

//! \file glc_csgoperatornode.h interface for the GLC_CsgOperatorNode class.

#ifndef GLC_CSGOPERATORNODE_H
#define GLC_CSGOPERATORNODE_H

#include "glc_csgnode.h"

#include "../glc_config.h"

class GLC_WireData;

class GLC_LIB_EXPORT GLC_CsgOperatorNode : public GLC_CsgNode
{
public:
    enum OperationType
    {
        CsgUnion,
        CsgDifference,
        CsgIntersection
    };

public:
    GLC_CsgOperatorNode();
    explicit GLC_CsgOperatorNode(const GLC_Matrix4x4& matrix, const GLC_3DRep& rep, OperationType operationType);
    explicit GLC_CsgOperatorNode(const GLC_CsgOperatorNode& other, OperationType operationType);

    virtual ~GLC_CsgOperatorNode();

public:
    const GLC_WireData& wireData() const
    {return m_WireData;}

    virtual GLC_CsgNode* clone() const;
    virtual bool update();
    virtual void createMesh();
    virtual QList<GLC_CsgNode*> chrildren() const;

public:
    void setChildNodes(GLC_CsgNode* pNode1, GLC_CsgNode* pNode2);
    void setOperationType(OperationType operationType)
    {m_OperationType= operationType;}

private:
    OperationType m_OperationType;
    GLC_CsgNode* m_pOpe1Node;
    GLC_CsgNode* m_pOpe2Node;

    GLC_WireData m_WireData;
};



#endif // GLC_CSGOPERATORNODE_H
