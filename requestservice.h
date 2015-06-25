#ifndef REQUESTSERVICE_H
#define REQUESTSERVICE_H

#include <QObject>
#include <QMutex>
#include <QEventLoop>
#include <QNetworkAccessManager>

#include "part.h"
#include "step.h"

class RequestService : public QObject
{
    Q_OBJECT
public:
    explicit RequestService(QObject *parent = 0);

protected:
    QEventLoop loop;
    QNetworkAccessManager manager;
    QMutex running;

    void postRequest(const QUrl &url, const QByteArray &content, const QString &contentType = "application/json");

public slots:
    void sendProgressReportRequest(const Part &part, const Step &step, const qint64 &dayCount = 0);

signals:
};

#endif // REQUESTSERVICE_H
