#-------------------------------------------------
#
# Project created by QtCreator 2012-08-24T15:19:59
#
#-------------------------------------------------

QT       += core gui sql

TARGET = PermRequest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    common.cpp \
    dlgconnection.cpp \
    dlgeditpeoples.cpp

HEADERS  += mainwindow.h \
    common.h \
    dlgconnection.h \
    dlgeditpeoples.h

FORMS    += mainwindow.ui \
    dlgconnection.ui \
    dlgeditpeoples.ui

RESOURCES += \
    res/main.qrc
