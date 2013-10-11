/*
 * glc_declarativeitem.h
 *
 *  Created on: 22/04/2013
 *      Author: laumaya
 */

#ifndef GLC_DECLARATIVEITEM_H
#define GLC_DECLARATIVEITEM_H

#include <QDeclarativeItem>
#include <QtOpenGL>
#include <QUrl>

#include <GLC_Viewport>
#include <GLC_Light>
#include <GLC_World>
#include <GLC_MoverController>

class QGLFramebufferObject;

class GLC_DeclarativeItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QVariant world READ world WRITE setWorld)

public:
    explicit GLC_DeclarativeItem(QDeclarativeItem* pParent= NULL);
    virtual ~GLC_DeclarativeItem();

    QVariant world() const;

public slots:
    void setWorld(QVariant worldVariant);

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * e);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * e);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * e);

protected:
    void initGl();
    void render(QPainter *painter);
    void renderForSelection(QPainter* painter);
    void renderWorld();
    void setupFbo(int width, int height);
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
    QGLFramebufferObject* m_pSourceFbo;
    QGLFramebufferObject* m_pTargetFbo;
    QGLFramebufferObject* m_pSelectionFbo;
    GLC_Vector2d m_CurrentPos;
    bool m_IsinSelectionMode;
};

#endif // GLC_DECLARATIVEITEM_H
