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
    diskthread.cpp \
    watcher.c \
    path.c \
    img.c

HEADERS += \
    banner.h \
    bannercache.h \
    wsserver.h \
    watcherthread.h \
    diskthread.h \
    watcher.h \
    path.h \
    img.h
