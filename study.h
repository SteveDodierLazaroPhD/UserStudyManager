#ifndef STUDY_H
#define STUDY_H

#include "participant.h"
#include "step.h"
#include <QString>
#include <QObject>
#include <QList>

#define PART_COUNT 2
#define STUDY_ID        "multitasking"

#define HOSTNAME        "https://study.cs.ucl.ac.uk/"
#define STUDY_FOLDER    STUDY_ID"/"
#define WEB_FOLDER      "web/"
#define APP_FOLDER      "web/a/"
#define APP_BASE        HOSTNAME STUDY_FOLDER APP_FOLDER
#define WEB_BASE        HOSTNAME STUDY_FOLDER WEB_FOLDER

#define APPS_SCHEME     "app"

class StudyUtils : public QObject
{
    Q_OBJECT

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

    inline bool isLoggedIn() const
        { return loggedIn; }
    inline short getMaxPart() const
        { return maxPart; }
    inline Participant *getParticipant() const
        { return participant; }
    inline const QList<Step> &getStepOrder() const
        { return stepOrder; }

    static StudyUtils *getUtils();
    static void clearUtils();

    static bool isPart(const QString &string);
    static QString getUrlRouteName(const QUrl &url);

    static inline QString getStudyId()
        { return STUDY_ID; }

signals:
    void onLoginStatusChanged(bool);
};

#endif // STUDY_H

