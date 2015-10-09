/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#include "UI/mainwindow.h"
#include "study.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StudyUtils::init();

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("study_" + QLocale::system().name());
    a.installTranslator(&myappTranslator);

    // Make absolutely certain that we register at least the starting date for part 1
    StudyUtils *utils = StudyUtils::getUtils();
    utils->registerInstall(Part::fromId(1));

    MainWindow w;
    w.show();

    return a.exec();
}
