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
#include <QPair>
#include <QSharedPointer>
#include <QList>

#include "../viewport/glc_viewport.h"
#include "../shading/glc_light.h"
#include "../sceneGraph/glc_world.h"
#include "../viewport/glc_movercontroller.h"
#include "../maths/glc_vector3d.h"
#include "glc_quickcamera.h"
#include "glc_quickselection.h"
#include "GLC_QuickViewHandler"

#include "../glc_config.h"

class QSGSimpleTextureNode;
class QGLFramebufferObject;

//////////////////////////////////////////////////////////////////////
//! \class GLC_QuickItem
/*! \brief GLC_QuickItem : Provide a way to use GLC_lib into a QQuickItem*/

/*! The GLC_QuickItem make it possible to render a GLC_World int a QML scene Graph*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_QuickItem : public QQuickItem
{
    Q_OBJECT

    //! The view handler of this GLC_QuickItem
    Q_PROPERTY(QVariant viewHandler READ viewHandler WRITE setViewhandler)

    //! The source 3D file name to load
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

    //! Space partitionning usage
    Q_PROPERTY(bool spacePartitionningEnabled READ spacePartitionningEnabled WRITE setSpacePartitionningEnabled NOTIFY spacePartitionningEnabledChanged)

    //! Camera of this view
    Q_PROPERTY(GLC_QuickCamera* camera READ camera)

    //! Default up vector
    Q_PROPERTY(QVector3D defaultUpVector READ defaultUpVector WRITE setDefaultUpVector)

    //! Current selection
    Q_PROPERTY(GLC_QuickSelection* selection READ selection)


//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
    explicit GLC_QuickItem(GLC_QuickItem* pParent= NULL);
    virtual ~GLC_QuickItem();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Return the used GLC_QuickViewHandler as a QVariant
    virtual QVariant viewHandler() const;

    inline QString source() const
    {return m_Source;}

    bool spacePartitionningEnabled() const;

    GLC_QuickCamera* camera() const
    {return m_pCamera;}

    QVector3D defaultUpVector() const;

    GLC_QuickSelection* selection() const
    {return m_pQuickSelection;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////

public slots:
    virtual void setViewhandler(QVariant viewHandler);
    virtual void invalidateSelectionBuffer();
    virtual void setMouseTracking(bool track);
    virtual void setSource(QString arg);
    virtual void setSpacePartitionningEnabled(bool enabled);
    virtual void setDefaultUpVector(const QVector3D &vect);

    void select(uint id);

//@}

signals:
    void sourceChanged(QString arg);
    void spacePartitionningEnabledChanged(bool arg);
    void selectionChanged();
    void frameBufferCreationFailed();
    void frameBufferBindingFailed();

//////////////////////////////////////////////////////////////////////
/*! \name QQuickItem interface*/
//@{
    //////////////////////////////////////////////////////////////////////

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode* pNode, UpdatePaintNodeData* pData);
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);
    virtual void mouseDoubleClickEvent(QMouseEvent * e);
    virtual void wheelEvent(QWheelEvent * e);
    virtual void touchEvent(QTouchEvent * e);
    virtual void hoverMoveEvent(QHoverEvent *event);
//@}

//////////////////////////////////////////////////////////////////////
// Protected services functions
//////////////////////////////////////////////////////////////////////
protected:
    virtual void setOpenGLState();
    virtual void initConnections();

    void render(QSGSimpleTextureNode* pTextureNode, UpdatePaintNodeData* pData);
    void renderForSelection();
    void renderForScreenShot();

    GLC_uint selectBody(GLC_uint instanceId, int x, int y);
    QPair<GLC_uint, GLC_uint> selectPrimitive(GLC_uint instanceId, int x, int y);

    virtual void doRender();
    void setupFbo(int width, int height, QSGSimpleTextureNode *pTextureNode);
    void setupAuxFbo(int width, int height);
    void setupScreenShotFbo(int width, int height);

    void pushOpenGLMatrix();
    void popOpenGLMatrix();
    void deleteViewBuffers();

//////////////////////////////////////////////////////////////////////
// Protected Members
//////////////////////////////////////////////////////////////////////
protected:
    QSharedPointer<GLC_QuickViewHandler> m_Viewhandler;
    QOpenGLFramebufferObject* m_pSourceFbo;
    QOpenGLFramebufferObject* m_pTargetFbo;
    QOpenGLFramebufferObject* m_pAuxFbo;
    QOpenGLFramebufferObject* m_pScreenShotFbo;
    bool m_SelectionBufferIsDirty;
    GLC_Point3d m_UnprojectedPoint;

    GLC_QuickCamera* m_pCamera;

    QString m_Source;

    GLC_QuickSelection* m_pQuickSelection;
};

#endif // GLC_QUICKITEM_H
