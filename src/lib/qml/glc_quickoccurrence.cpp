/*
 *  glc_quickoccurrence.cpp
 *
 *  Created on: 13/05/2014
 *      Author: Laurent Ribon
 */

#include "../sceneGraph/glc_structoccurrence.h"
#include "../sceneGraph/glc_structreference.h"
#include "../sceneGraph/glc_structinstance.h"

#include "glc_quickoccurrence.h"

GLC_QuickOccurrence::GLC_QuickOccurrence(GLC_StructOccurrence *pOcc)
    : QObject()
    , m_Id(pOcc->id())
    , m_ReferenceName(pOcc->structReference()->name())
    , m_InstanceName(pOcc->structInstance()->name())
{

}

