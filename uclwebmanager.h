#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QWebView>
#include <QObject>
#include <QList>

class UCLWebManager : public QWebView
{
    Q_OBJECT

public:
    explicit UCLWebManager(QWidget *parent = 0);
    ~UCLWebManager();

    void setUrl(const QUrl &url);
    void load(const QUrl &url);
    void load(const QNetworkRequest &request,
              QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
              const QByteArray &body = QByteArray());

protected:
    QList<QUrl> getCurrentPageInternalLinks() const;
    QList<QUrl> getCurrentPageExternalLinks() const;
    QList<QUrl> getCurrentPageSubmits() const;


    QString getUrlAppId(const QUrl &url);
    QString getUrlRouteName(const QUrl &url);

signals:
    void loggedIn(bool);

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
    void onUrlChange(const QUrl &url);

    void onLoggedIn();
    void onFailedLogin() const;

public slots:
    void openDesktopUrl(const QUrl &url);
    bool loadLoginPage();
    bool loadInfoPage();
    bool loadContactPage();
};

#endif // NETWORKMANAGER_H
