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

