QT += core gui widgets opengl

TARGET = example12
TEMPLATE = app
VERSION = 1.0

unix:OBJECTS_DIR = ./Build
unix:MOC_DIR = ./Build
unix:UI_DIR = ./Build
unix:RCC_DIR = ./Build

include(../../../glc_lib.pri)

SOURCES += main.cpp\
        MainWindow.cpp \
    OpenGLView.cpp

HEADERS  += MainWindow.h \
    OpenGLView.h

FORMS    += MainWindow.ui

RESOURCES += \
    Models.qrc
