/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

#ifndef GLC_EXT_H_
#define GLC_EXT_H_

#include <QtOpenGL>
#include "glext/glext.h"

#if !defined(Q_OS_MAC)
// ARB_vertex_buffer_object
extern PFNGLBINDBUFFERARBPROC			glBindBuffer;
extern PFNGLDELETEBUFFERSARBPROC		glDeleteBuffers;
extern PFNGLGENBUFFERSARBPROC			glGenBuffers;
extern PFNGLISBUFFERARBPROC				glIsBuffer;
extern PFNGLBUFFERDATAARBPROC			glBufferData;
extern PFNGLBUFFERSUBDATAARBPROC		glBufferSubData;
extern PFNGLGETBUFFERSUBDATAARBPROC		glGetBufferSubData;
extern PFNGLMAPBUFFERARBPROC			glMapBuffer;
extern PFNGLUNMAPBUFFERARBPROC			glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVARBPROC	glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVARBPROC	glGetBufferPointerv;
// glDrawRangElement
extern PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;

// GL_ARB_shader_objects
extern PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB;
extern PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB;
extern PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC         glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC        glCompileShaderARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
extern PFNGLATTACHOBJECTARBPROC         glAttachObjectARB;
extern PFNGLGETINFOLOGARBPROC           glGetInfoLogARB;
extern PFNGLLINKPROGRAMARBPROC          glLinkProgramARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB;
extern PFNGLUNIFORM4FARBPROC            glUniform4fARB;
extern PFNGLUNIFORM1IARBPROC            glUniform1iARB;

#endif

namespace glc
{
	//! Return true if the extension is supported
	bool extensionIsSupported(const QString&);
	
	//! Load VBO extension
	bool loadVboExtension();
	
	//! Load GLSL extensions
	bool loadGlSlExtension();
};
#endif /*GLC_EXT_H_*/
