# GLC_lib qmake configuration
TEMPLATE = lib
QT += opengl \
    core
CONFIG += exceptions \
    debug \
    warn_on
TARGET = GLC_lib
VERSION = 1.0.0

unix:OBJECTS_DIR = ./Build
unix:MOC_DIR = ./Build
unix:UI_DIR = ./Build

DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += lib3ds/atmosphere.h \
           lib3ds/background.h \
           lib3ds/camera.h \
           lib3ds/chunk.h \
           lib3ds/chunktable.h \
           lib3ds/ease.h \
           lib3ds/file.h \
           lib3ds/io.h \
           lib3ds/light.h \
           lib3ds/material.h \
           lib3ds/matrix.h \
           lib3ds/mesh.h \
           lib3ds/node.h \
           lib3ds/quat.h \
           lib3ds/shadow.h \
           lib3ds/tcb.h \
           lib3ds/tracks.h \
           lib3ds/types.h \
           lib3ds/vector.h \
           lib3ds/viewport.h

HEADERS += glc_utils_maths.h \
           glc_vector2d.h \
           glc_vector2df.h \
           glc_vector3d.h \
           glc_vector3df.h \
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
           glc_objtoworld.h \
           glc_stltoworld.h \
           glc_offtoworld.h \
           glc_3dstoworld.h \
           glc_geomtools.h

SOURCES += lib3ds/atmosphere.c \
           lib3ds/background.c \
           lib3ds/camera.c \
           lib3ds/chunk.c \
           lib3ds/ease.c \
           lib3ds/file.c \
           lib3ds/io.c \
           lib3ds/light.c \
           lib3ds/material.c \
           lib3ds/matrix.c \
           lib3ds/mesh.c \
           lib3ds/node.c \
           lib3ds/quat.c \
           lib3ds/shadow.c \
           lib3ds/tcb.c \
           lib3ds/tracks.c \
           lib3ds/vector.c \
           lib3ds/viewport.c
   
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
           glc_objtoworld.cpp \
           glc_stltoworld.cpp \
           glc_offtoworld.cpp \
           glc_3dstoworld.cpp \
           glc_geomtools.cpp

# Windows compilation configuration
win32:CONFIG *= dll

# install header
HEADERS_INST = include/GLC_BoundingBox \
    		   include/GLC_Box \
    		   include/GLC_Camera \
    		   include/GLC_Circle \
    		   include/GLC_Collection \
    		   include/GLC_Cylinder \
    		   include/GLC_Exception \
    		   include/GLC_Factory \
    		   include/GLC_FileFormatException \
    		   include/GLC_Geometry \
    		   include/GLC_ImagePlane \
    		   include/GLC_Instance \
    		   include/GLC_Interpolator \
    		   include/GLC_Light \
    		   include/GLC_Material \
    		   include/GLC_Matrix4x4 \
    		   include/GLC_Mesh2 \
    		   include/GLC_Node \
    		   include/GLC_Object \
    		   include/GLC_OpenGlException \
    		   include/GLC_Part \
    		   include/GLC_Point \
    		   include/GLC_Point2d \
    		   include/GLC_Point2df \
    		   include/GLC_Point3d \
    		   include/GLC_Point3df \
    		   include/GLC_Point4d \
    		   include/GLC_Product \
    		   include/GLC_Texture \
    		   include/GLC_Vector2d \
    		   include/GLC_Vector2df \
    		   include/GLC_Vector3d \
    		   include/GLC_Vector3df \
    		   include/GLC_Vector4d \
    		   include/GLC_Viewport \
    		   include/GLC_World
    			   
# Linux install configuration
unix { 
    # Location of HEADERS and library
    LIB_DIR = /usr/lib
    INCLUDE_DIR = /usr/include
}

# Windows Install configuration
win32 { 
    # Location of HEADERS and library
    LIB_DIR = C:/GLC_lib/lib
    INCLUDE_DIR = C:/GLC_lib/include
}    
# Adds a -P to preserve link
QMAKE_COPY_FILE = $${QMAKE_COPY_FILE} -P
            
include.path = $${INCLUDE_DIR}/GLC_lib
include.files = $${HEADERS} $${HEADERS_INST}
    
# install library
target.path = $${LIB_DIR}
    
# "make install" configuration options
INSTALLS += target include

