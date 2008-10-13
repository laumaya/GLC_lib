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
PFNGLCREATEPROGRAMARBPROC			glCreateProgram			= NULL;
PFNGLDELETEPROGRAMARBPROC			glDeleteProgram			= NULL;
PFNGLUSEPROGRAMARBPROC				glUseProgram			= NULL;
PFNGLCREATESHADERARBPROC			glCreateShader			= NULL;
PFNGLDELETESHADERARBPROC			glDeleteShader			= NULL;
PFNGLSHADERSOURCEARBPROC			glShaderSource			= NULL;
PFNGLCOMPILESHADERARBPROC			glCompileShader			= NULL;
PFNGLATTACHARBPROC					glAttachShader			= NULL;
PFNGLDETACHARBPROC					glDetachShader			= NULL;
PFNGLGETINFOLOGARBPROC				glGetInfoLog			= NULL;
PFNGLLINKPROGRAMARBPROC				glLinkProgram			= NULL;
PFNGLGETUNIFORMLOCATIONARBPROC		glGetUniformLocation	= NULL;
PFNGLUNIFORM4FARBPROC				glUniform4f				= NULL;
PFNGLUNIFORM1IARBPROC				glUniform1i				= NULL;
PFNGLGETSHADERIVARBPROC				glGetShaderiv			= NULL;
PFNGLGETPROGRAMIVARBPROC			glGetProgramiv			= NULL;

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
	glCreateProgram				= (PFNGLCREATEPROGRAMARBPROC)glcGetProcAddress("glCreateProgram");
	glDeleteProgram				= (PFNGLDELETEPROGRAMARBPROC)glcGetProcAddress("glDeleteProgram");
	glUseProgram				= (PFNGLUSEPROGRAMARBPROC)glcGetProcAddress("glUseProgram");
	glCreateShader				= (PFNGLCREATESHADERARBPROC)glcGetProcAddress("glCreateShader");
	glDeleteShader				= (PFNGLDELETESHADERARBPROC)glcGetProcAddress("glDeleteShader");	
	glShaderSource				= (PFNGLSHADERSOURCEARBPROC)glcGetProcAddress("glShaderSource");
	glCompileShader				= (PFNGLCOMPILESHADERARBPROC)glcGetProcAddress("glCompileShader");	
	glAttachShader				= (PFNGLATTACHARBPROC)glcGetProcAddress("glAttachShader");	
	glDetachShader				= (PFNGLDETACHARBPROC)glcGetProcAddress("glDetachShader");	
	glGetInfoLog				= (PFNGLGETINFOLOGARBPROC)glcGetProcAddress("glGetInfoLog");	
	glLinkProgram				= (PFNGLLINKPROGRAMARBPROC)glcGetProcAddress("glLinkProgram");	
	glGetUniformLocation		= (PFNGLGETUNIFORMLOCATIONARBPROC)glcGetProcAddress("glGetUniformLocation");	
	glUniform4f					= (PFNGLUNIFORM4FARBPROC)glcGetProcAddress("glUniform4f");
	glUniform1i					= (PFNGLUNIFORM1IARBPROC)glcGetProcAddress("glUniform1i");
	glGetShaderiv				= (PFNGLGETSHADERIVARBPROC)glcGetProcAddress("glGetShaderiv");
	glGetProgramiv				= (PFNGLGETPROGRAMIVARBPROC)glcGetProcAddress("glGetProgramiv");


	result= glCreateProgram and glDeleteProgram and glUseProgram and glCreateShader and glDeleteShader and
    glShaderSource and glCompileShader and glAttachShader and glDetachShader and glGetInfoLog and glLinkProgram and
    glGetUniformLocation and glUniform4f and glUniform1i and glGetShaderiv and glGetProgramiv;

#endif
    return result;
}

