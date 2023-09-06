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

//! \file glc_csgleafnode.h interface for the GLC_CsgLeafNode class.
//!
#ifndef GLC_CSGLEAFNODE_H
#define GLC_CSGLEAFNODE_H

#include "glc_csgnode.h"

#include "../glc_config.h"

class GLC_LIB_EXPORT GLC_CsgLeafNode : public GLC_CsgNode
{
public:
    GLC_CsgLeafNode();
    GLC_CsgLeafNode(const GLC_Matrix4x4& matrix, const GLC_3DRep& rep);
    GLC_CsgLeafNode(const GLC_CsgLeafNode& other);

public:
    GLC_CsgNode* clone() const override;
    bool update() override;
    void createMesh(bool createSharpEdge= true) override;
    QList<GLC_CsgNode*> chrildren() const override
    {return QList<GLC_CsgNode*>();}
    bool isRootNode() const override
    {return false;}
    void setRoot(bool) override {}

protected:
    void updateMaterialHash() override;

};

#endif // GLC_CSGLEAFNODE_H
