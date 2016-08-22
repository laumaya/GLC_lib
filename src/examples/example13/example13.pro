QT += core gui widgets opengl

TARGET = example13
TEMPLATE = app
VERSION = 1.0

unix:OBJECTS_DIR = ./Build
unix:MOC_DIR = ./Build
unix:UI_DIR = ./Build
unix:RCC_DIR = ./Build

include(../../../glc_lib.pri)

SOURCES += main.cpp\
        MainWindow.cpp \
    OpenGLView.cpp \
    Model.cpp

HEADERS  += MainWindow.h \
    OpenGLView.h \
    Model.h

FORMS    += MainWindow.ui

RESOURCES +=
