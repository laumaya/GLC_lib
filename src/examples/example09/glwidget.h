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
#include <QTimer>

#include <GLC_Factory>
#include <GLC_Viewport>
#include <GLC_MoverController>
#include <GLC_Light>

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *p_parent);
	~GLWidget();

    void doRender();
private:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void createScene();
	//Mouse events
	void mousePressEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);

	inline double getRandomMinMax( double fMin, double fMax )
	{
	    double fRandNum = (double)qrand() / RAND_MAX;
	    return fMin + (fMax - fMin) * fRandNum;
	}
//////////////////////////////////////////////////////////////////////
// Private slots Functions
//////////////////////////////////////////////////////////////////////
private slots:
	//! Rotate the view
	void rotateView();

private:
	GLC_Light m_Light;
	GLC_World m_World;
	GLC_Viewport m_GlView;
	GLC_MoverController m_MoverController;
	GLC_BoundingBox m_ShuttleBoundingBox;
	//! The timer used for motion
	QTimer m_MotionTimer;
};

#endif /*GLWIDGET_H_*/
