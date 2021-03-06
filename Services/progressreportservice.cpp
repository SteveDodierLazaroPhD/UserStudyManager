/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#include "study.h"
#include <libtar.h>
#include <fcntl.h>
#include "Services/progressreportservice.h"

#include <QtConcurrent>
#include <QStandardPaths>
#include <QZeitgeist5/QZeitgeist>
#include <QZeitgeist5/Log>
#include <QtDBus/QDBusPendingReply>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <fstream>

using namespace std;

ProgressReportService::ProgressReportService()
{
    cout << "Progress report service created" << endl;
}

ProgressReportService::~ProgressReportService()
{
}

bool ProgressReportService::processReportRequest(const QString &request)
{
    QJsonDocument jResponse = QJsonDocument::fromJson(request.toUtf8());
    QJsonObject jObj = jResponse.object();

    if (jObj["ReportProgress"].toString() == "ReadyForContent")
    {
        QJsonObject participant = jObj["Participant"].toObject();
        QJsonObject status = participant["Status"].toObject();
        Part part = Part::fromId(status["Part"].toInt());
        Step step = Step::fromName(status["Step"].toString());

        // We only have one type of upload, so checking that the user is at least beyond RUNNING is enough.
        if (part.isValid() && step.isValid() && part.hasReached(step, Step::RUNNING))
        {
            QtConcurrent::run(this, &ProgressReportService::calculateZeitgeistDayCount, part, Step::RUNNING);
            return true;
        }
        else
        {
            QString msg;
            if(!part.isValid())
                msg = "study part '" + part.toString() + "' is invalid.";
            else if(!step.isValid())
                msg = "study step '" + step.toString() + "' is invalid.";
            else
                msg = "study step '" + step.toString() + "' does not require any upload.";

            emit invalidReportRequest(msg);
            return false;
        }
    }
    else
    {
        QString msg("could not parse a ReportProgress message from the server ('" + jObj["ReportProgress"].toString() + "').");
        emit invalidReportRequest(msg);
        return false;
    }
}

qint64 ProgressReportService::calculateZeitgeistDayCount(const Part &part, const Step &step)
{
    emit startingReport(part, step);
    StudyUtils *utils = StudyUtils::getUtils();
    QDate start = utils->getInstallDate(part);
    QDate end = QDate::currentDate();

    QZeitgeist::Log log;
    qint64 loggedDays = 0;
    qint64 deltaToNow = start.daysTo(end) + 1;
    emit targetForReport(deltaToNow);

    //qDebug() << "Was installed on " << qPrintable(start.toString("yyyy-MM-dd")) << " aka " << start.toJulianDay() << endl;
    //qDebug() << "Will check for data until " << qPrintable(end.toString("yyyy-MM-dd")) << endl;
    //qDebug() << "Total number of days is " << deltaToNow << endl;
    qint64 startUnix = (start.toJulianDay() - 2440587.5) * DAY_IN_MSEC;
    for (qint64 i = 0; i < deltaToNow; i++, startUnix += DAY_IN_MSEC)
    {
        QZeitgeist::DataModel::TimeRange timeRange(startUnix, startUnix + DAY_IN_MSEC);
        QZeitgeist::DataModel::EventList eventList;
        QZeitgeist::DataModel::Event event;
        QZeitgeist::DataModel::Subject subject;
        event.addSubject(subject);
        eventList.append(event);

        QDBusPendingReply<QZeitgeist::DataModel::EventList> reply = log.findEvents(timeRange,
                                                                                   eventList,
                                                                                   QZeitgeist::Log::Available,
                                                                                   1,
                                                                                   QZeitgeist::Log::MostRecentEvents);

        reply.waitForFinished();
        if(reply.isFinished() && !reply.isError())
        {
            QZeitgeist::DataModel::EventList list = reply.value();
            if (list.count())
                loggedDays++;

            // qint64 jd = ((double) startUnix / DAY_IN_MSEC) + 2440587.5;
            // QDate surveyedDay = QDate::fromJulianDay(jd);
            // qDebug() << "Found " << list.count() << " events that day (" << qPrintable(surveyedDay.toString("yyyy-MM-dd")) << ")." << endl;
        }
        emit stepReport(i+1, loggedDays);
    }

    emit finishedProgressCalculation(part, step, loggedDays);
    return loggedDays;
}

void ProgressReportService::getZeitgeistAndPreloadLogger(const Part &part, const Step &step)
{
    /* List files to package */
    QString dirPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "zeitgeist", QStandardPaths::LocateDirectory);
    if(dirPath == nullptr || dirPath.isEmpty())
    {
        QString msg = QString("could not locate files to package for part '%1' and step '%2'").arg(part.toString()).arg(step.toString());
        emit invalidPackagingRequest(msg);
        return;
    }

    QDir dir(dirPath);
    QStringList nameFilter("*.log.gz");
    nameFilter.append("*deletions.log");
    nameFilter.append("activity.sqlite");
    QStringList targets = dir.entryList(nameFilter);
    targets.sort(Qt::CaseSensitive);

    /* Filter out files that are too old or too young */
    QStringList::iterator it = targets.begin();
    QRegExp dateExp("[0-9]{4}-[0-9]{2}-[0-9]{2}");

    StudyUtils *utils = StudyUtils::getUtils();
    QDate start = utils->getInstallDate(part);
    QDate end = QDate::currentDate();

    while (it!= targets.end())
    {
        QString log = *it;
        QString dateStr = log;
        dateStr.truncate(10);
        if (dateExp.exactMatch(dateStr))
        {
            QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
            if (date < start || date > end)
                targets.erase(it);
        }
        it++;
    }


    int targetCount = targets.count();
    emit targetForPackaging(targetCount, targets.value(0));

    /* Build archive */
    qint64 fileSize = 0;
    QString archivePath = dirPath + "/uploadTarget-p" + part.toString() + "-" + step.toString() + ".tar.gz";

    TAR *archive;
    tar_open(&archive, qPrintable(archivePath), NULL, O_WRONLY | O_CREAT, 0644, TAR_GNU);

    QFileInfo archiveInfo(archivePath);
    archiveInfo.setCaching(false);
    for (int k=0; k<targetCount; ++k)
    {
        QString targetName = targets.at(k);
        QString fullPath = QString("%1/%2").arg(dirPath).arg(targetName);
        std::cout << "Adding file '" << qPrintable(fullPath) << "' to the archive" << std::endl;

        tar_append_file(archive, qPrintable(fullPath), qPrintable(targetName));
        fileSize = archiveInfo.size();

        emit stepPackaging(k+1, targets.value(k+1), fileSize);
    }

    tar_append_eof(archive);
    tar_close(archive);
    std::cout << "Archive prepared successfully" << std::endl;
    fileSize = archiveInfo.size();

    QString md5sum;
    QFile f(archivePath);
    if (f.open(QFile::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f))
        {
            md5sum = QString(hash.result().toHex());
        }
    }

    emit finishedPackaging(part, step, archivePath, fileSize, md5sum);
}


void ProgressReportService::getZeitgeistAndFirejail(const Part &part, const Step &step)
{
    //TODO connect upload service to this signal and discard any current progress
    emit startingPackaging(part, step);

    /* Zeitgeist is and remains the main directory we use to store our data */
    QString dirPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "zeitgeist", QStandardPaths::LocateDirectory);
    if(dirPath == nullptr || dirPath.isEmpty())
    {
        QString msg = QString("could not locate files to package for part '%1' and step '%2'").arg(part.toString()).arg(step.toString());
        emit invalidPackagingRequest(msg);
        return;
    }

    /* List files to package from the firejail side */
    QStringList targets;
    QString dirPathFj = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "firejail", QStandardPaths::LocateDirectory);
    if(dirPathFj != nullptr && !dirPathFj.isEmpty())
    {
        QStringList nameFilter("*.log");
        QDir dir(dirPathFj);
        targets = dir.entryList(nameFilter);
        targets.sort(Qt::CaseSensitive);
    }

    /* Filter out files that are too old or too young */
    QStringList::iterator it = targets.begin();
    QRegExp dateExp("[0-9]{4}-[0-9]{2}-[0-9]{2}");

    StudyUtils *utils = StudyUtils::getUtils();
    QDate start = utils->getInstallDate(part);
    QDate end = QDate::currentDate();

    while (it!= targets.end())
    {
        QString log = *it;
        QString dateStr = log;
        dateStr.truncate(10);
        if (dateExp.exactMatch(dateStr))
        {
            QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
            if (date < start || date > end)
                targets.erase(it);
        }
        it++;
    }

    int targetCount = targets.count();
    emit targetForPackaging(targetCount, targets.value(0));

    /* Build archive */
    qint64 fileSize = 0;
    QString archivePath = dirPath + "/uploadTarget-p" + part.toString() + "-" + step.toString() + ".tar.gz";

    TAR *archive;
    tar_open(&archive, qPrintable(archivePath), NULL, O_WRONLY | O_CREAT, 0644, TAR_GNU);

    QFileInfo archiveInfo(archivePath);
    archiveInfo.setCaching(false);
    tar_append_file(archive, qPrintable(QString("%1/%2").arg(dirPath).arg("activity.sqlite")), "activity.sqlite");
    for (int k=0; k<targetCount; ++k)
    {
        QString targetName = targets.at(k);
        QString fullPath = QString("%1/%2").arg(dirPathFj).arg(targetName);
        std::cout << "Adding file '" << qPrintable(fullPath) << "' to the archive" << std::endl;

        tar_append_file(archive, qPrintable(fullPath), qPrintable(targetName));
        fileSize = archiveInfo.size();

        emit stepPackaging(k+1, targets.value(k+1), fileSize);
    }

    tar_append_eof(archive);
    tar_close(archive);
    std::cout << "Archive prepared successfully" << std::endl;
    fileSize = archiveInfo.size();

    QString md5sum;
    QFile f(archivePath);
    if (f.open(QFile::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f))
        {
            md5sum = QString(hash.result().toHex());
        }
    }

    emit finishedPackaging(part, step, archivePath, fileSize, md5sum);
}

void ProgressReportService::packageArchive(const Part &part, const Step &step)
{
    if (part.getId() == 1)
    {
        getZeitgeistAndPreloadLogger(part, step);
    }
    else if (part.getId() == 2)
    {
        getZeitgeistAndFirejail(part, step);
    }
}

bool ProgressReportService::processPackageArchiveRequest(const Part &part, const Step &step)
{
    if (part.isValid() && step.isValid() && part.hasReached(step, Step::RUNNING))
    {
        QtConcurrent::run(this, &ProgressReportService::packageArchive, part, step);
        return true;
    }
    else
    {
        QString msg = QString("could not package the archive for part '%1' and step '%2'").arg(part.toString()).arg(step.toString());
        emit invalidReportRequest(msg);
        return false;
    }
}
