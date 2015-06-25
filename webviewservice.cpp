#include "webviewservice.h"
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

URLParts WebViewService::parseUrl(const QUrl &url)
{
    URLParts parts = { QString(), Part::INVALID, Step::INVALID };

    QString str(url.toString());
    str.remove(APP_BASE);

    /* Not from our actual website */
    if (str.startsWith("http") || str.startsWith("ftp"))
    {
        parts.route = QString();
    }
    /* Index page */
    else if (str.isEmpty())
    {
        parts.route = QString("status");
    }
    /* General case: parsing the url */
    else
    {
        QStringList    bits = str.split("/");

        /* Authenticated space */
        int partIndex = 0;
        parts.part = Part::fromString(bits.at(partIndex));

        if (parts.part.isValid())
        {
            ++partIndex;
            if (bits.length() <= partIndex)
            {
                parts.route = QString("status");
                return parts;
            }
        }

        parts.route = bits.at(partIndex);
    }

    parts.step = Step::fromName(parts.route);
    return parts;
}

WebViewService::WebViewService(QWidget *parent) :
    QWebView(parent)
{
    connect(this, SIGNAL(linkClicked(QUrl)), this, SLOT(onHtmlLinkClicked(QUrl)));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));
    connect(this, SIGNAL(unsupportedStepQueried(Part,Step)), this, SLOT(onUnsupportedStepQueried()));
    loadLoginPage();
}

WebViewService::~WebViewService()
{
    disconnect (this, SIGNAL(urlChanged(QUrl)), 0, 0);
    disconnect (this, SIGNAL(linkClicked(QUrl)), 0, 0);
    disconnect (this, SIGNAL(loadStarted()), 0, 0);
    disconnect (this, SIGNAL(loadProgress(int)), 0, 0);
    disconnect (this, SIGNAL(loadFinished(bool)), 0, 0);
}

void WebViewService::setUrl(const QUrl &url)
{
    QWebView::setUrl(url);
}

void WebViewService::load(const QUrl &url)
{
    cout << "Loading a QUrl: " << qPrintable(url.toDisplayString()) << endl;
    QWebView::load(url);
}

void WebViewService::load(const QNetworkRequest &request,
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


void WebViewService::onHtmlLinkClicked(const QUrl &url)
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

void WebViewService::showPageForStep(const Participant *p)
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

void WebViewService::onPageLoaded(const bool success)
{
    /* Route to correct handler */
    URLParts parts = parseUrl(url());
    //qDebug() << "Parsed: " << qPrintable(parts.route) << " + " << qPrintable(parts.part.toString()) << " + " << qPrintable(parts.step.getName()) << endl;

    if (!success)
    {
        qDebug() << "Failed to load page '" << qPrintable(this->url().toDisplayString()) << "'." << endl;
        return;
    }

    this->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    /* Authentication failure or needs to auth - AuthController::appLoginAction() */
    if (parts.route == "login")
    {
        this->onLoginFormShown();
    }
    /* Authentication worked - AppPartController::loggedInAction() */
    else if (parts.route == "logged_in")
    {
        this->onLoggedIn();
    }
    /* AppPartController::statusAction() */
    else if (parts.route == "status")
    {
        this->onStatus();
    }


    /* AppPartController::showStatusAction() */
    else if (parts.route == "showstatus") { /* nothing to do */ }
    /* AppPartController::uploadAction() */
    else if (parts.route == "upload") { /* nothing to do */ }
    /* AppPartController::contactAction() */
    else if (parts.route == "contact") { /* nothing to do */ }
    /* AppPartController::informationAction() */
    else if (parts.route == "information") { /* nothing to do */ }


    /* AppPartController::uploadingAction() */
    else if (parts.route == "uploading")
    {
        //TODO
    }
    /* AppPartController::uploadResetAction() */
    else if (parts.route == "uploadreset")
    {
        //TODO
    }
    /* AppPartController::uploadDirectAction() */
    else if (parts.route == "upload_direct")
    {
        emit unsupportedServerAPIQueried(parts.route, "The study server is asking the application to use an uploading method that is not supported. This is a bug, please inform the researchers so they can fix it.");
    }
    /* AppPartController::reportProgressAction() */
    else if (parts.route == "reportprogress")
    {
        QString content = this->page()->mainFrame()->toPlainText();
        emit reportProgressRequested(content);
    }
    /* AppPartController::installAction() */
    else if (parts.route == "install")
    {
        this->onInstall(parts);
    }
    /* Other pages have not been adapted for in-app embedding, should not be displayed */
    else
    {
        cerr << "Unknown page '"<< qPrintable(this->url().toDisplayString()) <<"' loaded.\tRoute: " << qPrintable(parts.route) << endl;
        //TODO actually manage the errors
    }

    /* Manage all jResponse errors */
    /* AppPartController::installAction(part) -> jResponse('"InstallRegistered":"Success"') */
}

void WebViewService::onUnsupportedStepQueried()
{
}

void WebViewService::onLoggedIn()
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
            cerr << "An error occurred when parsing the server's response, could not login." << endl;
    }
    else
        cerr << "An error occurred when retrieving the local participant object, could not login." << endl;
}

void WebViewService::onLoginFormShown() const
{
    StudyUtils *inst = StudyUtils::getUtils();
    QSettings &settings = inst->getUserSettings();
    QWebFrame *frame = this->page()->mainFrame();

    if (frame)
    {
        QWebElement email= frame->findFirstElement("#login_username");
        email.setAttribute("value", settings.value("lastEmail", "").toString());

        QWebElement error = frame->findFirstElement(".error");
        if (!error.isNull())
        {
            // There was an error on the login form, let's remove the email address from the settings
            settings.remove("lastEmail");
        }
    }

    inst->loginFinalize(false);
}

void WebViewService::onStatus()
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

void WebViewService::onInstall(const URLParts &parts)
{
    // Inform the client to initialise the starting date of the study part
    StudyUtils *utils = StudyUtils::getUtils();
    utils->registerInstall(parts.part);

    // We don't need to do anything since simply visiting the "install" URL is enough to register the app
    loadStatusPage();
}

bool WebViewService::loadLoginPage()
{
    onHtmlLinkClicked(QString(APP_BASE) + "login");
    return true;
}
bool WebViewService::loadStatusPage()
{
    onHtmlLinkClicked(QString(APP_BASE) + "status");
    return true;
}

bool WebViewService::loadInfoPage()
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

bool WebViewService::loadUploadPage()
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

bool WebViewService::loadUploadPage(const Part &part, const Step &)
{
    QString target(APP_BASE);
    target+= part.toString()+"/upload";
    onHtmlLinkClicked(target);
    return true;
}

bool WebViewService::loadContactPage()
{
    onHtmlLinkClicked(QString(APP_BASE) + "contact");
    return true;
}

bool WebViewService::loadInstallPage(const Participant *&p)
{
    onHtmlLinkClicked(QString(APP_BASE) + p->getPart().toString() + "/install");
    return true;
}

bool WebViewService::loadShowStatusPage()
{
    onHtmlLinkClicked(QString(APP_BASE) + "showstatus");
    return true;
}

bool WebViewService::loadReportProgressPage()
{
    StudyUtils *inst = StudyUtils::getUtils();
    Participant *p = inst->getParticipant();
    onHtmlLinkClicked(QString(APP_BASE) + p->getPart().toString() + "/reportprogress");
    return true;
}

void WebViewService::openDesktopUrl(const QUrl &url)
{
  QDesktopServices::openUrl(url);
}
