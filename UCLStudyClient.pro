#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T22:23:17
#
#-------------------------------------------------

QT       += core gui webkitwidgets dbus concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ucl-study-manager
TEMPLATE = app
CONFIG += c++11

LIBS += -lqzeitgeist5 -ltar

INCLUDEPATH += ./Widgets/

SOURCES += main.cpp\
    study.cpp \
    Services/webviewservice.cpp \
    UI/mainwindow.cpp \
    Model/part.cpp \
    Model/participant.cpp \
    Model/step.cpp \
    Model/uploadjob.cpp \
    Services/requestservice.cpp \
    Services/progressreportservice.cpp \
    Services/uploadservice.cpp \
    Lib/tarball.cpp \
    Widgets/progressbutton.cpp \
    Widgets/progressbarbutton.cpp

HEADERS  += \
    study.h \
    Services/webviewservice.h \
    Model/step.h \
    Model/part.h \
    Model/participant.h \
    Model/uploadjob.h \
    Lib/tarball.h \
    Services/requestservice.h \
    Services/progressreportservice.h \
    Services/uploadservice.h \
    Widgets/progressbutton.h \
    UI/mainwindow.h \
    Widgets/progressbarbutton.h

FORMS    += mainwindow.ui \
    uploadingtoolbarform.ui

RESOURCES += \
    icons.qrc

DISTFILES += \
    data/multitasking.conf
