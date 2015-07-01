/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#ifndef DATAUPLOADJOB_H
#define DATAUPLOADJOB_H

#include "Model/part.h"
#include "Model/step.h"
#include <QString>
#include <QJsonObject>

class UploadJob
{
    Part part;
    Step step;
    qint64 dayCount;
    qint64 expectedSize;
    qint64 obtainedSize;
    QString checksum;

public:
    UploadJob();
    static UploadJob fromJson(const QJsonObject &json);
    QJsonObject toJson() const;

    inline const Part &getPart() const
    { return part; }

    inline const Step &getStep() const
    { return step; }

    inline qint64 getExpectedSize() const
    { return expectedSize; }

    inline qint64 getObtainedSize() const
    { return obtainedSize; }

    inline const QString &getChecksum() const
    { return checksum; }

    inline void setDayCount(const qint64 &count)
    { dayCount = count; }

    inline void setObtainedSize(const qint64 &size)
    { obtainedSize = size; }

    inline void setExpectedSize(const qint64 &size)
    { expectedSize = size; }

    inline void setChecksum(const QString &sum)
    { checksum = sum; }

    inline bool hasValidDayCount() const
    { return dayCount >= 0; }

    inline bool hasValidChecksum() const
    { return !checksum.isEmpty(); }

    inline bool hasValidObtainedSize() const
    { return obtainedSize >= 0; }

    inline bool hasValidExpectedSize() const
    { return expectedSize >= 0; }
};

#endif // DATAUPLOADJOB_H
