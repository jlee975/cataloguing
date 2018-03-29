#-------------------------------------------------
#
# Project created by QtCreator 2018-03-28T11:01:18
#
#-------------------------------------------------
include(../vars.pri)

TARGET = jlee
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt c++11 c++14

SOURCES += \
    fileutil/memorymappedfile.cpp

HEADERS += \
    tree/tree.h \
    fileutil/memorymappedfile.h
