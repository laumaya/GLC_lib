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

//! \file glc_csgnode.cpp Implementation for the GLC_CsgNode class.

#include <QtDebug>
#include <QtConcurrent>

#include "../3rdparty/csgjs/csgjs.h"
#include "../shading/glc_material.h"
#include "../maths/glc_geomtools.h"

#include "glc_mesh.h"

#include "glc_csgnode.h"

double GLC_CsgNode::m_EdgeDetectionAccuracy= 0.0001;
double GLC_CsgNode::m_EdgeDetectionAngleThreshold= 3.0;

GLC_CsgNode::GLC_CsgNode()
    : m_Id(glc::GLC_GenUserID())
    , m_Matrix()
    , m_3DRep()
    , m_MaterialHash()

    , m_pResultCsgModel(NULL)

    , m_Level(0)
{
    m_3DRep.addGeom(new GLC_Mesh);
}

GLC_CsgNode::GLC_CsgNode(const GLC_Matrix4x4& matrix, const GLC_3DRep& rep)
    : m_Id(glc::GLC_GenUserID())
    , m_Matrix(matrix)
    , m_3DRep(rep)
    , m_MaterialHash()

    , m_pResultCsgModel(NULL)

    , m_Level(0)
{

}

GLC_CsgNode::GLC_CsgNode(const GLC_CsgNode& other)
    : m_Id(glc::GLC_GenUserID())
    , m_Matrix(other.m_Matrix)
    , m_3DRep(other.m_3DRep)
    , m_MaterialHash(other.m_MaterialHash)

    , m_pResultCsgModel(new csgjs_model(*(other.m_pResultCsgModel)))

    , m_Level(other.m_Level)
{

}

GLC_CsgNode::~GLC_CsgNode()
{
    clearMaterialHash();
    delete m_pResultCsgModel;
}

double GLC_CsgNode::edgeDetectionAccuracy()
{
    return m_EdgeDetectionAccuracy;
}

double GLC_CsgNode::edgeDetectionAngleThreshold()
{
    return m_EdgeDetectionAngleThreshold;
}

void GLC_CsgNode::setEdgeDetectionAccuracy(double value)
{
    m_EdgeDetectionAccuracy= value;
}

void GLC_CsgNode::setEdgeDetectionAngleThresold(double value)
{
    m_EdgeDetectionAngleThreshold= value;
}

void GLC_CsgNode::setMatrix(const GLC_Matrix4x4& matrix)
{
    if (m_Matrix != matrix)
    {
        m_Matrix= matrix;
        delete m_pResultCsgModel;
        m_pResultCsgModel= NULL;
    }
}

void GLC_CsgNode::update(const GLC_Matrix4x4& matrix)
{
    setMatrix(matrix);
    update();
}

QList<QList<GLC_CsgNode*> > GLC_CsgNode::LevelList() const
{
    QList<QList<GLC_CsgNode*> > subject;
    QList<GLC_CsgNode*> firstLevel;
    firstLevel.append(const_cast<GLC_CsgNode*>(this));
    subject.append(firstLevel);
    int currentLevel= 1;

    createLvlList(&subject, currentLevel);


    return subject;
}

void GLC_CsgNode::multiThreadedUpdate()
{
    QList<QList<GLC_CsgNode*> > levelList= this->LevelList();
    std::reverse(levelList.begin(), levelList.end());
    const int count= levelList.count();
    for (int i= 0; i < count; ++i)
    {
        QList<GLC_CsgNode*> currentList= levelList.at(i);
        QtConcurrent::blockingMapped(currentList, mappedFutureFct);
    }
}

void GLC_CsgNode::createLvlList(QList<QList<GLC_CsgNode*> >* lvlList, int currentLevel) const
{
    QList<GLC_CsgNode*> children= this->chrildren();
    if (!children.isEmpty())
    {
        if (lvlList->count() <= currentLevel)
        {
            lvlList->append(QList<GLC_CsgNode*>());
        }

        QList<GLC_CsgNode*>& currentList= lvlList->operator [](currentLevel);
        currentList.append(children);
        ++currentLevel;
        const int count= children.count();
        for (int i= 0; i < count; ++i)
        {
            children.at(i)->createLvlList(lvlList, currentLevel);
        }
    }
}

void GLC_CsgNode::clearMaterialHash()
{
    QHash<GLC_uint, GLC_Material*>::iterator iMaterial= m_MaterialHash.begin();
    while (iMaterial != m_MaterialHash.end())
    {
        GLC_Material* pMaterial= iMaterial.value();
        pMaterial->delUsage(m_Id);
        if (pMaterial->isUnused())
        {
            delete pMaterial;
        }
        ++iMaterial;
    }

    m_MaterialHash.clear();
}

GLC_CsgNode*GLC_CsgNode::mappedFutureFct(GLC_CsgNode* pNode)
{
    pNode->update();
    return pNode;
}
