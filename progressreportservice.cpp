#include "progressreportservice.h"

#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QWebSettings>

#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <iostream>

using namespace std;

ProgressReportService::ProgressReportService()
{

}

ProgressReportService::~ProgressReportService()
{

}

void sendRequest()
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
        //TODO calculate
        QJsonObject p = jObj["Participant"].toObject();
        QJsonObject s = p["Status"].toObject();
        Part part = Part(s["Part"].toInt());
        Step step = Step::fromName(s["Step"].toString());

        if (part.isValid() && step.isValid())
        {
            emit startingProgressReport(part, step);
            calculateProgress(part, step);
            return true;
        }
        else
        {
            QString msg;
            if(!part.isValid())
                msg = "foo part";
            else if(!step.isValid())
                msg = "foo step";

            emit invalidProgressReportRequest(msg);
            return false;
        }
    }
    else
    {
        QString msg("Error whilst parsing a ReportProgress: the message type was  '" + jObj["ReportProgress"].toString() + "' which is unknown");
        emit invalidProgressReportRequest(msg);
        return false;
    }
}

bool ProgressReportService::calculateProgress(Part, Step)
{
     //then put a thread to calculate, which then sends a request back whilst we wait for a "Done" signal. FIXME
    cerr << "TODO CALCULATE " << endl;


    return true;
}
