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
#include <QFlags>

#include "sceneGraph/glc_selectionset.h"
#include "sceneGraph/glc_world.h"

#include "glc_global.h"
#include "glc_config.h"

class GLC_LIB_EXPORT GLC_SelectionEvent
{
public:
    enum Mode {
        ModeReplace=        0x0001,
        ModeExclusiveUnit=  0x0002,
        ModeSubstract=      0x0004,
        ModeInstance=       0x0020,
        ModeBody=           0x0040,
        ModePrimitive=      0x0080,
    };
    Q_DECLARE_FLAGS(Modes, Mode)

public:
    GLC_SelectionEvent();
    GLC_SelectionEvent(GLC_SelectionEvent::Modes modes, const GLC_SelectionSet& selectionSet);
    GLC_SelectionEvent(const GLC_SelectionEvent& other);

public:
    bool operator==(const GLC_SelectionEvent& other) const;

    inline Modes modes() const
    {return m_Modes;}

    inline GLC_SelectionSet selectionSet() const
    {return m_SelectionSet;}

public:
    GLC_SelectionEvent& operator=(const GLC_SelectionEvent& other);

    void clear();

    inline void setMode(Mode mode)
    {m_Modes= mode;}

    inline void setSelectionSet(const GLC_SelectionSet& selectionSet)
    {m_SelectionSet= selectionSet;}

    inline void setAttachedWorld(const GLC_World& world)
    {m_SelectionSet.setAttachedWorld(world);}

private:
    Modes m_Modes;
    GLC_SelectionSet m_SelectionSet;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GLC_SelectionEvent::Modes)
Q_DECLARE_METATYPE(GLC_SelectionEvent)

#endif // GLC_SELECTIONEVENT_H
