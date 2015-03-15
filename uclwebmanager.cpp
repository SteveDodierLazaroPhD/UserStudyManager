#include "uclwebmanager.h"
#include "study.h"
#include "participant.h"
#include <iostream>
#include <QDesktopServices>
#include <QWebFrame>
#include <QWebPage>
#include <QWebElementCollection>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

using namespace std;

UCLWebManager::UCLWebManager(QWidget *parent) :
    QWebView(parent)
{
    connect(this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChange(QUrl)));
    connect(this, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));
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

QList<QUrl> UCLWebManager::getCurrentPageInternalLinks() const
{
    QUrl baseUrl = page()->mainFrame()->baseUrl();
    QList<QUrl> list;

    QWebElementCollection collection = page()->mainFrame()->findAllElements("a");
    cout << "Entering a href " << collection.count() << endl;
    foreach (QWebElement element, collection)
    {
        QString href = element.attribute("href");
        if (!href.isEmpty())
        {
            QUrl absoluteUrl = baseUrl.resolved(QUrl(href));
            if (baseUrl.isParentOf(absoluteUrl))
                list.append(absoluteUrl);

            cout << qPrintable(absoluteUrl.toString()) << endl;
        }
    }

    cout << "Outcome: " << list.length() << " internal URLs" << endl;


    return list;
}

QList<QUrl> UCLWebManager::getCurrentPageExternalLinks() const
{
    QUrl baseUrl = page()->mainFrame()->baseUrl();
    QList<QUrl> list;

    QWebElementCollection collection = page()->mainFrame()->findAllElements("a");
    cout << "Entering external links with " << collection.count() << " candidates" << endl;
    foreach (QWebElement element, collection)
    {
        QString href = element.attribute("href");
        if (!href.isEmpty())
        {
            QUrl absoluteUrl = baseUrl.resolved(QUrl(href));
            if (!baseUrl.isParentOf(absoluteUrl))
                list.append(absoluteUrl);

            cout << qPrintable(absoluteUrl.toString()) << endl;
        }
    }

    cout << "Outcome: " << list.length() << " external URLs" << endl;

    return list;
}

QList<QUrl> UCLWebManager::getCurrentPageSubmits() const
{
    QUrl baseUrl = page()->mainFrame()->baseUrl();
    QList<QUrl> list;

    QWebElementCollection inputCollection= page()->mainFrame()->findAllElements("input");
    cout << "Entering input submit " << inputCollection.count() << endl;
    foreach (QWebElement element, inputCollection)
    {
        QString href = element.attribute("type");
        if (!href.isEmpty() && href.contains("submit", Qt::CaseInsensitive))
        {
            QUrl relativeUrl(href);

            QUrl absoluteUrl = baseUrl.resolved(relativeUrl);

            cout << "Submit:" << qPrintable(absoluteUrl.toString()) << endl;

            // Process
        }
    }

    cout << "Finished" << endl;
    return list;
}

QString UCLWebManager::getUrlAppId(const QUrl &url)
{
    QString str(url.toString());
    str.remove(APP_BASE);

    /* Not from our actual website */
    if (str.startsWith("http"))
        return QString();

    /* Index page */
    if (str.isEmpty())
        return QString("next");

    QStringList    bits = str.split("/");

    /* Authenticated space */
    if (StudyUtils::isAppId(bits.at(0)))
    {
        return bits.at(0);
    }
    else
    {
        return QString();
    }
}

QString UCLWebManager::getUrlRouteName(const QUrl &url)
{
    QString str(url.toString());
    str.remove(APP_BASE);

    /* Not from our actual website */
    if (str.startsWith("http"))
        return QString();

    /* Index page */
    if (str.isEmpty())
        return QString("next");

    QStringList    bits = str.split("/");

    /* Authenticated space */
    if (StudyUtils::isAppId(bits.at(0)))
    {
        if (bits.length() > 1)
            return bits.at(1);
        else
            return QString("next");
    }
    else
    {
        return bits.at(0);
    }
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
    QUrl base(APP_BASE);
    if (base.isParentOf(url))
    {
        cout << "Internal URI clicked: " << qPrintable(url.toDisplayString()) << endl;
        this->setUrl(url);
    }
    else
    {
        cout << "External URI clicked: " << qPrintable(url.toDisplayString()) << endl;
        this->openDesktopUrl(url);
    }

}

void UCLWebManager::onPageLoaded(const bool success)
{
//    StudyUtils *inst = StudyUtils::getUtils();

    if (!success)
    {
        qDebug() << "Failed to load page";
        return;
    }

    /* General page setup */
    this->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    /* Route to correct handler */
    QString appId = this->getUrlAppId(this->url());
    QString route = this->getUrlRouteName(this->url());

    /* Unauthenticated pages */
    if (appId.isEmpty())
    {
        /* Authentication succeeded */
        if (route == "logged_in")
        {
            this->onLoggedIn();
        }
        /* Authentication failed, mark us logged out */
        else if (route == "login")
        {
            this->onFailedLogin();
        }
        else
        {
            cerr << "Unknown page loaded. App id: " << qPrintable(appId) << "\tRoute: " << qPrintable(route) << endl;
        }
    }
    /* Authenticated pages */
    else
    {
        //TODO updateParticipant
        {
            cerr << "Unknown page loaded. App id: " << qPrintable(appId) << "\tRoute: " << qPrintable(route) << endl;
        }
    }


//    QList<QUrl> internals = getCurrentPageInternalLinks(); //TODO if url was on current page LINKS, then desktop
//    QList<QUrl> externals = getCurrentPageExternalLinks(); //TODO if url was on current page LINKS, then desktop
//    QList<QUrl> submits = getCurrentPageSubmits(); //TODO if url was on current page LINKS, then desktop
    // if on INPUTS, then not desktop but load()...
}

void UCLWebManager::onLoggedIn()
{
    StudyUtils         *inst  = StudyUtils::getUtils();
    QString             str   = this->page()->mainFrame()->toPlainText();
//    QByteArray          raw   = str.toLocal8Bit();
//    QJsonParseError     error;
//    QJsonDocument       doc   = QJsonDocument::fromJson(raw, &error);

//    if (error.error != QJsonParseError::NoError)
//    {
//        cerr << "Could not parse parameters returned by server after logging in (source \"" << qPrintable(str) << "\"; error \""<< qPrintable(error.errorString()) <<"\")." << endl;
//        //TODO flush session, error occurred
//        return;
//    }
//    else
//    {
//        QJsonObject arr = doc.object()["LoggedIn"].toObject();
//        QString username = arr["Username"].toString();
//        QString email = arr["Email"].toString();

        Participant *p = inst->getParticipant();
        if (p)
        {
            bool worked = p->updateFromJson(str);
            if (worked)
            {
                inst->loginFinalize(true);
                this->loadContactPage(); //TODO CHANGE THIS
            }
            else
            {
                cerr << "An error occurred when parsing the server's response, could not login." << endl;
            }
        }

//    }


//TODO    inst->login(true);
}

void UCLWebManager::onFailedLogin() const
{
    StudyUtils         *inst  = StudyUtils::getUtils();
    inst->loginFinalize(false);
}

bool UCLWebManager::loadLoginPage()
{
    onLinkClicked(QString(APP_BASE) + "login");
    return true;
}

bool UCLWebManager::loadInfoPage()
{
    StudyUtils *inst = StudyUtils::getUtils();
    Participant *p = inst->getParticipant();

    if (p && p->isLoggedIn())
    {
        const QString &id = p->getIdentity();
        const Part &part = p->getPart();
        QString target(APP_BASE);
        target+= id+"/"+part.toString()+"/information";
        onLinkClicked(target);
    }
    else
    {
        openDesktopUrl(QString(WEB_BASE) + "information");
    }

    return true;
}

bool UCLWebManager::loadContactPage()
{
    onLinkClicked(QString(APP_BASE) + "contact");
    return true;
}

void UCLWebManager::openDesktopUrl(const QUrl &url)
{
  QDesktopServices::openUrl(url);
}

