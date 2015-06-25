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
#include <QMutexLocker>
#include "requestservice.h"
#include "study.h"

RequestService::RequestService(QObject *parent) :
    QObject(parent),
    loop(),
    manager(),
    running()
{
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
}

void RequestService::postRequest(const QUrl &url, const QByteArray &content, const QString &contentType)
{
    QMutexLocker locker(&running);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    req.setHeader(QNetworkRequest::ContentLengthHeader, content.length());
    req.setHeader(QNetworkRequest::UserAgentHeader, "UCL Study Manager");

    QNetworkReply *reply = manager.post(req, content);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        //TODO
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

void RequestService::sendProgressReportRequest(const Part &part, const Step &step, const qint64 &dayCount) //FIXME //TODO
{
    if (!part.hasReached(step, Step::RUNNING))
        return;

    QUrl url(QString(APP_BASE) + part.toString()+"/reportprogress");
    QByteArray content = QString("{\"ReportProgress\": {\"Step\": \"%1\", \"Progress\": %2}")
            .arg(step.toString())
            .arg(dayCount)
            .toUtf8();

    postRequest(url, content, "application/json");
}


