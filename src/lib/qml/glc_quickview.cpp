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
//! \file glc_quickview.cpp implementation of the GLC_QuickView class.

#include <QOpenGLContext>
#include <QOffscreenSurface>

#include "glc_quickview.h"

#include "glc_quickitem.h"
#include "glc_qmlcamera.h"

#include "../glc_context.h"
#include "../glc_contextmanager.h"

GLC_QuickView::GLC_QuickView(QWindow *pParent)
    : QQuickView(pParent)
    , m_pContext(NULL)
    , m_pQOpenGLContext(NULL)
{
    qmlRegisterType<GLC_QuickItem>("glcview", 1, 0, "GLCView");
    qmlRegisterType<GLC_QMLCamera>();

    initConnection();
}

GLC_QuickView::GLC_QuickView(QQmlEngine *pEngine, QWindow *pParent)
    : QQuickView(pEngine, pParent)
    , m_pContext(NULL)
{
    initConnection();
}

GLC_QuickView::GLC_QuickView(const QUrl &source, QWindow *pParent)
    : QQuickView(source, pParent)
    , m_pContext(NULL)
{
    initConnection();
}

GLC_QuickView::~GLC_QuickView()
{
    delete m_pQOpenGLContext;
}

GLC_Context *GLC_QuickView::guiThreadContext() const
{
    return m_pContext;
}

void GLC_QuickView::sceneGraphInitializedDone()
{
    disconnect(this, SIGNAL(sceneGraphInitialized()), this, SLOT(sceneGraphInitializedDone()));

    QOpenGLContext* pContext= openglContext();
    Q_ASSERT(pContext);

    m_pQOpenGLContext= new QOpenGLContext();
    m_pQOpenGLContext->setShareContext(pContext);
    m_pQOpenGLContext->setFormat(pContext->format());
    m_pQOpenGLContext->create();
    Q_ASSERT(m_pQOpenGLContext->isValid());

    QOffscreenSurface* pFakeSurface = new QOffscreenSurface();
    pFakeSurface->setFormat(pContext->format());
    pFakeSurface->create();

    m_pContext= GLC_ContextManager::instance()->createContext(m_pQOpenGLContext, pFakeSurface);
    m_pContext->makeCurrent();
    qDebug() << m_pContext;
    initializeGL();
}

void GLC_QuickView::initConnection()
{
    connect(this, SIGNAL(sceneGraphInitialized()), this, SLOT(sceneGraphInitializedDone()));
}
