#ifndef STUDY_H
#define STUDY_H

#include "part.h"
#include "step.h"
#include <QString>

#define PART_COUNT 2
#define STUDY_ID        "multitasking"

#define HOSTNAME        "https://study.cs.ucl.ac.uk/"
#define STUDY_FOLDER    STUDY_ID"/"
#define WEB_APP_FOLDER  "web/a/"
#define URL_BASE        HOSTNAME STUDY_FOLDER WEB_APP_FOLDER

class StudyUtils {
private:
    QString studyId;
    QString appId;
    bool loggedIn;
    short maxPart;

    static StudyUtils *instance;
    explicit StudyUtils();

public:
    void setMaxPart(const short part);
    inline void setLoggedIn(const bool status)
        { loggedIn = status; }

    inline QString getStudyId() const
        { return studyId; }
    inline QString getAppId() const
        { return appId; }
    inline bool getLoggedIn() const
        { return loggedIn; }
    inline short getMaxPart() const
        { return maxPart; }

    static StudyUtils *getUtils();
    static void clearUtils();

    QString setParticipantIdentity(const QString &email);
    //QString buildStudyURI(const Part &part, const Step &step);
    //QString buildStudyURI(const Part &part, const QString &customUri);
};

#endif // STUDY_H

