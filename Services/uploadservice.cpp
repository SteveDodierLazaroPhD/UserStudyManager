/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#include "study.h"
#include "Services/uploadservice.h"
#include <iostream>
#include <QCryptographicHash>

using namespace std;

UploadService::UploadService(QObject *parent) :
    QObject(parent)
{
    cout << "Upload service created" << endl;
}

UploadService::~UploadService()
{
}

QByteArray UploadService::getPacketToUpload(const UploadJob &job)
{
    StudyUtils *utils = StudyUtils::getUtils();
    QString path = utils->getUploadableArchivePath(job.getPart(), job.getStep());
    QFile file(path);

    file.open(QFile::ReadOnly);
    if (!file.isReadable())
    {
        std::cerr << "Could not open the file at '" << qPrintable(path) << "' to upload it" << std::endl;
        file.close();
        return QByteArray();
    }

    bool sought = file.seek(job.getObtainedSize());
    if(!sought)
    {
        std::cerr << "Could not seek into the file at '" << qPrintable(path) << "', this is an I/O error on your operating system" << std::endl;
        return QByteArray();
    }

    char *data = (char *) malloc(sizeof(char) * UPLOAD_PACKET_LEN);
    qint64 dataLen = file.read(data, UPLOAD_PACKET_LEN);
    file.close();

    if(dataLen < 0)
    {
        free(data);
        std::cerr << "Could not read data from the file at '" << qPrintable(path) << "' even though it was readable" << std::endl;
        return QByteArray();
    }
    /* Paranoia */
    else if(dataLen > UPLOAD_PACKET_LEN)
        dataLen = UPLOAD_PACKET_LEN;

    QByteArray checksum;
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(data, dataLen);
    checksum = hash.result().toHex();
    if(checksum.isEmpty())
    {
        std::cerr << "Could not calculate a checksum for the " << dataLen << " bytes long data chunk to be uploaded next" << std::endl;
        return QByteArray();
    }

    QString number = QString("%1").arg(dataLen, 24, 10, QChar('0'));
    QByteArray packet = checksum + UPLOAD_PACKET_SEPARATOR;
    packet += number.toUtf8() + UPLOAD_PACKET_SEPARATOR;
    packet.append(data, dataLen);

    free(data);

    return packet;
}
