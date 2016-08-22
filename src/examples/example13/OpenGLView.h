/*
 *
 *
 *  Created on: 24/01/2016
 *      Author: Laurent Ribon
 */

#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include <GLC_OpenGLViewWidget>

#include <QOpenGLWidget>


class OpenGLView : public GLC_OpenGLViewWidget
{
    Q_OBJECT
public:
    explicit OpenGLView(QWidget *parent = 0);
    ~OpenGLView();

};

#endif // OPENGLVIEW_H
