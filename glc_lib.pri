win32 {
    LIBS += -L"../../lib" -lGLC_lib3
    INCLUDEPATH += "../../lib"
    LIBS += -lopengl32
}

macx {
     LIBS += -L"../../lib" -lGLC_lib.3
     INCLUDEPATH += "../../lib/"
}

unix:!macx {
     LIBS += -L"../../lib" -l:libGLC_lib.so.3
     INCLUDEPATH += "../../lib/"
}
