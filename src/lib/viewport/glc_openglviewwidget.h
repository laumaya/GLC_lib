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
    ~GLC_OpenGLViewWidget();
public:
    //! Return the used GLC_ViewHandler as a QVariant
    virtual QVariant viewHandler() const;

signals:
    void spacePartitionningEnabledChanged(bool arg);
    void selectionChanged();
    void frameBufferCreationFailed();
    void frameBufferBindingFailed();

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);

protected:
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);
    virtual void mouseDoubleClickEvent(QMouseEvent * e);
    virtual void wheelEvent(QWheelEvent * e);
    virtual void touchEvent(QTouchEvent * e);
    virtual void hoverMoveEvent(QHoverEvent *event);

    virtual void initConnections();

    void renderForSelection();
    void renderForScreenShot();

    GLC_uint selectBody(GLC_uint instanceId, int x, int y);
    QPair<GLC_uint, GLC_uint> selectPrimitive(GLC_uint instanceId, int x, int y);

    virtual void doRender();
    void setupAuxFbo(int width, int height);
    void setupScreenShotFbo(int width, int height);

public slots:
    virtual void setViewhandler(QVariant viewHandler);
    virtual void invalidateSelectionBuffer();
    virtual void setMouseTracking(bool track);
    virtual void setSpacePartitionningEnabled(bool enabled);
    virtual void setDefaultUpVector(const QVector3D &vect);

    void select(uint id);

private:

private:
    QOpenGLFramebufferObject* m_pAuxFbo;
    QOpenGLFramebufferObject* m_pScreenShotFbo;
    QSharedPointer<GLC_OpenGLViewHandler> m_Viewhandler;
};

#endif // GLC_OPENGLVIEWWIDGET_H
