LDIR = $$_PRO_FILE_PWD_/../../lib
INCLUDEPATH += "$$LDIR"

win32 {
    LIBS += -L"$$LDIR" -lGLC_lib3
    LIBS += -lopengl32
}

macx {
     LIBS += -L"$$LDIR" -lGLC_lib.3

     # bundle the .dylib files
     glclib.path = Contents/Frameworks
     glclib.files = $$files($$LDIR/*.dylib)
     QMAKE_BUNDLE_DATA += glclib

     # point @rpath to the libraries
     QMAKE_RPATHDIR += @executable_path/../Frameworks
}

unix:!macx {
     LIBS += -L"$$LDIR" -l:libGLC_lib.so.3
}
