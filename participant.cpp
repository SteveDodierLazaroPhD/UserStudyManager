#include "participant.h"
#include <QCryptographicHash>

Participant::Participant(QObject *parent) :
    QObject(parent),
    currentPart(Part::NOT_STARTED_YET),
    currentStep(Step::WAITING_ENROLLMENT),
    username("Anonymous"),
    email(),
    appId()
{
}

Participant::~Participant()
{
}


bool Participant::isLoggedIn() const
{
    return !this->appId.isEmpty();
}

const QString &Participant::getUsername() const
{
    return this->username;
}

const QString &Participant::getEmail() const
{
    return this->email;
}

const QString &Participant::getIdentity() const
{
    return this->appId;
}

const Part &Participant::getPart() const
{
    return this->currentPart;
}

const Step &Participant::getStep() const
{
    return this->currentStep;
}

void Participant::login(const QString &username, const QString &email)
{
    QByteArray plaintext;
    plaintext.append(email);
    QString hashed(QCryptographicHash::hash(plaintext, QCryptographicHash::Sha256).toHex());

    this->username = username;
    this->email = email;
    this->appId = hashed;

    this->updateStatus();
}

void Participant::updateStatus()
{
    //TODO network request
//    this->currentPart = ;
//    this->currentStep = ;
}
