/*
 *  glc_openglviewwidget.h
 *
 *  Created on: 07/02/2016
 *      Author: Laurent Ribon
 */
#ifndef GLC_OPENGLVIEWWIDGET_H
#define GLC_OPENGLVIEWWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFramebufferObject>
#include <QSharedPointer>

#include "glc_openglviewhandler.h"

#include "../glc_config.h"

class GLC_LIB_EXPORT GLC_OpenGLViewWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    GLC_OpenGLViewWidget(QWidget *parent = 0);
    virtual ~GLC_OpenGLViewWidget();
public:
    //! Return the used GLC_ViewHandler as a QVariant
    virtual QVariant viewHandler() const;

    QSharedPointer<GLC_OpenGLViewHandler> sharedViewHandler()
    {return m_Viewhandler;}

public slots:
    virtual void setViewhandler(QVariant viewHandler);
    virtual void setMouseTracking(bool track);
    void updateSelection();
    void select(uint id);
    QImage takeScreenShot();

signals:
    void frameBufferCreationFailed();
    void frameBufferBindingFailed();

protected:
    virtual void initializeGL();
    virtual void paintGL();

protected:
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);
    virtual void mouseDoubleClickEvent(QMouseEvent * e);
    virtual void wheelEvent(QWheelEvent * e);
    virtual void touchEvent(QTouchEvent * e);
    virtual void hoverMoveEvent(QHoverEvent *event);

    void renderForSelection();

    GLC_uint selectBody(GLC_uint instanceId, int x, int y);
    QPair<GLC_uint, GLC_uint> selectPrimitive(GLC_uint instanceId, int x, int y);

    virtual void doRender();
    void setupSelectionFbo(int width, int height);
    void setupScreenShotFbo(int width, int height);

private:
    QOpenGLFramebufferObject* m_pSelectionFbo;
    QOpenGLFramebufferObject* m_pScreenShotFbo;
    QSharedPointer<GLC_OpenGLViewHandler> m_Viewhandler;
    GLC_Point3d m_UnprojectedPoint;
};

#endif // GLC_OPENGLVIEWWIDGET_H
