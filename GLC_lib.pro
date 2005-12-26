#GLC_lib qmake configuration

TEMPLATE = lib
QT += opengl
CONFIG *= release create_prl warn_on

TARGET = GLC_lib
VERSION  = 0.9.4
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += glc_maths.h \
           glc_utils_maths.h \
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
           glc_vertex.h \
           glc_face.h \
           glc_mesh.h \
           glc_objtomesh.h \
           glc_camera.h \
           glc_imageplane.h \
           glc_viewport.h \
           glc_collection.h
           
             
SOURCES +=  glc_vector4d.cpp \
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
            glc_vertex.cpp \
            glc_face.cpp \
            glc_mesh.cpp \
           	glc_objtomesh.cpp \            
            glc_camera.cpp \
            glc_imageplane.cpp \
            glc_viewport.cpp \
            glc_collection.cpp
            
#Windows compilation configuration       
win32 {
    CONFIG *= dll
}

# Linux install configuration
unix {
  # Location of HEADERS and library
  
  LIB_DIR = /usr/lib
  INCLUDE_DIR = /usr/include

  # Adds a -P to preserve link
  QMAKE_COPY_FILE = $${QMAKE_COPY_FILE} -P

  # install header
  include.path = $${INCLUDE_DIR}/GLC_lib
  include.files = $${HEADERS}

  # install library
  target.path = $${LIB_DIR}

  # "make install" configuration options
  INSTALLS += target include

}
