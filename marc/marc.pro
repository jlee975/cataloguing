#-------------------------------------------------
#
# Project created by QtCreator 2018-03-28T10:54:41
#
#-------------------------------------------------
include(../vars.pri)

TARGET = marc
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt c++11 c++14

SOURCES += \
    collection.cpp \
    database.cpp \
    marcxml.cpp \
    definitions.cpp \
    indicator_type.cpp

HEADERS += \
    collection.h \
    database.h \
    marcxml.h \
    definitions.h \
    indicator_type.h

INCLUDEPATH += $$PWD/../jlee
INCLUDEPATH += /usr/include/libxml2
