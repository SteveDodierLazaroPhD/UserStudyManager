/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#ifndef STUDY_H
#define STUDY_H

#include "Model/participant.h"
#include "Model/step.h"
#include "Services/webviewservice.h"
#include "Services/progressreportservice.h"
#include "Services/requestservice.h"
#include "Services/uploadservice.h"
#include <QString>
#include <QList>
#include <QNetworkCookieJar>
#include <QSettings>
#include <QList>
#include <QMutex>

#define STUDY_ID        "multitasking"

#define HOSTNAME        "https://study.cs.ucl.ac.uk/"
#define STUDY_FOLDER    STUDY_ID"/"
#ifdef QT_DEBUG
#define WEB_FOLDER      "web/app_dev.php/"
#define APP_FOLDER      "web/app_dev.php/a/"
#else
#define WEB_FOLDER      "web/"
#define APP_FOLDER      "web/a/"
#endif
#define WEB_BASE        HOSTNAME STUDY_FOLDER WEB_FOLDER
#define APP_BASE        HOSTNAME STUDY_FOLDER APP_FOLDER

#define APPS_SCHEME     "app"

#define DATE_FORMAT     "yyyy-MM-dd"

class StudyUtils : public QObject
{
    Q_OBJECT

private:
    /* Local participant info */
    Participant           *participant;
    bool                   loggedIn;

    /* Shared service resources */
    QNetworkCookieJar     *jarjarbinks;

    /* Services */
    WebViewService        *webview;
    ProgressReportService *progressReport;
    UploadService         *upload;
    RequestService        *request;

    /* Settings */
    QSettings              globalSettings;
    QSettings              userSettings;
    QMutex                 settingsMutex;

    static StudyUtils *instance;
    explicit StudyUtils();
    ~StudyUtils();

    void initMaxPart();
    void initPartStepOrder();
public:
    static StudyUtils *getUtils();
    static inline void init() { getUtils(); }
    static void clearUtils();

    QDate getInstallDate(const Part &part);
    qint64 getMinQualifyingProgress(const Part &part, const Step &step);
    qint64 getCurrentProgress(const Part &part, const Step &step);

    void loginFinalize(bool status);

    bool isParticipantBeyond(const Part &p, const Step &s);

    static inline QString getStudyId()
        { return STUDY_ID; }

    inline bool isLoggedIn() const
        { return loggedIn; }
    inline Participant *getParticipant() const
        { return participant; }

    inline WebViewService *getWebViewService()
        { return webview; }
    inline ProgressReportService *getProgressReportService()
        { return progressReport; }
    inline UploadService *getUploadService()
        { return upload; }
    inline RequestService *getRequestService()
        { return request; }

    inline QSettings &getGlobalSettings()
        { return globalSettings; }
    inline QSettings &getUserSettings()
        { return userSettings; }

signals:
    void onLoginStatusChanged(bool);

public slots:
    void registerInstall(const Part &part);
    void saveCurrentProgress(const Part &part, const Step &step, const qint64 &loggedDays);
    qint64 getUploadableArchiveSize(const Part &part, const Step &step);
    QString getUploadableArchivePath(const Part &part, const Step &step);
    QString getUploadableArchiveChecksum(const Part &part, const Step &step);
    void saveUploadableArchive(const Part &part, const Step &step, const QString &filePath, const qint64 &fileSize, const QString &checksum);
};

#endif // STUDY_H

