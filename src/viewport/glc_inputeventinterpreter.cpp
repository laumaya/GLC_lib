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
#include <QMouseEvent>
#include <QTouchEvent>
#include <QWheelEvent>

#include <QtDebug>

#include "glc_viewhandlerdata.h"
#include "glc_inputeventinterpreter.h"


GLC_InputEventInterpreter::GLC_InputEventInterpreter(GLC_ViewHandlerData* pViewHandler)
    : m_pViewHandlerData(pViewHandler)
    , m_UseLodWhileMoving(true)
    , m_DefaultNavigationType(GLC_MoverController::TrackBall)
{
    Q_ASSERT(NULL != m_pViewHandlerData);
}

void GLC_InputEventInterpreter::setMover(GLC_MoverController::MoverType moverType, const GLC_UserInput &userInputs)
{
    m_pViewHandlerData->moverControllerHandle()->setActiveMover(moverType, userInputs);
    if (m_UseLodWhileMoving) {
        m_pViewHandlerData->world().collection()->setLodUsage(true, m_pViewHandlerData->viewportHandle());
    }
    m_pViewHandlerData->updateView();
}

void GLC_InputEventInterpreter::move(const GLC_UserInput &userInputs)
{
    m_pViewHandlerData->moverControllerHandle()->move(userInputs);
    m_pViewHandlerData->updateView();
}

void GLC_InputEventInterpreter::setNoMover()
{
    m_pViewHandlerData->moverControllerHandle()->setNoMover();
    if (m_UseLodWhileMoving) {
        m_pViewHandlerData->world().collection()->setLodUsage(false, m_pViewHandlerData->viewportHandle());
    }
    m_pViewHandlerData->updateView();
}

void GLC_InputEventInterpreter::select(int x, int y, GLC_SelectionEvent::Mode mode)
{
    m_pViewHandlerData->setNextSelection(x, y, mode);
}
