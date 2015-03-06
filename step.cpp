#include "step.h"

const Step Step::WAITING_ENROLLMENT(0);
const Step Step::CONSENT(1);
const Step Step::BRIEFING(10);
const Step Step::START(20);
const Step Step::PRIMARY_TASK(30);
const Step Step::RUNNING(100);
const Step Step::UPLOAD(200);
const Step Step::DEBRIEFING(210);
const Step Step::DONE(220);
const Step Step::INVALID(-1);

Step::Step(short stepOrder) :
    stepOrder(stepOrder)
{

}

Step::~Step()
{

}

bool Step::operator <=(Step const& other)
{
    return this->stepOrder <= other.stepOrder;
}


bool Step::operator >=(Step const& other)
{
    return this->stepOrder >= other.stepOrder;
}


bool Step::operator ==(Step const& other)
{
    return this->stepOrder == other.stepOrder;
}


bool Step::operator !=(Step const& other)
{
    return this->stepOrder != other.stepOrder;
}


bool Step::operator <(Step const& other)
{
    return this->stepOrder < other.stepOrder;
}


bool Step::operator >(Step const& other)
{
    return this->stepOrder > other.stepOrder;
}
