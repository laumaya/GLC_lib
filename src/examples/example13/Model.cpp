/*
 *  Model.cpp
 *
 *  Created on: 22/08/2016
 *      Author: Laurent Ribon
 */

#include "GLC_CsgLeafNode"
#include "GLC_CsgOperatorNode"
#include "GLC_Factory"
#include "GLC_3DRep"
#include "GLC_StructOccurrence"
#include "Model.h"

Model::Model()
    : m_World()
    , m_pRootCsgNode(NULL)
    , m_pChild2Node()
{
    init();
    update();
}

Model::~Model()
{
    delete m_pRootCsgNode;
}

void Model::updateChildMatrix(const GLC_Matrix4x4& matrix)
{
    m_pChild2Node->setMatrix(matrix * m_pChild2Node->matrix());
}

void Model::update()
{
    m_pRootCsgNode->update();
}

void Model::init()
{
    GLC_3DRep geom1(GLC_Factory::instance()->createBox(20, 20, 2));
    GLC_3DRep geom2(GLC_Factory::instance()->createCylinder(2.0, 10.0));

    GLC_CsgLeafNode* pNode1= new GLC_CsgLeafNode(GLC_Matrix4x4(), geom1);
    pNode1->setMatrix(GLC_Matrix4x4(glc::X_AXIS, glc::toRadian(24)));
    m_pChild2Node= new GLC_CsgLeafNode(GLC_Matrix4x4(), geom2);

    GLC_CsgOperatorNode* pRootNode= new GLC_CsgOperatorNode();
    pRootNode->setChildNodes(pNode1, m_pChild2Node);
    m_pRootCsgNode= pRootNode;

    GLC_StructOccurrence* pOcc= new GLC_StructOccurrence(new GLC_3DRep(m_pRootCsgNode->get3DRep()));
    m_World.rootOccurrence()->addChild(pOcc);
}
