#include "part.h"

short Part::maxPartId = 1;

const Part Part::NOT_STARTED_YET(0);
const Part Part::DONE(-1);
const Part Part::INVALID(-2);

Part::Part(short partId)
    : id(partId)
{
}

Part::~Part()
{

}

bool Part::operator <=(Part const& b)
{
    return this->id <= b.id;
}
bool Part::operator >=(Part const& b)
{
    return this->id >= b.id;
}
bool Part::operator ==(Part const& b)
{
    return this->id == b.id;
}
bool Part::operator !=(Part const& b)
{
    return this->id != b.id;
}
bool Part::operator <(Part const& b)
{
    return this->id < b.id;
}
bool Part::operator >(Part const& b)
{
    return this->id > b.id;
}

bool Part::isValid() const
{
    return this != &NOT_STARTED_YET
            && this != &DONE
            && this >= &INVALID
            && this->id <= Part::maxPartId;
}

int Part::i() const
{
    return id;
}

void Part::setMaxPart(short maxPart)
{
    Part::maxPartId = maxPart;
}
