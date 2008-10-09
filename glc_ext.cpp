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
#include "glc_ext.h"
#include <QString>

// Define glcGetProcAddress.
#if defined(Q_OS_WIN32)
#	define glcGetProcAddress(name) wglGetProcAddress((LPCSTR)name)
#else
#	if defined(Q_OS_MAC)
#   	define glcGetProcAddress(name) NSGLGetProcAddress(name)
#	else
#		if defined(Q_OS_LINUX)
#			define glcGetProcAddress(name) (*glXGetProcAddressARB)(name)
#		endif
#	endif
#endif

#if !defined(Q_OS_MAC)
// ARB_vertex_buffer_object
PFNGLBINDBUFFERARBPROC				glBindBuffer			= NULL;
PFNGLDELETEBUFFERSARBPROC			glDeleteBuffers			= NULL;
PFNGLGENBUFFERSARBPROC				glGenBuffers			= NULL;
PFNGLISBUFFERARBPROC				glIsBuffer				= NULL;
PFNGLBUFFERDATAARBPROC				glBufferData			= NULL;
PFNGLBUFFERSUBDATAARBPROC			glBufferSubData			= NULL;
PFNGLGETBUFFERSUBDATAARBPROC		glGetBufferSubData		= NULL;
PFNGLMAPBUFFERARBPROC				glMapBuffer				= NULL;
PFNGLUNMAPBUFFERARBPROC				glUnmapBuffer			= NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC	glGetBufferParameteriv	= NULL;
PFNGLGETBUFFERPOINTERVARBPROC		glGetBufferPointerv		= NULL;
// glDrawRangElement
PFNGLDRAWRANGEELEMENTSPROC 			glDrawRangeElements		= NULL;

// GL_ARB_shader_objects
PFNGLCREATEPROGRAMOBJECTARBPROC		glCreateProgramObjectARB	= NULL;
PFNGLDELETEOBJECTARBPROC			glDeleteObjectARB			= NULL;
PFNGLUSEPROGRAMOBJECTARBPROC		glUseProgramObjectARB		= NULL;
PFNGLCREATESHADEROBJECTARBPROC		glCreateShaderObjectARB		= NULL;
PFNGLSHADERSOURCEARBPROC			glShaderSourceARB			= NULL;
PFNGLCOMPILESHADERARBPROC			glCompileShaderARB			= NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC	glGetObjectParameterivARB	= NULL;
PFNGLATTACHOBJECTARBPROC			glAttachObjectARB			= NULL;
PFNGLGETINFOLOGARBPROC				glGetInfoLogARB				= NULL;
PFNGLLINKPROGRAMARBPROC				glLinkProgramARB			= NULL;
PFNGLGETUNIFORMLOCATIONARBPROC		glGetUniformLocationARB		= NULL;
PFNGLUNIFORM4FARBPROC				glUniform4fARB				= NULL;
PFNGLUNIFORM1IARBPROC				glUniform1iARB				= NULL;

#endif

//const QString glExtension(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));

// Return true if the extension is supported
bool glc::extensionIsSupported(const QString& extension)
{
	QString glExtension(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
	return glExtension.contains(extension);
}

// Return true if VBO extension is succesfully loaded
bool glc::loadVboExtension()
{
	bool result= true;
#if !defined(Q_OS_MAC)
    glBindBuffer				= (PFNGLBINDBUFFERARBPROC)glcGetProcAddress("glBindBuffer");
    glDeleteBuffers				= (PFNGLDELETEBUFFERSARBPROC)glcGetProcAddress("glDeleteBuffers");
    glGenBuffers				= (PFNGLGENBUFFERSARBPROC)glcGetProcAddress("glGenBuffers");
    glIsBuffer					= (PFNGLISBUFFERARBPROC)glcGetProcAddress("glIsBuffer");
    glBufferData				= (PFNGLBUFFERDATAARBPROC)glcGetProcAddress("glBufferData");
    glBufferSubData				= (PFNGLBUFFERSUBDATAARBPROC)glcGetProcAddress("glBufferSubData");
    glGetBufferSubData			= (PFNGLGETBUFFERSUBDATAARBPROC)glcGetProcAddress("glGetBufferSubData");
    glMapBuffer					= (PFNGLMAPBUFFERARBPROC)glcGetProcAddress("glMapBuffer");
    glUnmapBuffer				= (PFNGLUNMAPBUFFERARBPROC)glcGetProcAddress("glUnmapBuffer");
    glGetBufferParameteriv		= (PFNGLGETBUFFERPARAMETERIVARBPROC)glcGetProcAddress("glGetBufferParameteriv");
    glGetBufferPointerv			= (PFNGLGETBUFFERPOINTERVARBPROC)glcGetProcAddress("glGetBufferPointerv");
    
    glDrawRangeElements			= (PFNGLDRAWRANGEELEMENTSPROC)glcGetProcAddress("glDrawRangeElements");

    result= glBindBuffer and glDeleteBuffers and glGenBuffers and glIsBuffer and glBufferData and glBufferSubData and
    glGetBufferSubData and glMapBuffer and glUnmapBuffer and glGetBufferParameteriv and glGetBufferPointerv and glDrawRangeElements;   
#endif
    return result;

}

// Load GLSL extensions
bool glc::loadGlSlExtension()
{
	bool result= true;
#if !defined(Q_OS_MAC)
    glCreateProgramObject		= (PFNGLCREATEPROGRAMOBJECTARBPROC)glcGetProcAddress("glCreateProgramObject");
    glDeleteObject				= (PFNGLDELETEOBJECTARBPROC)glcGetProcAddress("glDeleteObject");
    glUseProgramObject			= (PFNGLUSEPROGRAMOBJECTARBPROC)glcGetProcAddress("glUseProgramObject");
    glCreateShaderObject		= (PFNGLCREATESHADEROBJECTARBPROC)glcGetProcAddress("glCreateShaderObject");
    glShaderSource				= (PFNGLSHADERSOURCEARBPROC)glcGetProcAddress("glShaderSource");
    glCompileShader				= (PFNGLCOMPILESHADERARBPROC)glcGetProcAddress("glCompileShader");
    glGetObjectParameteriv		= (PFNGLGETOBJECTPARAMETERIVARBPROC)glcGetProcAddress("glGetObjectParameteriv");
    glAttachObject				= (PFNGLATTACHOBJECTARBPROC)glcGetProcAddress("glAttachObject");
    glGetInfoLog				= (PFNGLGETINFOLOGARBPROC)glcGetProcAddress("glGetInfoLog");
    glLinkProgram				= (PFNGLLINKPROGRAMARBPROC)glcGetProcAddress("glLinkProgram");
    glGetUniformLocation		= (PFNGLGETUNIFORMLOCATIONARBPROC)glcGetProcAddress("glGetUniformLocation");
    glUniform4f					= (PFNGLUNIFORM4FARBPROC)glcGetProcAddress("glUniform4f");
	glUniform1i					= (PFNGLUNIFORM1IARBPROC)glcGetProcAddress("glUniform1i");

    result= glCreateProgramObject and glDeleteObject and glUseProgramObject and glCreateShaderObject and glCreateShaderObject and
    glCompileShader and glGetObjectParameteriv and glAttachObject and glGetInfoLog and glLinkProgram and glGetUniformLocation and
    glUniform4f and glUniform1i;

#endif
    return result;
}

