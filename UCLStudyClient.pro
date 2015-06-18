#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T22:23:17
#
#-------------------------------------------------

QT       += core gui webkitwidgets dbus concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UCLStudyClient
TEMPLATE = app

LIBS += -lqzeitgeist5

SOURCES += main.cpp\
        mainwindow.cpp \
    participant.cpp \
    part.cpp \
    step.cpp \
    study.cpp \
    webmanager.cpp \
    uploadservice.cpp \
    progressreportservice.cpp \
    progressbutton.cpp

HEADERS  += mainwindow.h \
    study.h \
    participant.h \
    part.h \
    step.h \
    webmanager.h \
    uploadservice.h \
    progressreportservice.h \
    progressbutton.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc
