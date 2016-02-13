/*
 *  glc_openglViewhandler.cpp
 *
 *  Created on: 07/02/2016
 *      Author: Laurent Ribon
 */

#include "glc_openglviewwidget.h"
#include "glc_openglviewhandler.h"

GLC_OpenGLViewHandler::GLC_OpenGLViewHandler()
    : GLC_ViewHandler()
    , m_pViewWidget(NULL)
{

}

GLC_OpenGLViewHandler::~GLC_OpenGLViewHandler()
{

}

void GLC_OpenGLViewHandler::updateGL(bool synchrone)
{
    Q_UNUSED(synchrone);
    if (NULL != m_pViewWidget)
    {
        m_pViewWidget->update();
    }
}

void GLC_OpenGLViewHandler::setOpenGLViewWidget(GLC_OpenGLViewWidget *pViewWidget)
{
    m_pViewWidget= pViewWidget;
}

QPair<GLC_SelectionSet, GLC_Point3d> GLC_OpenGLViewHandler::selectAndUnproject(int x, int y, GLC_SelectionEvent::Modes modes)
{
    QPair<GLC_SelectionSet, GLC_Point3d> subject;
    if (NULL != m_pViewWidget)
    {
        m_PointerPosition.setX(x);
        m_PointerPosition.setY(y);
        m_CurrentSelectionSet.clear();
        m_UnprojectedPoint.setVect(0.0, 0.0, 0.0);
        m_SelectionModes= modes;
        m_pViewWidget->updateSelection();
        subject= QPair<GLC_SelectionSet, GLC_Point3d>(m_CurrentSelectionSet, m_UnprojectedPoint);
    }
    return subject;
}

QImage GLC_OpenGLViewHandler::takeScreenshot(const GLC_ScreenShotSettings &screenShotSettings)
{
    QImage subject;
    if (NULL != m_pViewWidget)
    {
        m_ScreenshotSettings= screenShotSettings;
        subject= m_pViewWidget->takeScreenShot();
    }
    return subject;
}
