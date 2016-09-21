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


#include "../3rdparty/csgjs/csgjs.h"

#include "glc_mesh.h"
#include "glc_wiredata.h"
#include "glc_csghelper.h"

#include "glc_csgoperatornode.h"

GLC_CsgOperatorNode::GLC_CsgOperatorNode()
    : GLC_CsgNode()
    , m_OperationType(CsgUnion)
    , m_pOpe1Node(NULL)
    , m_pOpe2Node(NULL)

    , m_WireData()
{

}

GLC_CsgOperatorNode::GLC_CsgOperatorNode(const GLC_Matrix4x4& matrix, const GLC_3DRep& rep, GLC_CsgOperatorNode::OperationType operationType)
    : GLC_CsgNode(matrix, rep)
    , m_OperationType(operationType)
    , m_pOpe1Node(NULL)
    , m_pOpe2Node(NULL)

    , m_WireData()
{

}

GLC_CsgOperatorNode::GLC_CsgOperatorNode(const GLC_CsgOperatorNode& other, OperationType operationType)
    : GLC_CsgNode(other)
    , m_OperationType(operationType)
    , m_pOpe1Node(other.m_pOpe1Node)
    , m_pOpe2Node(other.m_pOpe2Node)

    , m_WireData(other.m_WireData)
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

bool GLC_CsgOperatorNode::update()
{
    Q_ASSERT((NULL != m_pOpe1Node) && (NULL != m_pOpe2Node));

    const bool updateNeededByOpe1= m_pOpe1Node->update();
    const bool updateNeededByOpe2= m_pOpe2Node->update();

    bool subject= (updateNeededByOpe1 || updateNeededByOpe2 || (m_pResultCsgModel == NULL));
    if (subject)
    {
        csgjs_model* pModel1= m_pOpe1Node->csgjsModel();
        csgjs_model* pModel2= m_pOpe2Node->csgjsModel();

        Q_ASSERT(NULL != pModel1);
        Q_ASSERT(NULL != pModel2);

        delete m_pResultCsgModel;
        m_pResultCsgModel= NULL;

        if (m_OperationType == CsgDifference)
        {
            m_pResultCsgModel= new csgjs_model(csgjs_difference(*pModel1, *pModel2));
        }
        else if (m_OperationType == CsgIntersection)
        {
            m_pResultCsgModel= new csgjs_model(csgjs_intersection(*pModel1, *pModel2));
        }
        else
        {
            m_pResultCsgModel= new csgjs_model(csgjs_union(*pModel1, *pModel2));
        }

        m_WireData.clear();
        m_WireData.add(m_pOpe1Node->wireData(), m_pOpe1Node->matrix());
        m_WireData.add(m_pOpe2Node->wireData(), m_pOpe2Node->matrix());
    }

    return subject;
}

void GLC_CsgOperatorNode::createMesh()
{
    Q_ASSERT(NULL != m_pResultCsgModel);
    GLC_Mesh* pMesh= GLC_CsgHelper::meshFromCsgModel(*m_pResultCsgModel, m_pMaterial);
    pMesh->addVerticeGroups(m_WireData, m_Matrix);
    m_3DRep.clear();
    m_3DRep.addGeom(pMesh);
}

QList<GLC_CsgNode*> GLC_CsgOperatorNode::chrildren() const
{
    QList<GLC_CsgNode*> subject;
    subject << m_pOpe1Node << m_pOpe2Node;

    return subject;
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
