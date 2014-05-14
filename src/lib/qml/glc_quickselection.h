/*
 *  glc_quickselection.h
 *
 *  Created on: 13/05/2014
 *      Author: Laurent Ribon
 */

#ifndef GLC_QUICKSELECTION_H
#define GLC_QUICKSELECTION_H

#include <QObject>

#include <GLC_QuickOccurrence>

#include "../glc_config.h"
#include "../sceneGraph/glc_world.h"

class GLC_LIB_EXPORT GLC_QuickSelection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<GLC_QuickOccurrence*> occurrences READ occurrences)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(GLC_QuickOccurrence* first READ first)
    Q_PROPERTY(bool isEmpty READ isEmpty)

public:
    explicit GLC_QuickSelection(QObject *parent = 0);
    virtual ~GLC_QuickSelection();

public:
    QList<GLC_QuickOccurrence*> occurrences() const
    {return m_Occurrences;}

    int count() const
    {return m_Occurrences.count();}

    GLC_QuickOccurrence* first()
    {return m_Occurrences.first();}

    Q_INVOKABLE GLC_QuickOccurrence* at(int i)
    {return m_Occurrences.at(i);}

    void setWorld(const GLC_World& world);

    bool isEmpty()
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
