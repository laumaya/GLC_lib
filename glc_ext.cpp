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
PFNGLCREATEPROGRAMOBJECTARBPROC		glCreateProgram			= NULL;
PFNGLDELETEPROGRAMSARBPROC			glDeletePrograms		= NULL;
PFNGLUSEPROGRAMOBJECTARBPROC		glUseProgram			= NULL;
PFNGLCREATESHADEROBJECTARBPROC		glCreateShader			= NULL;
PFNGLDELETESHADERPROC				glDeleteShader			= NULL;
PFNGLSHADERSOURCEARBPROC			glShaderSource			= NULL;
PFNGLCOMPILESHADERARBPROC			glCompileShader			= NULL;
PFNGLATTACHOBJECTARBPROC			glAttachShader			= NULL;
PFNGLDETACHOBJECTARBPROC			glDetachShader			= NULL;
PFNGLLINKPROGRAMARBPROC				glLinkProgram			= NULL;
PFNGLGETUNIFORMLOCATIONARBPROC		glGetUniformLocation	= NULL;
PFNGLUNIFORM4FARBPROC				glUniform4f				= NULL;
PFNGLUNIFORM1IARBPROC				glUniform1i				= NULL;
PFNGLGETSHADERIVPROC				glGetShaderiv			= NULL;
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
	glCreateProgram				= (PFNGLCREATEPROGRAMOBJECTARBPROC)glcGetProcAddress("glCreateProgram");
	if (not glCreateProgram) qDebug() << "not glCreateProgram";
	glDeletePrograms			= (PFNGLDELETEPROGRAMSARBPROC)glcGetProcAddress("glDeleteProgram");
	if (not glDeletePrograms) qDebug() << "not glDeleteProgram";
	glUseProgram				= (PFNGLUSEPROGRAMOBJECTARBPROC)glcGetProcAddress("glUseProgram");
	if (not glUseProgram) qDebug() << "not glUseProgram";
	glCreateShader				= (PFNGLCREATESHADEROBJECTARBPROC)glcGetProcAddress("glCreateShader");
	if (not glCreateShader) qDebug() << "not glCreateShader";
	glDeleteShader				= (PFNGLDELETESHADERPROC)glcGetProcAddress("glDeleteShader");	
	if (not glDeleteShader) qDebug() << "not glDeleteShader";
	glShaderSource				= (PFNGLSHADERSOURCEARBPROC)glcGetProcAddress("glShaderSource");
	if (not glShaderSource) qDebug() << "not glShaderSource";
	glCompileShader				= (PFNGLCOMPILESHADERARBPROC)glcGetProcAddress("glCompileShader");	
	if (not glCompileShader) qDebug() << "not glCompileShader";
	glAttachShader				= (PFNGLATTACHOBJECTARBPROC)glcGetProcAddress("glAttachShader");	
	if (not glAttachShader) qDebug() << "not glAttachShader";
	glDetachShader				= (PFNGLDETACHOBJECTARBPROC)glcGetProcAddress("glDetachShader");	
	if (not glDetachShader) qDebug() << "not glDetachShader";
	glLinkProgram				= (PFNGLLINKPROGRAMARBPROC)glcGetProcAddress("glLinkProgram");
	if (not glLinkProgram) qDebug() << "not glLinkProgram";
	glGetUniformLocation		= (PFNGLGETUNIFORMLOCATIONARBPROC)glcGetProcAddress("glGetUniformLocation");
	if (not glGetUniformLocation) qDebug() << "not glGetUniformLocation";
	glUniform4f					= (PFNGLUNIFORM4FARBPROC)glcGetProcAddress("glUniform4f");
	if (not glUniform4f) qDebug() << "not glUniform4f";
	glUniform1i					= (PFNGLUNIFORM1IARBPROC)glcGetProcAddress("glUniform1i");
	if (not glUniform1i) qDebug() << "not glUniform1i";
	glGetShaderiv				= (PFNGLGETSHADERIVPROC)glcGetProcAddress("glGetShaderiv");
	if (not glGetShaderiv) qDebug() << "not glGetShaderiv";
	glGetProgramiv				= (PFNGLGETPROGRAMIVARBPROC)glcGetProcAddress("glGetProgramiv");
	if (not glGetProgramiv) qDebug() << "not glGetProgramiv";


	result= glCreateProgram and glDeletePrograms and glUseProgram and glCreateShader and glDeleteShader and
    glShaderSource and glCompileShader and glAttachShader and glDetachShader and glLinkProgram and
    glGetUniformLocation and glUniform4f and glUniform1i and glGetShaderiv and glGetProgramiv;

#endif
    return result;
}

