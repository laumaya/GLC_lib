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

#include "glc_context.h"
#include "glc_contextmanager.h"
#include "shading/glc_shader.h"

#include "glc_state.h"

GLC_Context* GLC_Context::m_pCurrentContext= NULL;

GLC_Context::GLC_Context(const QGLFormat& format)
    : QGLContext(format)
    , m_ContextSharedData()
{
    qDebug() << "GLC_Context::GLC_Context";
    GLC_ContextManager::instance()->addContext(this);
}

GLC_Context::~GLC_Context()
{
    qDebug() << "GLC_Context::~GLC_Context()";
    GLC_ContextManager::instance()->remove(this);
}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

GLC_Context* GLC_Context::current()
{
    return m_pCurrentContext;
}

void GLC_Context::glcUseVertexPointer(const GLvoid *pointer)
{
    Q_ASSERT(this == m_pCurrentContext);

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
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, pointer);
        glEnableVertexAttribArray(location);
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
    Q_ASSERT(this == m_pCurrentContext);

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    glDisableVertexAttribArray(pShader->positionAttributeId());
#else
    if ((NULL != pShader) && (pShader->positionAttributeId() != -1))
    {
        glDisableVertexAttribArray(pShader->positionAttributeId());
    }
    else
    {
        glDisableClientState(GL_VERTEX_ARRAY);
    }
#endif
}

void GLC_Context::glcUseNormalPointer(const GLvoid *pointer)
{
    Q_ASSERT(this == m_pCurrentContext);

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
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, pointer);
        glEnableVertexAttribArray(location);
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
    Q_ASSERT(this == m_pCurrentContext);

    GLC_Shader* pShader= GLC_Shader::currentShaderHandle();
#ifdef GLC_OPENGL_ES_2
    Q_ASSERT(NULL != pShader);
    glDisableVertexAttribArray(pShader->normalAttributeId());
#else
    if ((NULL != pShader) && (pShader->normalAttributeId() != -1))
    {
        glDisableVertexAttribArray(pShader->normalAttributeId());
    }
    else
    {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
#endif
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

void GLC_Context::makeCurrent()
{
    QGLContext::makeCurrent();
    GLC_State::init();
    m_ContextSharedData->init();
    GLC_ContextManager::instance()->setCurrent(this);
    m_pCurrentContext= this;
}

void GLC_Context::doneCurrent()
{
    QGLContext::doneCurrent();
    GLC_ContextManager::instance()->setCurrent(NULL);
    m_pCurrentContext= NULL;
}

void GLC_Context::useDefaultShader()
{
    Q_ASSERT(m_pCurrentContext == this);
    m_ContextSharedData->useDefaultShader();
}

void GLC_Context::unuseDefaultShader()
{
    Q_ASSERT(m_pCurrentContext == this);
    m_ContextSharedData->unuseDefaultShader();
}

bool GLC_Context::chooseContext(const QGLContext* shareContext)
{
    qDebug() << "GLC_Context::chooseContext";
    const bool success= QGLContext::chooseContext(shareContext);
    if (!success)
    {
        qDebug() << "enable to create context " << this;
    }
    else if (NULL != shareContext)
    {
        GLC_Context* pContext= const_cast<GLC_Context*>(dynamic_cast<const GLC_Context*>(shareContext));
        Q_ASSERT(NULL != pContext);
        m_ContextSharedData= pContext->m_ContextSharedData;
    }
    else
    {
        m_ContextSharedData= QSharedPointer<GLC_ContextSharedData>(new GLC_ContextSharedData());
    }

    return success;
}

