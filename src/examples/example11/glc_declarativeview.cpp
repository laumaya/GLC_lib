/*
 * glc_declarativeview.cpp
 *
 *  Created on: 22/04/2013
 *      Author: laumaya
 */

#include <QGLWidget>
#include <QGLContext>

#include "glc_declarativeview.h"

GLC_QuickView::GLC_QuickView(QWidget *pParent)
    : QDeclarativeView(pParent)
{
    QDeclarativeView::setViewport(new QGLWidget(new QGLContext(QGLFormat(QGL::SampleBuffers)), this));
    QDeclarativeView::setRenderHint(QPainter::SmoothPixmapTransform);
    QDeclarativeView::setRenderHint(QPainter::Antialiasing);
    QDeclarativeView::setResizeMode(QDeclarativeView::SizeViewToRootObject);
}
