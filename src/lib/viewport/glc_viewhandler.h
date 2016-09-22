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
#include <QMutex>

#include "../shading/glc_light.h"
#include "../sceneGraph/glc_world.h"
#include "../glc_selectionevent.h"
#include "../maths/glc_vector3d.h"
#include "../3DWidget/glc_3dwidgetmanager.h"
#include "glc_screenshotsettings.h"
#include "glc_userinput.h"

#include "../glc_config.h"

class GLC_Viewport;
class GLC_MoverController;
class GLC_SpacePartitioning;
class GLC_InputEventInterpreter;
class GLC_QuickView;

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
    void invalidateSelectionBuffer();
    void acceptHoverEvent(bool track);
    void selectionChanged();

    // Error signals
    void frameBufferCreationFailed();
    void frameBufferBindingFailed();


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

    inline GLC_SelectionEvent::Modes selectionModes() const
    {return m_SelectionModes;}

    inline GLC_SelectionEvent::Mode selectionMode() const
    {return m_SelectionMode;}

    inline QPoint pointerPosition() const
    {return m_PointerPosition;}

    inline GLC_InputEventInterpreter* eventInterpreter() const
    {return m_pInputEventInterpreter;}

    inline bool isEnable() const
    {return m_Enabled;}

    inline bool hasVisibleWidget() const
    {return m_3DWidgetManager.hasVisibleWidget();}

    inline GLC_3DWidgetManager get3DWidgetManager() const
    {return m_3DWidgetManager;}

    inline bool screenShotModeIsOn() const
    {return m_ScreenShotMode;}

    inline const GLC_ScreenShotSettings& screenShotSettings() const
    {return m_ScreenshotSettings;}

    inline glc::RenderFlag currentRenderFlag() const
    {return m_RenderFlag;}

    bool spacePartitionningEnabled() const;
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public slots:
    virtual void updateGL(bool synchrone= false)= 0;

    virtual void clearSelectionBuffer();

public:
    virtual void renderingFinished(){}

    virtual void setDefaultUpVector(const GLC_Vector3d &vect);

    void setInputEventInterpreter(GLC_InputEventInterpreter* pEventInterpreter);

    virtual void setWorld(const GLC_World& world);

    void setSamples(int samples);

    void setSpacePartitionningEnabled(bool enabled);

    void setSpacePartitioning(GLC_SpacePartitioning* pSpacePartitioning);

    void unSetSpacePartitionning();

    virtual QPair<GLC_SelectionSet, GLC_Point3d> selectAndUnproject(int x, int y, GLC_SelectionEvent::Modes modes)= 0;

    virtual void unsetSelection();

    virtual void selectionUpdated(const GLC_SelectionEvent &selectionEvent);

    virtual void setSize(int width, int height, int devicePixelRatio= 1);

    virtual void setMouseTracking(bool track);

    void updateCurrentSelectionSet(const GLC_SelectionSet &selectionSet, const GLC_Point3d& point);

    inline void setEnable(bool enabled)
    {m_Enabled= enabled;}

    void setLight(GLC_Light* pLight);

    inline void setCurrentRenderFlag(glc::RenderFlag renderFlag)
    {m_RenderFlag= renderFlag;}

    inline void setSelectionMode(GLC_SelectionEvent::Mode mode)
    {m_SelectionMode= mode;}

    void setScreenShotMode(bool mode)
    {m_ScreenShotMode= mode;}
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
    virtual void processHoverMoveEvent(QHoverEvent* pHoverEvent);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    virtual void renderBackGround();
    virtual void render();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Protected services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
protected:


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
    GLC_SelectionEvent::Mode m_SelectionMode;
    GLC_SelectionEvent::Modes m_SelectionModes;
    GLC_SelectionSet m_CurrentSelectionSet;
    GLC_Point3d m_UnprojectedPoint;
    GLC_3DWidgetManager m_3DWidgetManager;

    bool m_ScreenShotMode;
    GLC_ScreenShotSettings m_ScreenshotSettings;

    glc::RenderFlag m_RenderFlag;

private:
    bool m_Enabled;
};

Q_DECLARE_METATYPE(GLC_ViewHandler*)

#endif // GLC_VIEWHANDLER_H
