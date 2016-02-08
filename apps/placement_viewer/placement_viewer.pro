#-------------------------------------------------
#
# Project created by QtCreator 2016-02-05T16:45:18
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = placement_viewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qsfmlcanvas.cpp \
    mysfmlcanvas.cpp \
    open_circuit_dialog.cpp \
    application.cpp

HEADERS  += mainwindow.h \
    qsfmlcanvas.h \
    mysfmlcanvas.h \
    open_circuit_dialog.h \
    application.h
FORMS    += mainwindow.ui \
    open_circuit_dialog.ui

DISTFILES += \
    CMakeLists.txt

LIBS += -lsfml-graphics -lsfml-window -lsfml-system
