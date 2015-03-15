#ifndef STUDY_H
#define STUDY_H

#include "participant.h"
#include "step.h"
#include <QString>
#include<QList>

#define PART_COUNT 2
#define STUDY_ID        "multitasking"

#define HOSTNAME        "https://study.cs.ucl.ac.uk/"
#define STUDY_FOLDER    STUDY_ID"/"
#define WEB_FOLDER      "web/"
#define APP_FOLDER      "web/a/"
#define APP_BASE        HOSTNAME STUDY_FOLDER APP_FOLDER
#define WEB_BASE        HOSTNAME STUDY_FOLDER WEB_FOLDER

class StudyUtils {
private:
    Participant *participant;
    bool loggedIn;
    short maxPart;
    QList<Step> stepOrder;

    static StudyUtils *instance;
    explicit StudyUtils();
    ~StudyUtils();

public:
    void setMaxPart(const short part);
    void loginFinalize(bool status);

    inline bool getLoggedIn() const
        { return loggedIn; }
    inline short getMaxPart() const
        { return maxPart; }
    inline Participant *getParticipant() const
        { return participant; }
    inline const QList<Step> &getStepOrder() const
        { return stepOrder; }

    static StudyUtils *getUtils();
    static void clearUtils();

    static bool isAppId(const QString &string);
    static inline QString getStudyId()
        { return STUDY_ID; }

};

#endif // STUDY_H

