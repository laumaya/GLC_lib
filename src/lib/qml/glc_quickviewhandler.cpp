/*
 *  glc_quickviewhandler.cpp
 *
 *  Created on: 07/02/2016
 *      Author: Laurent Ribon
 */
#include "glc_quickviewhandler.h"

GLC_QuickViewHandler::GLC_QuickViewHandler()
    : GLC_ViewHandler()
    , m_isRendering(false)
    , m_ScreenShotImage()
{

}

GLC_QuickViewHandler::~GLC_QuickViewHandler()
{

}

void GLC_QuickViewHandler::renderingFinished()
{
    m_isRendering= false;
}

void GLC_QuickViewHandler::updateGL(bool synchrone)
{
    m_isRendering= synchrone;
    emit isDirty();
    while (isEnable() && m_isRendering)
    {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

QImage GLC_QuickViewHandler::takeScreenshot(const GLC_ScreenShotSettings &screenShotSettings)
{
    m_ScreenshotSettings= screenShotSettings;
    m_ScreenShotMode= true;
    updateGL(true);  // Execute OpenGL synchronously to get screenshot image
    m_ScreenShotMode= false;
    return m_ScreenShotImage;
}

QPair<GLC_SelectionSet, GLC_Point3d> GLC_QuickViewHandler::selectAndUnproject(int x, int y, GLC_SelectionEvent::Modes modes)
{
    m_RenderingMode= GLC_ViewHandler::selectRenderMode;
    m_PointerPosition.setX(x);
    m_PointerPosition.setY(y);
    m_CurrentSelectionSet.clear();
    m_UnprojectedPoint.setVect(0.0, 0.0, 0.0);
    m_SelectionModes= modes;
    updateGL(true); // Execute OpenGL synchronously to get selection Set
    QPair<GLC_SelectionSet, GLC_Point3d> subject(m_CurrentSelectionSet, m_UnprojectedPoint);
    return subject;
}

void GLC_QuickViewHandler::updateSynchronized()
{
    updateGL(true);
}

void GLC_QuickViewHandler::setScreenShotImage(const QImage &image)
{
    m_ScreenShotImage= image;
}
