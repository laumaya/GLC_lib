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
//! \file glc_contextshareddata.h interface for the GLC_ContextSharedData class.

#ifndef GLC_CONTEXTSHAREDDATA_H_
#define GLC_CONTEXTSHAREDDATA_H_

#include <QtOpenGL>
#include <QHash>
#include <QMap>
#include <QVector>

#include "maths/glc_matrix4x4.h"
#include "glc_uniformshaderdata.h"

#include "glc_config.h"

class GLC_Shader;

class GLC_LIB_EXPORT GLC_ContextSharedData
{
public:
	GLC_ContextSharedData();
	virtual ~GLC_ContextSharedData();

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

    //! Return the model view matrix
    inline GLC_Matrix4x4 modelViewMatrix() const
    {Q_ASSERT(m_MatrixStackHash.contains(GL_MODELVIEW)); return m_MatrixStackHash.value(GL_MODELVIEW)->top();}

    //! Return the projection matrix
    inline GLC_Matrix4x4 projectionMatrix() const
    {Q_ASSERT(m_MatrixStackHash.contains(GL_PROJECTION)); return m_MatrixStackHash.value(GL_PROJECTION)->top();}

    //! Return lighting enable state
    inline bool lightingIsEnable() const
    {return m_LightingIsEnable.top();}

    //! Return two sided lighting enable state
    inline bool twoSidedIsEnable() const
    {return m_TwoSidedLighting.top();}

    //! Return color material enable state
    inline bool colorMaterialIsEnable() const
    {return m_ColorMaterialIsEnable.top();}

    //! Return the vector of enable light
    inline QVector<int> enableLights() const
    {return m_LightsEnableState.values().toVector();}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

    //! Init context shared data, the context must be current
    void init();

    //! Update uniform variable
    inline void updateUniformVariables(GLC_Context* pContext)
    {m_UniformShaderData.updateAll(pContext);}

    //! Use the default shader
    void useDefaultShader();

    //! UnUse the default shader
    void unuseDefaultShader();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Set the matrix mode
    void glcMatrixMode(GLenum mode);

    //! Replace the current matrix with the identity
    void glcLoadIdentity();

    //! push and pop the current matrix stack
    void glcPushMatrix();
    void glcPopMatrix();

    //! Replace the current matrix with the specified matrix
    void glcLoadMatrix(const GLC_Matrix4x4& matrix);

    //! Multiply the current matrix with the specified matrix
    void glcMultMatrix(const GLC_Matrix4x4& matrix);

    //! Multiply the current matrix by a general scaling matrix
    void glcScaled(double x, double y, double z);

    //! Multiply the current matrix with an orthographic matrix
    void glcOrtho(double left, double right, double bottom, double top, double nearVal, double farVal);

    //! Multiply the current matrix by a perspective matrix
    void glcFrustum(double left, double right, double bottom, double top, double nearVal, double farVal);

    //! Enable color material
    void glcEnableColorMaterial(bool enable);

    //! Enable lighting
    void glcEnableLighting(bool enable);

    //! Set two sided light parameter
    void glcSetTwoSidedLight(GLint twoSided);

    //! Enable the given light id
    void glcEnableLight(GLenum lightId);

    //! Disable the given light id
    void glcDisableLight(GLenum lightId);

//@}

private:
    void initDefaultShader();
    void initLightEnableState();

private:
    GLC_Shader* m_pDefaultShader;
    bool m_IsClean;

    //! The current matrix mode
    GLenum m_CurrentMatrixMode;

    //! Mapping between matrixMode and matrix stack
    QHash<GLenum, QStack<GLC_Matrix4x4>* > m_MatrixStackHash;

    //! The uniform data of the current shader
    GLC_UniformShaderData m_UniformShaderData;

    //! Enable color material state
    QStack<bool> m_ColorMaterialIsEnable;

    //! Enable lighting state
    QStack<bool> m_LightingIsEnable;

    //! Two sided lighting
    QStack<bool> m_TwoSidedLighting;

    //! Lights enable state
    QMap<GLenum, int> m_LightsEnableState;

};

#endif /* GLC_CONTEXTSHAREDDATA_H_ */
