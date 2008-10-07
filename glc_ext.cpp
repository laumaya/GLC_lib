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
#  define glcGetProcAddress(name) wglGetProcAddress((LPCSTR)name)
#else
#  if defined(Q_OS_LINUX)
#    define glcGetProcAddress(name) (*glXGetProcAddressARB)(name)
#  endif
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
    glBindBuffer				= (PFNGLBINDBUFFERARBPROC)glcGetProcAddress("glBindBufferARB");
    glDeleteBuffers				= (PFNGLDELETEBUFFERSARBPROC)glcGetProcAddress("glDeleteBuffersARB");
    glGenBuffers				= (PFNGLGENBUFFERSARBPROC)glcGetProcAddress("glGenBuffersARB");
    glIsBuffer					= (PFNGLISBUFFERARBPROC)glcGetProcAddress("glIsBufferARB");
    glBufferData				= (PFNGLBUFFERDATAARBPROC)glcGetProcAddress("glBufferDataARB");
    glBufferSubData				= (PFNGLBUFFERSUBDATAARBPROC)glcGetProcAddress("glBufferSubDataARB");
    glGetBufferSubData			= (PFNGLGETBUFFERSUBDATAARBPROC)glcGetProcAddress("glGetBufferSubDataARB");
    glMapBuffer					= (PFNGLMAPBUFFERARBPROC)glcGetProcAddress("glMapBufferARB");
    glUnmapBuffer				= (PFNGLUNMAPBUFFERARBPROC)glcGetProcAddress("glUnmapBufferARB");
    glGetBufferParameteriv		= (PFNGLGETBUFFERPARAMETERIVARBPROC)glcGetProcAddress("glGetBufferParameterivARB");
    glGetBufferPointerv			= (PFNGLGETBUFFERPOINTERVARBPROC)glcGetProcAddress("glGetBufferPointervARB");

    result = glBindBuffer and glDeleteBuffers and glGenBuffers and glIsBuffer and glBufferData and glBufferSubData and
    glGetBufferSubData and glMapBuffer and glUnmapBuffer and glGetBufferParameteriv and glGetBufferPointerv;   
#endif
    return result;

}

