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
    wsserver.cpp \
    diskreader.cpp \
    imgworker.cpp \
    diskwriter.cpp \
    args.c \
    read-file.c \
    watcher.c \
    display.c \
    packet.c \
    banner.c \
    cache.c \
    hex.c \
    global.c \
    path.c \
    file.c \
    buf.c \
    img_file.c \
    img_data.c

HEADERS += \
    wsserver.h \
    diskreader.h \
    imgworker.h \
    diskwriter.h \
    args.h \
    read-file.h \
    watcher.h \
    display.h \
    packet.h \
    banner.h \
    img.h \
    cache.h \
    hex.h \
    global.h \
    path.h \
    file.h \
    buf.h \
    img_file.h \
    img_data.h
