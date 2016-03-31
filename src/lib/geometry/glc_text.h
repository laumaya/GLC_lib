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

//! \file glc_text.h interface for the GLC_Text class.
//!
#ifndef GLC_TEXT_H
#define GLC_TEXT_H

#include <QString>
#include <QColor>
#include <QFont>

#include "glc_mesh.h"

#include "../glc_config.h"


class GLC_LIB_EXPORT GLC_Text : public GLC_Mesh
{
public:
    GLC_Text();
    GLC_Text(const QString& text, const QColor& color= Qt::black, const QFont& font= QFont());
    GLC_Text(const GLC_Text& other);

    virtual ~GLC_Text();

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    QString text() const
    {return m_Text;}

    QColor color() const
    {return m_Color;}

    QFont font() const
    {return m_Font;}

    virtual GLC_Geometry* clone() const;

    double width() const
    {return m_Width;}

    double height() const
    {return m_Height;}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    void setText(const QString& text);
    void setColor(const QColor& color);
    void setFont(const QFont& font);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:

    //! Virtual interface for OpenGL Geometry set up.
    /*! This Virtual function is implemented here.\n
     *  Throw GLC_OpenGlException*/
    virtual void glDraw(const GLC_RenderProperties&renderProperties);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
    void initMaterial();
    void createMesh();
    void createText();
    void updateMaterial();
    GLC_Texture* texture(double width, double height) const;
//@}

private:
    double m_Width;
    double m_Height;

    QString m_Text;
    QColor m_Color;
    QFont m_Font;

    bool m_MaterialNeedToBeUpdated;
};

#endif // GLC_TEXT_H
