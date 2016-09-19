/*
 *  MainWindow.cpp
 *
 *  Created on: 24/01/2016
 *      Author: Laurent Ribon
 */

#include <GLC_Factory>
#include <GLC_World>
#include <GLC_OpenGLViewHandler>
#include <GLC_Matrix4x4>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "OpenGLView.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pOpenGLView(NULL)
    , m_Model()
    , m_Timer()
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setSamples(8);
    format.setMajorVersion(3);
    QSurfaceFormat::setDefaultFormat(format);
    m_pOpenGLView= new OpenGLView(this);

    ui->setupUi(this);
    setCentralWidget(m_pOpenGLView);


    QSharedPointer<GLC_OpenGLViewHandler> viewHandlerPointer(new GLC_OpenGLViewHandler);
    viewHandlerPointer->setWorld(m_Model.world());
    viewHandlerPointer->viewportHandle()->cameraHandle()->setDefaultUpVector(glc::Z_AXIS);
    viewHandlerPointer->viewportHandle()->cameraHandle()->setIsoView();
    viewHandlerPointer->setCurrentRenderFlag(glc::WireRenderFlag);
    QVariant viewHandler;
    viewHandler.setValue(viewHandlerPointer);
    m_pOpenGLView->setViewhandler(viewHandler);

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(updateMotion()), Qt::DirectConnection);
    m_Timer.start(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMotion()
{
    static double offset= 0.1;
    static double length= offset;

    GLC_Matrix4x4 matrix(offset, 0.0, 0.0);
    m_Model.updateChildMatrix(matrix);
    m_Model.update();
    m_pOpenGLView->update();

    if ((length > 5) || (length < -5))
    {
        offset= -offset;
    }
    length+= offset;
}
