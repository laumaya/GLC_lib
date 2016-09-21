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

#include <QtGui>
#include <QList>
#include <QtDebug>

#include <GLC_GeomTools>
#include <GLC_Vector3d>


#include "mainwindow.h"
#include "glwidget.h"

MainWindow::MainWindow()
: p_GLWidget(NULL)
{
	p_GLWidget= new GLWidget(this);
	setCentralWidget(p_GLWidget);


    // Test
    const GLC_Point3d p0(100.0, 89.0, 0.0);
    const GLC_Point3d p1(43.0, 80.0, 0.0);
    const GLC_Point3d p2(60.0, 15.0, 0.0);

    QList<GLC_Point3d> segments;
    segments << p0 << p1 << p2;

    QList<GLC_Point3d> segmentsAndArc= glc::AddCorner(segments, 18.0, 2);

    const int count= segmentsAndArc.count();
    for (int i= 0; i < count; ++i)
    {
        qDebug() << "Point " << i << " = " << segmentsAndArc.at(i).toString();
    }

}

