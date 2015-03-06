#include "mainwindow.h"
#include "study.h"
#include "part.h"
#include "study.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <iostream>

using namespace std;

/**
 * @brief studySetup Use this function to configure your study
 */
static void studySetup()
{
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("study_" + QLocale::system().name());
    a.installTranslator(&myappTranslator);

    studySetup();

    MainWindow w;
    w.show();

    return a.exec();
}
