#ifndef UCLPROGRESSREPORTMANAGER_H
#define UCLPROGRESSREPORTMANAGER_H

#include <QObject>
#include "part.h"
#include "step.h"

class ProgressReportService : public QObject
{
    Q_OBJECT
protected:
    bool hasLockedProgressFile;
    //TODO UCLProgress currentProgress;

public:
    ProgressReportService();
    ~ProgressReportService();

    bool processReportRequest(const QString &request);
    bool calculateProgress(Part, Step);

protected:
    void openZeitgeistConnection();
    int getZeitgeistDayCount();


signals:
    void startingProgressReport(const Part &, const Step &);
    void invalidProgressReportRequest(const QString &msg);
};

#endif // UCLPROGRESSREPORTMANAGER_H
