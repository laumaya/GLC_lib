/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
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

#ifndef GLWIDGET_H_
#define GLWIDGET_H_

#include <QGLWidget>
//////////////////////////// GLC specific///////////////////////////////////////
// The factory
#include <GLC_Factory>
// Light
#include <GLC_Light>
// The Viewport with a default camera
#include <GLC_Viewport>
// The collection which manage GLC_object
#include <GLC_3DViewCollection>
// The Mover controller is used to change the point of view
#include <GLC_MoverController>
//////////////////////////End GLC specific/////////////////////////////////////

class GLWidget : public QGLWidget
{
public:
	GLWidget(QWidget *p_parent);
	~GLWidget();

private:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	// Create GLC_Object to display
	void CreateScene();

	//Mouse events
	void mousePressEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);
private:
//////////////////////////// GLC specific///////////////////////////////////////
	GLC_Light m_Light;
	GLC_3DViewCollection m_Collection;
	GLC_Viewport m_GlView;
	GLC_MoverController m_MoverController;
//////////////////////////End GLC specific/////////////////////////////////////
};

#endif /*GLWIDGET_H_*/
