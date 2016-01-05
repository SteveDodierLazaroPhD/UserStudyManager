/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#ifndef UCLPROGRESSREPORTMANAGER_H
#define UCLPROGRESSREPORTMANAGER_H

#include "Model/part.h"
#include "Model/step.h"
#include <QObject>

#define DAY_IN_MSEC  86400000
#define HOUR_IN_MSEC  3600000
#define MIN_IN_MSEC     60000
#define SEC_IN_MSEC      1000

class ProgressReportService : public QObject
{
    Q_OBJECT
protected:
    //FIXME
    bool hasLockedProgressFile;
    //TODO UCLProgress currentProgress;

public:
    ProgressReportService();
    ~ProgressReportService();

public slots:
    bool processReportRequest(const QString &request);
    bool processPackageArchiveRequest(const Part &part, const Step &step);

protected:
    qint64 calculateZeitgeistDayCount(const Part &, const Step &);
    void getZeitgeistAndPreloadLogger(const Part &, const Step &);
    void getZeitgeistAndFirejail(const Part &, const Step &);
    void packageArchive(const Part &, const Step &);

signals:
    void startingReport(const Part &, const Step &);
    void targetForReport(const qint64 &target);
    void stepReport(const qint64 &step, const qint64 &tmpResult);
    void finishedProgressCalculation(const Part &, const Step &, const qint64 &result);
    void invalidReportRequest(const QString &msg);

    void startingPackaging(const Part &, const Step &);
    void targetForPackaging(const qint64 &target, const QString &nextFile);
    void stepPackaging(const qint64 &step, const QString &nextFile, const qint64 &tmpSize);
    void finishedPackaging(const Part &, const Step &, const QString &filePath, const qint64 &fileSize, const QString &checksum);
    void invalidPackagingRequest(const QString &msg);
};

#endif // UCLPROGRESSREPORTMANAGER_H
