/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#include "study.h"
#include <iostream>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSettings>
#include <QUrl>
#include <QMutexLocker>

using namespace std;

StudyUtils *StudyUtils::instance = NULL;

//TODO implement a map that links route names to their respective parts...

StudyUtils::StudyUtils() :
    participant(NULL),
    loggedIn(false),
    jarjarbinks(new QNetworkCookieJar()),
    webview(new WebViewService()),
    progressReport(new ProgressReportService()),
    upload(new UploadService()),
    request(new RequestService(webview)),
    globalSettings(QSettings::SystemScope, "UCL", STUDY_ID),
    userSettings(QSettings::UserScope, "UCL", STUDY_ID),
    settingsMutex()
{
    initMaxPart();
    initPartStepOrder();

    participant = new Participant();

    connect(progressReport, SIGNAL(finishedProgressCalculation(Part,Step,qint64)), this, SLOT(saveCurrentProgress(Part,Step,qint64)));
    connect(progressReport, SIGNAL(finishedProgressCalculation(Part,Step,qint64)), request, SLOT(sendProgressReportRequest(Part,Step,qint64)));
    connect(progressReport, SIGNAL(finishedPackaging(Part,Step,QString,qint64,QString)), this, SLOT(saveUploadableArchive(Part,Step,QString,qint64,QString)));

    connect(webview, SIGNAL(reportProgressRequested(const QString &)), progressReport, SLOT(processReportRequest(QString)));
    connect(webview, SIGNAL(uploadJobActionRequested(const QString &)), request, SLOT(dispatchUploadMessage(QString)));

    webview->setCookieJar(jarjarbinks);
    jarjarbinks->setParent(nullptr);
    request->getManager().setCookieJar(jarjarbinks);
    jarjarbinks->setParent(nullptr);
}

StudyUtils::~StudyUtils()
{
    userSettings.sync();

    if (participant)
        delete participant;

    delete request;
    delete upload;
    delete progressReport;
    delete webview;
    delete jarjarbinks;
}

void StudyUtils::initMaxPart()
{
    bool ok = false;
    int partCount = globalSettings.value("partCount", 1).toUInt(&ok);
    if(!ok)
        partCount = 0;

    Part::setMaxPart(partCount);
}

void StudyUtils::initPartStepOrder()
{
    QStringList def;
    def.append("consent");
    def.append("install");
    def.append("running");
    def.append("done");

    bool ok = false;
    int partCount = globalSettings.value("partCount", 1).toUInt(&ok);
    if(!ok)
        partCount = 0;

    for(int i=1; i<=partCount; ++i)
    {
        globalSettings.beginGroup(QString("Part%1").arg(i));
        QStringList steps = globalSettings.value("steps", def).toStringList();
        globalSettings.endGroup();

        QList<Step> stepsForPart;
        QStringList::const_iterator it = steps.begin();
        while (it != steps.end())
        {
            Step s = Step::fromName(*it);
            //TODO check the step's own settings group for a custom "mustDo" label
            stepsForPart.append(s);
            it++;
        }

        Part p(i);
        p.registerSteps(stepsForPart);
        Part::registerPart(p);
    }
}

void StudyUtils::registerInstall(const Part &part)
{
    QMutexLocker locker(&settingsMutex);
    userSettings.beginGroup(QString("Part%1").arg(part.toString()));
    userSettings.setValue("startDate", QDate::currentDate().toString(DATE_FORMAT));
    userSettings.endGroup();
    userSettings.sync();
}

QDate StudyUtils::getInstallDate(const Part &part)
{
    QMutexLocker locker(&settingsMutex);
    userSettings.beginGroup(QString("Part%1").arg(part.toString()));
    QDate date = userSettings.value("startDate", QDate::currentDate()).toDate();
    userSettings.endGroup();
    return date;
}

qint64 StudyUtils::getMinQualifyingProgress(const Part &part, const Step &step)
{
    QMutexLocker locker(&settingsMutex);
    globalSettings.beginGroup(QString("Part%1").arg(part.toString()));
    globalSettings.beginGroup(QString("%1").arg(step.toString()));

    bool ok = false;
    qint64 min = globalSettings.value("minQualifyingProgress", 0).toLongLong(&ok);
    if(!ok)
        min = 0;

    globalSettings.endGroup();
    globalSettings.endGroup();

    return min;
}

qint64 StudyUtils::getCurrentProgress(const Part &part, const Step &step)
{
    QMutexLocker locker(&settingsMutex);
    userSettings.beginGroup(QString("Part%1").arg(part.toString()));
    userSettings.beginGroup(QString("%1").arg(step.toString()));

    bool ok = false;
    qint64 prog = userSettings.value("currentProgress", 0).toLongLong(&ok);
    if(!ok)
        prog = 0;

    userSettings.endGroup();
    userSettings.endGroup();

    return prog;
}

void StudyUtils::saveCurrentProgress(const Part &part, const Step &step, const qint64 &loggedDays)
{
    QMutexLocker locker(&settingsMutex);
    userSettings.beginGroup(QString("Part%1").arg(part.toString()));
    userSettings.beginGroup(QString("%1").arg(step.toString()));
    userSettings.setValue("currentProgress", loggedDays);
    userSettings.endGroup();
    userSettings.endGroup();
}

qint64 StudyUtils::getUploadableArchiveSize(const Part &part, const Step &step)
{
    QMutexLocker locker(&settingsMutex);
    userSettings.beginGroup(QString("Part%1").arg(part.toString()));
    userSettings.beginGroup(QString("%1").arg(step.toString()));

    bool ok = false;
    qint64 size = userSettings.value("archiveSize", 0).toLongLong(&ok);
    if(!ok)
        size = 0;

    userSettings.endGroup();
    userSettings.endGroup();

    return size;
}

QString StudyUtils::getUploadableArchivePath(const Part &part, const Step &step)
{
    QMutexLocker locker(&settingsMutex);
    userSettings.beginGroup(QString("Part%1").arg(part.toString()));
    userSettings.beginGroup(QString("%1").arg(step.toString()));
    QString name = userSettings.value("archivePath", 0).toString();
    userSettings.endGroup();
    userSettings.endGroup();

    return name;
}

QString StudyUtils::getUploadableArchiveChecksum(const Part &part, const Step &step)
{
    QMutexLocker locker(&settingsMutex);
    userSettings.beginGroup(QString("Part%1").arg(part.toString()));
    userSettings.beginGroup(QString("%1").arg(step.toString()));
    QString name = userSettings.value("archiveChecksum").toString();
    userSettings.endGroup();
    userSettings.endGroup();

    return name;
}

void StudyUtils::saveUploadableArchive(const Part &part, const Step &step, const QString &filePath, const qint64 &fileSize, const QString &checksum)
{
    QMutexLocker locker(&settingsMutex);
    userSettings.beginGroup(QString("Part%1").arg(part.toString()));
    userSettings.beginGroup(QString("%1").arg(step.toString()));
    userSettings.setValue("archivePath", filePath);
    userSettings.setValue("archiveSize", fileSize);
    userSettings.setValue("archiveChecksum", checksum);
    userSettings.sync();
    userSettings.endGroup();
    userSettings.endGroup();
}

void StudyUtils::loginFinalize(bool status)
{
    loggedIn = status;

    if (loggedIn)
    {
        QMutexLocker locker(&settingsMutex);
        cout << "Logged in as " << qPrintable(participant->getUsername()) << " (email " << qPrintable(participant->getEmail()) << ")." << endl;
        userSettings.setValue("lastEmail", participant->getEmail());
    }

    emit onLoginStatusChanged(status);
}

StudyUtils * StudyUtils::getUtils()
{
    if (instance == NULL)
    {
        instance = new StudyUtils();
    }

    return instance;
}

void StudyUtils::clearUtils()
{
    if (instance)
    {
        delete instance;
        instance = NULL;
    }
}

bool StudyUtils::isParticipantBeyond(const Part &p, const Step &s)
{
    if (participant->getPart() != p)
        return participant->getPart() > p;
    else
        return p.isBeyond(participant->getStep(), s);
}
