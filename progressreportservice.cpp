#include "progressreportservice.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QWebSettings>

#include <QtConcurrent>
#include <QStandardPaths>

#include <QZeitgeist/QZeitgeist>
#include <QZeitgeist/Log>
#include <QtDBus/QDBusPendingReply>

#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <iostream>
#include "study.h"

#define DAY_IN_MSEC  86400000
#define HOUR_IN_MSEC  3600000
#define MIN_IN_MSEC     60000
#define SEC_IN_MSEC      1000

using namespace std;

ProgressReportService::ProgressReportService()
{
    cout << "Progress report service created" << endl;
}

ProgressReportService::~ProgressReportService()
{
}

void sendRequest() //FIXME //TODO
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req( QUrl( QString("http://time.jsontest.com/") ) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {

        QString strReply = (QString)reply->readAll();

        //parse json
        qDebug() << "Response:" << strReply;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());

        QJsonObject jsonObj = jsonResponse.object();

        qDebug() << "Time:" << jsonObj["time"].toString();
        qDebug() << "Date:" << jsonObj["date"].toString();

        delete reply;
    }
    else {
        //failure
        qDebug() << "Failure" <<reply->errorString();
        delete reply;
    }
}




bool ProgressReportService::processReportRequest(const QString &request)
{
    QJsonDocument jResponse = QJsonDocument::fromJson(request.toUtf8());
    QJsonObject jObj = jResponse.object();

    if (jObj["ReportProgress"].toString() == "ReadyForContent")
    {
        QJsonObject participant = jObj["Participant"].toObject();
        QJsonObject status = participant["Status"].toObject();
        Part part = Part(status["Part"].toInt());
        Step step = Step::fromName(status["Step"].toString());

        // We only have one type of upload, so checking that the user is at least beyond RUNNING is enough.
        if (part.isValid() && step.isValid() && part.hasReached(step, Step::RUNNING))
        {
            QtConcurrent::run(this, &ProgressReportService::calculateZeitgeistDayCount, part, step);
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
        emit stepReport(i, loggedDays);
    }

    emit finishedProgressCalculation(part, step, loggedDays);
    StudyUtils::getUtils()->saveCurrentProgress(part, step, loggedDays);
    return loggedDays;
}

void ProgressReportService::packageArchive(const Part &part, const Step &step)
{
    emit startingPackaging(part, step);

    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "zeitgeist", QStandardPaths::LocateDirectory);

    QString filePath(path);
    qint64 fileSize = 0;

    emit finishedPackaging(part, step, filePath, fileSize);
    StudyUtils::getUtils()->saveUploadableArchive(part, step, filePath, fileSize);
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
