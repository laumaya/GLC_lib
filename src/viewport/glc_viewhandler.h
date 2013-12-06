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

#ifndef GLC_QUICKITEMANCHOR_H
#define GLC_QUICKITEMANCHOR_H

#include <QSharedPointer>
#include <QMetaType>

#include "glc_viewhandlerdata.h"

#include "glc_viewport.h"
#include "glc_movercontroller.h"
#include "../shading/glc_light.h"
#include "../sceneGraph/glc_world.h"

#include "../glc_config.h"

class GLC_LIB_EXPORT GLC_ViewHandler
{
public:
    GLC_ViewHandler();
    GLC_ViewHandler(const GLC_ViewHandler& other);
    GLC_ViewHandler(const QSharedPointer<GLC_ViewHandlerData> &sharedData);
    virtual ~GLC_ViewHandler();

public:
    inline GLC_World world() const
    {return m_Data.data()->world();}

    inline GLC_Light* lightHandle() const
    {return m_Data.data()->lightHandle();}

    inline GLC_Viewport* viewportHandle() const
    {return m_Data.data()->viewportHandle();}

    inline int samples() const
    {return m_Data.data()->samples();}

    inline GLC_MoverController* moverControllerHandle() const
    {return m_Data.data()->moverControllerHandle();}

    GLC_ViewHandlerData* data() const
    {return m_Data.data();}

    inline bool isInSelectionMode() const
    {return m_Data.data()->isInSelectionMode();}

    inline QPoint selectionPoint() const
    {return m_Data.data()->selectionPoint();}


public:

    inline void processMousePressEvent(QMouseEvent* pMouseEvent)
    {m_Data.data()->processMousePressEvent(pMouseEvent);}

    inline void processMouseMoveEvent(QMouseEvent* pMouseEvent)
    {m_Data.data()->processMouseMoveEvent(pMouseEvent);}

    inline void processMouseReleaseEvent(QMouseEvent* pMouseEvent)
    {m_Data.data()->processMouseReleaseEvent(pMouseEvent);}

    inline void processMouseDblClickEvent(QMouseEvent* pMouseEvent)
    {m_Data.data()->processMouseDblClickEvent(pMouseEvent);}

    inline void processWheelEvent(QWheelEvent* pWWheelEvent)
    {m_Data.data()->processWheelEvent(pWWheelEvent);}

    inline void processTouchEvent(QTouchEvent* pTouchEvent)
    {m_Data.data()->processTouchEvent(pTouchEvent);}

public:

    inline void updateQuickItem()
    {m_Data.data()->updateView();}

    inline void setWorld(const GLC_World& world)
    {m_Data.data()->setWorld(world);}

    inline void setSamples(int samples)
    {m_Data.data()->setSamples(samples);}

    inline void setSpacePartitioning(GLC_SpacePartitioning* pSpacePartitioning)
    {m_Data.data()->setSpacePartitioning(pSpacePartitioning);}

    inline void setNextSelection(int x, int y, GLC_SelectionEvent::Mode mode)
    {m_Data.data()->setNextSelection(x, y, mode);}

    inline void unsetSelection()
    {m_Data.data()->unsetSelection();}

    inline void updateSelection(GLC_uint id)
    {m_Data.data()->updateSelection(id);}


private:
    QSharedPointer<GLC_ViewHandlerData> m_Data;
};

Q_DECLARE_METATYPE(GLC_ViewHandler)

#endif // GLC_QUICKITEMANCHOR_H
