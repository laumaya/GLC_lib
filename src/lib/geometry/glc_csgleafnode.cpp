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

//! \file glc_csgleafnode.cpp Implementation for the GLC_CsgLeafNode class.

#include "../3rdparty/csgjs/csgjs.h"

#include "glc_mesh.h"
#include "glc_csghelper.h"

#include "glc_csgleafnode.h"

GLC_CsgLeafNode::GLC_CsgLeafNode()
    : GLC_CsgNode()
{

}

GLC_CsgLeafNode::GLC_CsgLeafNode(const GLC_Matrix4x4& matrix, const GLC_3DRep& rep)
    : GLC_CsgNode(matrix, rep)
{

}

GLC_CsgLeafNode::GLC_CsgLeafNode(const GLC_CsgLeafNode& other)
    : GLC_CsgNode(other)
{

}

const GLC_WireData& GLC_CsgLeafNode::wireData() const
{
    Q_ASSERT(!m_3DRep.isEmpty());
    Q_ASSERT(m_3DRep.numberOfBody() == 1);

    return m_3DRep.geomAt(0)->wireData();
}

GLC_CsgNode*GLC_CsgLeafNode::clone() const
{
    return new GLC_CsgLeafNode(*this);
}

bool GLC_CsgLeafNode::update()
{
    bool subject= false;
    Q_ASSERT(!m_3DRep.isEmpty());
    Q_ASSERT(m_3DRep.numberOfBody() == 1);

    const bool geomChange= m_3DRep.geomAt(0)->update();
    if (geomChange || (m_pResultCsgModel == NULL))
    {
        delete m_pResultCsgModel;
        GLC_Mesh* pMesh= dynamic_cast<GLC_Mesh*>(m_3DRep.geomAt(0));
        Q_ASSERT(NULL != pMesh);
        m_pResultCsgModel= GLC_CsgHelper::csgModelFromMesh(pMesh, m_Matrix);
        subject= true;
    }

    return subject;
}
