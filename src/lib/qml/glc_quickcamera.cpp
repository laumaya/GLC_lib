/*
 *  glc_qmlcamera.cpp
 *
 *  Created on: 02/05/2014
 *      Author: Laurent Ribon
 */

#include <QtDebug>

#include "glc_quickcamera.h"

#include "../viewport/glc_camera.h"
#include "../maths/glc_vector3d.h"


GLC_QuickCamera::GLC_QuickCamera(QObject *parent)
    : QObject(parent)
    , m_Camera(NULL)
{

}

QVector3D GLC_QuickCamera::upVector() const
{
    QVector3D subject;
    if (!m_Camera.isNull())
    {
        subject= m_Camera->upVector().toQVector3D();
    }

    return subject;
}

QVector3D GLC_QuickCamera::defaultUpVector() const
{
    QVector3D subject;
    if (!m_Camera.isNull())
    {
        subject= m_Camera->defaultUpVector().toQVector3D();
    }

    return subject;
}

void GLC_QuickCamera::setCamera(GLC_Camera *pCamera)
{
    Q_ASSERT(NULL != pCamera);

    if (!m_Camera.isNull())
    {
        disconnect(m_Camera, SIGNAL(changed()), this, SIGNAL(changed()));
    }

    m_Camera= pCamera;
    connect(m_Camera, SIGNAL(changed()), this, SIGNAL(changed()));
}

QVector3D GLC_QuickCamera::target() const
{
    QVector3D subject;
    if (!m_Camera.isNull())
    {
        subject= m_Camera->target().toQVector3D();
    }

    return subject;
}

QVector3D GLC_QuickCamera::eye() const
{
    QVector3D subject;
    if (!m_Camera.isNull())
    {
        subject= m_Camera->eye().toQVector3D();
    }

    return subject;
}

void GLC_QuickCamera::setUpVector(const QVector3D &arg)
{
    if (!m_Camera.isNull())
    {
        if (m_Camera->upVector().toQVector3D() != arg)
        {
            GLC_Vector3d vector(arg);
            m_Camera->setUpCam(vector);
            emit upVectorChanged(arg);
            emit updateView();
        }
    }
}

void GLC_QuickCamera::setDefaultUpVector(const QVector3D &arg)
{
    if (!m_Camera.isNull())
    {
        if (m_Camera->defaultUpVector().toQVector3D() != arg)
        {
            m_Camera->setDefaultUpVector(arg);
            m_Camera->setUpCam(arg);
            emit defaultUpVectorChanged(arg);
            emit updateView();
        }
    }
}

void GLC_QuickCamera::setTarget(QVector3D arg)
{
    if (!m_Camera.isNull())
    {
        if (target() != arg) {
            m_Camera->setTargetCam(GLC_Vector3d(arg));
            emit targetChanged(arg);
            emit updateView();
        }
    }
}

void GLC_QuickCamera::setEye(QVector3D arg)
{
    if (!m_Camera.isNull())
    {
        if (eye() != arg) {
            m_Camera->setEyeCam(GLC_Vector3d(arg));
            emit eyeChanged(arg);
            emit updateView();
        }
    }
}

