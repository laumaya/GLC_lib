TEMPLATE = app
CONFIG += warn_on
TARGET = glc_openglscene
VERSION = 1.0

unix:OBJECTS_DIR = ./Build
unix:MOC_DIR = ./Build
unix:UI_DIR = ./Build
unix:RCC_DIR = ./Build

include(../examples.pri)

QT += core \
    gui \
    opengl \
    xml
    
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

# Input
HEADERS += GLCScene.h GLCGraphicsView.h
SOURCES += main.cpp GLCScene.cpp GLCGraphicsView.cpp

QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9
RESOURCES += glc_openglscene.qrc

include(../../install.pri)

target.path = $${GLC_LIB_DIR}/examples
INSTALLS += target
