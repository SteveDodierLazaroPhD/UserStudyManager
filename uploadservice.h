#ifndef UCLNETQUERIES_H
#define UCLNETQUERIES_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class UploadService : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit UploadService(QObject *parent = 0);
    ~UploadService();

    /**
     * @brief createRequest We hook this function to interrupt specific form submissions
     * @param op type of operation
     * @param request the request we're working with
     * @param outgoingData the data sent by the client
     * @return the underlying QNetworkAccessManager's QNetworkReply
     */
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);

signals:

    /**
     * @brief onUploadFormSubmitted indicates the upload form has been submitted by the user and upload can start
     */
    void onUploadFormSubmitted();

    /**
     * @brief uploadStarted
     */
    void uploadStarted();
    /**
     * @brief uploadFinished
     */
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    /**
     * @brief uploadFinished
     */
    void uploadFinished();

public slots:
};

#endif // UCLNETQUERIES_H
