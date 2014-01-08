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

#ifndef GLC_DEFAULTEVENTINTERPRETER_H
#define GLC_DEFAULTEVENTINTERPRETER_H

#include <QPoint>

#include "glc_inputeventinterpreter.h"

class GLC_DefaultEventInterpreter : public GLC_InputEventInterpreter
{
public:
    GLC_DefaultEventInterpreter(GLC_ViewHandler* pViewHandler);

    // GLC_InputEventInterpreter interface
public:
    virtual bool processMousePressEvent(QMouseEvent *e);
    virtual bool processMouseMoveEvent(QMouseEvent *e);
    virtual bool processMouseReleaseEvent(QMouseEvent *e);
    virtual bool processMouseDblClickEvent(QMouseEvent *pMouseEvent);
    virtual bool processWheelEvent(QWheelEvent *pWheelEvent);
    virtual bool processTouchEvent(QTouchEvent *pTouchEvent);

protected:
    bool m_MovingCamera;
    QPoint m_StartingPos;
    Qt::MouseButton m_EventButton;

};

#endif // GLC_DEFAULTEVENTINTERPRETER_H
