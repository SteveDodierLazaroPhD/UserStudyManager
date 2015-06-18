#include <iostream>
#include "uploadservice.h"
#include "study.h"
using namespace std;

UploadService::UploadService(QObject *parent) :
    QNetworkAccessManager(parent)
{
    cout << "Upload service created" << endl;
}

UploadService::~UploadService()
{

}

QNetworkReply *UploadService::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    if (QNetworkAccessManager::PostOperation == op)
    {
        const QString &route = StudyUtils::getUrlRouteName(request.url());
        /* We intercept this call, this code is experimental/useless though */
        if (route == "upload")
        {
            emit onUploadFormSubmitted();
        }
    }

    QNetworkReply *rep = QNetworkAccessManager::createRequest(op, request, outgoingData);
    return rep;
}

