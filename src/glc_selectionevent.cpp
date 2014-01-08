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

#include "glc_selectionevent.h"

GLC_SelectionEvent::GLC_SelectionEvent()
    : m_Modes(GLC_SelectionEvent::ModeDefault)
    , m_SelectionSet()
{

}

GLC_SelectionEvent::GLC_SelectionEvent(GLC_SelectionEvent::Modes modes, const GLC_SelectionSet &selectionSet)
    : m_Modes(modes)
    , m_SelectionSet(selectionSet)
{

}

GLC_SelectionEvent::GLC_SelectionEvent(const GLC_SelectionEvent &other)
    : m_Modes(other.m_Modes)
    , m_SelectionSet(other.m_SelectionSet)
{

}

bool GLC_SelectionEvent::operator==(const GLC_SelectionEvent &other) const
{
    bool subject= (m_Modes == other.m_Modes);
    subject= subject && (m_SelectionSet == other.m_SelectionSet);

    return subject;
}

GLC_SelectionEvent &GLC_SelectionEvent::operator=(const GLC_SelectionEvent &other)
{
    if (this != &other)
    {
        m_Modes= other.m_Modes;
        m_SelectionSet= other.m_SelectionSet;
    }

    return *this;
}

void GLC_SelectionEvent::clear()
{
    m_Modes= GLC_SelectionEvent::ModeDefault;
    m_SelectionSet.clear();
}
