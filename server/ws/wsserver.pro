QT       += core websockets gui

TARGET = wsserver
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    banner.cpp \
    wsserver.cpp \
    watcherthread.cpp \
    watcher.c

HEADERS += \
    banner.h \
    wsserver.h \
    watcherthread.h \
    watcher.h
