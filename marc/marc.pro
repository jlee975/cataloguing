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
    marcxml.cpp

HEADERS += \
    collection.h \
    database.h \
    marcxml.h

INCLUDEPATH += $$PWD/../jlee
INCLUDEPATH += /usr/include/libxml2
