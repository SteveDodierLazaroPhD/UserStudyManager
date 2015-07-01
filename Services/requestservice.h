/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#ifndef REQUESTSERVICE_H
#define REQUESTSERVICE_H

#include "Model/part.h"
#include "Model/step.h"

#include <QObject>
#include <QMutex>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QJsonObject>

class RequestService : public QObject
{
    Q_OBJECT
public:
    explicit RequestService(QObject *parent = 0);

    inline QNetworkAccessManager &getManager()
    { return manager; }

protected:
    QEventLoop loop;
    QNetworkAccessManager manager;
    QMutex running;

    QString postRequest(const QUrl &url, const QByteArray &content, const QString &contentType = "application/json", QString *errorPlaceholder = nullptr);

    void sendUploadContentPacket(const QJsonObject &jsonObj);
    void sendUploadJobParameters(const QJsonObject &jsonObj);

public slots:
    void sendProgressReportRequest(const Part &part, const Step &step, const qint64 &dayCount = 0);
    void dispatchUploadMessage(const QString &message);

private slots:
    void onError(const Part &part, const Step &step, const QString &errMsg);

signals:
    void progressReportFailed(const Part &part, const Step &step, const QString &errMsg);
    void progressReportSucceeded(const Part &part, const Step &step, const qint64 &dayCount);
    void uploadStarted(const Part &part, const Step &step);
    void uploadParametersSet(const Part &part, const Step &step, const qint64 &sentSize);
    void uploadParametersSucceeded(const QString &reply);
    void uploadParametersError(const Part &part, const Step &step, const QString &errMsg);
    void uploadStep(const Part &part, const Step &step, const qint64 &sentSize);
    void uploadStepSucceeded(const QString &reply);
    void uploadStepError(const Part &part, const Step &step, const QString &errMsg);
    void uploadGenericError(const Part &part, const Step &step, const QString &errMsg);
    void uploadSucceeded(const Part &part, const Step &step);
    void uploadFailed(const Part &part, const Step &step, const QString &errMsg);
};

#endif // REQUESTSERVICE_H
