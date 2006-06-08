/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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

//! \file glc_openglexception.h implementation of the GLC_OpenGlException class.

#include "glc_openglexception.h"

//////////////////////////////////////////////////////////////////////
// Constructor destructor
//////////////////////////////////////////////////////////////////////

GLC_OpenGlException::GLC_OpenGlException(const std::string& message, GLenum glError)
:GLC_Exception(message)
{
	switch (glError)
	{
		case GL_INVALID_ENUM :
			m_GlErrorDescription= "GLenum argument out of range";
			break;
		case GL_INVALID_VALUE :
			m_GlErrorDescription= "Numeric argument out of range";
			break;
		case GL_INVALID_OPERATION :
			m_GlErrorDescription= "Operation illegal in current state";
			break;
		case GL_STACK_OVERFLOW :
			m_GlErrorDescription= "Command would cause a stack overflow";
			break;
		case GL_STACK_UNDERFLOW :
			m_GlErrorDescription= "Command would cause a stack underflow";
			break;
		case GL_OUT_OF_MEMORY :
			m_GlErrorDescription= "Not enough memmory left to execute command";
			break;
		default :
			m_GlErrorDescription= "VERY BAD : UNKNOWN ERROR";
			break;
			
	}
}

GLC_OpenGlException::~GLC_OpenGlException() throw()
{
	
}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Return exception description
const char* GLC_OpenGlException::what() const throw()
{
	std::string exceptionmsg("GLC_OpenGlException : ");
	exceptionmsg.append(m_ErrorDescription);
	exceptionmsg.append(m_GlErrorDescription);
	return exceptionmsg.c_str();
}
