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

//! \file glc_imagePlane.h interface for the GLC_ImagePlane class.

#ifndef GLC_IMAGEPLANE_H_
#define GLC_IMAGEPLANE_H_

#include <QSizeF>

#include "../shading/glc_material.h"
#include "../sceneGraph/glc_3dviewinstance.h"

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_ImagePlane
/*! \brief GLC_ImagePlane : Viewport background image*/

/*! An GLC_ImagePlane is just a plane with a image texture.*/
//////////////////////////////////////////////////////////////////////

class GLC_LIB_EXPORT GLC_ImagePlane
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Construct image plane from the given image file name
    GLC_ImagePlane(const QString& ImageName, bool ratioPreserved);

    //! Construct image plane from the given image
    GLC_ImagePlane(const QImage& image, bool ratioPreserved);

	~GLC_ImagePlane();
//@}

public:
    bool rotioIsPreserved() const
    {return m_RatioIsPreserved;}

public:
    void setRatioPreserved(bool preserved)
    {m_RatioIsPreserved= preserved;}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Render this image plane
    void render(double screenRatio);
//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////

private:

	//! The image representation
    GLC_3DViewInstance* m_pRepresentation;

    //! Used image size
    QSizeF m_Size;

    bool m_RatioIsPreserved;
};

#endif //GLC_IMAGEPLANE_H_
