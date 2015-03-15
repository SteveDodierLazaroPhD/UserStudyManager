#include "participant.h"
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <iostream>
#include <QDebug>

using namespace std;

Participant::Participant(QObject *parent) :
    QObject(parent),
    username("Anonymous"),
    email(),
    appId(),
    loggedIn(false),
    currentPart(Part::NOT_STARTED_YET),
    currentStep(Step::WAITING_ENROLLMENT)
{
}

Participant::~Participant()
{
}

bool Participant::updateFromJson(const QString &str)
{
    QByteArray          raw   = str.toLocal8Bit();
    QJsonParseError     error;
    QJsonDocument       doc   = QJsonDocument::fromJson(raw, &error);
    QJsonObject         obj,part;

    if (error.error != QJsonParseError::NoError)
    {
        cerr << "Could not update participant (source \"" << qPrintable(str) << "\"; error \""<< qPrintable(error.errorString()) <<"\")." << endl;
        return false;
    }
    else
    {
        //TODO parse participant
        part = doc.object()["Participant"].toObject();
        obj = part["LoggedIn"].toObject();
        if (!obj.isEmpty())
        {
            QString username = obj["Username"].toString();
            QString email = obj["Email"].toString();
            this->login(username, email);
        }
        obj = part["LoggedOut"].toObject();
        if (!obj.isEmpty())
        {
            this->logout();
        }
        obj = part["Status"].toObject();
        if (!obj.isEmpty())
        {
            int part = obj["Part"].toInt();
            QString step = obj["Step"].toString();
            this->currentPart = part;
            this->currentStep = step;
        }
    }

    qDebug() << str << endl
             << "Name: " << username << endl
             << "Email: " << email << endl
             << "Part: " << currentPart.toString() << endl
             << "Step: " << currentStep.getName() << endl;
    return true;
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
    if (loggedIn)
        return;

    QByteArray plaintext;
    plaintext.append(email);
    QString hashed(QCryptographicHash::hash(plaintext, QCryptographicHash::Sha256).toHex());

    this->username = username;
    this->email = email;
    this->appId = hashed;
    this->loggedIn = true;
}

void Participant::logout()
{
    this->appId.clear();
    this->loggedIn = false;
}
