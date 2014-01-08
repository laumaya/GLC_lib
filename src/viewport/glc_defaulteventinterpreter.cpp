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

#include "glc_defaulteventinterpreter.h"
#include "glc_viewhandler.h"
#include "glc_userinput.h"

GLC_DefaultEventInterpreter::GLC_DefaultEventInterpreter(GLC_ViewHandler* pViewHandler)
    : GLC_InputEventInterpreter(pViewHandler)
    , m_MovingCamera(false)
    , m_StartingPos()
    , m_EventButton(Qt::NoButton)
{
}

bool GLC_DefaultEventInterpreter::processMousePressEvent(QMouseEvent *e)
{
    bool subject= false;
    if (!m_MovingCamera)
    {
        m_StartingPos= e->pos();
        m_EventButton= e->button();
    }

    return subject;
}

bool GLC_DefaultEventInterpreter::processMouseMoveEvent(QMouseEvent *e)
{
    bool subject= false;

    if (!m_MovingCamera)
    {
        QPoint point= e->pos() - m_StartingPos;
        if (point.manhattanLength() > 5)
        {
            switch (m_EventButton)
            {
            case (Qt::RightButton):
                setMover(m_DefaultNavigationType, GLC_UserInput(e->x(), e->y()));
                subject= true;
                break;
            case (Qt::LeftButton):
                setMover(GLC_MoverController::Zoom, GLC_UserInput(e->x(), e->y()));
                subject= true;
                break;
            case (Qt::MidButton):
                setMover(GLC_MoverController::Pan, GLC_UserInput(e->x(), e->y()));
                subject= true;
                break;
            default:
                break;
            }
            m_MovingCamera= subject;
        }
    }
    else if (m_pViewHandler->moverControllerHandle()->hasActiveMover())
    {
        move(GLC_UserInput(e->x(), e->y()));
        subject= true;
    }

    return subject;
}

bool GLC_DefaultEventInterpreter::processMouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    bool subject= false;

    if (m_MovingCamera)
    {
        setNoMover();
        subject= true;
        m_MovingCamera= false;
    }
    else if ((e->button() == Qt::LeftButton))
    {
        select(e->x(), e->y(), GLC_SelectionEvent::ModeReplace | GLC_SelectionEvent::ModeInstance);
        subject= true;
    }

    return subject;
}

bool GLC_DefaultEventInterpreter::processMouseDblClickEvent(QMouseEvent *pMouseEvent)
{
    Q_UNUSED(pMouseEvent);
    return false;
}

bool GLC_DefaultEventInterpreter::processWheelEvent(QWheelEvent *pWheelEvent)
{
    Q_UNUSED(pWheelEvent);
    return false;
}

bool GLC_DefaultEventInterpreter::processTouchEvent(QTouchEvent *pTouchEvent)
{
    Q_UNUSED(pTouchEvent);
    return false;
}
