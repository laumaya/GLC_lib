TEMPLATE = subdirs
CONFIG += ordered

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

SUBDIRS += src
