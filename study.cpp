#include "study.h"
#include <iostream>
#include <QRegularExpression>

using namespace std;

StudyUtils *StudyUtils::instance = NULL;

StudyUtils::StudyUtils() :
    participant(NULL),
    loggedIn(false),
    maxPart(PART_COUNT)
{
    setMaxPart(PART_COUNT);
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

void StudyUtils::login(const QString &username, const QString &email, bool status)
{
    loggedIn = status;
    cout << "New login status: " << (loggedIn? "logged in" : "logged out") << endl;

    if (loggedIn)
    {
        if (participant)
            delete participant;
        participant = new Participant();
        participant->login(username, email);
        cout << "Logged in as " << qPrintable(username) << " (email " << qPrintable(email) << ")." << endl;
    }
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

bool StudyUtils::isAppId(const QString &string)
{
    QRegularExpression hexMatcher("^[0-9A-F]{64}$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = hexMatcher.match(string);
    return match.hasMatch();
}
