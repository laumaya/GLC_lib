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
//! \file glc_context.cpp implementation of the GLC_Context class.

#include <QOpenGLFunctions>

#include "glc_context.h"
#include "glc_contextmanager.h"
#include "shading/glc_shader.h"

#include "glc_state.h"

GLC_Context::GLC_Context(QOpenGLContext *pOpenGLContext)
    : QObject()
    , m_pOpenGLContext(pOpenGLContext)
    , m_pSurface(m_pOpenGLContext->surface())
    , m_ContextSharedData()
{
    qDebug() << "GLC_Context::GLC_Context";
    connect(m_pOpenGLContext, SIGNAL(aboutToBeDestroyed()), this, SLOT(openGLContextDestroyed()), Qt::DirectConnection);

    GLC_ContextManager::instance()->addContext(this);
}

GLC_Context::~GLC_Context()
{
    qDebug() << "GLC_Context::~GLC_Context()";

}

GLC_Context *GLC_Context::current()
{
    return GLC_ContextManager::instance()->currentContext();
}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

void GLC_Context::glcUseVertexPointer(const GLvoid *pointer)
{
    Q_ASSERT(m_pOpenGLContext);
    QOpenGLFunctions* pGlFunctions= m_pOpenGLContext->functions();

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    const GLuint location= pShader->positionAttributeId();
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, pointer);
    glEnableVertexAttribArray(location);
#else
    if ((NULL != pShader) && (pShader->positionAttributeId() != -1))
    {
        const GLuint location= pShader->positionAttributeId();
        pGlFunctions->glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, pointer);
        pGlFunctions->glEnableVertexAttribArray(location);
    }
    else
    {
        glVertexPointer(3, GL_FLOAT, 0, pointer);
        glEnableClientState(GL_VERTEX_ARRAY);
    }
#endif
}

void GLC_Context::glcDisableVertexClientState()
{
    Q_ASSERT(m_pOpenGLContext);
    QOpenGLFunctions* pGlFunctions= m_pOpenGLContext->functions();

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    glDisableVertexAttribArray(pShader->positionAttributeId());
#else
    if ((NULL != pShader) && (pShader->positionAttributeId() != -1))
    {
        pGlFunctions->glDisableVertexAttribArray(pShader->positionAttributeId());
    }
    else
    {
        glDisableClientState(GL_VERTEX_ARRAY);
    }
#endif
}

void GLC_Context::glcUseNormalPointer(const GLvoid *pointer)
{
    Q_ASSERT(m_pOpenGLContext);
    QOpenGLFunctions* pGlFunctions= m_pOpenGLContext->functions();

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    const GLuint location= pShader->normalAttributeId();
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, pointer);
    glEnableVertexAttribArray(location);
#else
    if ((NULL != pShader) && (pShader->positionAttributeId() != -1))
    {
        const GLuint location= pShader->normalAttributeId();
        pGlFunctions->glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, pointer);
        pGlFunctions->glEnableVertexAttribArray(location);
    }
    else
    {
        glNormalPointer(GL_FLOAT, 0, pointer);
        glEnableClientState(GL_NORMAL_ARRAY);
    }
#endif
}

void GLC_Context::glcDisableNormalClientState()
{
    Q_ASSERT(m_pOpenGLContext);
    QOpenGLFunctions* pGlFunctions= m_pOpenGLContext->functions();

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    glDisableVertexAttribArray(pShader->normalAttributeId());
#else
    if ((NULL != pShader) && (pShader->normalAttributeId() != -1))
    {
        pGlFunctions->glDisableVertexAttribArray(pShader->normalAttributeId());
    }
    else
    {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
#endif
}

void GLC_Context::glcUseTexturePointer(const GLvoid *pointer)
{
    Q_ASSERT(m_pOpenGLContext);
    QOpenGLFunctions* pGlFunctions= m_pOpenGLContext->functions();

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    const GLuint location= pShader->textureAttributeId();
    glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 0, pointer);
    glEnableVertexAttribArray(location);
#else
    if ((NULL != pShader) && (pShader->textureAttributeId() != -1))
    {
        const GLuint location= pShader->textureAttributeId();
        pGlFunctions->glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 0, pointer);
        pGlFunctions->glEnableVertexAttribArray(location);
    }
    else
    {
        glTexCoordPointer(2, GL_FLOAT, 0, pointer);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
#endif
}

void GLC_Context::glcDisableTextureClientState()
{
    Q_ASSERT(m_pOpenGLContext);
    QOpenGLFunctions* pGlFunctions= m_pOpenGLContext->functions();

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    glDisableVertexAttribArray(pShader->textureAttributeId());
#else
    if ((NULL != pShader) && (pShader->textureAttributeId() != -1))
    {
        pGlFunctions->glDisableVertexAttribArray(pShader->textureAttributeId());
    }
    else
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
#endif
}

void GLC_Context::glcUseColorPointer(const GLvoid *pointer)
{
    Q_ASSERT(m_pOpenGLContext);
    QOpenGLFunctions* pGlFunctions= m_pOpenGLContext->functions();

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    const GLuint location= pShader->colorAttributeId();
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, pointer);
    glEnableVertexAttribArray(location);
#else
    if ((NULL != pShader) && (pShader->colorAttributeId() != -1))
    {
        const GLuint location= pShader->colorAttributeId();
        pGlFunctions->glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, pointer);
        pGlFunctions->glEnableVertexAttribArray(location);
    }
    else
    {
        glColorPointer(4, GL_FLOAT, 0, pointer);
        glEnableClientState(GL_COLOR_ARRAY);
    }
#endif
}

void GLC_Context::glcDisableColorClientState()
{
    Q_ASSERT(m_pOpenGLContext);
    QOpenGLFunctions* pGlFunctions= m_pOpenGLContext->functions();

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    glDisableVertexAttribArray(pShader->colorAttributeId());
#else
    if ((NULL != pShader) && (pShader->colorAttributeId() != -1))
    {
        pGlFunctions->glDisableVertexAttribArray(pShader->colorAttributeId());
    }
    else
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }
#endif

}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

void GLC_Context::setCurrent()
{
    m_pSurface= m_pOpenGLContext->surface();
    GLC_State::init();
    if (m_ContextSharedData.isNull())
    {
        m_ContextSharedData= QSharedPointer<GLC_ContextSharedData>(new GLC_ContextSharedData());
    }
    m_ContextSharedData->init();
}

void GLC_Context::useDefaultShader()
{
    Q_ASSERT(m_pOpenGLContext);
    m_ContextSharedData->useDefaultShader();
}

void GLC_Context::unuseDefaultShader()
{
    Q_ASSERT(m_pOpenGLContext);
    m_ContextSharedData->unuseDefaultShader();
}

void GLC_Context::openGLContextDestroyed()
{
    qDebug() << "GLC_Context::openGLContextDestroyed()";
    Q_ASSERT(NULL != m_pSurface);
    m_pOpenGLContext->makeCurrent(m_pSurface);
    m_ContextSharedData.clear();
    emit destroyed(this);
}

