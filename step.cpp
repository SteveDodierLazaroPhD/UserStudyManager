#include "step.h"
#include "study.h"

const Step Step::INVALID;
const Step Step::WAITING_ENROLLMENT("waiting_enrollment");
const Step Step::CONSENT("consent");
const Step Step::BRIEFING("briefing");
const Step Step::START("start");
const Step Step::PRIMARY_TASK("primary_task");
const Step Step::RUNNING("running");
const Step Step::UPLOAD("upload");
const Step Step::DEBRIEFING("debriefing");
const Step Step::DONE("done");

//const Step Step::WAITING_ENROLLMENT("waiting_enrollment", 0);
//const Step Step::CONSENT("consent", 1);
//const Step Step::BRIEFING("briefing", 10);
//const Step Step::START("start", 20);
//const Step Step::PRIMARY_TASK("primary_task", 30);
//const Step Step::RUNNING("running", 100);
//const Step Step::UPLOAD("upload", 200);
//const Step Step::DEBRIEFING("debriefing", 210);
//const Step Step::DONE("done", 220);
//const Step Step::INVALID(QString(), -1);


//Step::Step(const QString &name, short stepOrder) :
//    name(name),
//    stepOrder(stepOrder)
//{
//}

//Step::Step(short stepOrder) :
//    stepOrder(stepOrder)
//{
//    this->findNameFromStepOrder();
//}

Step::Step(const QString &name) :
    name(name)
{
//    this->findStepOrderFromName();
}

Step::~Step()
{
}

Step &Step::operator =(Step const& other)
{
    this->name = other.name;
    return *this;
}

bool Step::operator <=(Step const& other)
{
    return !this->operator >(other);
}


bool Step::operator >=(Step const& other)
{
    return !this->operator <(other);
}


bool Step::operator ==(Step const& other)
{
    return this->name == other.name;
}


bool Step::operator !=(Step const& other)
{
    return this->name != other.name;
}


bool Step::operator <(Step const& other)
{
    StudyUtils *inst = StudyUtils::getUtils();
    const QList<Step> &so = inst->getStepOrder();
    return so.indexOf(*this) < so.indexOf(other);
}


bool Step::operator >(Step const& other)
{
    StudyUtils *inst = StudyUtils::getUtils();
    const QList<Step> &so = inst->getStepOrder();
    return so.indexOf(*this) > so.indexOf(other);
}
