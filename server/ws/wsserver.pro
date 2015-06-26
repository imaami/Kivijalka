QT       += core websockets gui

TARGET = wsserver
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CFLAGS += -march=native -O3 -std=gnu11
QMAKE_CXXFLAGS += -march=native -O3 -std=gnu++11

SOURCES += \
    main.cpp \
    banner.cpp \
    bannercache.cpp \
    wsserver.cpp \
    diskreader.cpp \
    imgworker.cpp \
    diskwriter.cpp \
    read-file.c \
    watcher.c \
    global.c \
    path.c \
    file.c \
    img_file.c \
    img_data.c

HEADERS += \
    banner.h \
    bannercache.h \
    wsserver.h \
    diskreader.h \
    imgworker.h \
    diskwriter.h \
    read-file.h \
    watcher.h \
    global.h \
    path.h \
    file.h \
    img_file.h \
    img_data.h
