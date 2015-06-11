#include "webmanager.h"
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

WebManager::WebManager(QWidget *parent) :
    QWebView(parent)
{
    connect(this, SIGNAL(linkClicked(QUrl)), this, SLOT(onHtmlLinkClicked(QUrl)));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));
    connect(this, SIGNAL(unsupportedStepQueried(Part,Step)), this, SLOT(onUnsupportedStepQueried()));
    loadLoginPage();
}

WebManager::~WebManager()
{
    emit loadFinished(false); /* cleanup our clients */
    disconnect (this, SIGNAL(urlChanged(QUrl)), 0, 0);
    disconnect (this, SIGNAL(linkClicked(QUrl)), 0, 0);
    disconnect (this, SIGNAL(loadStarted()), 0, 0);
    disconnect (this, SIGNAL(loadProgress(int)), 0, 0);
    disconnect (this, SIGNAL(loadFinished(bool)), 0, 0);
}

void WebManager::setUrl(const QUrl &url)
{
    cout << "Setting a QUrl: " << qPrintable(url.toDisplayString()) << endl;
    QWebView::setUrl(url);
}

void WebManager::load(const QUrl &url)
{
    cout << "Loading a QUrl: " << qPrintable(url.toDisplayString()) << endl;
    QWebView::load(url);
}

void WebManager::load(const QNetworkRequest &request,
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


void WebManager::onHtmlLinkClicked(const QUrl &url)
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

void WebManager::showPageForStep(const Participant *p)
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
        loadInstallPage(p);
    }
    else if (step == Step::PRIMARY_TASK)
    {
        emit unsupportedStepQueried(part, step);
    }
    else if (step == Step::RUNNING)
    {
        loadShowStatusPage();
    }
    else if (step == Step::UPLOAD)
    {
        loadUploadPage();
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
        emit unsupportedServerAPIQueried(step.getName(), "The study server is asking the application to perform actions that the application does not understand. This is a bug, please inform the researchers so they can fix it.");
    }
}

void WebManager::onPageLoaded(const bool success)
{
    /* Route to correct handler */
    //TODO extract all other url parameters into a hashtable.
    QString route = StudyUtils::getUrlRouteName(this->url());

    if (!success)
    {
        qDebug() << "Failed to load page '" << qPrintable(this->url().toDisplayString()) << "'." << endl;
        return;
    }

    this->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);


    /* Authentication failure or needs to auth - AuthController::appLoginAction() */
    if (route == "login")
    {
        this->onFailedLogin();
        //TODO this->onLogin();
        //TODO retrieve the last email @ from QSettings and inject it!
    }
    /* Authentication worked - AppPartController::loggedInAction() */
    else if (route == "logged_in")
    {
        this->onLoggedIn();
    }
    /* AppPartController::statusAction() */
    else if (route == "status")
    {
        this->onStatus();
    }


    /* AppPartController::showStatusAction() */
    else if (route == "showstatus") { /* nothing to do */ }
    /* AppPartController::uploadAction() */
    else if (route == "upload") { /* nothing to do */ }
    /* AppPartController::contactAction() */
    else if (route == "contact") { /* nothing to do */ }
    /* AppPartController::informationAction() */
    else if (route == "information") { /* nothing to do */ }


    /* AppPartController::uploadingAction() */
    else if (route == "uploading")
    {
        //TODO
    }
    /* AppPartController::uploadResetAction() */
    else if (route == "uploadreset")
    {
        //TODO
    }
    /* AppPartController::uploadDirectAction() */
    else if (route == "upload_direct")
    {
        cerr << "Unsupported route: " << qPrintable(route) << endl;
        emit unsupportedServerAPIQueried(route, "The study server is asking the application to use an uploading method that is not supported. This is a bug, please inform the researchers so they can fix it.");
    }
    /* AppPartController::reportProgressAction() */
    else if (route == "reportprogress")
    {
        QString content = this->page()->mainFrame()->toPlainText();
        emit reportProgressRequested(content);
    }
    /* AppPartController::installAction() */
    else if (route == "install")
    {
        this->onInstall();
    }
    /* Other pages have not been adapted for in-app embedding, should not be displayed */
    else
    {
        cerr << "Unknown page '"<< qPrintable(this->url().toDisplayString()) <<"' loaded.\tRoute: " << qPrintable(route) << endl;
        //TODO actually manage the errors
    }

    /* Manage all jResponse errors */
    /* AppPartController::installAction(part) -> jResponse('"InstallRegistered":"Success"') */
}

void WebManager::onUnsupportedStepQueried()
{
}

void WebManager::onLoggedIn()
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

            //TODO store email @ in QSettings
        }
        else
            cerr << "An error occurred when parsing the server's response, could not login." << endl;
    }
    else
        cerr << "An error occurred when retrieving the local participant object, could not login." << endl;
}

void WebManager::onFailedLogin() const
{
    StudyUtils         *inst  = StudyUtils::getUtils();
    inst->loginFinalize(false);
}

void WebManager::onStatus()
{
    StudyUtils         *inst  = StudyUtils::getUtils();
    QString             str   = this->page()->mainFrame()->toPlainText();

    Participant *p = inst->getParticipant();
    if (p)
    {
        if (p->updateFromJson(str))
            this->showPageForStep(p);
        else
            cerr << "An error occurred when parsing the server's response, could not continue." << endl;
    }
}

void WebManager::onInstall()
{
    // We don't need to do anything since simply visiting the "install" URL is enough to register the app
    loadStatusPage();
}

bool WebManager::loadLoginPage()
{
    onHtmlLinkClicked(QString(APP_BASE) + "login");
    return true;
}
bool WebManager::loadStatusPage()
{
    onHtmlLinkClicked(QString(APP_BASE) + "status");
    return true;
}

bool WebManager::loadInfoPage()
{
    StudyUtils *inst = StudyUtils::getUtils();
    Participant *p = inst->getParticipant();

    if (p && p->isLoggedIn())
    {
        const Part &part = p->getPart();
        QString target(APP_BASE);
        target+= part.toString()+"/information";
        onHtmlLinkClicked(target);
    }
    else
    {
        openDesktopUrl(QString(WEB_BASE) + "information");
    }

    return true;
}

bool WebManager::loadUploadPage()
{
    StudyUtils *inst = StudyUtils::getUtils();
    Participant *p = inst->getParticipant();

    if (p && p->isLoggedIn())
    {
        const Part &part = p->getPart();
        QString target(APP_BASE);
        target+= part.toString()+"/upload";
        onHtmlLinkClicked(target);
        return true;
    }
    else
        return false;
}

bool WebManager::loadContactPage()
{
    onHtmlLinkClicked(QString(APP_BASE) + "contact");
    return true;
}

bool WebManager::loadInstallPage(const Participant *&p)
{
    onHtmlLinkClicked(QString(APP_BASE) + p->getPart().toString() + "/install");
    return true;
}

bool WebManager::loadShowStatusPage()
{
    onHtmlLinkClicked(QString(APP_BASE) + "showstatus");
    return true;
}

void WebManager::openDesktopUrl(const QUrl &url)
{
  QDesktopServices::openUrl(url);
}