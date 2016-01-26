/*
 *
 *
 *  Created on: 24/01/2016
 *      Author: Laurent Ribon
 */

#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include <GLC_Viewport>
#include <GLC_MoverController>
#include <GLC_Light>
#include <GLC_World>

#include <QOpenGLWidget>

class OpenGLView : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit OpenGLView(QWidget *parent = 0);

public:
    void setWorld(const GLC_World& world);

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);

signals:

public slots:

private:
    GLC_Viewport m_GlView;
    GLC_MoverController m_MoverController;
    GLC_Light m_Light;
    GLC_World m_World;
};

#endif // OPENGLVIEW_H
