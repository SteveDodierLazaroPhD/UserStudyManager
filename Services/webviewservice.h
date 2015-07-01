/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "Model/participant.h"
#include <QWebView>
#include <QObject>
#include <QList>

struct URLParts {
    QString route;
    Part part;
    Step step;
};

/**
 * @brief The WebManager class acts both as a QWebView and as a service for
 * managed the requests done between the web view and the server. It contains
 * a routing logic that triggers events in the client application based on
 * the URL that the server redirects us to. Some such events are ignored (the
 * web page displayed by the server is self-contained), whilst some must be
 * forwarded to the main window via signals. It is the main window's job to
 * dispatch to the appropriate services and adjust the UI.
 */
class WebViewService : public QWebView
{
    Q_OBJECT
protected:
    static URLParts parseUrl(const QUrl &url);

public:
    explicit WebViewService(QWidget *parent = 0);
    ~WebViewService();

    void setUrl(const QUrl &url);
    void load(const QUrl &url);
    void load(const QNetworkRequest &request,
              QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
              const QByteArray &body = QByteArray());

    void setCookieJar(QNetworkCookieJar *jar);

protected:
//    QList<QUrl> getCurrentPageInternalLinks() const;
//    QList<QUrl> getCurrentPageExternalLinks() const;
//    QList<QUrl> getCurrentPageSubmits() const;

    /**
     * @brief showPageForStep Dispatches the user to a local page based on their current
     * status. Used to internally manage the special "status" route, which we're sent to
     * by the server right after logging in or when the server doesn't know which page
     * to serve us. We must determine the best page to load based on the current status,
     * and either display local content or send a request to the server to have that
     * page loaded.
     *
     * @param p #Participant
     */
    void showPageForStep(Participant const*p);

protected slots:
    /**
     * @brief onHtmlLinkClicked Dispatches clicked urls to the controller or to an outside Web browser
     * @param url the url that has been clicked
     */
    void onHtmlLinkClicked(const QUrl &url);

    /**
     * @brief onPageLoaded Dispatches the last loaded page to controllers that will
     * process their content and modify the UI to display something to the user. This
     * function must support all the routes used by the server's AppPartController
     * plus the relevant AuthController routes.
     *
     * @param success whether the current page was successfully loaded or not
     */
    void onPageLoaded(const bool success);

    /**
     * @brief onUnsupportedStepQueried Displays a background image in the Web view
     * until something else becomes available. This can typically be called by
     * onStatus for when the user is in a state known to be illegal, or when an
     * error has occurred that cannot be programmatically delt with.
     */
    void onUnsupportedStepQueried();

    /**
     * @brief onLoggedIn is the page we're sent to right after a successful
     * login, to update the client's participant object. We then redirect the
     * user to the appropriate page by calling showPageForStep(p);
     */
    void onLoggedIn();

    void onLoginFormShown() const;
    void onStatus();
    void onInstall(const URLParts &parts);

public slots:
    void openDesktopUrl(const QUrl &url);

    bool loadLoginPage();
    bool loadStatusPage();
    bool loadInfoPage();
    bool loadContactPage();
    bool loadInstallPage(const Participant *&p);
    bool loadUploadPage();
    bool loadUploadPage(const Part &part, const Step &step);
    bool loadShowStatusPage();
    bool loadReportProgressPage();
    
signals:
    /**
     * @brief unsupportedStepQueried Emitted when the web view is asked to render a page
     * for a specific step, but that page falls outside the scope of the app API and the
     * native website would be a better fit.
     */
    void unsupportedStepQueried(Part, Step);

    /**
     * @brief unsupportedServerAPIQueried Emitted when the web manager is asked to reply
     * to an app API function it does not understand, and when doing nothing would render
     * unintelligible JS in the web view.
     */
    void unsupportedServerAPIQueried(QString id, QString description);

    /**
     * @brief uploadJobActionRequested
     * @param content
     */
    void uploadJobActionRequested(const QString &content);

    /**
     * @brief reportProgressRequested TODO
     * @param content
     */
    void reportProgressRequested(const QString &content);
};

#endif // NETWORKMANAGER_H
