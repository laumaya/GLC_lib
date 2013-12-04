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

#include "glc_viewhandlerdata.h"

#include "../glc_factory.h"
#include "../sceneGraph/glc_octree.h"

GLC_ViewHandlerData::GLC_ViewHandlerData()
    : QObject()
    , m_World()
    , m_pLight(new GLC_Light())
    , m_pViewport(new GLC_Viewport())
    , m_pMoverController(NULL)
    , m_Samples(16)
{
    m_pLight->setTwoSided(true);
    m_pLight->setPosition(10.0, 10.0, 10.0);

    m_pViewport->cameraHandle()->setEyeCam(GLC_Point3d(1.0, 1.0, 1.0));

    QColor repColor;
    repColor.setRgbF(1.0, 0.11372, 0.11372, 1.0);
    m_pMoverController= new GLC_MoverController(GLC_Factory::instance()->createDefaultMoverController(repColor, m_pViewport));
}

GLC_ViewHandlerData::~GLC_ViewHandlerData()
{
    delete m_pLight;
    delete m_pViewport;
    delete m_pMoverController;
}

void GLC_ViewHandlerData::setWorld(const GLC_World &world)
{
    m_World= world;
    GLC_Octree* pOctree= new GLC_Octree(m_World.collection());
    pOctree->updateSpacePartitioning();
    m_World.collection()->bindSpacePartitioning(pOctree);
    m_pViewport->reframe(m_World.boundingBox());

    emit isDirty();
}

void GLC_ViewHandlerData::setSamples(int samples)
{
    if (m_Samples != samples)
    {
        m_Samples= samples;
        emit isDirty();
    }
}

void GLC_ViewHandlerData::updateBackGround()
{
    //if (m_pViewport->clearBackground();
}
