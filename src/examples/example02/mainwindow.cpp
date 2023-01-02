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

#include <QtDebug>
#include <QList>
#include <GLC_GeomTools>
#include <GLC_Point2d>


#include "mainwindow.h"
#include "glwidget.h"

MainWindow::MainWindow()
: p_GLWidget(NULL)
{	

    // Circle 1
    GLC_Point2d p0(150.0, 0.0);
    GLC_Point2d p1(180, 100.0);
    double r0= 80.0;
    double r1= 80.0;
    QList<GLC_Point2d> intersections= glc::findIntersectionBetwen2Circle(p0, r0, p1, r1);
    if (intersections.isEmpty())
    {
        qDebug() << "No intersection";
    }
    else
    {
        qDebug() << "Intersection count = " << intersections.count();
        for (int i= 0; i < intersections.count(); ++i)
        {
            qDebug().noquote() << intersections.at(i).toString();
        }
    }



    //p_GLWidget= new GLWidget(this);
    //setCentralWidget(p_GLWidget);
}
