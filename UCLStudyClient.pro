#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T22:23:17
#
#-------------------------------------------------

QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UCLStudyClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    participant.cpp \
    part.cpp \
    step.cpp \
    study.cpp \
    uclwebmanager.cpp \
    ucluploadmanager.cpp

HEADERS  += mainwindow.h \
    study.h \
    participant.h \
    part.h \
    step.h \
    uclwebmanager.h \
    ucluploadmanager.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc
