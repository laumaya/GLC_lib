/*
 *  Model.cpp
 *
 *  Created on: 22/08/2016
 *      Author: Laurent Ribon
 */

#include <GLC_CsgLeafNode>
#include <GLC_CsgOperatorNode>
#include <GLC_Factory>
#include <GLC_3DRep>
#include <GLC_StructOccurrence>
#include "Model.h"
#include <GLC_ExtrudedMesh>

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
    m_pRootCsgNode->createMesh();
}

void Model::init()
{
    GLC_Point3d p1(-20.0, 20.0, 0.0);
    GLC_Point3d p2(-20.0, -20.0, 0.0);
    GLC_Point3d p3(20.0, -20.0, 0.0);
    GLC_Point3d p4(20.0, 20.0, 0.0);
    QList<GLC_Point3d> list1;
    list1 << p1 << p2 << p3 << p4;

    GLC_Point3d p5(-10.0, 10.0, 0.0);
    GLC_Point3d p6(-10.0, -10.0, 0.0);
    GLC_Point3d p7(10.0, -10.0, 0.0);
    GLC_Point3d p8(10.0, 10.0, 0.0);
    QList<GLC_Point3d> list2;
    list2 << p5 << p6 << p7 << p8;

    GLC_3DRep geom1(new GLC_Box(40, 40, 2));
    GLC_3DRep geom2(new GLC_ExtrudedMesh(list2, glc::Z_AXIS, 2.0));

    GLC_CsgLeafNode* pNode1= new GLC_CsgLeafNode(GLC_Matrix4x4(0.0, 0.0, 1.0), geom1);
    m_pChild2Node= new GLC_CsgLeafNode(GLC_Matrix4x4(0.0, 0.0, 0.0), geom2);

    GLC_CsgOperatorNode* pRootNode= new GLC_CsgOperatorNode();
    pRootNode->setChildNodes(pNode1, m_pChild2Node);
    pRootNode->setOperationType(GLC_CsgOperatorNode::CsgDifference);
    m_pRootCsgNode= pRootNode;

    GLC_StructOccurrence* pOcc= new GLC_StructOccurrence(new GLC_3DRep(m_pRootCsgNode->get3DRep()));
    m_World.rootOccurrence()->addChild(pOcc);
}
