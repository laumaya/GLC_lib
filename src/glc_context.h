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
#include <QGLContext>
#include <QGLFormat>
#include <QSharedPointer>
#include <QtDebug>

#include "glc_config.h"
#include "maths/glc_matrix4x4.h"
#include "glc_contextshareddata.h"
#include "glc_uniformshaderdata.h"

class GLC_ContextSharedData;

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
class GLC_LIB_EXPORT GLC_Context : public QGLContext
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
	GLC_Context(const QGLFormat& format);
	virtual ~GLC_Context();

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return the current context
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
//@}
//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Set the matrix mode
    inline void glcMatrixMode(GLenum mode)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcMatrixMode(mode);}

    //! Replace the current matrix with the identity
    inline void glcLoadIdentity()
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcLoadIdentity();}

	//! push and pop the current matrix stack
    inline void glcPushMatrix()
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcPushMatrix();}

    inline void glcPopMatrix()
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcPopMatrix();}

	//! Replace the current matrix with the specified matrix
    inline void glcLoadMatrix(const GLC_Matrix4x4& matrix)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcLoadMatrix(matrix);}

	//! Multiply the current matrix with the specified matrix
    inline void glcMultMatrix(const GLC_Matrix4x4& matrix)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcMultMatrix(matrix);}

	//! Multiply the current matrix by a translation matrix
	inline void glcTranslated(double x, double y, double z)
	{glcMultMatrix(GLC_Matrix4x4(x, y, z));}

	//! Multiply the current matrix by a general scaling matrix
    inline void glcScaled(double x, double y, double z)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcScaled(x, y, z);}

	//! Multiply the current matrix with an orthographic matrix
    inline void glcOrtho(double left, double right, double bottom, double top, double nearVal, double farVal)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcOrtho(left, right, bottom, top, nearVal, farVal);}

	//! Multiply the current matrix by a perspective matrix
    inline void glcFrustum(double left, double right, double bottom, double top, double nearVal, double farVal)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcFrustum(left, right, bottom, top, nearVal, farVal);}

	//! Enable lighting
    inline void glcEnableLighting(bool enable)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcEnableLighting(enable);}

    //! Enable the given light
    inline void glcEnableLight(GLenum lightId)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcEnableLight(lightId);}

    //! Enable the given light
    inline void glcDisableLight(GLenum lightId)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcDisableLight(lightId);}

    //! Set two sided light parameter
    inline void glcSetTwoSidedLight(GLint twoSided)
    {Q_ASSERT(QGLContext::isValid()); m_ContextSharedData->glcSetTwoSidedLight(twoSided);}

    //! Use vertex array pointer and enable it
    void glcUseVertexPointer(const GLvoid* pointer);

    //! Disable the vertex client state
    void glcDisableVertexClientState();

    //! Use Normal array pointer and enable it
    void glcUseNormalPointer(const GLvoid* pointer);

    //! Disable the vertex client state
    void glcDisableNormalClientState();

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:

	//! Make this context the current one
	virtual void makeCurrent();

	//! Make no context to be the current one
	virtual void doneCurrent();

	//! Update uniform variable
	inline void updateUniformVariables()
    {m_ContextSharedData->updateUniformVariables(this);}

    //! Use the default shader
    void useDefaultShader();

    //! UnUse the default shader
    inline void unuseDefaultShader();

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
protected:
//@{

	virtual bool chooseContext(const QGLContext* shareContext= 0);
//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:

	//! The context shared data
	QSharedPointer<GLC_ContextSharedData> m_ContextSharedData;

    //! The current context
    static GLC_Context* m_pCurrentContext;
};

#endif /* GLC_CONTEXT_H_ */
