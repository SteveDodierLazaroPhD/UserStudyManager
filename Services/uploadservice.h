/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#ifndef UCLNETQUERIES_H
#define UCLNETQUERIES_H

#include <QObject>
#include <QFile>
#include <QByteArray>
#include "Model/uploadjob.h"

#define UPLOAD_PACKET_SEPARATOR "------"
#define UPLOAD_PACKET_LEN        32000

class UploadService : public QObject
{
    Q_OBJECT
public:
    explicit UploadService(QObject *parent = 0);
    ~UploadService();

    QByteArray getPacketToUpload(const UploadJob &job);

};

#endif // UCLNETQUERIES_H
