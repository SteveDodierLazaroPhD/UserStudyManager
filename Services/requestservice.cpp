/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#include "study.h"
#include "Model/uploadjob.h"
#include "Services/requestservice.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QWebSettings>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMutexLocker>
#include <QDebug>
#include <iostream>

RequestService::RequestService(QObject *parent) :
    QObject(parent),
    loop(),
    manager(),
    running()
{
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    QObject::connect(this, SIGNAL(uploadStepError(Part,Step,QString)), this, SLOT(onError(Part,Step,QString)));
    QObject::connect(this, SIGNAL(uploadParametersError(Part,Step,QString)), this, SLOT(onError(Part,Step,QString)));
    QObject::connect(this, SIGNAL(uploadGenericError(Part,Step,QString)), this, SLOT(onError(Part,Step,QString)));
    QObject::connect(this, SIGNAL(uploadParametersSucceeded(QString)), this, SLOT(dispatchUploadMessage(QString)));
    QObject::connect(this, SIGNAL(uploadStepSucceeded(QString)), this, SLOT(dispatchUploadMessage(QString)));
}

QString RequestService::postRequest(const QUrl &url, const QByteArray &content, const QString &contentType, QString *errorPlaceholder)
{
    QMutexLocker locker(&running);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    req.setHeader(QNetworkRequest::ContentLengthHeader, content.length());
    req.setHeader(QNetworkRequest::UserAgentHeader, "UCL Study Manager");

    QNetworkReply *reply = manager.post(req, content);
    loop.exec();

    QString replyContent;
    if (reply->error() == QNetworkReply::NoError)
        replyContent = (QString)reply->readAll();
    else if (errorPlaceholder)
    {
        *errorPlaceholder = "Request to '"+url.toString()+"' failed: "+reply->errorString();
        qDebug() << *errorPlaceholder;
    }

    qDebug() << "Request to: " << qPrintable(url.toString());
    qDebug() << "Content: "<< (content.length() > 20000? "Content too long":qPrintable(content));
    qDebug() << "Content length: "<< content.length();
    qDebug() << "Reply: " << qPrintable(replyContent);

    delete reply;
    return replyContent;
}

///* Progress reporting API */
void RequestService::sendProgressReportRequest(const Part &part, const Step &step, const qint64 &dayCount)
{
    if (!part.hasReached(step, Step::RUNNING))
        return;

    QUrl url(QString(APP_BASE) + part.toString()+"/reportprogress");
    QByteArray content = QString("{\"ReportProgress\": {\"Step\": \"%1\", \"Progress\": %2}}")
            .arg(step.toString())
            .arg(dayCount)
            .toUtf8();

    QString error;
    QString reply = postRequest(url, content, "application/json", &error);

    if (reply.isEmpty())
    {
        emit progressReportFailed(part, step, error);
        return;
    }

    QJsonObject jsonObj = QJsonDocument::fromJson(reply.toUtf8()).object();

    if (jsonObj["ReportProgress"].toString() == "Failure")
    {
        emit progressReportFailed(part, step, jsonObj["FailureCause"].toString());
        return;
    }
    else if (jsonObj["ReportProgress"].toString() == "ReadyForContent")
    {
        QJsonObject stepProgress = jsonObj["StepProgress"].toObject();
        if (Part::fromString(stepProgress["Part"].toString()) == part ||
            Step::fromName(stepProgress["Step"].toString()) == step ||
            stepProgress["Progress"].toInt() == dayCount)
        {
            emit progressReportSucceeded(part, step, dayCount);
            return;
        }
    }

    emit progressReportFailed(part, step, "An unknown error occurred when sending a progress report, please try again.");
}

///* Upload API */
void RequestService::dispatchUploadMessage(const QString &message)
{
    QJsonObject jsonObj = QJsonDocument::fromJson(message.toUtf8()).object();
    QString messageType = jsonObj["Uploading"].toString();

    if (messageType == "ReadyJobParameters")
    {
        const QJsonObject & jsonJob = jsonObj["UploadJob"].toObject();
        UploadJob job = UploadJob::fromJson(jsonJob);
        const Part &part = job.getPart();
        const Step &step = job.getStep();

        emit uploadStarted(part, step);
        sendUploadJobParameters(jsonObj);
    }
    else if (messageType == "ReadyData")
    {
        sendUploadContentPacket(jsonObj);
    }
    else
    {
        const QJsonObject & jsonJob = jsonObj["UploadJob"].toObject();
        UploadJob job = UploadJob::fromJson(jsonJob);
        const Part &part = job.getPart();
        const Step &step = job.getStep();

        if (messageType == "Done")
            emit uploadSucceeded(part, step);
        else if (messageType == "Failure")
        {
            QString errMsg = jsonObj["FailureCause"].toString();
            emit uploadGenericError(part, step, errMsg);
        }
        else
        {
            QString errMsg = QString("Unsupported response received from the server: '%1'").arg(messageType);
            emit uploadGenericError(part, step, errMsg);
        }
    }
}

void RequestService::onError(const Part &part, const Step &step, const QString &errMsg)
{
    qDebug() << "Error: " << qPrintable(errMsg);
    emit uploadFailed(part, step, errMsg);
}

//{"Uploading":"ReadyData", "UploadJob":{"Part": 1, "Step": "running", "DayCount": 79, "ExpectedSize": "55449088", "ObtainedSize": "0", "Checksum": "2a19bc337f8d3a2026d8af8ab2365472"}, "Participant":{"LoggedIn":{"Username":"bidulgi","Email":"bidulgi@geomi.com"},"Status":{"Part":1,"Step":"running"}}}
void RequestService::sendUploadContentPacket(const QJsonObject &jsonObj)
{
    StudyUtils *utils = StudyUtils::getUtils();
    UploadService *upload = utils->getUploadService();

    const QJsonObject & jsonJob = jsonObj["UploadJob"].toObject();
    UploadJob job = UploadJob::fromJson(jsonJob);

    QUrl url(QString(APP_BASE) + job.getPart().toString()+"/uploading?Uploading=Uploading");
    QByteArray packet = upload->getPacketToUpload(job);
    if (packet.isEmpty())
    {
        emit uploadStepError(job.getPart(), job.getStep(), "Could not open the local archive to send it to the server.");
        return;
    }

    QString error;
    QString reply = postRequest(url, packet, "application/octet-stream", &error);

    if (reply.isEmpty())
    {
        emit uploadStepError(job.getPart(), job.getStep(), error);
        return;
    }

    QJsonObject replyObj = QJsonDocument::fromJson(reply.toUtf8()).object();
    if (replyObj["Uploading"].toString() == "Failure")
    {
        emit uploadStepError(job.getPart(), job.getStep(), replyObj["FailureCause"].toString());
        return;
    }
    else if (replyObj["Uploading"].toString() == "ReadyData")
    {
        QJsonObject errReport = replyObj["ErrorReport"].toObject();
        if(errReport.size() > 0)
        {
            if(errReport["LengthOffset"].toInt())
            {
                emit uploadStepError(job.getPart(), job.getStep(), QString("Did not send the expected amount of data (delta %1").arg(errReport["LengthOffset"].toInt()));
            }
            else if(errReport["HashMismatch"].toBool(false))
            {
                emit uploadStepError(job.getPart(), job.getStep(), "Hashes didn't match for these parts");

            }
            else if(errReport["ExpectedSizeOverflow"].toBool(false))
            {
                emit uploadStepError(job.getPart(), job.getStep(), "Sent more data than was expected for the whole file");
            }
        }
        else
        {
            UploadJob replyJob = UploadJob::fromJson(replyObj["UploadJob"].toObject());
            emit uploadStep(job.getPart(), job.getStep(), replyJob.getObtainedSize());
            emit uploadStepSucceeded(reply);
        }
    }
    else if (replyObj["Uploading"].toString() == "Done")
    {
        emit uploadSucceeded(job.getPart(), job.getStep());
    }
    else
        emit uploadStepError(job.getPart(), job.getStep(), "An unknown error occurred while uploading your data, please try again.");
}

// {"Uploading":"ReadyJobParameters", "UploadJob":{"Part": 1, "Step": "running", "DayCount": 79, "ExpectedSize": null, "ObtainedSize": 0, "Checksum": null}, "Participant":{"LoggedIn":{"Username":"bidulgi","Email":"bidulgi@geomi.com"},"Status":{"Part":1,"Step":"running"}}}
void RequestService::sendUploadJobParameters(const QJsonObject &jsonObj)
{
    StudyUtils *utils = StudyUtils::getUtils();

    const QJsonObject & jsonJob = jsonObj["UploadJob"].toObject();
    UploadJob job = UploadJob::fromJson(jsonJob);
    const Part &part = job.getPart();
    const Step &step = job.getStep();

    if (!part.isValid() || !step.isValid())
    {
        QString errMsg = QString("The study server is asking us to start an invalid upload job (part '%1', step '%1'), aborting.").arg(part.toString()).arg(step.getName());
        emit uploadParametersError(part, step, errMsg);
        return;
    }

    /* Find and fix missing bits, before reconverting to JSON */
    if (!job.hasValidDayCount())
        job.setDayCount(utils->getCurrentProgress(part, step));
    if (!job.hasValidChecksum())
        job.setChecksum(utils->getUploadableArchiveChecksum(part, step));
    if (!job.hasValidExpectedSize())
        job.setExpectedSize(utils->getUploadableArchiveSize(part, step));
    if (!job.hasValidObtainedSize())
        job.setObtainedSize(0);

    QJsonObject jsonToSend;
    jsonToSend.insert("Uploading", "JobParameters");
    jsonToSend.insert("UploadJob", job.toJson());

    QUrl url(QString(APP_BASE) + part.toString()+"/uploading?Uploading=JobParameters");
    QByteArray content = QJsonDocument(jsonToSend).toJson();

    QString error;
    QString reply = postRequest(url, content, "application/json", &error);

    if (reply.isEmpty())
    {
        emit uploadParametersError(part, step, error);
        return;
    }

    QJsonObject replyObj = QJsonDocument::fromJson(reply.toUtf8()).object();
    QJsonDocument doc(replyObj);
    if (replyObj["Uploading"].toString() == "Failure")
    {
        emit uploadParametersError(part, step, replyObj["FailureCause"].toString());
        return;
    }
    else if (replyObj["Uploading"].toString() == "ReadyData")
    {
        emit uploadParametersSet(part, step, job.getExpectedSize());
        emit uploadParametersSucceeded(reply);
    }
    else
        emit uploadParametersError(part, step, "An unknown error occurred when sending information about the upload, please try again.");
}




