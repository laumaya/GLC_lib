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
#include <GLC_Vector2d>


#include "mainwindow.h"
#include "glwidget.h"

MainWindow::MainWindow()
: p_GLWidget(NULL)
{
	p_GLWidget= new GLWidget(this);
	setCentralWidget(p_GLWidget);


    const double depth= 22.5;
    const double radius1= 120;
    const double radius2= 140;
    const double flatSideLength= 20;
    const double length= 300;

    // Test
    GLC_Point2d center0(flatSideLength / 2.0, -depth + radius1);
    GLC_Point2d center1(length / 2, -radius2);

    // GLC_Point2d center0(0.0, 0.0);
    // GLC_Point2d center1(20, 0.0);

    const QList<GLC_Line2d> lines(glc::linesTangentToTwoCircles(center0, radius1, center1, radius2));
    const int count= lines.count();
    qDebug() << "Tangent found : " << count;
    for (const GLC_Line2d& line : lines)
    {
        const GLC_Point2d startPoint(line.startingPoint());
        const GLC_Vector2d direction(line.direction());
        const GLC_Vector2d center(center0);

        const QList<GLC_Point2d> intersection(glc::line2CircleIntersection(startPoint, direction, center, radius1));

        const int intersectionCount= intersection.count();
        qDebug() << "IntersectionCount : " << intersectionCount;
        for (const GLC_Point2d& inter : intersection)
        {
            qDebug().noquote() << "Intersection : " << inter.toString();
        }
    }

}

