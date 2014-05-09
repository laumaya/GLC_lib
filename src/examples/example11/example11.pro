TEMPLATE = app
CONFIG += warn_on
TARGET = glc_declarative
VERSION = 1.0

unix:OBJECTS_DIR = ./Build
unix:MOC_DIR = ./Build
unix:UI_DIR = ./Build
unix:RCC_DIR = ./Build

include(../examples.pri)

QT += opengl declarative

SOURCES += main.cpp \
    glc_declarativeitem.cpp \
    glc_declarativeview.cpp

HEADERS += \
    glc_declarativeitem.h \
    glc_declarativeview.h

OTHER_FILES += \
    GLC_Declarative.qml

include(../../../install.pri)

target.path = $${GLC_LIB_DIR}/examples
INSTALLS += target

RESOURCES += \
    qml.qrc \
    models.qrc
