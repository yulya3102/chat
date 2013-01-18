#-------------------------------------------------
#
# Project created by QtCreator 2012-12-06T23:32:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chat
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sender.cpp \
    receiver.cpp \
    cmd.cpp \
    chat.cpp \
    undeliveredmessage.cpp \
    nickname.cpp

HEADERS  += mainwindow.h \
    sender.h \
    receiver.h \
    cmd.h \
    chat.h \
    undeliveredmessage.h \
    nickname.h

FORMS    += mainwindow.ui
