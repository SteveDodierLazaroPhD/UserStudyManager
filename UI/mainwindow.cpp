/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#include "ui_mainwindow.h"
#include "ui_uploadingtoolbarform.h"
#include "UI/mainwindow.h"
#include "study.h"
#include "Services/progressreportservice.h"
#include "Services/requestservice.h"
#include "Services/uploadservice.h"
#include "Services/webviewservice.h"
#include <iostream>
#include <QProcess>
#include <QTimer>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QToolButton>


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

void MainWindow::setUploadOngoing(const bool &currentUploadStatus, const Part &part, const Step &step)
{
    if(currentUploadStatus && !ongoingUpload)
    {
        ongoingUpload = true;
        uploadPart = part;
        uploadStep = step;
        form->pushButton->show();
        form->progressBar->show();
    }
    else if (!currentUploadStatus && ongoingUpload)
    {
        ongoingUpload = false;
        uploadPart = Part::INVALID;
        uploadStep = Step::INVALID;
        form->pushButton->hide();
        form->progressBar->hide();
    }
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

void MainWindow::showUploadUI()
{
    hideAll();
    ui->uploadServiceUI->show();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    form(new Ui::UploadingToolbarForm),
    ongoingUpload(false)
{
    StudyUtils *inst  = StudyUtils::getUtils();
    this->nm = inst->getWebViewService();

    ui->setupUi(this);
    ui->statusBar->hide();
    ui->centralWidget->layout()->addWidget(nm);

    /* Toolbar buttons */
    connect(ui->actionActivityJournal, SIGNAL(triggered()), this, SLOT(launchActivityJournal()));
    connect(ui->actionActivityLogManager, SIGNAL(triggered()), this, SLOT(launchActivityLogManager()));
    connect(ui->actionInformationSheet, SIGNAL(triggered()), nm, SLOT(loadInfoPage()));
    connect(ui->actionStatus, SIGNAL(triggered()), nm, SLOT(loadShowStatusPage()));
    connect(ui->actionPrepareUpload, SIGNAL(triggered()), nm, SLOT(loadReportProgressPage()));
    connect(ui->actionContact, SIGNAL(triggered()), nm, SLOT(loadContactPage()));
    connect(ui->actionUploadData, SIGNAL(triggered()), nm, SLOT(loadUploadPage()));

    /* Web manager events */
    RequestService *request = inst->getRequestService();
    connect(nm, SIGNAL(loadStarted()), this, SLOT(onPageLoadStarted()));
    connect(nm, SIGNAL(loadProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(nm, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));
    connect(nm, SIGNAL(unsupportedStepQueried(Part, Step)), this, SLOT(onUnsupportedStepQueried(Part, Step)));
    connect(nm, SIGNAL(reportProgressRequested(const QString &)), this, SLOT(onProgressReportQueried(QString)));
    connect(nm, SIGNAL(uploadJobActionRequested(const QString &)), this, SLOT(onUploadJobActionRequested(QString)));

    /* Progress report service - progress calculation */
    ProgressReportService *progress = inst->getProgressReportService();
    connect(progress, SIGNAL(startingReport(Part,Step)), this, SLOT(onReportStarting(Part,Step)));
    connect(progress, SIGNAL(targetForReport(qint64)), this, SLOT(onReportTargetElicited(qint64)));
    connect(progress, SIGNAL(stepReport(qint64,qint64)), this, SLOT(onReportStep(qint64,qint64)));
    connect(progress, SIGNAL(finishedProgressCalculation(Part, Step, qint64)), this, SLOT(onReportSuccess(Part, Step, qint64)));
    connect(progress, SIGNAL(invalidReportRequest(QString)), this, SLOT(onReportRequestFailure(QString)));
    connect(ui->progressGoToPackagingButton, SIGNAL(clicked(Part,Step)), progress, SLOT(processPackageArchiveRequest(Part,Step)));

    /* Progress report service - archive packaging */
    connect(progress, SIGNAL(startingPackaging(Part,Step)), this, SLOT(onPackagingStarting(Part,Step)));
    connect(progress, SIGNAL(targetForPackaging(qint64,QString)), this, SLOT(onPackagingTargetElicited(qint64,QString)));
    connect(progress, SIGNAL(stepPackaging(qint64,QString,qint64)), this, SLOT(onPackagingStep(qint64,QString,qint64)));
    connect(progress, SIGNAL(finishedPackaging(Part,Step,QString,qint64,QString)), this, SLOT(onPackagingSuccess(Part,Step,QString,qint64,QString)));
    connect(progress, SIGNAL(invalidPackagingRequest(QString)), this, SLOT(onPackagingFailure(QString)));
    connect(ui->progressGoToUploadButton, SIGNAL(clicked(Part,Step)), nm, SLOT(loadUploadPage(Part,Step)));

    connect(ui->buttonUIButton, SIGNAL(clicked()), this, SLOT(onLoadWebsiteButtonClicked()));

    /* Job erasing TODO */
    ui->progressWarningEraseJob->setVisible(false);
    //TODO    connect uploadStarted/Finished to a userSettings var indicating upload status and existence of job
    //TODO    initialise based on whether userSettings indicate existence of job
    //TODO    ensure, when creating an archive, that the requestService cancels the existing job towards the server


    /* Upload progress bar in the toolbar */
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolBar->addWidget(spacer);

    QWidget *toolbarUploadButtonPlacerholder = new QWidget(ui->mainToolBar);
    form->setupUi(toolbarUploadButtonPlacerholder);
    ui->mainToolBar->addWidget(toolbarUploadButtonPlacerholder);
    form->pushButton->hide();
    form->progressBar->hide();
    connect(request, SIGNAL(uploadStarted(Part,Step)), this, SLOT(onUploadStarted(Part,Step)));
    connect(request, SIGNAL(uploadParametersSet(Part,Step,qint64)), this, SLOT(onUploadTargetSet(Part,Step,qint64)));
    connect(request, SIGNAL(uploadStep(Part,Step,qint64)), this, SLOT(onUploadStep(Part,Step,qint64)));
    connect(request, SIGNAL(uploadSucceeded(Part,Step)), this, SLOT(onUploadSucceeded(Part,Step)));
    connect(request, SIGNAL(uploadFailed(Part,Step,QString)), this, SLOT(onUploadFailed(Part,Step,QString)));

    connect(form->pushButton, SIGNAL(clicked(bool)), this, SLOT(onShowUploadButtonClicked()));

    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionContact, SLOT(setEnabled(bool)));
    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionUploadData, SLOT(setEnabled(bool)));
    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionPrepareUpload, SLOT(setEnabled(bool)));
    connect(inst, SIGNAL(onLoginStatusChanged(bool)), ui->actionStatus, SLOT(setEnabled(bool)));

    this->showWebUI();
    this->loadSettings();

    ui->actionContact->setEnabled(inst->isLoggedIn());
    ui->actionUploadData->setEnabled(inst->isLoggedIn());
    ui->actionPrepareUpload->setEnabled(inst->isLoggedIn());
    ui->actionStatus->setEnabled(inst->isLoggedIn());

    QDesktopServices::setUrlHandler(APPS_SCHEME, this, "launchUrl");
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

void MainWindow::onProgressReportQueried(const QString &)
{
    this->showProgressCalcUI();
}

void MainWindow::onUploadJobActionRequested(const QString &)
{
    this->showUploadUI();
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
    ui->daysCollected->setText(QString("%1 days").arg(result));

    if (result >= StudyUtils::getUtils()->getMinQualifyingProgress(part, step))
    {
        ui->progressGoToPackagingButton->setEnabled(true);
        ui->progressDayBar->setValue(ui->progressDayBar->maximum());
        ui->progressDayStatusLabel->setText("<i>Done!</i>");
    }
    else
    {
        ui->progressGoToPackagingButton->setEnabled(false);
        ui->progressDayBar->setValue(ui->progressDayBar->minimum());
        ui->progressDayStatusLabel->setText("<i>You must collect more data to proceed</i>");
    }
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

void MainWindow::onPackagingStarting(const Part &, const Step &)
{
    ui->progressArchiveBar->reset();
    ui->progressArchiveBar->setEnabled(true);
    ui->progressArchiveStatusLabel->setText("");
    ui->progressArchiveFileCount->setText("<i>calculating...<i>");
    ui->progressArchiveSize->setText("<i>0...<i>");

    ui->progressGoToPackagingButton->setEnabled(false);
    ui->progressGoToUploadButton->setEnabled(false);
}

void MainWindow::onPackagingTargetElicited(const qint64 &target, const QString &nextFile)
{
    ui->progressArchiveBar->setValue(0);
    ui->progressArchiveBar->setMaximum(target);
    ui->progressArchiveFileCount->setText(QString("%1").arg(target));
    if (nextFile.isEmpty())
        ui->progressArchiveStatusLabel->setText(QString("<i>No files to archive...<i>"));
    else
        ui->progressArchiveStatusLabel->setText(QString("<i>Archiving %1...<i>").arg(nextFile));
}

static QString sizeToHumanReadable(const qint64 &fileSize)
{
    float size = fileSize;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(size >= 1024 && i.hasNext())
     {
        unit = i.next();
        size /= 1024;
    }

    return QString("%1 %2").arg(QString().setNum(size,'f',2)).arg(unit);
}

void MainWindow::onPackagingStep(const qint64 &filesChecked, const QString &nextFile, const qint64 &tmpFileSize)
{
    ui->progressArchiveBar->setValue(filesChecked);
    ui->progressArchiveSize->setText(QString("<i>%1...</i>").arg(sizeToHumanReadable(tmpFileSize)));
    if (nextFile.isEmpty())
        ui->progressArchiveStatusLabel->setText(QString("<i>Archived all files<i>"));
    else
        ui->progressArchiveStatusLabel->setText(QString("<i>Archiving %1...<i>").arg(nextFile));
}

void MainWindow::onPackagingSuccess(const Part &part, const Step &step, const QString &filename, const qint64 &fileSize, const QString &/*checksum*/)
{
    ui->progressGoToUploadButton->setCurrentPart(part);
    ui->progressGoToUploadButton->setCurrentStep(step);
    ui->progressGoToUploadButton->setEnabled(true);
    ui->progressArchiveBar->setValue(ui->progressArchiveBar->maximum());
    ui->progressArchiveSize->setText(QString("<i>%1...</i>").arg(sizeToHumanReadable(fileSize)));
    ui->progressDayStatusLabel->setText(QString("<i>Archive %1 created successfully!</i>").arg(filename));
}

void MainWindow::onPackagingFailure(const QString &message)
{
    ui->progressArchiveBar->setEnabled(false);
    ui->progressGoToPackagingButton->setEnabled(true);
    ui->progressGoToUploadButton->setEnabled(false);

    ui->progressArchiveStatusLabel->setText(QString("<span style=\" font-weight:600; color:#ff2424;\">Error: %1</span>").arg(message));
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

void MainWindow::onUploadStarted(const Part &part, const Step &step)
{
    setUploadOngoing(true, part, step);
    form->progressBar->reset();
    ui->uploadBar->reset();
    ui->uploadStatusLabel->setText("<i>currently sending your data...</i>");
    showUploadUI();
}

void MainWindow::onUploadTargetSet(const Part &part, const Step &step, const qint64 &expectedSize)
{
    setUploadOngoing(true, part, step);
    if (uploadPart == part && uploadStep == step)
    {
        form->progressBar->setMaximum(expectedSize);
        ui->uploadBar->setMaximum(expectedSize);
    }
}

void MainWindow::onUploadStep(const Part &part, const Step &step, const qint64 &sentSize)
{
    setUploadOngoing(true, part, step);
    if (uploadPart == part && uploadStep == step)
    {
        form->progressBar->setValue(sentSize);
        ui->uploadBar->setValue(sentSize);
    }
}

void MainWindow::onUploadSucceeded(const Part &part, const Step &step)
{
    if (uploadPart == part && uploadStep == step)
    {
        setUploadOngoing(false, part, step);
        ui->uploadStatusLabel->setText("<i>Done!</i>");
        ui->uploadBar->setValue(ui->uploadBar->maximum());
        form->progressBar->setValue(form->progressBar->maximum());
    }
}

void MainWindow::onUploadFailed(const Part &part, const Step &step, const QString &errMsg)
{
    if (uploadPart == part && uploadStep == step)
    {
        setUploadOngoing(false, part, step);
        ui->uploadStatusLabel->setText(QString("<span style=\" font-weight:600; color:#ff2424;\">Upload interrupted: %1</span>").arg(errMsg));
    }
}

void MainWindow::onShowUploadButtonClicked()
{
    showUploadUI();
}

void MainWindow::onLoadWebsiteButtonClicked()
{
    nm->openDesktopUrl(QUrl(WEB_BASE));
}
