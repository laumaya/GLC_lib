/*
 *  glc_qmlcamera.h
 *
 *  Created on: 02/05/2014
 *      Author: Laurent Ribon
 */

#ifndef GLC_QUICKCAMERA_H
#define GLC_QUICKCAMERA_H

#include <QObject>
#include <QPointer>
#include <QVector3D>

#include "../glc_config.h"

class GLC_Camera;

class GLC_LIB_EXPORT GLC_QuickCamera : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector NOTIFY upVectorChanged)
    Q_PROPERTY(QVector3D defaultUpVector READ defaultUpVector WRITE setDefaultUpVector NOTIFY defaultUpVectorChanged)
    Q_PROPERTY(QVector3D target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QVector3D eye READ eye WRITE setEye NOTIFY eyeChanged)

public:
    explicit GLC_QuickCamera(QObject *parent = 0);

public:
    QVector3D upVector() const;
    QVector3D defaultUpVector() const;

public:
    void setCamera(GLC_Camera* pCamera);

    QVector3D target() const;
    QVector3D eye() const;

signals:
    void changed();
    void updateView();
    void upVectorChanged(QVector3D arg);
    void defaultUpVectorChanged(QVector3D arg);
    void targetChanged(QVector3D arg);
    void eyeChanged(QVector3D arg);

public slots:
    void setUpVector(const QVector3D& arg);
    void setDefaultUpVector(const QVector3D& arg);
    void setTarget(QVector3D arg);
    void setEye(QVector3D arg);

private:
    QPointer<GLC_Camera> m_Camera;
};

#endif // GLC_QUICKCAMERA_H
