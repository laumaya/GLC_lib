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

//! \file glc_state.h interface for the GLC_State class.

#ifndef GLC_STATE_H_
#define GLC_STATE_H_

class GLC_State
{
private:
	GLC_State();
public:
	~GLC_State();

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return true if GLSL is supported
	inline static bool glslIsSupported() {return m_GlslSupported;}
	//! Return true if slection shader is used
	inline static bool selectionShaderUsed() {return m_UseSelectionShader;}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set GLSL support
	static void setGlslSupport();
	//! Set selection shader usage
	static void setSelectionShaderUsage(const bool);
	
//@}

//////////////////////////////////////////////////////////////////////
//Private attributes
//////////////////////////////////////////////////////////////////////
private:
	//! GLSL supported flag
	static bool m_GlslSupported;
	
	//! Use selectionShader flag
	static bool m_UseSelectionShader;

};

#endif /*GLC_STATE_H_*/
