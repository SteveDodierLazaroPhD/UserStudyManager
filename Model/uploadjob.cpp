/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#include "Model/uploadjob.h"
#include <QJsonValue>
#include <QJsonObject>
#include <iostream>

UploadJob::UploadJob()
{
}

UploadJob UploadJob::fromJson(const QJsonObject &json)
{
    bool ok = false;
    UploadJob job;
    job.part = Part::fromId(json["Part"].toInt());
    job.step = Step::fromName(json["Step"].toString());
    job.dayCount = json["DayCount"].toString("-1").toLongLong(&ok, 10);
    if(!ok)
        job.dayCount = -1;
    job.expectedSize = json["ExpectedSize"].toString("-1").toLongLong(&ok, 10);
    if(!ok)
        job.expectedSize = -1;
    job.obtainedSize = json["ObtainedSize"].toString("-1").toLongLong(&ok, 10);
    if(!ok)
        job.obtainedSize = -1;
    job.checksum = json["Checksum"].toString();

    return job;
}

QJsonObject UploadJob::toJson() const
{
    QJsonObject obj;

    obj.insert("Part", this->part.getId());
    obj.insert("Step", this->step.getName());
    obj.insert("DayCount", QString::number(this->dayCount, 10));
    obj.insert("ExpectedSize", QString::number(this->expectedSize, 10));
    obj.insert("ObtainedSize", QString::number(this->obtainedSize, 10));
    obj.insert("Checksum", this->checksum);

    return obj;
}
