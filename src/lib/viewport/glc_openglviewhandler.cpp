/*
 *  glc_openglViewhandler.cpp
 *
 *  Created on: 07/02/2016
 *      Author: Laurent Ribon
 */

#include "glc_openglviewinterface.h"
#include "glc_openglviewhandler.h"

GLC_OpenGLViewHandler::GLC_OpenGLViewHandler()
    : GLC_ViewHandler()
    , m_pViewInterface(nullptr)
{

}

GLC_OpenGLViewHandler::~GLC_OpenGLViewHandler()
{

}

void GLC_OpenGLViewHandler::updateGL(bool synchrone)
{
    if (nullptr != m_pViewInterface)
    {

        if (synchrone)
        {
            m_pViewInterface->interfaceRepaint();
        }
        else
        {
            m_pViewInterface->interfaceUpdate();
        }
    }
}

void GLC_OpenGLViewHandler::setOpenGLViewWidget(GLC_OpenGLViewInterface* pViewInterface)
{
    m_pViewInterface= pViewInterface;
}

void GLC_OpenGLViewHandler::updateSelectionBufferOnRender(bool update)
{
    m_pViewInterface->interfaceUpdateSelectionBufferOnRender(update);
}

void GLC_OpenGLViewHandler::updateViewBufferOnRender(bool update)
{
    m_pViewInterface->interfaceUpdateViewBufferOnRender(update);
}

QPair<GLC_SelectionSet, GLC_Point3d> GLC_OpenGLViewHandler::selectAndUnproject(int x, int y, GLC_SelectionEvent::Modes modes)
{
    QPair<GLC_SelectionSet, GLC_Point3d> subject;
    if (nullptr != m_pViewInterface)
    {
        m_PointerPosition.setX(x);
        m_PointerPosition.setY(y);
        m_CurrentSelectionSet.clear();
        m_UnprojectedPoint.setVect(0.0, 0.0, 0.0);
        m_SelectionModes= modes;
        m_pViewInterface->interfaceUpdateSelection();
        subject= QPair<GLC_SelectionSet, GLC_Point3d>(m_CurrentSelectionSet, m_UnprojectedPoint);
    }
    return subject;
}

QImage GLC_OpenGLViewHandler::takeScreenshot(const GLC_ScreenShotSettings &screenShotSettings)
{
    QImage subject;
    if (nullptr != m_pViewInterface)
    {
        m_ScreenshotSettings= screenShotSettings;
        subject= m_pViewInterface->interfaceTakeScreenShot();
    }
    return subject;
}
