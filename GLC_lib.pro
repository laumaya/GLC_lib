# GLC_lib qmake configuration
TEMPLATE = lib
QT += opengl \
    core
CONFIG += exceptions \
    release \
    create_prl \
    warn_on
TARGET = GLC_lib
VERSION = 0.9.8

unix:OBJECTS_DIR = ./Build
unix:MOC_DIR = ./Build
unix:UI_DIR = ./Build

DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += glc_maths.h \
    glc_utils_maths.h \
    glc_vector2d.h \
    glc_vector3d.h \
    glc_vector4d.h \
    glc_matrix4x4.h \
    glc_interpolator.h \
    glc_enum.h \
    glc_object.h \
    glc_material.h \
    glc_texture.h \
    glc_geometry.h \
    glc_circle.h \
    glc_orbitcircle.h \
    glc_cylinder.h \
    glc_light.h \
    glc_point.h \
    glc_box.h \
    glc_mesh2.h \
    glc_objtomesh2.h \
    glc_camera.h \
    glc_imageplane.h \
    glc_viewport.h \
    glc_collection.h \
    glc_factory.h \
    glc_boundingbox.h \
    glc_instance.h \
    glc_exception.h \
    glc_openglexception.h \
    glc_fileformatexception.h \
    glc_selectionmaterial.h \
    glc_world.h \
    glc_node.h \
    glc_product.h \
    glc_part.h \
    glc_objmtlloader.h \
    glc_objtoworld.h
    
SOURCES += glc_vector4d.cpp \
    glc_vector3d.cpp \
    glc_vector2d.cpp \
    glc_matrix4x4.cpp \
    glc_interpolator.cpp \
    glc_enum.cpp \
    glc_object.cpp \
    glc_material.cpp \
    glc_texture.cpp \
    glc_geometry.cpp \
    glc_circle.cpp \
    glc_orbitcircle.cpp \
    glc_cylinder.cpp \
    glc_light.cpp \
    glc_point.cpp \
    glc_box.cpp \
    glc_mesh2.cpp \
    glc_objtomesh2.cpp \
    glc_camera.cpp \
    glc_imageplane.cpp \
    glc_viewport.cpp \
    glc_collection.cpp \
    glc_factory.cpp \
    glc_boundingbox.cpp \
    glc_instance.cpp \
    glc_exception.cpp \
    glc_openglexception.cpp \
    glc_fileformatexception.cpp \
    glc_selectionmaterial.cpp \
    glc_world.cpp \
    glc_node.cpp \
    glc_product.cpp \
    glc_part.cpp \
    glc_objmtlloader.cpp \
    glc_objtoworld.cpp

# Windows compilation configuration
win32:CONFIG *= dll

# Linux install configuration
unix { 
    # Location of HEADERS and library
    LIB_DIR = /usr/lib
    INCLUDE_DIR = /usr/include
    
    # Adds a -P to preserve link
    QMAKE_COPY_FILE = $${QMAKE_COPY_FILE} \
        -P
    
    # install header
    include.path = $${INCLUDE_DIR}/GLC_lib
    include.files = $${HEADERS}
    
    # install library
    target.path = $${LIB_DIR}
    
    # "make install" configuration options
    INSTALLS += target \
        include
}
