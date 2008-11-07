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
extern PFNGLCREATEPROGRAMOBJECTARBPROC	glCreateProgram;
extern PFNGLDELETEPROGRAMSARBPROC	  	glDeletePrograms;
extern PFNGLUSEPROGRAMOBJECTARBPROC		glUseProgram;
extern PFNGLCREATESHADEROBJECTARBPROC	glCreateShader;
extern PFNGLDELETESHADERPROC			glDeleteShader;
extern PFNGLSHADERSOURCEARBPROC         glShaderSource;
extern PFNGLCOMPILESHADERARBPROC        glCompileShader;
extern PFNGLATTACHOBJECTARBPROC			glAttachShader;
extern PFNGLDETACHOBJECTARBPROC			glDetachShader;
extern PFNGLLINKPROGRAMARBPROC          glLinkProgram;
extern PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocation;
extern PFNGLUNIFORM4FARBPROC            glUniform4f;
extern PFNGLUNIFORM1IARBPROC            glUniform1i;
extern PFNGLGETSHADERIVPROC				glGetShaderiv;
extern PFNGLGETPROGRAMIVARBPROC			glGetProgramiv;
extern PFNGLISPROGRAMARBPROC			glIsProgram;
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