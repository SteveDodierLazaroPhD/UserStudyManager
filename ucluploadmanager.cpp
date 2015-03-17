#include <iostream>
#include "ucluploadmanager.h"
#include "study.h"
using namespace std;

UCLUploadManager::UCLUploadManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
    cout << "Upload manager created" << endl;
}

UCLUploadManager::~UCLUploadManager()
{

}

QNetworkReply *UCLUploadManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
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

