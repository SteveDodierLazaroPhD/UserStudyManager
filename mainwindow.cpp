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

void MainWindow::showQuickLinkUI()
{
    ui->progressBar->hide();
    ui->buttonUI->show();
    nm->hide();
}

void MainWindow::showWebUI()
{
    ui->progressBar->hide();
    ui->buttonUI->hide();
    nm->show();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ongoingUpload(false)
{
    StudyUtils *inst  = StudyUtils::getUtils();
    this->nm = new UCLWebManager(this);

    ui->setupUi(this);
    ui->statusBar->hide();
    ui->centralWidget->layout()->addWidget(nm);
    this->showWebUI();

    connect(nm, SIGNAL(uploadStarted()), this, SLOT(onUploadStart()));
    connect(nm, SIGNAL(uploadFinished()), this, SLOT(onUploadFinished()));

    connect(ui->actionActivityJournal, SIGNAL(triggered()), this, SLOT(launchActivityJournal()));
    connect(ui->actionActivityLogManager, SIGNAL(triggered()), this, SLOT(launchActivityLogManager()));
    connect(ui->actionInformationSheet, SIGNAL(triggered()), nm, SLOT(loadInfoPage()));
    connect(ui->actionContact, SIGNAL(triggered()), nm, SLOT(loadContactPage()));
    connect(ui->actionUploadData, SIGNAL(triggered()), nm, SLOT(loadUploadPage()));

    connect(nm, SIGNAL(loadStarted()), this, SLOT(onPageLoadStarted()));
    connect(nm, SIGNAL(loadProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(nm, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));
    connect(nm, SIGNAL(unsupportedStepQueried(Part, Step)), this, SLOT(onStepQueried(Part, Step)));
    connect(ui->buttonUIButton, SIGNAL(clicked()), this, SLOT(onLoadWebsiteButtonClicked()));

    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionContact, SLOT(setEnabled(bool)));
    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionUploadData, SLOT(setEnabled(bool)));
    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionCurrentProgress, SLOT(setEnabled(bool)));
    ui->actionContact->setEnabled(inst->isLoggedIn());
    ui->actionUploadData->setEnabled(inst->isLoggedIn());
    ui->actionCurrentProgress->setEnabled(inst->isLoggedIn());

    QDesktopServices::setUrlHandler(APPS_SCHEME, this, "launchUrl");

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
    this->showWebUI();
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

void MainWindow::onStepQueried(Part /*part*/, Step step)
{
    this->showQuickLinkUI();
    ui->buttonUILabel->setText(step.getMustDoLabel());
}

void MainWindow::onUploadStart()
{
    ongoingUpload = true;
    setToolbarEnabled();
}

void MainWindow::onUploadFinished()
{
    ongoingUpload = false;
    setToolbarEnabled();
}

void MainWindow::setToolbarEnabled()
{
    ui->mainToolBar->setEnabled(!ongoingUpload);
}

void MainWindow::onLoadWebsiteButtonClicked()
{
    nm->openDesktopUrl(QUrl(WEB_BASE));
}
