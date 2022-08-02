/*
 *  glc_quickselection.h
 *
 *  Created on: 13/05/2014
 *      Author: Laurent Ribon
 */

#ifndef GLC_QUICKSELECTION_H
#define GLC_QUICKSELECTION_H

#include <QObject>

#include "glc_quickoccurrence.h"

#include "../glc_config.h"
#include "../sceneGraph/glc_world.h"

class GLC_LIB_EXPORT GLC_QuickSelection : public QObject
{
    Q_OBJECT

public:
    explicit GLC_QuickSelection(QObject *parent = 0);
    virtual ~GLC_QuickSelection();

public:
    Q_INVOKABLE int count() const
    {return m_Occurrences.count();}

    Q_INVOKABLE GLC_QuickOccurrence* first() const
    {return m_Occurrences.constFirst();}

    Q_INVOKABLE GLC_QuickOccurrence* at(int i) const
    {return m_Occurrences.at(i);}

    void setWorld(const GLC_World& world);

    Q_INVOKABLE bool isEmpty() const
    {return m_Occurrences.isEmpty();}

signals:
    void selectionChanged();

public slots:
    void update();

private:
    void clear();

private:
    QList<GLC_QuickOccurrence*> m_Occurrences;
    GLC_World m_World;

};

#endif // GLC_QUICKSELECTION_H
