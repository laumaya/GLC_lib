#GLC_lib qmake configuration

TEMPLATE = lib
QT += opengl
CONFIG += release create_prl warn_on

TARGET += GLC_lib
VERSION  = 09.01
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += glc_utils_maths.h \
           glc_vector4d.h \
           glc_matrix4x4.h \
           glc_interpolateur.h \
           glc_enum.h \
           glc_object.h \
		   glc_material.h \
           glc_texture.h \		   
           glc_geometrie.h \
           glc_cercle.h \
           glc_orbitcercle.h \
           glc_cylindre.h \
           glc_lumiere.h \
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
            glc_interpolateur.cpp \
            glc_enum.cpp \
			glc_object.cpp \
		    glc_material.cpp \
            glc_texture.cpp \		   
            glc_geometrie.cpp \
            glc_cercle.cpp \
            glc_orbitcercle.cpp \
            glc_cylindre.cpp \
            glc_lumiere.cpp \
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
