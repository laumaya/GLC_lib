/*
 *  MainWindow.cpp
 *
 *  Created on: 24/01/2016
 *      Author: Laurent Ribon
 */

#include <GLC_Factory>
#include <GLC_World>
#include <GLC_OpenGLViewHandler>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "OpenGLView.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pOpenGLView(NULL)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setSamples(8);
    format.setMajorVersion(3);
    QSurfaceFormat::setDefaultFormat(format);
    m_pOpenGLView= new OpenGLView(this);

    ui->setupUi(this);
    setCentralWidget(m_pOpenGLView);

    QFile file(":/Models/logo.obj");
    GLC_World world= GLC_Factory::instance()->createWorldFromFile(file);

    QSharedPointer<GLC_OpenGLViewHandler> viewHandlerPointer(new GLC_OpenGLViewHandler);
    viewHandlerPointer->setWorld(world);
    QVariant viewHandler;
    viewHandler.setValue(viewHandlerPointer);
    m_pOpenGLView->setViewhandler(viewHandler);
}

MainWindow::~MainWindow()
{
    delete ui;
}
