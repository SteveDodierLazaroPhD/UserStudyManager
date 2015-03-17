#include "study.h"
#include <iostream>
#include <QRegularExpression>
#include <QUrl>

using namespace std;

StudyUtils *StudyUtils::instance = NULL;

StudyUtils::StudyUtils() :
    participant(NULL),
    loggedIn(false),
    maxPart(PART_COUNT),
    stepOrder()
{
    setMaxPart(PART_COUNT);
    stepOrder.append(Step::INVALID);
    stepOrder.append(Step::WAITING_ENROLLMENT);
    stepOrder.append(Step::CONSENT);
    stepOrder.append(Step::BRIEFING);
    stepOrder.append(Step::INSTALL);
    stepOrder.append(Step::PRIMARY_TASK);
    stepOrder.append(Step::RUNNING);
    stepOrder.append(Step::UPLOAD);
    stepOrder.append(Step::DEBRIEFING);
    stepOrder.append(Step::DONE);

    participant = new Participant();
}

StudyUtils::~StudyUtils()
{
    if (participant)
        delete participant;
}

void StudyUtils::setMaxPart(const short part)
{
    maxPart = part;
    Part::setMaxPart(maxPart);
}

void StudyUtils::loginFinalize(bool status)
{
    loggedIn = status;
    cout << "New login status: " << (loggedIn? "logged in" : "logged out") << endl;

    if (loggedIn)
    {
        cout << "Logged in as " << qPrintable(participant->getUsername()) << " (email " << qPrintable(participant->getEmail()) << ")." << endl;
    }

    emit onLoginStatusChanged(status);
}

StudyUtils * StudyUtils::getUtils()
{
    if (instance == NULL)
    {
        instance = new StudyUtils();
    }

    return instance;
}

void StudyUtils::clearUtils()
{
    if (instance)
    {
        delete instance;
        instance = NULL;
    }
}

bool StudyUtils::isPart(const QString &string)
{
    QRegularExpression intMatcher("^[0-9]+$");
    QRegularExpressionMatch match = intMatcher.match(string);
    return match.hasMatch();
}

QString StudyUtils::getUrlRouteName(const QUrl &url)
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
    int partIndex = 0;

    if (StudyUtils::isPart(bits.at(partIndex)))
    {
        ++partIndex;
        if (bits.length() <= partIndex)
            return QString("status");
    }

    return bits.at(partIndex);
}
