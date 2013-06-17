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
//! \file glc_contextshareddata.cpp implementation of the GLC_ContextSharedData class.

#include <QtDebug>

#include "glc_contextshareddata.h"
#include "shading/glc_shader.h"
#include "glc_state.h"

GLC_ContextSharedData::GLC_ContextSharedData()
    : m_pDefaultShader(NULL)
    , m_IsClean(false)
{
    qDebug() << "GLC_ContextSharedData::GLC_ContextSharedData()";
}

GLC_ContextSharedData::~GLC_ContextSharedData()
{
    qDebug() << "GLC_ContextSharedData::~GLC_ContextSharedData()";
    delete m_pDefaultShader;
}

void GLC_ContextSharedData::init()
{
    if (!m_IsClean)
    {
        initDefaultShader();

#ifdef GLC_OPENGL_ES_2
        useDefaultShader();
#endif

        m_IsClean= true;
    }
}

void GLC_ContextSharedData::useDefaultShader()
{
    Q_ASSERT(m_IsClean);
    Q_ASSERT(NULL != m_pDefaultShader);
    m_pDefaultShader->use();
}

void GLC_ContextSharedData::unuseDefaultShader()
{
    Q_ASSERT(m_IsClean);
    Q_ASSERT(NULL != m_pDefaultShader);
    m_pDefaultShader->unuse();
}

void GLC_ContextSharedData::initDefaultShader()
{
    qDebug() << "GLC_ContextSharedData::initDefaultShader()";
    if (GLC_State::glslSupported())
    {
        QFile vertexShader(":/GLC_lib_Shaders/default_vert");
        Q_ASSERT(vertexShader.exists());

        QFile fragmentShader(":/GLC_lib_Shaders/default_frag");
        Q_ASSERT(fragmentShader.exists());

        m_pDefaultShader= new GLC_Shader(vertexShader, fragmentShader);
        m_pDefaultShader->createAndCompileProgrammShader();
    }
}
