/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

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
bool GLC_State::m_PointSpriteSupported= false;
bool GLC_State::m_UseShader= true;
bool GLC_State::m_UseSelectionShader= false;
bool GLC_State::m_IsInSelectionMode= false;
bool GLC_State::m_IsPixelCullingActivated= true;

QString GLC_State::m_Version;
QString GLC_State::m_Vendor;
QString GLC_State::m_Renderer;


GLC_State::~GLC_State()
{
}


// Return true if VBO is supported
bool GLC_State::vboSupported()
{
	return m_VboSupported;
}

// Return true if VBO is used
bool GLC_State::vboUsed()
{
	return m_UseVbo;
}

// Return true if GLSL is supported
bool GLC_State::glslSupported()
{
	return m_GlslSupported;
}

// Return true if GLSL is used
bool GLC_State::glslUsed()
{
	return m_UseShader;
}

// Return true if Point Sprite is supported
bool GLC_State::pointSpriteSupported()
{
	return m_PointSpriteSupported;
}

// Return true if selection shader is used
bool GLC_State::selectionShaderUsed()
{
	return m_UseSelectionShader;
}

// Return true if is in selection mode
bool GLC_State::isInSelectionMode()
{
	return m_IsInSelectionMode;
}

// Return the Opengl version
QString GLC_State::version()
{
	return m_Version;
}

// Return the Opengl vendor
QString GLC_State::vendor()
{
	return m_Vendor;
}

// Return the Opengl renderer
QString GLC_State::renderer()
{
	return m_Renderer;
}

// Return true if OpenGL Vendor is NVIDIA
bool GLC_State::vendorIsNvidia()
{
	return m_Vendor.contains("NVIDIA");
}

// Return true if pixel culling is activate
bool GLC_State::isPixelCullingActivated()
{
	return m_IsPixelCullingActivated;
}




//! Intialize the state
void GLC_State::init()
{
	setVboSupport();
	setGlslSupport();
	setPointSpriteSupport();
	m_Version= (char *) glGetString(GL_VERSION);
	m_Vendor= (char *) glGetString(GL_VENDOR);
	m_Renderer= (char *) glGetString(GL_RENDERER);
}

// Set VBO support
void GLC_State::setVboSupport()
{
	m_VboSupported= glc::extensionIsSupported("ARB_vertex_buffer_object") && glc::loadVboExtension();
}

// Set VBO usage
void GLC_State::setVboUsage(const bool vboUsed)
{
	m_UseVbo= m_VboSupported && vboUsed;
}

// Set GLSL support
void GLC_State::setGlslSupport()
{
	m_GlslSupported= glc::extensionIsSupported("GL_ARB_shading_language_100") && glc::loadGlSlExtension();
}

// Set Point Sprite support
void GLC_State::setPointSpriteSupport()
{
	m_PointSpriteSupported= glc::extensionIsSupported("GL_ARB_point_parameters") && glc::loadPointSpriteExtension();
}

// Set GLSL usage
void GLC_State::setGlslUsage(const bool glslUsage)
{
	m_UseShader= m_GlslSupported && glslUsage;
}

// Set selection shader usage
void GLC_State::setSelectionShaderUsage(const bool shaderUsed)
{
	m_UseSelectionShader= shaderUsed && m_GlslSupported;
}

// Set selection mode
void GLC_State::setSelectionMode(const bool mode)
{
	m_IsInSelectionMode= mode;
}

// Set pixel culling state
void GLC_State::setPixelCullingUsage(const bool activation)
{
	m_IsPixelCullingActivated= activation;
}
