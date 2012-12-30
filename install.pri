# Linux and macx install configuration
unix {
    # Location of HEADERS and library
    GLC_LIB_DIR = $$(HOME)/GLC-lib-2.5
    LIB_DIR = /usr/local/lib
    INCLUDE_DIR = /usr/local/include/GLC-lib-2.5
    # Adds a -P to preserve link
        QMAKE_COPY_FILE = $${QMAKE_COPY_FILE} -P
}

# Windows Install configuration
win32 {
    # Location of HEADERS and library
    GLC_LIB_DIR = C:/GLC-lib-2.5
    LIB_DIR = $${GLC_LIB_DIR}/lib
    INCLUDE_DIR = $${GLC_LIB_DIR}/include
}
