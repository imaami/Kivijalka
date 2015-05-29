QT       += core websockets gui

TARGET = wsserver
CONFIG   += console c++11 link_pkgconfig
CONFIG   -= app_bundle
PKGCONFIG += MagickWand

TEMPLATE = app

QMAKE_CFLAGS += -march=native -O3 -std=gnu11
QMAKE_CXXFLAGS += -march=native -O3 -std=gnu++11

SOURCES += \
    main.cpp \
    banner.cpp \
    bannercache.cpp \
    wsserver.cpp \
    watcherthread.cpp \
    diskreader.cpp \
    imgthread.cpp \
    diskwriter.cpp \
    read-file.c \
    watcher.c \
    global.c \
    path.c \
    img.c

HEADERS += \
    atomic.h \
    banner.h \
    bannercache.h \
    wsserver.h \
    watcherthread.h \
    diskreader.h \
    imgthread.h \
    diskwriter.h \
    read-file.h \
    watcher.h \
    global.h \
    path.h \
    img.h
