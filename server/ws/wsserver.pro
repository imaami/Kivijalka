QT       += core network websockets gui

TARGET = wsserver
CONFIG   += console c++11 link_pkgconfig
CONFIG   -= app_bundle
PKGCONFIG += openssl uuid

TEMPLATE = app

QMAKE_CFLAGS += -march=native -O3 -std=gnu11
QMAKE_CXXFLAGS += -march=native -O3 -std=gnu++11

SOURCES += \
    main.cpp \
    bannercache.cpp \
    wsserver.cpp \
    diskreader.cpp \
    imgworker.cpp \
    diskwriter.cpp \
    read-file.c \
    watcher.c \
    display.c \
    banner.c \
    banner_cache.c \
    global.c \
    path.c \
    file.c \
    buf.c \
    img_file.c \
    img_data.c

HEADERS += \
    bannercache.h \
    wsserver.h \
    diskreader.h \
    imgworker.h \
    diskwriter.h \
    read-file.h \
    watcher.h \
    display.h \
    banner.h \
    banner_cache.h \
    global.h \
    path.h \
    file.h \
    buf.h \
    img_file.h \
    img_data.h
