#-------------------------------------------------
#
# Project created by QtCreator 2016-04-21T20:28:08
#
#-------------------------------------------------

QT       += core gui
QT       +=serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = serial_v4
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialinfodialog.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    serialinfodialog.h \
    qcustomplot.h

FORMS    += mainwindow.ui \
    serialinfodialog.ui
