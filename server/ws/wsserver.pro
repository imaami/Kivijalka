QT       += core websockets gui

TARGET = wsserver
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

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
