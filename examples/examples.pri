win32 {
    LIBS += -L"../../src" -lGLC_lib3
    INCLUDEPATH += "../../src"
}

unix {
     LIBS += -L"../../src" -lGLC_lib.3
     INCLUDEPATH += "../../src/"
}
