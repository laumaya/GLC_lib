win32 {
    LIBS += -L"../../lib" -lGLC_lib3
    INCLUDEPATH += "../../lib"
}

unix {
     LIBS += -L"../../lib" -lGLC_lib.3
     INCLUDEPATH += "../../lib/"
}
