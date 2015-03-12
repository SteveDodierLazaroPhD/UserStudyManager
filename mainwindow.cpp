#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "study.h"
#include "uclwebmanager.h"
#include <iostream>
#include <QProcess>
#include <QTimer>
#include <QDesktopServices>
#include <QStandardPaths>


using namespace std;

void MainWindow::launchProcess(const QString &name) const
{
    QString path = QStandardPaths::findExecutable(name);

    QProcess process;
    bool started = process.startDetached(path);
    if (!started)
        qDebug() << "Could not start application \"" << qPrintable(path) << "\"." << endl;
}

void MainWindow::launchActivityJournal() const
{
    this->launchProcess("ucl-study-journal");
}

void MainWindow::launchActivityLogManager() const
{
    this->launchProcess("ucl-study-log-manager");
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->nm = new UCLWebManager(this);

    ui->setupUi(this);
    ui->progressBar->hide();
    ui->statusBar->hide();
    ui->centralWidget->layout()->addWidget(nm);

    connect(ui->actionActivityJournal, SIGNAL(triggered()), this, SLOT(launchActivityJournal()));
    connect(ui->actionActivityLogManager, SIGNAL(triggered()), this, SLOT(launchActivityLogManager()));
    connect(ui->actionInformationSheet, SIGNAL(triggered()), nm, SLOT(loadInfoPage()));
    connect(ui->actionContact, SIGNAL(triggered()), nm, SLOT(loadContactPage()));

    connect(nm, SIGNAL(loadStarted()), this, SLOT(onPageLoadStarted()));
    connect(nm, SIGNAL(loadProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(nm, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));

    // TODO re-add post patching ui->mainToolBar->hide();
    connect(nm, SIGNAL(loggedIn(bool)), ui->mainToolBar, SLOT(setVisible(bool)));

    //TODO finish fixing connects
    //TODO setSensitiveFalse, connect to loggedIn signal and loggedOut signal.

}

MainWindow::~MainWindow()
{
    delete nm;
    delete ui;
}


void MainWindow::onPageLoadStarted()
{
    ui->progressBar->reset();
    ui->progressBar->show();
}

void MainWindow::onPageLoaded(const bool success)
{
    if (!success)
    {
        ui->statusBar->show();
        ui->progressBar->setValue(0);
        ui->statusBar->showMessage("Failed to load page", 3000);
        QTimer::singleShot(1500, ui->statusBar, SLOT(hide()));
    }

    QTimer::singleShot(200, ui->progressBar, SLOT(hide()));
}
