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
//! \file glc_text.cpp Implementation for the GLC_Text class.

#include <QFontMetrics>
#include <QPixmap>
#include <QPainter>

#include "../shading/glc_texture.h"
#include "../shading/glc_material.h"

#include "glc_text.h"

GLC_Text::GLC_Text()
    : GLC_Mesh()
    , m_Width(1.0)
    , m_Height(1.0)
    , m_Text("Text")
    , m_Color(Qt::black)
    , m_Font(QFont())
    , m_MaterialNeedToBeUpdated(true)
{
    initMaterial();
}

GLC_Text::GLC_Text(const QString &text, const QColor &color, const QFont& font)
    : GLC_Mesh()
    , m_Width(1.0)
    , m_Height(1.0)
    , m_Text(text)
    , m_Color(color)
    , m_Font(font)
    , m_MaterialNeedToBeUpdated(true)

{
    initMaterial();
}

GLC_Text::GLC_Text(const GLC_Text &other)
    : GLC_Mesh(other)
    , m_Width(other.m_Width)
    , m_Height(other.m_Height)
    , m_Text(other.m_Text)
    , m_Color(other.m_Color)
    , m_Font(other.m_Font)
    , m_MaterialNeedToBeUpdated(other.m_MaterialNeedToBeUpdated)
{

}

GLC_Text::~GLC_Text()
{

}

GLC_Geometry *GLC_Text::clone() const
{
    return new GLC_Text(*this);
}

bool GLC_Text::update()
{
    bool subject;
    if (GLC_Mesh::isEmpty())
    {
        createText();
        subject= true;
    }
    else
    {
        subject= false;
    }

    return subject;
}

void GLC_Text::setText(const QString &text)
{
    m_Text= text;
    QFontMetrics fontMetrics(m_Font);
    m_Width= fontMetrics.width(m_Text);
    m_Height= fontMetrics.height();

    m_MaterialNeedToBeUpdated= true;
    GLC_Mesh::clearMeshWireAndBoundingBox();
}

void GLC_Text::setColor(const QColor &color)
{
    m_Color= color;
    m_MaterialNeedToBeUpdated= true;
}

void GLC_Text::setFont(const QFont &font)
{
    m_Font= font;
    QFontMetrics fontMetrics(m_Font);
    m_Width= fontMetrics.width(m_Text);
    m_Height= fontMetrics.height();

    m_MaterialNeedToBeUpdated= true;
    GLC_Mesh::clearMeshWireAndBoundingBox();
}

void GLC_Text::glDraw(const GLC_RenderProperties &renderProperties)
{
    if (GLC_Mesh::isEmpty())
    {
        createText();
    }
    else if (m_MaterialNeedToBeUpdated)
    {
        updateMaterial();
    }

    GLint oldValue;
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &oldValue);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    GLC_Mesh::glDraw(renderProperties);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, oldValue);
}

void GLC_Text::initMaterial()
{
    GLC_Material* pMaterial;
    if (hasMaterial())
    {
        pMaterial= this->firstMaterial();
    }
    else
    {
        pMaterial= new GLC_Material();
        GLC_Mesh::addMaterial(pMaterial);
    }

    pMaterial->setOpacity(0.99);
}

void GLC_Text::createMesh()
{
    Q_ASSERT(GLC_Mesh::isEmpty());

    const GLfloat lgX= static_cast<const GLfloat>(m_Width / 2.0);
    const GLfloat lgY= static_cast<const GLfloat>(m_Height / 2.0);

    GLfloatVector verticeVector;
    GLfloatVector normalsVector;
    GLfloatVector texelVector;

    // the unique face of this rectangle
    verticeVector << -lgX; verticeVector << -lgY; verticeVector << 0.0f;
    normalsVector << 0.0f; normalsVector << 0.0f; normalsVector << 1.0f;
    texelVector << 0.0f; texelVector << 0.0f;

    verticeVector << lgX; verticeVector << -lgY; verticeVector << 0.0f;
    normalsVector << 0.0f; normalsVector << 0.0f; normalsVector << 1.0f;
    texelVector << 1.0f; texelVector << 0.0f;

    verticeVector << lgX; verticeVector << lgY; verticeVector << 0.0f;
    normalsVector << 0.0f; normalsVector << 0.0f; normalsVector << 1.0f;
    texelVector << 1.0f; texelVector << 1.0f;

    verticeVector << -lgX; verticeVector << lgY; verticeVector << 0.0f;
    normalsVector << 0.0f; normalsVector << 0.0f; normalsVector << 1.0f;
    texelVector << 0.0f; texelVector << 1.0f;

    // Add bulk data in to the mesh
    GLC_Mesh::addVertice(verticeVector);
    GLC_Mesh::addNormals(normalsVector);
    GLC_Mesh::addTexels(texelVector);

    // Set the material to use
    GLC_Material* pMaterial;
    if (hasMaterial())
    {
        pMaterial= this->firstMaterial();
    }
    else
    {
        pMaterial= new GLC_Material();
    }

    IndexList index;
    // Face 1
    index << 0 << 1 << 3 << 2;
    GLC_Mesh::addTrianglesStrip(pMaterial, index);

    GLC_Mesh::finish();
}

void GLC_Text::createText()
{
    QFontMetrics fontMetrics(m_Font);
    m_Width= fontMetrics.width(m_Text);
    m_Height= fontMetrics.height();

    m_MaterialNeedToBeUpdated= true;
    GLC_Mesh::clearMeshWireAndBoundingBox();

    updateMaterial();
    createMesh();
}

void GLC_Text::updateMaterial()
{
    GLC_Material* pMaterial= this->firstMaterial();

    pMaterial->setTexture(texture(m_Width, m_Height));
    m_MaterialNeedToBeUpdated= false;
}

GLC_Texture *GLC_Text::texture(double width, double height) const
{
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);
    QPainter painter;

    painter.begin(&pixmap);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing);
    painter.setFont(m_Font);
    painter.setPen(m_Color);
    QFontMetrics fontMetrics(m_Font);
    painter.drawText(0, fontMetrics.ascent(), m_Text);
    painter.end();

    QImage image= pixmap.toImage();

    GLC_Texture *pSubject= new GLC_Texture(image);

    return pSubject;
}
