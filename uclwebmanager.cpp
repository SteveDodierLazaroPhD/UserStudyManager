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
#include <QProcess>
#include <QStandardPaths>

using namespace std;

UCLWebManager::UCLWebManager(QWidget *parent) :
    QWebView(parent),
    manager(new UCLUploadManager())
{
    page()->setNetworkAccessManager(manager);
    connect(manager, SIGNAL(onUploadFormSubmitted()), this, SIGNAL(uploadStarted()));
    connect(this, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));
    connect(this, SIGNAL(unsupportedStepQueried(Part,Step)), this, SLOT(onUnsupportedStepQueried()));
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

    delete manager;
}

void UCLWebManager::setUrl(const QUrl &url)
{
    cout << "Setting a QUrl: " << qPrintable(url.toDisplayString()) << endl;
    QWebView::setUrl(url);
}

void UCLWebManager::load(const QUrl &url)
{
    cout << "Loading a QUrl: " << qPrintable(url.toDisplayString()) << endl;
    QWebView::load(url);
}

void UCLWebManager::load(const QNetworkRequest &request,
                    QNetworkAccessManager::Operation operation,
                    const QByteArray &body)
{
    cout << "Loading a network request: " << qPrintable(request.url().toDisplayString()) << endl;
    cout << "test cpx" << endl;
    QWebView::load(request, operation, body);
}

//QList<QUrl> UCLWebManager::getCurrentPageInternalLinks() const
//{
//    QUrl baseUrl = page()->mainFrame()->baseUrl();
//    QList<QUrl> list;

//    QWebElementCollection collection = page()->mainFrame()->findAllElements("a");
//    cout << "Entering a href " << collection.count() << endl;
//    foreach (QWebElement element, collection)
//    {
//        QString href = element.attribute("href");
//        if (!href.isEmpty())
//        {
//            QUrl absoluteUrl = baseUrl.resolved(QUrl(href));
//            if (baseUrl.isParentOf(absoluteUrl))
//                list.append(absoluteUrl);

//            cout << qPrintable(absoluteUrl.toString()) << endl;
//        }
//    }

//    cout << "Outcome: " << list.length() << " internal URLs" << endl;


//    return list;
//}

//QList<QUrl> UCLWebManager::getCurrentPageExternalLinks() const
//{
//    QUrl baseUrl = page()->mainFrame()->baseUrl();
//    QList<QUrl> list;

//    QWebElementCollection collection = page()->mainFrame()->findAllElements("a");
//    cout << "Entering external links with " << collection.count() << " candidates" << endl;
//    foreach (QWebElement element, collection)
//    {
//        QString href = element.attribute("href");
//        if (!href.isEmpty())
//        {
//            QUrl absoluteUrl = baseUrl.resolved(QUrl(href));
//            if (!baseUrl.isParentOf(absoluteUrl))
//                list.append(absoluteUrl);

//            cout << qPrintable(absoluteUrl.toString()) << endl;
//        }
//    }

//    cout << "Outcome: " << list.length() << " external URLs" << endl;

//    return list;
//}

//QList<QUrl> UCLWebManager::getCurrentPageSubmits() const
//{
//    QUrl baseUrl = page()->mainFrame()->baseUrl();
//    QList<QUrl> list;

//    QWebElementCollection inputCollection= page()->mainFrame()->findAllElements("button");
//    cout << "Entering input submit " << inputCollection.count() << endl;
//    foreach (QWebElement element, inputCollection)
//    {
//        QString href = element.attribute("type");
//        if (!href.isEmpty() && href.contains("submit", Qt::CaseInsensitive))
//        {
//            QUrl relativeUrl(href);

//            QUrl absoluteUrl = baseUrl.resolved(relativeUrl);

//            cout << "Submit:" << qPrintable(absoluteUrl.toString()) << endl;

//            // Process
//        }
//    }

//    cout << "Finished" << endl;
//    return list;
//}


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

void UCLWebManager::showPageForStep(const Participant *p)
{
    if (!p)
        return;

    const Part &part = p->getPart();
    const Step &step = p->getStep();
    if (step == Step::WAITING_ENROLLMENT)
    {
        emit unsupportedStepQueried(part, step);
    }
    else if (step == Step::CONSENT)
    {
        emit unsupportedStepQueried(part, step);
    }
    else if (step == Step::BRIEFING)
    {
        emit unsupportedStepQueried(part, step);
    }
    else if (step == Step::INSTALL)
    {

    }
    else if (step == Step::PRIMARY_TASK)
    {
        emit unsupportedStepQueried(part, step);
    }
    else if (step == Step::RUNNING)
    {

    }
    else if (step == Step::UPLOAD)
    {

    }
    else if (step == Step::JSON_UPLOAD)
    {
        emit unsupportedStepQueried(part, step);
    }
    else if (step == Step::DEBRIEFING)
    {
        emit unsupportedStepQueried(part, step);
    }
    else if (step == Step::DONE)
    {
        emit unsupportedStepQueried(part, step);
    }
    else
    {
        cerr << "Unsupported step: " << qPrintable(step.getName()) << endl;
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
    QString appId = StudyUtils::getUrlAppId(this->url());
    QString route = StudyUtils::getUrlRouteName(this->url());

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
        /* Contact page can legitimately be embedded within the app */
        else if (route == "contact")
        {
        }
        /* Other pages have not been adapted for in-app embedding, should not be displayed */
        else
        {
            cerr << "Unknown page loaded. App id: " << qPrintable(appId) << "\tRoute: " << qPrintable(route) << endl;
        }
    }
    /* Authenticated pages */
    else
    {
        /* The status page instructs us to display local information corresponding to the user's current part and step */
        if (route == "status")
        {
            this->onStatus();
        }
        else if (route == "upload")
        {
            //TODO connect the upload handler to clicks
        }
        else if (route == "information")
        {
        }
        else
        {
            cerr << "Unknown page loaded. App id: " << qPrintable(appId) << "\tRoute: " << qPrintable(route) << endl;
        }
    }


//    QList<QUrl> internals = getCurrentPageInternalLinks(); //TODO if url was on current page LINKS, then desktop
//    QList<QUrl> externals = getCurrentPageExternalLinks(); //TODO if url was on current page LINKS, then desktop
//    QList<QUrl> submits = getCurrentPageSubmits(); //TODO if url was on current page LINKS, then desktop
    // if on INPUTS, then not desktop but load()...
}

void UCLWebManager::onUnsupportedStepQueried()
{
}

void UCLWebManager::onLoggedIn()
{
    StudyUtils         *inst  = StudyUtils::getUtils();
    QString             str   = this->page()->mainFrame()->toPlainText();

    Participant *p = inst->getParticipant();
    if (p)
    {
        bool worked = p->updateFromJson(str);
        if (worked)
        {
            inst->loginFinalize(true);
            this->showPageForStep(p);
        }
        else
        {
            cerr << "An error occurred when parsing the server's response, could not login." << endl;
        }
    }
    else
    {
        cerr << "An error occurred when retrieving the local participant object, could not login." << endl;
    }
}

void UCLWebManager::onFailedLogin() const
{
    StudyUtils         *inst  = StudyUtils::getUtils();
    inst->loginFinalize(false);
}

void UCLWebManager::onStatus()
{
    StudyUtils         *inst  = StudyUtils::getUtils();
    QString             str   = this->page()->mainFrame()->toPlainText();

    Participant *p = inst->getParticipant();
    if (p)
    {
        bool worked = p->updateFromJson(str);
        if (worked)
        {
            this->showPageForStep(p);
        }
        else
        {
            cerr << "An error occurred when parsing the server's response, could not continue." << endl;
        }
    }
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

bool UCLWebManager::loadUploadPage()
{
    StudyUtils *inst = StudyUtils::getUtils();
    Participant *p = inst->getParticipant();

    if (p && p->isLoggedIn())
    {
        const QString &id = p->getIdentity();
        const Part &part = p->getPart();
        QString target(APP_BASE);
        target+= id+"/"+part.toString()+"/upload";
        onLinkClicked(target);
        return true;
    }
    else
        return false;
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
