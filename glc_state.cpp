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

//! \file glc_state.cpp implementation of the GLC_State class.

#include "glc_state.h"
#include "glc_ext.h"

bool GLC_State::m_VboSupported= false;
bool GLC_State::m_UseVbo= true;
bool GLC_State::m_GlslSupported= false;
bool GLC_State::m_UseShader= true;
bool GLC_State::m_UseSelectionShader= false;
bool GLC_State::m_IsInSelectionMode= false;
QString GLC_State::m_Version;
QString GLC_State::m_Vendor;
QString GLC_State::m_Renderer;


GLC_State::~GLC_State()
{
}

//! Intialize the state
void GLC_State::init()
{
	setVboSupport();
	setGlslSupport();
	m_Version= (char *) glGetString(GL_VERSION);
	m_Vendor= (char *) glGetString(GL_VENDOR);
	m_Renderer= (char *) glGetString(GL_RENDERER);
}

// Set VBO support
void GLC_State::setVboSupport()
{
	m_VboSupported= glc::extensionIsSupported("ARB_vertex_buffer_object") and glc::loadVboExtension();
}

// Set VBO usage
void GLC_State::setVboUsage(const bool vboUsed)
{
	m_UseVbo= m_VboSupported and vboUsed;
}

// Set GLSL support
void GLC_State::setGlslSupport()
{
	m_GlslSupported= glc::extensionIsSupported("GL_ARB_shading_language_100") and glc::loadGlSlExtension();
}

// Set GLSL usage
void GLC_State::setGlslUsage(const bool glslUsage)
{
	m_UseShader= m_GlslSupported and glslUsage;
}

// Set selection shader usage
void GLC_State::setSelectionShaderUsage(const bool shaderUsed)
{
	m_UseSelectionShader= shaderUsed and m_GlslSupported;
}
