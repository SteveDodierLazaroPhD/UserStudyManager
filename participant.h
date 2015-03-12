#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <QObject>
#include <QString>
#include "part.h"
#include "step.h"

class Participant : public QObject
{
    Q_OBJECT

    friend class StudyUtils;

    QString username;
    QString email;
    QString appId;
    Part currentPart;
    Step currentStep;

public:
    explicit Participant(QObject *parent = 0);
    ~Participant();

    bool isLoggedIn() const;
    const QString &getUsername() const;
    const QString &getEmail() const;
    const QString &getIdentity() const;
    const Part &getPart() const;
    const Step &getStep() const;

    void updateStatus();

protected:
    void login(const QString &username, const QString &email);

signals:

public slots:
};

#endif // PARTICIPANT_H
