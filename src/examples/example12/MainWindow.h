/*
 *  MainWindow.h
 *
 *  Created on: 24/01/2016
 *      Author: Laurent Ribon
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class OpenGLView;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    OpenGLView* m_pOpenGLView;
};

#endif // MAINWINDOW_H
