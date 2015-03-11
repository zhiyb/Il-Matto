#-------------------------------------------------
#
# Project created by QtCreator 2015-03-03T13:27:20
#
#-------------------------------------------------

QT       += core gui
QT	+= serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Capture
TEMPLATE = app

QMAKE_CFLAGS	+= -DF_CPU=12000000
QMAKE_CXXFLAGS	+= -DF_CPU=12000000

SOURCES += main.cpp\
        mainwindow.cpp \
    connection.cpp

HEADERS  += mainwindow.h \
    connection.h
