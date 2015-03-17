#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QWebView>
#include <QObject>
#include <QList>
#include "participant.h"
#include "ucluploadmanager.h"

class UCLWebManager : public QWebView
{
    Q_OBJECT
protected:
    UCLUploadManager *manager;

public:
    explicit UCLWebManager(QWidget *parent = 0);
    ~UCLWebManager();

    void setUrl(const QUrl &url);
    void load(const QUrl &url);
    void load(const QNetworkRequest &request,
              QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
              const QByteArray &body = QByteArray());

protected:
//    QList<QUrl> getCurrentPageInternalLinks() const;
//    QList<QUrl> getCurrentPageExternalLinks() const;
//    QList<QUrl> getCurrentPageSubmits() const;

    /**
     * @brief showPageForStep  If the current step is supported by an app API, dispatches to the appropriate method.
     * Else, tells the main window UI to display a button to the website taking the user to the appropriate step.
     * @param p #Participant
     */
    void showPageForStep(Participant const*p);

protected slots:
    /**
     * @brief onLinkClicked Dispatches clicked urls to the controller or to an outside Web browser
     * @param url the url that has been clicked
     */
    void onLinkClicked(const QUrl &url);

    /**
     * @brief onPageLoaded Dispatches locally loaded pages to controllers that will process their content and modify the UI to display something to the user
     * @param success whether the current page was successfully loaded or not
     */
    void onPageLoaded(const bool success);

    /**
     * @brief onUnsupportedStepQueried Displays a background image in the Web view until something else becomes available.
     */
    void onUnsupportedStepQueried();

    void onLoggedIn();
    void onFailedLogin() const;
    void onStatus();

public slots:
    void openDesktopUrl(const QUrl &url);
    bool loadLoginPage();
    bool loadInfoPage();
    bool loadContactPage();
    bool loadUploadPage();
    
signals:
    /**
     * @brief uploadStarted
     */
    void uploadStarted();
    /**
     * @brief uploadFinished
     */
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    /**
     * @brief uploadFinished
     */
    void uploadFinished();

    /**
     * @brief unsupportedStepQueried Emitted when the web view is asked to render a page for a specific step, but that page falls outside the scope of the app API and the native website would be a better fit.
     */
    void unsupportedStepQueried(Part, Step);
};

#endif // NETWORKMANAGER_H
