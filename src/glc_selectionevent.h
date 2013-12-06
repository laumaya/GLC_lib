/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

#ifndef GLC_SELECTIONEVENT_H
#define GLC_SELECTIONEVENT_H

#include <QMetaType>
#include <QList>

#include "glc_global.h"

class GLC_SelectionEvent
{
public:
    enum Mode {
        Replace= 0,
        Add,
        Remove
    };

public:
    GLC_SelectionEvent();
    explicit GLC_SelectionEvent(Mode mode, const QList<GLC_uint>& listOfId);
    GLC_SelectionEvent(const GLC_SelectionEvent& other);

public:
    bool operator==(const GLC_SelectionEvent& other) const;

    inline Mode mode() const
    {return m_Mode;}

    inline QList<GLC_uint> selectionList() const
    {return m_SelectionList;}

    inline int domainId() const
    {return m_DomainId;}

public:
    GLC_SelectionEvent& operator=(const GLC_SelectionEvent& other);

    void clear();

    inline void setMode(Mode mode)
    {m_Mode= mode;}

    inline void setSelectionList(const QList<GLC_uint>& selectionList)
    {m_SelectionList= selectionList;}

private:
    Mode m_Mode;
    QList<GLC_uint> m_SelectionList;
    int m_DomainId;
};

Q_DECLARE_METATYPE(GLC_SelectionEvent)

#endif // GLC_SELECTIONEVENT_H
