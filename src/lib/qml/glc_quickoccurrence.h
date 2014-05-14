/*
 *  glc_quickoccurrence.h
 *
 *  Created on: 13/05/2014
 *      Author: Laurent Ribon
 */

#ifndef GLC_QUICKOCCURRENCE_H
#define GLC_QUICKOCCURRENCE_H

#include <QObject>

#include "../glc_global.h"
#include "../glc_config.h"

class GLC_StructOccurrence;

class GLC_LIB_EXPORT GLC_QuickOccurrence : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint id READ id)
    Q_PROPERTY(QString referenceName READ referenceName)
    Q_PROPERTY(QString instanceName READ instanceName)

public:
    explicit GLC_QuickOccurrence(GLC_StructOccurrence * pOcc);

public:
    uint id() const
    {return m_Id;}

    QString referenceName() const
    {return m_ReferenceName;}

    QString instanceName() const
    {return m_InstanceName;}

protected:
    uint m_Id;
    QString m_ReferenceName;
    QString m_InstanceName;

};

#endif // GLC_QUICKOCCURRENCE_H
