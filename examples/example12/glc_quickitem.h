/*
 * glc_declarativeitem.h
 *
 *  Created on: 22/04/2013
 *      Author: laumaya
 */

#ifndef GLC_QUICKITEM_H
#define GLC_QUICKITEM_H

#include <QQuickItem>
#include <QtOpenGL>
#include <QUrl>
#include <QOpenGLFramebufferObject>

#include <GLC_Viewport>
#include <GLC_Light>
#include <GLC_World>
#include <GLC_MoverController>

class QSGSimpleTextureNode;
class QGLFramebufferObject;

class GLC_QuickItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVariant world READ world WRITE setWorld)

public:
    explicit GLC_QuickItem(GLC_QuickItem* pParent= NULL);
    virtual ~GLC_QuickItem();

    QVariant world() const;

public slots:
    void setWorld(QVariant worldVariant);

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode* pNode, UpdatePaintNodeData* pData);
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);

protected:
    void initGl();
    void render(QSGSimpleTextureNode* pTextureNode, UpdatePaintNodeData* pData);
    void renderForSelection(QSGSimpleTextureNode* pTextureNode, UpdatePaintNodeData *pData);
    void renderWorld();
    void setupFbo(int width, int height, QSGSimpleTextureNode *pTextureNode);
    void setupSelectionFbo(int width, int height);
    void pushMatrix();
    void popMatrix();
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
