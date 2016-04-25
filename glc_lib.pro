TEMPLATE = subdirs
CONFIG += ordered
QMAKE_MAC_SDK = macosx10.11

# We need opengl, minimum es2 or desktop
!contains(QT_CONFIG, opengl) {
    error(GLC_lib requires OpenGL!)
}
contains(QT_CONFIG, opengles1) {
    error(GLC_lib does not support OpenGL ES 1!)
}
contains(QT_CONFIG, opengles2) {
    error(GLC_lib does not support OpenGL ES 2!)
}

greaterThan(QT_MAJOR_VERSION, 4){
cache()
}

SUBDIRS += src/lib \
           src/plugins \
           src/examples
