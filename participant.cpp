#include "participant.h"

Participant::Participant(QObject *parent) :
    QObject(parent),
    currentPart(Part::NOT_STARTED_YET)
{

}

Participant::~Participant()
{

}
