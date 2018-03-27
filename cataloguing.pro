#-------------------------------------------------
#
# Project created by QtCreator 2018-03-22T21:28:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cataloguing
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    marc/collection.cpp \
    marc/database.cpp \
    collectionmodel.cpp \
    recordmodel.cpp

HEADERS += \
        mainwindow.h \
    marc/collection.h \
    marc/database.h \
    collectionmodel.h \
    recordmodel.h \
    tree.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += /usr/include/libxml2

unix|win32: LIBS += -lxml2

QMAKE_CXXFLAGS = -std=c++17 -pipe
QMAKE_CXXFLAGS_DEBUG = -O0 -ggdb3
QMAKE_CXXFLAGS_RELEASE = -O2
QMAKE_CXXFLAGS_WARN_OFF = warnoff

QMAKE_CXXFLAGS_WARN_ON = -Wall -Wsign-compare -Wpointer-arith -Winit-self \
    -Wcast-qual -Wredundant-decls -Wcast-align -Wwrite-strings \
    -Woverloaded-virtual -Wformat -Wno-unknown-pragmas -Wnon-virtual-dtor \
    -Wshadow
