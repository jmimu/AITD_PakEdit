QT       += core gui xml
#CONFIG += debug
#CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AITD_PakEdit
TEMPLATE = app

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O0

SOURCES += src/main.cpp \
    fitd/renderer.cpp \
    src/bmp.cpp \
    src/mainwindow.cpp \
    src/alonefile.cpp \
    src/pakfile.cpp \
    src/jsoncpp.cpp \
    src/db.cpp \
    src/alonefloor.cpp \
    src/alonebody.cpp \
    src/settings.cpp \
    fitd/floor.cpp \
    fitd/hqr.cpp \
    fitd/room.cpp \
    fitd/fileAccess.cpp \
    fitd/cosTable.cpp \
    fitd/vars.cpp \
    fitd/unpack.cpp \
    fitd/pak.cpp

HEADERS  += src/mainwindow.h \
    fitd/renderer.h \
    src/alonefile.h \
    src/bmp.h \
    src/pakfile.h \
    src/json/json-forwards.h \
    src/json/json.h \
    src/db.h \
    src/alonefloor.h \
    src/alonebody.h \
    src/settings.h \
    src/types.h \
    src/version.h \
    fitd/floor.h \
    fitd/hqr.h \
    fitd/room.h \
    fitd/fileAccess.h \
    fitd/anim.h \
    fitd/unpack.h \
    fitd/vars.h \
    fitd/pak.h

FORMS    += src/mainwindow.ui

win32:INCLUDEPATH = C:\libs\
win32:LIBS =        C:\libs\

INCLUDEPATH += src/ \
	       fitd/.

LIBS += -lz
QMAKE_CXXFLAGS += -g

RESOURCES += \
    src/data.qrc

unix {
    target.path = /usr/bin/
    INSTALLS += target
}
