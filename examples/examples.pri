win32 {
    LIBS += -L"../../src" -lGLC_lib2
    INCLUDEPATH += "../../src"
}

unix {
     LIBS += -L"../../src" -lGLC_lib
     INCLUDEPATH += "../../src/"
}
