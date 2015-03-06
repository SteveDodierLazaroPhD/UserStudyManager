#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <QObject>
#include <QString>
#include "part.h"

class Participant : public QObject
{
    Q_OBJECT

    QString username;
    QString email;
    Part currentPart;
//    Step currentStep;

public:
    explicit Participant(QObject *parent = 0);
    explicit Participant(Part p, /* Step s, */QObject *parent = 0);
    ~Participant();

signals:

public slots:
};

#endif // PARTICIPANT_H
