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

//! \file glc_imagePlane.cpp implementation of the GLC_ImagePlane class.
#include "glc_imageplane.h"
#include "glc_viewport.h"
#include "../glc_openglexception.h"
#include "../glc_factory.h"
#include "../glc_context.h"
#include "../glc_contextmanager.h"

#include <QtDebug>

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

GLC_ImagePlane::GLC_ImagePlane(const QString& ImageName, bool ratioPreserved)
    : m_p3DViewInstance(NULL)
    , m_Size()
    , m_RatioIsPreserved(ratioPreserved)
{
	GLC_Texture* pImgTexture= GLC_Factory::instance()->createTexture(ImageName);
	pImgTexture->setMaxTextureSize(pImgTexture->imageOfTexture().size());
    m_Size= pImgTexture->size();

    m_p3DViewInstance= new GLC_3DViewInstance(GLC_Factory::instance()->createRectangle(m_Size.width(), m_Size.height()));
    m_p3DViewInstance->geomAt(0)->addMaterial(new GLC_Material(pImgTexture));
}

GLC_ImagePlane::GLC_ImagePlane(const QImage& image, bool ratioPreserved)
    : m_p3DViewInstance(NULL)
    , m_Size()
    , m_RatioIsPreserved(ratioPreserved)
{
	GLC_Texture* pImgTexture= GLC_Factory::instance()->createTexture(image);
	pImgTexture->setMaxTextureSize(image.size());
    m_Size= pImgTexture->size();

    m_p3DViewInstance= new GLC_3DViewInstance(GLC_Factory::instance()->createRectangle(m_Size.width(), m_Size.height()));
    m_p3DViewInstance->geomAt(0)->addMaterial(new GLC_Material(pImgTexture));
}

GLC_ImagePlane::GLC_ImagePlane(const GLC_ImagePlane &other)
    : m_p3DViewInstance(new GLC_3DViewInstance(*(other.m_p3DViewInstance)))
    , m_Size(other.m_Size)
    , m_RatioIsPreserved(other.m_RatioIsPreserved)
{

}

GLC_ImagePlane::~GLC_ImagePlane()
{
    delete m_p3DViewInstance;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

void GLC_ImagePlane::render(double screenRatio)
{
    double width;
    double height;

    if (m_RatioIsPreserved)
    {
        const double imageRatio= m_Size.width() / m_Size.height();
        if (screenRatio > imageRatio)
        {
            double usedSize;
            if (imageRatio > 1.0)
            {
                usedSize= qMax(m_Size.width(), m_Size.height());
            }
            else
            {
                usedSize= qMin(m_Size.width(), m_Size.height());
            }
            width= (usedSize) / 2.0;
            height= (usedSize / screenRatio) / 2.0;
        }
        else
        {
            double usedSize;
            if (imageRatio > 1.0)
            {
                usedSize= qMin(m_Size.width(), m_Size.height());
            }
            else
            {
                usedSize= qMax(m_Size.width(), m_Size.height());
            }
            width= (usedSize * screenRatio) / 2.0;
            height= (usedSize) / 2.0;
        }
    }
    else
    {
        width= m_Size.width() / 2.0;
        height= m_Size.height() / 2.0;
    }

    GLC_Context* pContext= GLC_ContextManager::instance()->currentContext();
    pContext->glcMatrixMode(GL_PROJECTION);
    pContext->glcPushMatrix();
    pContext->glcLoadIdentity();
    pContext->glcOrtho(-width, width, -height, height, -1.0, 1.0);
    pContext->glcMatrixMode(GL_MODELVIEW);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    m_p3DViewInstance->render();

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_DEPTH_TEST);

    pContext->glcMatrixMode(GL_PROJECTION);
    pContext->glcPopMatrix();
    pContext->glcMatrixMode(GL_MODELVIEW);
}
