#-------------------------------------------------
#
# Project created by QtCreator 2018-03-28T10:54:41
#
#-------------------------------------------------

TARGET = marc
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt c++11 c++14

SOURCES += \
    collection.cpp \
    database.cpp \
    marcxml.cpp

HEADERS += \
    collection.h \
    database.h \
    marcxml.h

QMAKE_CXXFLAGS = -std=c++17 -pipe
QMAKE_CXXFLAGS_DEBUG = -O0 -ggdb3
QMAKE_CXXFLAGS_RELEASE = -O2
QMAKE_CXXFLAGS_WARN_OFF = warnoff

QMAKE_CXXFLAGS_WARN_ON = -Wall -Wsign-compare -Wpointer-arith -Winit-self \
    -Wcast-qual -Wredundant-decls -Wcast-align -Wwrite-strings \
    -Woverloaded-virtual -Wformat -Wno-unknown-pragmas -Wnon-virtual-dtor \
    -Wshadow

INCLUDEPATH += $$PWD/../jlee
INCLUDEPATH += /usr/include/libxml2
