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

//! \file glc_geometry.cpp Implementation of the GLC_Geometry class.

#include "../shading/glc_selectionmaterial.h"
#include "../glc_openglexception.h"
#include "../glc_state.h"
#include "../glc_context.h"
#include "../glc_contextmanager.h"

#include "glc_geometry.h"

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////
// Default constructor
GLC_Geometry::GLC_Geometry(const QString& name, const bool typeIsWire)
    : m_GeometryIsValid(false)	// By default geometry is invalid
    , m_pBoundingBox(NULL)
    , m_MaterialHash()
    , m_UseColorPerVertex(false)
    , m_IsSelected(false)
    , m_WireData()
    , m_WireColor(60, 60, 60)
    , m_LineWidth(1.0f)
    , m_IsWire(typeIsWire)		// the geometry type
    , m_TransparentMaterialNumber(0)
    , m_Id(glc::GLC_GenGeomID())
    , m_Name(name)
    , m_UseVbo(GLC_State::vboUsed())
{

}
// Copy constructor
GLC_Geometry::GLC_Geometry(const GLC_Geometry& other)
    : m_GeometryIsValid(false)	// By default geometry is invalid
    , m_pBoundingBox(NULL)
    , m_MaterialHash()
    , m_UseColorPerVertex(other.m_UseColorPerVertex)
    , m_IsSelected(false)
    , m_WireData(other.m_WireData)
    , m_WireColor(other.m_WireColor)
    , m_LineWidth(other.m_LineWidth)
    , m_IsWire(other.m_IsWire)
    , m_TransparentMaterialNumber()
    , m_Id(glc::GLC_GenGeomID())
    , m_Name(other.m_Name)
    , m_UseVbo(other.m_UseVbo)
{

    innerCopy(other);
}

// Overload "=" operator
GLC_Geometry& GLC_Geometry::operator=(const GLC_Geometry& other)
{
    if (this != &other)
    {
        clear();
        m_UseColorPerVertex= other.m_UseColorPerVertex;
        m_WireData= other.m_WireData;
        m_WireColor= other.m_WireColor;
        m_LineWidth= other.m_LineWidth;
        m_IsWire= other.m_IsWire;
        m_Name= other.m_Name;
        m_UseVbo= other.m_UseVbo;

        innerCopy(other);
    }
    return *this;
}

GLC_Geometry::~GLC_Geometry()
{
    // delete mesh inner material
    {
        MaterialHash::const_iterator i= m_MaterialHash.begin();
        while (i != m_MaterialHash.constEnd())
        {
            // delete the material if necessary
            i.value()->delGLC_Geom(id());
            if (i.value()->isUnused()) delete i.value();
            ++i;
        }
    }
    m_MaterialHash.clear();

    delete m_pBoundingBox;

}

int GLC_Geometry::primitiveCount() const
{
    return m_WireData.verticeGroupCount();
}

/////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get number of faces
unsigned int GLC_Geometry::faceCount(int) const
{
    return 0;
}

// Get number of vertex
unsigned int GLC_Geometry::vertexCount() const
{
    return 0;
}

double GLC_Geometry::volume()
{
    return 0.0;
}

/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Clear the content of the geometry and makes it empty
void GLC_Geometry::clear()
{
    clearGeometry();
}

// Replace the Master material
void GLC_Geometry::replaceMasterMaterial(GLC_Material* pMaterial)
{
    Q_ASSERT(!m_IsWire);
    if (!m_MaterialHash.isEmpty())
    {
        if (pMaterial != firstMaterial())
        {
            // Remove the first material
            MaterialHash::iterator iMaterial= m_MaterialHash.begin();
            removeMaterial(iMaterial.value()->id());

            // Add the new material
            addMaterial(pMaterial);
        }
    }
    else
    {
        addMaterial(pMaterial);
    }
}

void GLC_Geometry::replaceMaterial(const GLC_uint id, GLC_Material* pMat)
{
    Q_ASSERT(!m_IsWire);
    Q_ASSERT(m_MaterialHash.contains(id));
    removeMaterial(id);
    addMaterial(pMat);
}

void GLC_Geometry::updateMaterialId(GLC_uint oldId, GLC_uint newId)
{
    Q_ASSERT(m_MaterialHash.contains(oldId));
    GLC_Material* pMat= m_MaterialHash.take(oldId);
    pMat->setId(newId);
    m_MaterialHash.insert(newId, pMat);
}

//! Update the transparent material number
void GLC_Geometry::updateTransparentMaterialNumber()
{
    m_TransparentMaterialNumber= 0;
    MaterialHash::const_iterator iMat= m_MaterialHash.constBegin();
    while (iMat != m_MaterialHash.constEnd())
    {
        if (iMat.value()->isTransparent())
        {
            ++m_TransparentMaterialNumber;
        }
        ++iMat;
    }
    if (m_WireColor.alpha() != 255)
    {
        ++m_TransparentMaterialNumber;
    }
}

void GLC_Geometry::addVerticeGroups(const GLC_Geometry& other, const GLC_Matrix4x4& matrix)
{
    m_WireData.add(other.m_WireData, matrix);
}

void GLC_Geometry::addVerticeGroups(const GLC_WireData& wireData, const GLC_Matrix4x4& matrix)
{
    m_WireData.add(wireData, matrix);
}

// Add material to mesh
void GLC_Geometry::addMaterial(GLC_Material* pMaterial)
{
    if (pMaterial != NULL)
    {
        const GLC_uint id= pMaterial->id();
        Q_ASSERT(!m_MaterialHash.contains(id));
        pMaterial->addGLC_Geom(this);
        m_MaterialHash.insert(id, pMaterial);

        if (pMaterial->isTransparent())
        {
            ++m_TransparentMaterialNumber;
        }
    }
}

void GLC_Geometry::setWireColor(const QColor& color)
{
    bool previousColorIsTransparent= (m_WireColor.alpha() != 255);
    bool newColorIsTransparent= (color.alpha() != 255);

    if (previousColorIsTransparent != newColorIsTransparent)
    {
        if (newColorIsTransparent) ++m_TransparentMaterialNumber;
        else if (previousColorIsTransparent) --m_TransparentMaterialNumber;
    }

    m_WireColor= color;
}

void GLC_Geometry::releaseVboClientSide(bool update)
{
    m_WireData.releaseVboClientSide(update);
}

void GLC_Geometry::setVboUsage(bool usage)
{
    m_UseVbo= usage;
    m_WireData.setVboUsage(m_UseVbo);
}

void GLC_Geometry::transformVertice(const GLC_Matrix4x4& matrix)
{
    if (matrix.type() != GLC_Matrix4x4::Identity)
    {
        delete m_pBoundingBox;
        m_pBoundingBox= NULL;
        const int stride= 3;
        GLfloatVector* pVectPos= m_WireData.positionVectorHandle();
        const int verticeCount= pVectPos->size() / stride;
        for (int i= 0; i < verticeCount; ++i)
        {
            GLC_Vector3d newPos(pVectPos->at(stride * i), pVectPos->at(stride * i + 1), pVectPos->at(stride * i + 2));
            newPos= matrix * newPos;
            pVectPos->operator[](stride * i)= static_cast<GLfloat>(newPos.x());
            pVectPos->operator[](stride * i + 1)= static_cast<GLfloat>(newPos.y());
            pVectPos->operator[](stride * i + 2)= static_cast<GLfloat>(newPos.z());
        }
        GLC_Geometry::releaseVboClientSide(true);
    }
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// if the geometry have a texture, load it
void GLC_Geometry::glLoadTexture(void)
{
    MaterialHash::iterator iMaterial= m_MaterialHash.begin();

    while (iMaterial != m_MaterialHash.constEnd())
    {
        // Load texture of mesh materials
        iMaterial.value()->glLoadTexture();
        ++iMaterial;
    }
}

// Geometry display
void GLC_Geometry::render(const GLC_RenderProperties& renderProperties)
{
    Q_ASSERT(!m_IsWire || (m_IsWire && m_MaterialHash.isEmpty()));
    bool renderWire= (renderProperties.renderingFlag() == glc::TransparentRenderFlag) && isTransparent();
    renderWire= renderWire || ((renderProperties.renderingFlag() != glc::TransparentRenderFlag) && !isTransparent());
    if (!m_IsWire || renderWire)
    {
        if (m_MaterialHash.isEmpty() && !m_IsWire)
        {
            GLC_Material* pMaterial= new GLC_Material();
            pMaterial->setName(name());
            addMaterial(pMaterial);
        }

        m_IsSelected= renderProperties.isSelected();

        const bool hasActiveShader= GLC_Shader::hasActiveShader();
        GLC_Shader* pActiveShader= GLC_Shader::currentShaderHandle();

        // Define Geometry's property
        if(!GLC_State::isInSelectionMode())
        {
            glPropGeom(renderProperties);
            if (m_IsWire)
            {
                if (hasActiveShader)
                {
                    GLC_Shader::unuse();
                    QOpenGLContext::currentContext()->functions()->glUseProgram(0);
                }
            }
        }

        glDraw(renderProperties);
        if (!GLC_State::isInSelectionMode() && m_IsWire)
        {
            if (hasActiveShader)
            {
                pActiveShader->use();
            }
        }

        m_IsSelected= false;
        m_GeometryIsValid= true;

        // OpenGL error handler
        GLenum error= glGetError();
        if (error != GL_NO_ERROR)
        {
            GLC_OpenGlException OpenGlException("GLC_Geometry::render " + name(), error);
            throw(OpenGlException);
        }
    }
}

// Virtual interface for OpenGL Geometry properties.
void GLC_Geometry::glPropGeom(const GLC_RenderProperties& renderProperties)
{
    GLC_Context* pContext= GLC_ContextManager::instance()->currentContext();
    glLineWidth(lineWidth());

    if(m_IsWire)
    {
        glLineWidth(m_LineWidth);
        pContext->glcEnableLighting(false);
        glDisable(GL_TEXTURE_2D);
        if (!renderProperties.isSelected())
        {
            // Set polyline colors
            GLfloat color[4]= {static_cast<float>(m_WireColor.redF()),
                               static_cast<float>(m_WireColor.greenF()),
                               static_cast<float>(m_WireColor.blueF()),
                               static_cast<float>(m_WireColor.alphaF())};

            glColor4fv(color);
        }
        else
        {
            GLC_SelectionMaterial::glExecute();
        }
    }
    else if (m_MaterialHash.size() == 1)
    {
        GLC_Material* pCurrentMaterial= m_MaterialHash.begin().value();
        if (pCurrentMaterial->hasTexture())
        {
            pContext->glcEnableLighting(true);
            pCurrentMaterial->glExecute();
            if (renderProperties.isSelected()) GLC_SelectionMaterial::glExecute();
        }
        else
        {
            pContext->glcEnableLighting(true);
            if (renderProperties.isSelected()) GLC_SelectionMaterial::glExecute();
            else pCurrentMaterial->glExecute();
        }
    }
}

// Remove the specified material from the geometry
void GLC_Geometry::removeMaterial(GLC_uint id)
{
    Q_ASSERT(containsMaterial(id));
    // Remove the first material
    GLC_Material* pMaterial= m_MaterialHash.value(id);
    // delete the material if necessary
    pMaterial->delGLC_Geom(this->id());
    if (pMaterial->isTransparent())
    {
        --m_TransparentMaterialNumber;
    }
    if (pMaterial->isUnused()) delete pMaterial;
    m_MaterialHash.remove(id);

}

// Clear the content of this object and makes it empty
void  GLC_Geometry::clearGeometry()
{
    m_GeometryIsValid= false;

    delete m_pBoundingBox;
    m_pBoundingBox= NULL;

    // delete mesh inner material
    {
        MaterialHash::const_iterator i= m_MaterialHash.begin();
        while (i != m_MaterialHash.constEnd())
        {
            // delete the material if necessary
            i.value()->delGLC_Geom(id());
            if (i.value()->isUnused()) delete i.value();
            ++i;
        }
    }
    m_MaterialHash.clear();

    m_UseColorPerVertex= false;
    m_IsSelected= false;
    m_WireData.clear();
    m_IsWire= false;
    m_TransparentMaterialNumber= 0;
    m_Name.clear();

}

void GLC_Geometry::innerCopy(const GLC_Geometry& other)
{
    // Add this mesh to inner material
    MaterialHash::const_iterator i= other.m_MaterialHash.constBegin();
    while (i != other.m_MaterialHash.constEnd())
    {
        GLC_Material* pMat= new GLC_Material(*(i.value()));
        // Warning id doesn't change
        addMaterial(pMat);
        ++i;
    }

    if (NULL != other.m_pBoundingBox)
    {
        m_pBoundingBox= new GLC_BoundingBox(*other.m_pBoundingBox);
    }
}
