/*
 *  glc_openglviewhandler.h
 *
 *  Created on: 07/02/2016
 *      Author: Laurent Ribon
 */
#ifndef GLC_OPENGLVIEWHANDLER_H
#define GLC_OPENGLVIEWHANDLER_H


#include "glc_viewhandler.h"

#include "../glc_config.h"

class GLC_OpenGLViewWidget;

class GLC_LIB_EXPORT GLC_OpenGLViewHandler : public GLC_ViewHandler
{
    Q_OBJECT

public:
    GLC_OpenGLViewHandler();
    ~GLC_OpenGLViewHandler();

public:
    virtual void updateGL(bool synchrone);
    void setOpenGLViewWidget(GLC_OpenGLViewWidget* pViewWidget);

public:
    virtual QPair<GLC_SelectionSet, GLC_Point3d> selectAndUnproject(int x, int y, GLC_SelectionEvent::Modes modes);
    virtual QImage takeScreenshot(const GLC_ScreenShotSettings& screenShotSettings);

private:
    GLC_OpenGLViewWidget* m_pViewWidget;
};

Q_DECLARE_METATYPE(GLC_OpenGLViewHandler*)

#endif // GLC_OPENGLVIEWHANDLER_H
