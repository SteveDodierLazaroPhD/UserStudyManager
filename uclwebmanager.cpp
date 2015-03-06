#include "uclwebmanager.h"
#include "study.h"
#include <iostream>
#include <QDesktopServices>

using namespace std;

UCLWebManager::UCLWebManager(QWidget *parent) :
    QWebView(parent)
{
    connect(this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChange(QUrl)));
    connect(this, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)));
    loadLoginPage();
}

UCLWebManager::~UCLWebManager()
{
    emit loadFinished(false); /* cleanup our clients */
    disconnect (this, SIGNAL(urlChanged(QUrl)), 0, 0);
    disconnect (this, SIGNAL(linkClicked(QUrl)), 0, 0);
    disconnect (this, SIGNAL(loadStarted()), 0, 0);
    disconnect (this, SIGNAL(loadProgress(int)), 0, 0);
    disconnect (this, SIGNAL(loadFinished(bool)), 0, 0);
}


void UCLWebManager::setUrl(const QUrl &url)
{
    cout << "TODO rebuild request entirely!" << endl;
    QWebView::setUrl(url);

}

void UCLWebManager::load(const QUrl &url)
{
    cout << "test" << endl;
    QWebView::load(url);
}

void UCLWebManager::load(const QNetworkRequest &request,
                    QNetworkAccessManager::Operation operation,
                    const QByteArray &body)
{

    cout << "test cpx" << endl;
    QWebView::load(request, operation, body);
}

void UCLWebManager::onUrlChange(const QUrl &url)
{
    disconnect (this, SIGNAL(urlChanged(QUrl)), 0, 0);
    cout << "Hello, the url has changed to:" << qPrintable(url.toDisplayString()) << endl;

    //QWebView::load(url);
    connect(this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChange(QUrl)));
}

void UCLWebManager::onLinkClicked(const QUrl &url)
{

    cout << "Hello, link clicked " << qPrintable(url.toDisplayString()) << endl;

}

void UCLWebManager::onLoggedIn(const bool success)
{
    StudyUtils *inst = StudyUtils::getUtils();

    inst->setLoggedIn(success);
}

bool UCLWebManager::loadLoginPage()
{
    setUrl(QString(URL_BASE) + "login");
    return true;
}


bool UCLWebManager::loadContactPage()
{
    setUrl(QString(URL_BASE) + "contact");

    return true;
}

void UCLWebManager::openDesktopUrl(const QUrl &url)
{
  QDesktopServices::openUrl(url);
}

