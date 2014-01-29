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
//! \file glc_quickview.h interface for the GLC_QuickView class.

#ifndef GLC_QUICKVIEW_H
#define GLC_QUICKVIEW_H

#include <QQuickView>

#include "../glc_config.h"

class GLC_Context;
class QWindow;
class QUrl;
class QQmlEngine;

//////////////////////////////////////////////////////////////////////
//! \class GLC_QuickView
/*! \brief GLC_QuickView : Provide a shared OpenGL context in the GUI thread
 * of the scene graph rendering thread OpenGL context */

/*! The GLC_QuickItem make it possible to render a GLC_World int a QML scene Graph*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_QuickView : public QQuickView
{
    Q_OBJECT
public:
    GLC_QuickView(QWindow* pParent = 0);
    GLC_QuickView(QQmlEngine* pEngine, QWindow* pParent);
    GLC_QuickView(const QUrl& source, QWindow* pParent = 0);
    virtual ~GLC_QuickView();

public:
    GLC_Context* guiThreadContext() const;

private slots:
    void sceneGraphInitializedDone();

private:
    void initConnection();

private:

    //! The shared context of the guiThread
    GLC_Context* m_pContext;
};

#endif // GLC_QUICKVIEW_H
