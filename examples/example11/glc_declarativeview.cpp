/*
 * glc_declarativeview.cpp
 *
 *  Created on: 22/04/2013
 *      Author: laumaya
 */

#include <QGLWidget>

#include <GLC_Context>

#include "glc_declarativeview.h"

GLC_DeclarativeView::GLC_DeclarativeView(QWidget *pParent)
    : QDeclarativeView(pParent)
{
    QDeclarativeView::setViewport(new QGLWidget(new GLC_Context(QGLFormat(QGL::SampleBuffers)), this));
    QDeclarativeView::setRenderHint(QPainter::SmoothPixmapTransform);
    QDeclarativeView::setRenderHint(QPainter::Antialiasing);
    QDeclarativeView::setResizeMode(QDeclarativeView::SizeViewToRootObject);
}
