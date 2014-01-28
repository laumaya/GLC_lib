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
//! \file glc_text.h Interface for the GLC_Text class.

#ifndef GLC_TEXT_H
#define GLC_TEXT_H

class QFont;
class GLC_Point2d;
class QString;

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Text
/*! \brief GLC_Text : 2D textured text render class */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_Text
{
private:
    GLC_Text();

public:
    static void renderText(const QFont& font, const GLC_Point2d& pos, const QString& text);

private:


private:
    GLC_Text* m_pTextInstance;
};

#endif // GLC_TEXT_H
