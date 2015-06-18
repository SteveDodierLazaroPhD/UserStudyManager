#include "study.h"
#include <iostream>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSettings>
#include <QUrl>

using namespace std;

StudyUtils *StudyUtils::instance = NULL;

//TODO implement a map that links route names to their respective parts...

StudyUtils::StudyUtils() :
    participant(NULL),
    loggedIn(false),
    progressReport(new ProgressReportService()),
    upload(new UploadService()),
    globalSettings(QSettings::SystemScope, "UCL", STUDY_ID),
    userSettings(QSettings::UserScope, "UCL", STUDY_ID)
{
    initMaxPart();
    initPartStepOrder();

    participant = new Participant();
}

StudyUtils::~StudyUtils()
{
    userSettings.sync();

    if (participant)
        delete participant;

    delete progressReport;
    delete upload;
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
    //TODO review this completely in the form of groups
    //TODO inject descriptions via groups too!
    QJsonObject steps = globalSettings.value("steps", "{1: [\"waiting-enrollment\", \"consent\", \"install\", \"upload\", \"done\"]}").toJsonObject();

    QJsonObject::iterator it = steps.begin();
    while (it != steps.end())
    {
        bool ok = false;
        uint id = it.key().toUInt(&ok, 10);
        if (ok)
        {
            Part p(id);
            QJsonArray array = it.value().toArray();
            QList<Step> stepsForPart;
            QJsonArray::iterator ait = array.begin();
            while (ait != array.end())
            {
                stepsForPart.append(Step::fromName((*ait).toString()));
                ait++;
            }

            p.registerSteps(stepsForPart);
        }
        it++;
    }
}

void StudyUtils::registerInstall(const Part &part)
{
    QString start = QString("startPart%1Date").arg(part.getId());
    QString date = QDate::currentDate().toString(DATE_FORMAT);

    userSettings.setValue(start, date);
    userSettings.sync();
}

QDate StudyUtils::getInstallDate(const Part &part) const
{
    QString start = QString("startPart%1Date").arg(part.getId());
    return userSettings.value(start, QDate::currentDate()).toDate();
}

qint64 StudyUtils::getMinQualifyingProgress(const Part &part, const Step &step)
{
    globalSettings.beginGroup(QString("Part %1").arg(part.toString()));
    globalSettings.beginGroup(QString("Step %1").arg(step.toString()));

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
    globalSettings.beginGroup(QString("Part %1").arg(part.toString()));
    globalSettings.beginGroup(QString("Step %1").arg(step.toString()));

    bool ok = false;
    qint64 prog = globalSettings.value("currentProgress", 0).toLongLong(&ok);
    if(!ok)
        prog = 0;

    globalSettings.endGroup();
    globalSettings.endGroup();

    return prog;
}

void StudyUtils::saveCurrentProgress(const Part &part, const Step &step, const qint64 &loggedDays)
{
    globalSettings.beginGroup(QString("Part %1").arg(part.toString()));
    globalSettings.beginGroup(QString("Step %1").arg(step.toString()));

    //TODO SET UPSTREAM TOO !
    globalSettings.setValue("currentProgress", loggedDays);

    globalSettings.endGroup();
    globalSettings.endGroup();
}

void StudyUtils::saveUploadableArchive(const Part &part, const Step &step, const QString &filePath, const qint64 &fileSize)
{
    globalSettings.beginGroup(QString("Part %1").arg(part.toString()));
    globalSettings.beginGroup(QString("Step %1").arg(step.toString()));

    globalSettings.setValue("uplodableArchivePath", filePath);
    globalSettings.setValue("uplodableArchiveSize", fileSize);

    globalSettings.endGroup();
    globalSettings.endGroup();
}

void StudyUtils::loginFinalize(bool status)
{
    loggedIn = status;

    if (loggedIn)
    {
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

QString StudyUtils::getUrlRouteName(const QUrl &url)
{
    QString str(url.toString());
    str.remove(APP_BASE);

    /* Not from our actual website */
    if (str.startsWith("http") || str.startsWith("ftp"))
        return QString();

    /* Index page */
    if (str.isEmpty())
        return QString("next");

    QStringList    bits = str.split("/");

    /* Authenticated space */
    int partIndex = 0;

    if (Part::fromString(bits.at(partIndex)).isValid())
    {
        ++partIndex;
        if (bits.length() <= partIndex)
            return QString("status");
    }

    return bits.at(partIndex);
}
