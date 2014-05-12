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

    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector)
    Q_PROPERTY(QVector3D defaultUpVector READ defaultUpVector WRITE setDefaultUpVector)

public:
    explicit GLC_QuickCamera(QObject *parent = 0);

public:
    QVector3D upVector() const;
    QVector3D defaultUpVector() const;

public:
    void setCamera(GLC_Camera* pCamera);

signals:
    void changed();
    void updateView();

public slots:
    void setUpVector(const QVector3D& arg);
    void setDefaultUpVector(const QVector3D& arg);

private:
    QPointer<GLC_Camera> m_Camera;
};

#endif // GLC_QUICKCAMERA_H
