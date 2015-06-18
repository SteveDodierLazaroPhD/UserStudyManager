#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "study.h"
#include "webmanager.h"
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

void MainWindow::launchUrl(const QUrl &url) const
{
    launchProcess(url.toString().section("app://", 1));
}

void MainWindow::saveSettings()
{
    StudyUtils *inst  = StudyUtils::getUtils();
    QSettings &settings = inst->getUserSettings();

    settings.beginGroup("MainWindow");
    settings.setValue("maximised", isMaximized());
    if(!isMaximized())
    {
        /* Since we don't save size and pos on maximise, we allow
         * good default settings for when the user re-minimises
         * the window rather than making the minimised window the
         * same size and pos as the maximised one */
        settings.setValue("size", size());
        settings.setValue("pos", pos());
    }
    settings.endGroup();
    settings.sync();
}

void MainWindow::loadSettings()
{
    StudyUtils *inst  = StudyUtils::getUtils();
    QSettings &settings = inst->getUserSettings();

    settings.beginGroup("MainWindow");
    QSize prefSize = settings.value("size", QSize(0, 0)).toSize();
    if (prefSize != QSize(0, 0))
        resize(prefSize);
    QPoint prefPos= settings.value("pos", QPoint(0, 0)).toPoint();
    if (prefPos != QPoint(0, 0))
        move(prefPos);
    if (settings.value("maximised", false).toBool())
        setWindowState(windowState() | Qt::WindowMaximized);
    settings.endGroup();
}

void MainWindow::hideAll()
{
    ui->progressBar->hide();
    ui->progressServiceUI->hide();
    ui->uploadServiceUI->hide();
    ui->quickLinkUI->hide();
    nm->hide();
    ui->line->show();
}

void MainWindow::showQuickLinkUI()
{
    hideAll();
    ui->quickLinkUI->show();
}

void MainWindow::showWebUI()
{
    hideAll();
    ui->line->hide();
    nm->show();
}

void MainWindow::showProgressCalcUI()
{
    hideAll();
    ui->progressServiceUI->show();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ongoingUpload(false)
{
    StudyUtils *inst  = StudyUtils::getUtils();
    this->nm = new WebManager(this);

    ui->setupUi(this);
    ui->statusBar->hide();
    ui->centralWidget->layout()->addWidget(nm);
    this->showWebUI();
    this->loadSettings();

    UploadService *upload = inst->getUploadService();
    connect(upload, SIGNAL(uploadStarted()), this, SLOT(onUploadStart()));
    connect(upload, SIGNAL(uploadFinished()), this, SLOT(onUploadFinished()));

    connect(ui->actionActivityJournal, SIGNAL(triggered()), this, SLOT(launchActivityJournal()));
    connect(ui->actionActivityLogManager, SIGNAL(triggered()), this, SLOT(launchActivityLogManager()));
    connect(ui->actionInformationSheet, SIGNAL(triggered()), nm, SLOT(loadInfoPage()));
    connect(ui->actionStatus, SIGNAL(triggered()), nm, SLOT(loadShowStatusPage()));
    connect(ui->actionPrepareUpload, SIGNAL(triggered()), nm, SLOT(loadReportProgressPage()));
    connect(ui->actionContact, SIGNAL(triggered()), nm, SLOT(loadContactPage()));
    connect(ui->actionUploadData, SIGNAL(triggered()), nm, SLOT(loadUploadPage()));

    connect(nm, SIGNAL(loadStarted()), this, SLOT(onPageLoadStarted()));
    connect(nm, SIGNAL(loadProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(nm, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));
    connect(nm, SIGNAL(unsupportedStepQueried(Part, Step)), this, SLOT(onUnsupportedStepQueried(Part, Step)));
    connect(nm, SIGNAL(reportProgressRequested(const QString &)), this, SLOT(onProgressReportQueried(QString)));

    ProgressReportService *progress = inst->getProgressReportService();
    connect(progress, SIGNAL(startingReport(Part,Step)), this, SLOT(onReportStarting(Part,Step)));
    connect(progress, SIGNAL(targetForReport(qint64)), this, SLOT(onReportTargetElicited(qint64)));
    connect(progress, SIGNAL(stepReport(qint64,qint64)), this, SLOT(onReportStep(qint64,qint64)));
    connect(progress, SIGNAL(finishedProgressCalculation(Part, Step, qint64)), this, SLOT(onReportSuccess(Part, Step, qint64)));
    connect(progress, SIGNAL(invalidReportRequest(QString)), this, SLOT(onReportRequestFailure(QString)));

    connect(ui->progressGoToPackagingButton, SIGNAL(clicked(Part,Step)), progress, SLOT(processPackageArchiveRequest(Part,Step)));

    connect(ui->buttonUIButton, SIGNAL(clicked()), this, SLOT(onLoadWebsiteButtonClicked()));

    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionContact, SLOT(setEnabled(bool)));
    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionUploadData, SLOT(setEnabled(bool)));
    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionPrepareUpload, SLOT(setEnabled(bool)));
    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionStatus, SLOT(setEnabled(bool)));
    ui->actionContact->setEnabled(inst->isLoggedIn());
    ui->actionUploadData->setEnabled(inst->isLoggedIn());
    ui->actionPrepareUpload->setEnabled(inst->isLoggedIn());
    ui->actionStatus->setEnabled(inst->isLoggedIn());

    QDesktopServices::setUrlHandler(APPS_SCHEME, this, "launchUrl");

    //TODO finish fixing connects
    //TODO setSensitiveFalse, connect to loggedIn signal and loggedOut signal.

}

MainWindow::~MainWindow()
{
    saveSettings();
    delete nm;
    delete ui;
}


void MainWindow::onPageLoadStarted()
{
    this->showWebUI();
    ui->progressBar->reset();
    ui->progressBar->show();
}

void MainWindow::onProgressReportQueried(const QString &content)
{
    ProgressReportService *report = StudyUtils::getUtils()->getProgressReportService();
    report->processReportRequest(content);
    this->showProgressCalcUI();
}

void MainWindow::onReportStarting(const Part &part, const Step &step)//TODO , const int opCount)
{
    ui->progressDayBar->reset();
    ui->progressDayBar->setEnabled(true);
    ui->progressDayStatusLabel->setText("");
    ui->daysCollected->setText("<i>0...<i>");
    ui->minNeeded->setText(QString("%1 days").arg(StudyUtils::getUtils()->getMinQualifyingProgress(part, step)));

    ui->progressArchiveBar->reset();
    ui->progressArchiveBar->setEnabled(false);
    ui->progressArchiveStatusLabel->setText("");

    ui->progressGoToPackagingButton->setEnabled(false);
    ui->progressGoToUploadButton->setEnabled(false);
}

void MainWindow::onReportTargetElicited(const qint64 &targetDaysToCheck)
{
    ui->progressDayBar->setValue(0);
    ui->progressDayBar->setMaximum(targetDaysToCheck);
    ui->progressDayStatusLabel->setText(QString("<i>Verifying the past %1 days...<i>").arg(targetDaysToCheck));
}

void MainWindow::onReportStep(const qint64 &daysChecked, const qint64 &tmpResult)
{
    ui->progressDayBar->setValue(daysChecked);
    ui->daysCollected->setText(QString("<i>%1...</i>").arg(tmpResult));
}

void MainWindow::onReportSuccess(const Part &part, const Step &step, const qint64 &result)
{
    ui->progressGoToPackagingButton->setCurrentPart(part);
    ui->progressGoToPackagingButton->setCurrentStep(step);
    ui->progressGoToPackagingButton->setEnabled(true);
    ui->progressDayBar->setValue(ui->progressDayBar->maximum());
    ui->daysCollected->setText(QString("%1 days").arg(result));
    ui->progressDayStatusLabel->setText("<i>Done!");
}

void MainWindow::onReportRequestFailure(const QString &message)
{
    ui->progressDayBar->setEnabled(false);
    ui->progressArchiveBar->setEnabled(false);
    ui->progressGoToPackagingButton->setEnabled(false);
    ui->progressGoToUploadButton->setEnabled(false);

    ui->daysCollected->setText("<i>unknown...<i>");
    ui->minNeeded->setText("<i>unknown...<i>");

    ui->progressDayStatusLabel->setText(QString("<span style=\" font-weight:600; color:#ff2424;\">Error: %1</span>").arg(message));
    ui->progressArchiveStatusLabel->setText("");
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

void MainWindow::onUnsupportedStepQueried(Part /*part*/, Step step)
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
