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
    watcherthread.cpp \
    watcher.c

HEADERS += \
    banner.h \
    bannercache.h \
    wsserver.h \
    watcherthread.h \
    watcher.h
