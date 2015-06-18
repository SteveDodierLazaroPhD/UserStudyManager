#ifndef UCLPROGRESSREPORTMANAGER_H
#define UCLPROGRESSREPORTMANAGER_H

#include <QObject>
#include "part.h"
#include "step.h"

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
    void packageArchive(const Part &, const Step &);

signals:
    void startingReport(const Part &, const Step &);
    void targetForReport(const qint64 &target);
    void stepReport(const qint64 &step, const qint64 &tmpResult);
    void finishedProgressCalculation(const Part &, const Step &, const qint64 &result);
    void invalidReportRequest(const QString &msg);

    void startingPackaging(const Part &, const Step &);
    void targetForPackaging(const qint64 &target);
    void stepPackaging(const qint64 &step, const qint64 &tmpSize);
    void finishedPackaging(const Part &, const Step &, const QString &filePath, const qint64 &fileSize);
    void invalidPackagingRequest(const QString &msg);
};

#endif // UCLPROGRESSREPORTMANAGER_H
