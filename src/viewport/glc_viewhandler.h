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

#ifndef GLC_VIEWHANDLER_H
#define GLC_VIEWHANDLER_H

#include <QObject>
#include <QColor>
#include <QPoint>

#include "../shading/glc_light.h"
#include "../sceneGraph/glc_world.h"
#include "../glc_selectionevent.h"
#include "../maths/glc_vector3d.h"

#include "glc_userinput.h"

#include "../glc_config.h"

class GLC_Viewport;
class GLC_MoverController;
class GLC_SpacePartitioning;
class GLC_InputEventInterpreter;

class GLC_LIB_EXPORT GLC_ViewHandler: public QObject
{
    Q_OBJECT

public:
    enum RenderingMode {
        normalRenderMode,
        selectRenderMode,
        unprojectRenderMode
    };

public:
    explicit GLC_ViewHandler(QObject* pParent= NULL);
    virtual ~GLC_ViewHandler();

signals :
    void isDirty();
    void invalidateSelectionBuffer();
    void userInputUpdated(const GLC_UserInput& userInput);

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    inline GLC_World world() const
    {return m_World;}

    inline GLC_Light* lightHandle() const
    {return m_pLight;}

    inline GLC_Viewport* viewportHandle() const
    {return m_pViewport;}

    inline int samples() const
    {return m_Samples;}

    inline GLC_MoverController* moverControllerHandle() const
    {return m_pMoverController;}

    inline GLC_ViewHandler::RenderingMode renderingMode() const
    {return (m_RenderingMode);}

    inline bool selectionMode() const
    {return m_SelectionModes;}

    inline QPoint pointerPosition() const
    {return m_PointerPosition;}

    inline GLC_InputEventInterpreter* eventInterpreter() const
    {return m_pInputEventInterpreter;}

    inline bool updateIsBlocked() const
    {return m_BlockUpdate;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public slots:
    virtual void updateGL();

public:
    void setInputEventInterpreter(GLC_InputEventInterpreter* pEventInterpreter);

    virtual void setWorld(const GLC_World& world);

    void setSamples(int samples);

    void setSpacePartitioning(GLC_SpacePartitioning* pSpacePartitioning);

    virtual void setNextSelection(int x, int y, GLC_SelectionEvent::Modes modes);

    virtual void unsetSelection();

    virtual void updateSelection(GLC_uint id);

    virtual void setUnprojectedPoint(const GLC_Point3d& point);

    inline void blockUpdate(bool blocked)
    {m_BlockUpdate= blocked;}

    void setLight(GLC_Light* pLight);

    void schedulesUnprojectePoint(int x, int y);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Event handling Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

    virtual void processMousePressEvent(QMouseEvent* pMouseEvent);
    virtual void processMouseMoveEvent(QMouseEvent* pMouseEvent);
    virtual void processMouseReleaseEvent(QMouseEvent* pMouseEvent);
    virtual void processMouseDblClickEvent(QMouseEvent* pMouseEvent);
    virtual void processWheelEvent(QWheelEvent* pWWheelEvent);

    virtual void processTouchEvent(QTouchEvent* pTouchEvent);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    void updateBackGround();
    virtual void render();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Protected services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    virtual void setDefaultUpVector(const GLC_Vector3d &vect);

//@}

protected:
    GLC_World m_World;
    GLC_Light* m_pLight;
    GLC_Viewport* m_pViewport;
    GLC_MoverController* m_pMoverController;
    int m_Samples;
    GLC_SpacePartitioning* m_pSpacePartitioning;
    GLC_InputEventInterpreter* m_pInputEventInterpreter;
    GLC_ViewHandler::RenderingMode m_RenderingMode;
    QPoint m_PointerPosition;
    GLC_SelectionEvent::Modes m_SelectionModes;
    GLC_Point3d m_UnprojectedPoint;

private:
    bool m_BlockUpdate;
};

Q_DECLARE_METATYPE(GLC_ViewHandler*)

#endif // GLC_VIEWHANDLER_H
