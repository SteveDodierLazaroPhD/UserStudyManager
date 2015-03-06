#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QWebView>
#include <QObject>

class UCLWebManager : public QWebView
{
    Q_OBJECT

public:
    explicit UCLWebManager(QWidget *parent = 0);
    ~UCLWebManager();
    void openDesktopUrl(const QUrl &url);

    void setUrl(const QUrl &url);
    void load(const QUrl &url);
    void load(const QNetworkRequest &request,
              QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
              const QByteArray &body = QByteArray());

signals:
    void loggedIn(bool);

protected slots:
    void onUrlChange(const QUrl &url);
    void onLinkClicked(const QUrl &url);
    void onLoggedIn(const bool success);

public slots:
    bool loadLoginPage();
    bool loadContactPage();
};

#endif // NETWORKMANAGER_H
