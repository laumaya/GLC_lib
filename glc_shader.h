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

//! \file glc_shader.h interface for the GLC_Shader class.

#ifndef GLC_SHADER_H_
#define GLC_SHADER_H_

#include "glc_ext.h"
#include <QStack>
#include <QFile>
//////////////////////////////////////////////////////////////////////
//! \class GLC_Shader
/*! \brief GLC_Shader : OpenGL shader abstraction*/

/*! An GLC_Shader encapsulate vertex, fragment shader and programm\n
 *  GLC_Shader provide functionnality to load, compile and execute
 * 	GLSL vertex and fragment shader.
 */
 
//////////////////////////////////////////////////////////////////////
class GLC_Shader
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default constructor
	GLC_Shader();
	//! Construct shader with specifie vertex and fragment
	GLC_Shader(QFile&, QFile&);
	//! Copy constructor
	GLC_Shader(const GLC_Shader&);
	//! Shader destructor
	~GLC_Shader();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return true if the shader is usable
	inline bool isUsable() const {return m_ProgramShader != 0;}
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Set Vertex and fragment shaders
	void setVertexAndFragmentShader(QFile&, QFile&);
//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Use this shader programm
	/*! Throw GLC_Exception if the programm is not usable*/
	void useProgramm();
	//! Use previous programm shader
	static void usePreviousProgramm();
	//! Compile and attach shaders to a program shader
	/*! Throw GLC_Exception if vertex and fragment shader are not been set*/
	void createAndCompileProgrammShader();
	//! Create and compile vertex shader
	void createAndLinkVertexShader();
	//! Create and compile fragment shader
	void createAndLinkFragmentShader();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
	//! Load Vertex shader
	void loadVertexShader();
	//! Load fragment shaders
	void loadFragmentShader();
	//! Set Vertex shader
	void setVertexShader(QFile&);
	//! Set fragment shaders
	void setFragmentShader(QFile&);
	//! Return char* of an Ascii file
	QByteArray readShaderFile(QFile&);


//@}
	
//////////////////////////////////////////////////////////////////////
// private members
//////////////////////////////////////////////////////////////////////
private:
	//! The programm ID Stack
	static QStack<GLuint> m_ProgrammStack;
	//! The current programm ID
	static GLuint m_CurrentProgramm;

	//! Vertex shader file
	QByteArray m_VertexByteArray;
	//! Vertex shader ID
	GLuint m_VertexShader;
	//! Fragment shader file
	QByteArray m_FragmentByteArray;
	//! Fragment shader ID
	GLuint m_FragmentShader;
	//! Programm shader ID
	GLuint m_ProgramShader;

};

#endif /*GLC_SHADER_H_*/
