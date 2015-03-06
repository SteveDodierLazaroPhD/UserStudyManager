#include "study.h"
#include <QCryptographicHash>
#include <iostream>

using namespace std;

StudyUtils *StudyUtils::instance = NULL;

StudyUtils::StudyUtils() :
    studyId(STUDY_ID),
    appId(QString()),
    loggedIn(false),
    maxPart(PART_COUNT)
{
    setMaxPart(PART_COUNT);
}

void StudyUtils::setMaxPart(const short part)
{
    maxPart = part;
    Part::setMaxPart(maxPart);
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

QString StudyUtils::setParticipantIdentity(const QString &email)
{
    //TODO compute hash of email
    QByteArray plaintext;
    plaintext.append(email);
    QString hashed(QCryptographicHash::hash(plaintext, QCryptographicHash::Sha256).toHex());

    appId = hashed;
    return hashed;
}
