/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include "Model/part.h"
#include "Model/step.h"
#include <QObject>
#include <QString>

class Participant : public QObject
{
    Q_OBJECT

    friend class StudyUtils;

    QString username;
    QString email;
    bool loggedIn;
    Part currentPart;
    Step currentStep;

public:
    explicit Participant(QObject *parent = 0);
    ~Participant();

    bool updateFromJson(const QString &str);

    bool isLoggedIn() const;
    const QString &getUsername() const;
    const QString &getEmail() const;
    const Part &getPart() const;
    const Step &getStep() const;


protected:
    void login(const QString &username, const QString &email);
    void logout();

signals:

public slots:
};

#endif // PARTICIPANT_H
