TEMPLATE = app
CONFIG += warn_on
TARGET = glc_declarative
VERSION = 1.0

unix:OBJECTS_DIR = ./Build
unix:MOC_DIR = ./Build
unix:UI_DIR = ./Build
unix:RCC_DIR = ./Build

include(../examples.pri)

QT += opengl qml quick

win32 {
    LIBS += -L"$$(GLC_LIB_DIR)/lib" \
        -lGLC_lib2
    DEPENDPATH+= "$$(GLC_LIB_DIR)/lib"
    INCLUDEPATH += "$$(GLC_LIB_DIR)/include"
}

unix {
    LIBS += -lGLC_lib
    INCLUDEPATH += "/usr/local/include/GLC_lib"
}


SOURCES += main.cpp

HEADERS +=

OTHER_FILES += \
    GLC_Quick.qml

include(../../install.pri)

target.path = $${GLC_LIB_DIR}/examples
INSTALLS += target

RESOURCES += \
    qml.qrc \
    models.qrc
