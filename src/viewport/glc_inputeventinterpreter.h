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

#ifndef GLC_INPUTEVENTINTERPRETER_H
#define GLC_INPUTEVENTINTERPRETER_H

#include "glc_movercontroller.h"
#include "../glc_selectionevent.h"

#include "../glc_config.h"

class QInputEvent;
class QMouseEvent;
class QTouchEvent;
class QWheelEvent;
class QHoverEvent;
class GLC_UserInput;
class GLC_Viewport;

class GLC_ViewHandler;

class GLC_LIB_EXPORT GLC_InputEventInterpreter : public QObject
{
    Q_OBJECT

public:
    explicit GLC_InputEventInterpreter(GLC_ViewHandler* pViewHandler);
    virtual ~GLC_InputEventInterpreter();

public:
    inline bool lodIsUse() const
    {return m_UseLodWhileMoving;}

    inline GLC_MoverController::MoverType defaultNavigationMoverType() const
    {return m_DefaultNavigationType;}

public:
    inline void setLodUsage(bool used)
    {m_UseLodWhileMoving= used;}

    inline void setDefaultNavigationType(GLC_MoverController::MoverType type)
    {m_DefaultNavigationType= type;}

public:
    virtual bool processMousePressEvent(QMouseEvent* pMouseEvent)= 0;
    virtual bool processMouseMoveEvent(QMouseEvent* pMouseEvent)= 0;
    virtual bool processMouseReleaseEvent(QMouseEvent* pMouseEvent)= 0;
    virtual bool processMouseDblClickEvent(QMouseEvent* pMouseEvent)= 0;
    virtual bool processWheelEvent(QWheelEvent* pWWheelEvent)= 0;

    virtual bool processTouchEvent(QTouchEvent* pTouchEvent)= 0;

    inline void setUserState(int state)
    {m_UserState= state;}

    inline int userState() const
    {return m_UserState;}

protected:
    virtual void setMover(GLC_MoverController::MoverType moverType, const GLC_UserInput &userInputs);
    virtual bool move(const GLC_UserInput& userInputs);
    virtual void setNoMover();
    virtual void select(int x, int y, GLC_SelectionEvent::Modes modes);

protected:
    GLC_ViewHandler* m_pViewHandler;
    bool m_UseLodWhileMoving;
    GLC_MoverController::MoverType m_DefaultNavigationType;
    bool m_DoubleClicking;
    int m_UserState;
};

#endif // GLC_INPUTEVENTINTERPRETER_H
