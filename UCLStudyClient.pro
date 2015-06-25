#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T22:23:17
#
#-------------------------------------------------

QT       += core gui webkitwidgets dbus concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UCLStudyClient
TEMPLATE = app
CONFIG += c++11

LIBS += -lqzeitgeist5

SOURCES += main.cpp\
        mainwindow.cpp \
    participant.cpp \
    part.cpp \
    step.cpp \
    study.cpp \
    uploadservice.cpp \
    progressreportservice.cpp \
    progressbutton.cpp \
    tarball.cpp \
    requestservice.cpp \
    webviewservice.cpp

HEADERS  += mainwindow.h \
    study.h \
    participant.h \
    part.h \
    step.h \
    uploadservice.h \
    progressreportservice.h \
    progressbutton.h \
    tarball.h \
    requestservice.h \
    webviewservice.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc

DISTFILES += \
    data/conf.ini/Configuration
