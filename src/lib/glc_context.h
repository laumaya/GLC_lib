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
//! \file glc_context.h interface for the GLC_Context class.

#ifndef GLC_CONTEXT_H_
#define GLC_CONTEXT_H_

#include <QtOpenGL>
#include <QGLFormat>
#include <QSharedPointer>
#include <QtDebug>

#include "glc_config.h"
#include "maths/glc_matrix4x4.h"
#include "glc_contextshareddata.h"
#include "glc_uniformshaderdata.h"

class GLC_ContextSharedData;
class QOpenGLContext;
class QSurface;

// OpenGL ES define
#if defined(QT_OPENGL_ES_2)
#define GLC_OPENGL_ES_2 1

#define GL_MODELVIEW					0x1700
#define GL_PROJECTION					0x1701
#endif


//#define GLC_OPENGL_ES_2 1

//////////////////////////////////////////////////////////////////////
//! \class GLC_Context
/*! \brief GLC_Context : Encapsulates OpenGL rendering context*/

/*! The GLC_Context class store all GLC state associated to an OpenGL rendering context.
 * This class is also used to simplified OpenGL and OpenGL-ES interoperability
 */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_Context : public QObject
{
    Q_OBJECT
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
    GLC_Context(QOpenGLContext *pOpenGLContext, QSurface* pSurface);
	virtual ~GLC_Context();

//@}

signals:
    void destroyed(GLC_Context* pContext);

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

    static GLC_Context* current();

	//! Return the model view matrix
	inline GLC_Matrix4x4 modelViewMatrix() const
    {return m_ContextSharedData->modelViewMatrix();}

	//! Return the projection matrix
	inline GLC_Matrix4x4 projectionMatrix() const
    {return m_ContextSharedData->projectionMatrix();}

	//! Return lighting enable state
	inline bool lightingIsEnable() const
    {return m_ContextSharedData->lightingIsEnable();}

    //! Return two sided lighting enable state
    inline bool twoSidedIsEnable() const
    {return m_ContextSharedData->twoSidedIsEnable();}

    //! Return color material enable state
    inline bool colorMaterialIsEnable() const
    {return m_ContextSharedData->colorMaterialIsEnable();}

    //! Return the vector of enable light
    inline QVector<int> enableLights() const
    {return m_ContextSharedData->enableLights();}

    //! Return the OpenGLContext handle of this GLC_Context
    inline QOpenGLContext* contextHandle() const
    {return m_pOpenGLContext;}

//@}
//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Set the matrix mode
    inline void glcMatrixMode(GLenum mode)
    {m_ContextSharedData->glcMatrixMode(mode);}

    //! Replace the current matrix with the identity
    inline void glcLoadIdentity()
    {m_ContextSharedData->glcLoadIdentity();}

	//! push and pop the current matrix stack
    inline void glcPushMatrix()
    {m_ContextSharedData->glcPushMatrix();}

    inline void glcPopMatrix()
    {m_ContextSharedData->glcPopMatrix();}

	//! Replace the current matrix with the specified matrix
    inline void glcLoadMatrix(const GLC_Matrix4x4& matrix)
    {m_ContextSharedData->glcLoadMatrix(matrix);}

	//! Multiply the current matrix with the specified matrix
    inline void glcMultMatrix(const GLC_Matrix4x4& matrix)
    {m_ContextSharedData->glcMultMatrix(matrix);}

	//! Multiply the current matrix by a translation matrix
	inline void glcTranslated(double x, double y, double z)
	{glcMultMatrix(GLC_Matrix4x4(x, y, z));}

	//! Multiply the current matrix by a general scaling matrix
    inline void glcScaled(double x, double y, double z)
    {m_ContextSharedData->glcScaled(x, y, z);}

	//! Multiply the current matrix with an orthographic matrix
    inline void glcOrtho(double left, double right, double bottom, double top, double nearVal, double farVal)
    {m_ContextSharedData->glcOrtho(left, right, bottom, top, nearVal, farVal);}

	//! Multiply the current matrix by a perspective matrix
    inline void glcFrustum(double left, double right, double bottom, double top, double nearVal, double farVal)
    {m_ContextSharedData->glcFrustum(left, right, bottom, top, nearVal, farVal);}

    //! Enable color material
    inline void glcEnableColorMaterial(bool enable)
    {m_ContextSharedData->glcEnableColorMaterial(enable);}

	//! Enable lighting
    inline void glcEnableLighting(bool enable)
    {m_ContextSharedData->glcEnableLighting(enable);}

    //! Enable the given light
    inline void glcEnableLight(GLenum lightId)
    {m_ContextSharedData->glcEnableLight(lightId);}

    //! Enable the given light
    inline void glcDisableLight(GLenum lightId)
    {m_ContextSharedData->glcDisableLight(lightId);}

    //! Set two sided light parameter
    inline void glcSetTwoSidedLight(GLint twoSided)
    {m_ContextSharedData->glcSetTwoSidedLight(twoSided);}

    //! Use vertex array pointer and enable it
    void glcUseVertexPointer(const GLvoid* pointer);

    //! Disable the vertex client state
    void glcDisableVertexClientState();

    //! Use Normal array pointer and enable it
    void glcUseNormalPointer(const GLvoid* pointer);

    //! Disable the normal client state
    void glcDisableNormalClientState();

    //! Use Texture array pointer and enable it
    void glcUseTexturePointer(const GLvoid* pointer);

    //! Disable the normal client state
    void glcDisableTextureClientState();

    //! Use Color array pointer and enable it
    void glcUseColorPointer(const GLvoid* pointer);

    //! Disable the color client state
    void glcDisableColorClientState();

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

    //! Make the attached QOpenGL the current one
    bool makeCurrent();

    //! Set this context the current one
    void setCurrent();

	//! Update uniform variable
	inline void updateUniformVariables()
    {m_ContextSharedData->updateUniformVariables(this);}

    //! Use the default shader
    void useDefaultShader();

    //! UnUse the default shader
    inline void unuseDefaultShader();

    inline void shareWith(GLC_Context *pContext)
    {m_ContextSharedData= pContext->m_ContextSharedData;}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private slots:
//@{
    void openGLContextDestroyed();
//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

    QOpenGLContext* m_pOpenGLContext;
    QSurface* m_pSurface;

	//! The context shared data
	QSharedPointer<GLC_ContextSharedData> m_ContextSharedData;
};

#endif /* GLC_CONTEXT_H_ */
