/*
 *  glc_openglviewinterface.h
 *
 *  Created on: 17/6/2020
 *      Author: Laurent Ribon
 */
#ifndef GLC_OPENGLVIEWINTERFACE_H
#define GLC_OPENGLVIEWINTERFACE_H

#include <QImage>

#include "../glc_config.h"

class GLC_LIB_EXPORT GLC_OpenGLViewInterface
{
public:
    GLC_OpenGLViewInterface(){}

public:
    virtual QImage interfaceTakeScreenShot()= 0;
    virtual void interfaceUpdateSelection()= 0;
    virtual void interfaceUpdateSelectionBufferOnRender(bool)= 0;
    virtual void interfaceUpdateViewBufferOnRender(bool)= 0;
    virtual void interfaceUpdate()= 0;
    virtual void interfaceRepaint()= 0;
};

Q_DECLARE_METATYPE(GLC_OpenGLViewInterface*)

#endif // GLC_OPENGLVIEWINTERFACE_H
