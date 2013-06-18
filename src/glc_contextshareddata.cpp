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
    , m_CurrentMatrixMode()
    , m_MatrixStackHash()
    , m_UniformShaderData()
    , m_LightingIsEnable()
    , m_LightsEnableState()
{
    qDebug() << "GLC_ContextSharedData::GLC_ContextSharedData()";

    QStack<GLC_Matrix4x4>* pStack1= new QStack<GLC_Matrix4x4>();
    pStack1->push(GLC_Matrix4x4());
    m_MatrixStackHash.insert(GL_MODELVIEW, pStack1);

    QStack<GLC_Matrix4x4>* pStack2= new QStack<GLC_Matrix4x4>();
    pStack2->push(GLC_Matrix4x4());
    m_MatrixStackHash.insert(GL_PROJECTION, pStack2);

    m_LightingIsEnable.push(false);

    initLightEnableState();

}

GLC_ContextSharedData::~GLC_ContextSharedData()
{
    qDebug() << "GLC_ContextSharedData::~GLC_ContextSharedData()";

    QHash<GLenum, QStack<GLC_Matrix4x4>* >::iterator iStack= m_MatrixStackHash.begin();
    while (iStack != m_MatrixStackHash.end())
    {
        delete iStack.value();
        ++iStack;
    }

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

void GLC_ContextSharedData::glcMatrixMode(GLenum mode)
{
    Q_ASSERT((mode == GL_MODELVIEW) || (mode == GL_PROJECTION));

    m_CurrentMatrixMode= mode;
#ifdef GLC_OPENGL_ES_2

#else
    glMatrixMode(m_CurrentMatrixMode);
#endif
}

void GLC_ContextSharedData::glcLoadIdentity()
{
    m_MatrixStackHash.value(m_CurrentMatrixMode)->top().setToIdentity();

#ifdef GLC_OPENGL_ES_2
    m_UniformShaderData.setModelViewProjectionMatrix(m_MatrixStackHash.value(GL_MODELVIEW)->top(), m_MatrixStackHash.value(GL_PROJECTION)->top());
#else
    if (GLC_Shader::hasActiveShader())
    {
        m_UniformShaderData.setModelViewProjectionMatrix(m_MatrixStackHash.value(GL_MODELVIEW)->top(), m_MatrixStackHash.value(GL_PROJECTION)->top());
    }
    glLoadIdentity();
#endif
}

void GLC_ContextSharedData::glcPushMatrix()
{
    m_MatrixStackHash.value(m_CurrentMatrixMode)->push(m_MatrixStackHash.value(m_CurrentMatrixMode)->top());

#ifndef GLC_OPENGL_ES_2
    glPushMatrix();
#endif
}

void GLC_ContextSharedData::glcPopMatrix()
{
    m_MatrixStackHash.value(m_CurrentMatrixMode)->pop();

#ifdef GLC_OPENGL_ES_2
    this->glcLoadMatrix(m_MatrixStackHash.value(m_CurrentMatrixMode)->top());
#else
    if (GLC_Shader::hasActiveShader())
    {
        this->glcLoadMatrix(m_MatrixStackHash.value(m_CurrentMatrixMode)->top());
    }
    glPopMatrix();
#endif
}

void GLC_ContextSharedData::glcLoadMatrix(const GLC_Matrix4x4 &matrix)
{
    m_MatrixStackHash.value(m_CurrentMatrixMode)->top()= matrix;

#ifdef GLC_OPENGL_ES_2
    m_UniformShaderData.setModelViewProjectionMatrix(m_MatrixStackHash.value(GL_MODELVIEW)->top(), m_MatrixStackHash.value(GL_PROJECTION)->top());
#else
    if (GLC_Shader::hasActiveShader())
    {
        m_UniformShaderData.setModelViewProjectionMatrix(m_MatrixStackHash.value(GL_MODELVIEW)->top(), m_MatrixStackHash.value(GL_PROJECTION)->top());
    }
    ::glLoadMatrixd(matrix.getData());
#endif
}

void GLC_ContextSharedData::glcMultMatrix(const GLC_Matrix4x4 &matrix)
{
    const GLC_Matrix4x4 current= m_MatrixStackHash.value(m_CurrentMatrixMode)->top();
    m_MatrixStackHash.value(m_CurrentMatrixMode)->top()= m_MatrixStackHash.value(m_CurrentMatrixMode)->top() * matrix;
#ifdef GLC_OPENGL_ES_2
    m_UniformShaderData.setModelViewProjectionMatrix(m_MatrixStackHash.value(GL_MODELVIEW)->top(), m_MatrixStackHash.value(GL_PROJECTION)->top());
#else
    if (GLC_Shader::hasActiveShader())
    {
        m_UniformShaderData.setModelViewProjectionMatrix(m_MatrixStackHash.value(GL_MODELVIEW)->top(), m_MatrixStackHash.value(GL_PROJECTION)->top());
    }
    ::glMultMatrixd(matrix.getData());
#endif
}

void GLC_ContextSharedData::glcScaled(double x, double y, double z)
{
    GLC_Matrix4x4 scale;
    scale.setMatScaling(x, y, z);
    glcMultMatrix(scale);
}

void GLC_ContextSharedData::glcOrtho(double left, double right, double bottom, double top, double nearVal, double farVal)
{
    GLC_Matrix4x4 orthoMatrix;
    double* m= orthoMatrix.setData();

    const double tx= - (right + left) / (right - left);
    const double ty= - (top + bottom) / (top - bottom);
    const double tz= - (farVal + nearVal) / (farVal - nearVal);
    m[0]= 2.0 / (right - left);
    m[5]= 2.0 / (top - bottom);
    m[10]= -2.0 / (farVal - nearVal);
    m[12]= tx;
    m[13]= ty;
    m[14]= tz;

    glcMultMatrix(orthoMatrix);
}

void GLC_ContextSharedData::glcFrustum(double left, double right, double bottom, double top, double nearVal, double farVal)
{
    GLC_Matrix4x4 perspMatrix;
    double* m= perspMatrix.setData();

    const double a= (right + left) / (right - left);
    const double b= (top + bottom) / (top - bottom);
    const double c= - (farVal + nearVal) / (farVal - nearVal);
    const double d= - (2.0 * farVal * nearVal) / (farVal - nearVal);

    m[0]= (2.0 * nearVal) / (right - left);
    m[5]= (2.0 * nearVal) / (top - bottom);
    m[8]= a;
    m[9]= b;
    m[10]= c;
    m[11]= -1.0;
    m[14]= d;
    m[15]= 0.0;

    glcMultMatrix(perspMatrix);
}

void GLC_ContextSharedData::glcEnableLighting(bool enable)
{
    if (enable != m_LightingIsEnable.top())
    {
        m_LightingIsEnable.top()= enable;

#ifdef GLC_OPENGL_ES_2

        m_UniformShaderData.setLightingState(m_LightingIsEnable);
#else
        if (GLC_Shader::hasActiveShader())
        {
            m_UniformShaderData.setLightingState(m_LightingIsEnable.top());
        }
        if (m_LightingIsEnable.top()) ::glEnable(GL_LIGHTING);
        else ::glDisable(GL_LIGHTING);
#endif

    }
}

void GLC_ContextSharedData::glcEnableLight(GLenum lightId)
{
    Q_ASSERT(m_LightsEnableState.contains(lightId));

    if (!m_LightsEnableState.value(lightId))
    {
        m_LightsEnableState[lightId]= true;

#ifdef GLC_OPENGL_ES_2

        m_UniformShaderData.setLightsEnableState(m_LightsEnableState.values().toVector());
#else
        if (GLC_Shader::hasActiveShader())
        {
            QVector<int> enableVect= m_LightsEnableState.values().toVector();
            m_UniformShaderData.setLightsEnableState(enableVect);
        }

        ::glEnable(lightId);
#endif

    }
}

void GLC_ContextSharedData::glcDisableLight(GLenum lightId)
{
    Q_ASSERT(m_LightsEnableState.contains(lightId));

    if (!m_LightsEnableState.value(lightId))
    {
        m_LightsEnableState[lightId]= false;

#ifdef GLC_OPENGL_ES_2

        m_UniformShaderData.setLightsEnableState(m_LightsEnableState.values().toVector());
#else
        if (GLC_Shader::hasActiveShader())
        {
            QVector<int> enableVect= m_LightsEnableState.values().toVector();
            m_UniformShaderData.setLightsEnableState(enableVect);
        }

        ::glDisable(lightId);
#endif

    }
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

void GLC_ContextSharedData::initLightEnableState()
{
    const int count= GLC_Light::maxLightCount();
    for (int i= 0; i < count; ++i)
    {
        m_LightsEnableState.insert(GL_LIGHT0 + i, false);
    }
}
