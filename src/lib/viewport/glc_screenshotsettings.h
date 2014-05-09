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
//! \file glc_screenshotsettings.h Interface for the GLC_ScreenShotSettings class.

#ifndef GLC_SCREENSHOTSETTINGS_H
#define GLC_SCREENSHOTSETTINGS_H

#include <QImage>
#include <QSize>
#include <QColor>
#include <QMetaType>

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_ScreenShotSettings
/*! \brief GLC_ScreenShotSettings : Regroup necessary screenshot settings*/
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_ScreenShotSettings
{
public:
    enum Mode
    {
        Default,
        Image,
        Color
    };

public:
    GLC_ScreenShotSettings();
    GLC_ScreenShotSettings(const GLC_ScreenShotSettings& other);
    virtual ~GLC_ScreenShotSettings();

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////

    //! Return true if this settings is equal to the given settings
    bool operator ==(const GLC_ScreenShotSettings& other) const;

    //! Return true if this settings is not equal to the given settings
    inline bool operator !=(const GLC_ScreenShotSettings& other) const
    {return !(this->operator ==(other));}

    //! Return the mode of this screenShot settings
    inline GLC_ScreenShotSettings::Mode mode() const
    {return m_Mode;}

    //! Return the size of this screenshot settings
    inline const QSize& size() const
    {return m_Size;}

    //! Return the background image of this screenshot settings
    inline const QImage& backgroundImage() const
    {return m_BackgroundImage;}

    //! Return the background color of this screenshot settings
    inline const QColor& backgroundColor() const
    {return m_BackgroundColor;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////

    //! Assign the given screenshot settings  to this settings
    GLC_ScreenShotSettings &operator =(const GLC_ScreenShotSettings& other);

    //! Set the mode of this screenshot settings
    inline void setMode(GLC_ScreenShotSettings::Mode mode)
    {m_Mode= mode;}

    //! Set the size of this screenshot settings
    inline void setSize(const QSize& size)
    {m_Size= size;}

    //! Set the background image of this screenshot settings
    inline void setBackgroundImage(const QImage& image)
    {m_BackgroundImage= image;}

    //! Set the background color of this screenshot settings
    inline void setBackgroundColor(const QColor& color)
    {m_BackgroundColor= color;}

//@}

private:
    GLC_ScreenShotSettings::Mode m_Mode;
    QSize m_Size;
    QImage m_BackgroundImage;
    QColor m_BackgroundColor;
};

Q_DECLARE_METATYPE(GLC_ScreenShotSettings)

#endif // GLC_SCREENSHOTSETTINGS_H
