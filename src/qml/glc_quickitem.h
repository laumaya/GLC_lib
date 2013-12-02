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
//! \file glc_quickitem.h interface for the GLC_QuickItem class.

#ifndef GLC_QUICKITEM_H
#define GLC_QUICKITEM_H

#include <QQuickItem>
#include <QtOpenGL>
#include <QUrl>
#include <QOpenGLFramebufferObject>

#include "../viewport/glc_viewport.h"
#include "../shading/glc_light.h"
#include "../sceneGraph/glc_world.h"
#include "../viewport/glc_movercontroller.h"

#include "../glc_config.h"

class QSGSimpleTextureNode;
class QGLFramebufferObject;

//////////////////////////////////////////////////////////////////////
//! \class GLC_QuickItem
/*! \brief GLC_QuickItem : Provide a way to use GLC_lib into a QQuickItem*/

/*! The GLC_QuickItem make it possible to render a GLC_World int a QML scene Graph
*/

//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_QuickItem : public QQuickItem
{
    Q_OBJECT

    //! The world to render into this QQuickItem
    Q_PROPERTY(QVariant world READ worldVariant WRITE setWorld)

public:
    explicit GLC_QuickItem(GLC_QuickItem* pParent= NULL);
    virtual ~GLC_QuickItem();

    //! Return the used GLC_World as a QVariant
    QVariant worldVariant() const;

    //! Returned the used GLC_World
    inline GLC_World world() const
    {return m_World;}

public slots:
    void setWorld(QVariant worldVariant);

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode* pNode, UpdatePaintNodeData* pData);
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);

protected:
    void setOpenGLState();
    void render(QSGSimpleTextureNode* pTextureNode, UpdatePaintNodeData* pData);
    void renderForSelection(QSGSimpleTextureNode* pTextureNode, UpdatePaintNodeData *pData);
    virtual void doRender();
    void defaultRenderWorld();
    void setupFbo(int width, int height, QSGSimpleTextureNode *pTextureNode);
    void setupSelectionFbo(int width, int height);
    void pushOpenGLState();
    void popOpenGLState();
    void select(qreal x, qreal y);

protected:
    GLC_Viewport m_Viewport;
    GLC_World m_World;
    GLC_Light m_Light;
    GLC_MoverController m_MoverController;
    bool m_FirstRender;
    QOpenGLFramebufferObject* m_pSourceFbo;
    QOpenGLFramebufferObject* m_pTargetFbo;
    QOpenGLFramebufferObject* m_pSelectionFbo;
    GLC_Vector2d m_CurrentPos;
    bool m_IsinSelectionMode;
};

#endif // GLC_QUICKITEM_H
