/*
 *  main.cpp
 *
 *  Created on: 24/01/2016
 *      Author: Laurent Ribon
 */

#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
