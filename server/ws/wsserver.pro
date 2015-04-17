QT       += core websockets
QT       -= gui

TARGET = wsserver
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    wsserver.cpp \
    watcherthread.cpp \
    watcher.c

HEADERS += \
    wsserver.h \
    watcherthread.h \
    watcher.h
