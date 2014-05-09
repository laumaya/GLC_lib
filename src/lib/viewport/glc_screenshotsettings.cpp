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

//! \file glc_screenshotsettings.cpp Implementation of the GLC_ScreenShotSettings class.

#include "glc_screenshotsettings.h"

GLC_ScreenShotSettings::GLC_ScreenShotSettings()
    : m_Mode(GLC_ScreenShotSettings::Default)
    , m_Size(100, 100)
    , m_BackgroundImage()
    , m_BackgroundColor()
{

}

GLC_ScreenShotSettings::GLC_ScreenShotSettings(const GLC_ScreenShotSettings &other)
    : m_Mode(other.m_Mode)
    , m_Size(other.m_Size)
    , m_BackgroundImage(other.m_BackgroundImage)
    , m_BackgroundColor(other.m_BackgroundColor)
{

}

GLC_ScreenShotSettings::~GLC_ScreenShotSettings()
{

}

bool GLC_ScreenShotSettings::operator ==(const GLC_ScreenShotSettings &other) const
{
    bool subject= (this == &other);
    subject= subject && (m_Mode == other.m_Mode);
    subject= subject && (m_Size == m_Size);
    subject= subject && (m_BackgroundImage == other.m_BackgroundImage);
    subject= subject && (m_BackgroundColor == other.m_BackgroundColor);

    return subject;
}

GLC_ScreenShotSettings& GLC_ScreenShotSettings::operator =(const GLC_ScreenShotSettings &other)
{
    if (this->operator !=(other))
    {
        m_Mode= other.m_Mode;
        m_Size= other.m_Size;
        m_BackgroundImage= other.m_BackgroundImage;
        m_BackgroundColor= other.m_BackgroundColor;
    }

    return *this;
}
