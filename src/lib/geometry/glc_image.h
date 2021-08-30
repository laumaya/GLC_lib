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

//! \file glc_image.h interface for the GLC_Image class.
//!
#ifndef GLC_IMAGE_H
#define GLC_IMAGE_H

#include <QString>
#include <QImage>

#include "glc_mesh.h"

#include "../glc_config.h"


class GLC_LIB_EXPORT GLC_Image : public GLC_Mesh
{
public:
    GLC_Image();
    GLC_Image(const QImage& image);
    GLC_Image(const GLC_Image& other);

    virtual ~GLC_Image();

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    const QImage& image() const
    {return m_Image;}

    GLC_Geometry* clone() const override;

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
    bool update() override;
    void setImage(const QImage& image);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:

    //! Virtual interface for OpenGL Geometry set up.
    /*! This Virtual function is implemented here.\n
     *  Throw GLC_OpenGlException*/
    void glDraw(const GLC_RenderProperties&renderProperties) override;

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
    void initMaterial();
    void createMesh();
    void createGeometryFromImage();
    void updateMaterial();
//@}

private:
    double m_Width;
    double m_Height;

    QImage m_Image;

    bool m_MaterialNeedToBeUpdated;
};

#endif // GLC_IMAGE_H
