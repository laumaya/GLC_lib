/*
 *  MainWindow.h
 *
 *  Created on: 24/01/2016
 *      Author: Laurent Ribon
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "Model.h"

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

private slots:
    void updateMotion();

private:
    Ui::MainWindow *ui;
    OpenGLView* m_pOpenGLView;
    Model m_Model;
    QTimer m_Timer;
};

#endif // MAINWINDOW_H
