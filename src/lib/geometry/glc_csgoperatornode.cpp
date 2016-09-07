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

//! \file glc_csgoperatornode.cpp Implementation for the GLC_CsgOperatorNode class.

#include "glc_mesh.h"
#include "glc_csghelper.h"

#include "glc_csgoperatornode.h"

GLC_CsgOperatorNode::GLC_CsgOperatorNode()
    : GLC_CsgNode()
    , m_OperationType(CsgUnion)
    , m_pOpe1Node(NULL)
    , m_pOpe2Node(NULL)
{

}

GLC_CsgOperatorNode::GLC_CsgOperatorNode(const GLC_Matrix4x4& matrix, const GLC_3DRep& rep, GLC_CsgOperatorNode::OperationType operationType)
    : GLC_CsgNode(matrix, rep)
    , m_OperationType(operationType)
    , m_pOpe1Node(NULL)
    , m_pOpe2Node(NULL)
{

}

GLC_CsgOperatorNode::GLC_CsgOperatorNode(const GLC_CsgOperatorNode& other, OperationType operationType)
    : GLC_CsgNode(other)
    , m_OperationType(operationType)
    , m_pOpe1Node(other.m_pOpe1Node)
    , m_pOpe2Node(other.m_pOpe2Node)
{

}

GLC_CsgOperatorNode::~GLC_CsgOperatorNode()
{
    delete m_pOpe1Node;
    delete m_pOpe2Node;
}

GLC_CsgNode* GLC_CsgOperatorNode::clone() const
{
    return new GLC_CsgOperatorNode(*this);
}

void GLC_CsgOperatorNode::update()
{
    Q_ASSERT((NULL != m_pOpe1Node) && (NULL != m_pOpe2Node));
    m_pOpe1Node->update();
    m_pOpe2Node->update();

    GLC_3DRep ope1Rep(m_pOpe1Node->get3DRep());
    GLC_3DRep ope2Rep(m_pOpe2Node->get3DRep());

    Q_ASSERT(!ope1Rep.isEmpty());
    Q_ASSERT(!ope2Rep.isEmpty());

    Q_ASSERT(ope1Rep.numberOfBody() == 1);
    Q_ASSERT(ope1Rep.numberOfBody() == 1);

    GLC_Mesh* pMesh1= dynamic_cast<GLC_Mesh*>(ope1Rep.geomAt(0));
    GLC_Mesh* pMesh2= dynamic_cast<GLC_Mesh*>(ope2Rep.geomAt(0));

    Q_ASSERT(NULL != pMesh1);
    Q_ASSERT(NULL != pMesh2);

    GLC_Mesh* pOperatorMesh= dynamic_cast<GLC_Mesh*>(m_3DRep.geomAt(0));
    Q_ASSERT(NULL != pOperatorMesh);

    pOperatorMesh->clear();
    if (m_OperationType == CsgDifference)
    {
        GLC_CsgHelper::soustract(pOperatorMesh, pMesh1, m_pOpe1Node->matrix(), pMesh2, m_pOpe2Node->matrix(), m_pMaterial);
    }
    else if (m_OperationType == CsgIntersection)
    {
        GLC_CsgHelper::intersection(pOperatorMesh, pMesh1, m_pOpe1Node->matrix(), pMesh2, m_pOpe2Node->matrix(), m_pMaterial);
    }
    else
    {
        GLC_CsgHelper::add(pOperatorMesh, pMesh1, m_pOpe1Node->matrix(), pMesh2, m_pOpe2Node->matrix(), m_pMaterial);
    }
}

void GLC_CsgOperatorNode::setChildNodes(GLC_CsgNode* pNode1, GLC_CsgNode* pNode2)
{
    Q_ASSERT(NULL != pNode1);
    Q_ASSERT(NULL != pNode2);

    if ((pNode1 != m_pOpe1Node) && (pNode2 != m_pOpe1Node))
    {
        delete m_pOpe1Node;
    }

    if ((pNode1 != m_pOpe2Node) && (pNode2 != m_pOpe2Node))
    {
        delete m_pOpe2Node;
    }

    m_pOpe1Node= pNode1;
    m_pOpe2Node= pNode2;
}
