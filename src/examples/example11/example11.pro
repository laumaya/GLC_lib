TEMPLATE = app
CONFIG += warn_on
TARGET = glc_quick
VERSION = 1.0

unix:OBJECTS_DIR = ./Build
unix:MOC_DIR = ./Build
unix:UI_DIR = ./Build
unix:RCC_DIR = ./Build

include(../../../glc_lib.pri)

QT += opengl qml quick

SOURCES += main.cpp

HEADERS +=

OTHER_FILES += \
    GLC_Quick.qml

include(../../../install.pri)

target.path = $${GLC_LIB_DIR}/examples
INSTALLS += target

RESOURCES += \
    qml.qrc \
    models.qrc
