#include "part.h"
#include <QMetaType>
#include <iostream>

short Part::maxPartId = 0;

const Part Part::NOT_STARTED_YET(0);
const Part Part::DONE(-1);
const Part Part::INVALID(-2);

Part::Part(short partId):
    id(partId),
    steps()
{
    qRegisterMetaType<Part>("Part");
}

Part::Part(const Part &other) :
    id(other.id),
    steps(other.steps)
{
    qRegisterMetaType<Part>("Part");
}

Part::~Part()
{

}

bool Part::operator <=(Part const& b) const
{
    return this->id <= b.id;
}
bool Part::operator >=(Part const& b) const
{
    return this->id >= b.id;
}
bool Part::operator ==(Part const& b) const
{
    return this->id == b.id;
}
bool Part::operator !=(Part const& b) const
{
    return this->id != b.id;
}
bool Part::operator <(Part const& b) const
{
    return this->id < b.id;
}
bool Part::operator >(Part const& b) const
{
    return this->id > b.id;
}

bool Part::isValid() const
{
    return (this->operator !=(NOT_STARTED_YET)) && (this->operator !=(DONE)) && (this->operator >(INVALID)) && (this->id <= Part::maxPartId);
}

int Part::getId() const
{
    return id;
}

QString Part::toString() const
{
    return QString::number(id);
}

Part Part::fromString(const QString &string)
{
    bool ok = false;
    short id = string.toShort(&ok, 10);

    if (ok)
        return Part(id);
    else
        return Part::INVALID;
}

void Part::setMaxPart(short maxPart)
{
    Part::maxPartId = maxPart;
}

void Part::registerSteps(const QList<Step> &s)
{
    this->steps = s;
}

const QList<Step> &Part::getSteps() const
{
    return steps;
}

bool Part::hasReached(const Step &step, const Step &threshold) const
{
    return step.operator ==(threshold) || isBeyond(step, threshold);
}

bool Part::isBeyond(const Step &step, const Step &threshold) const
{
    std::cout << "This part has  " << steps.size() << " steps registered." << std::endl;
    QList<Step>::ConstIterator it = steps.constBegin();
    while (it != steps.constEnd())
    {
        if (step == *it)
            return false;
        else if (threshold == *it)
            return true;
    }

    return false;
}
