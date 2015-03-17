#include "step.h"
#include "study.h"

const Step Step::INVALID;
const Step Step::WAITING_ENROLLMENT("waiting_enrollment", "You must wait till you are enrolled in the study to proceed further.");
const Step Step::CONSENT("consent", "You must read the information sheet and consent form on the website before you can proceed further.");
const Step Step::BRIEFING("briefing", "You must meet the researchers for a briefing session before you can proceed further.");
const Step Step::INSTALL("install", "You must install the study software before you can proceed further.");
const Step Step::PRIMARY_TASK("primary_task", "You must perform the task assigned to you before you can proceed further.");
const Step Step::RUNNING("running", "You must keep running the study software before you can proceed further.");
const Step Step::UPLOAD("upload", "You must upload collected data before you can proceed further.");
const Step Step::JSON_UPLOAD("json_upload", "You must upload collected data before you can proceed further.");
const Step Step::DEBRIEFING("debriefing", "You must meet the researchers for a debriefing session before you can proceed further.");
const Step Step::DONE("done", "This part of the study is completed.");

//const Step Step::WAITING_ENROLLMENT("waiting_enrollment", 0);
//const Step Step::CONSENT("consent", 1);
//const Step Step::BRIEFING("briefing", 10);
//const Step Step::INSTALL("install", 20);
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

Step::Step(const QString &name, const QString &mustDoLabel) :
    name(name),
    mustDoLabel(mustDoLabel)
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

bool Step::operator <=(Step const& other) const
{
    return !this->operator >(other);
}


bool Step::operator >=(Step const& other) const
{
    return !this->operator <(other);
}


bool Step::operator ==(Step const& other) const
{
    return this->name == other.name;
}


bool Step::operator !=(Step const& other) const
{
    return this->name != other.name;
}


bool Step::operator <(Step const& other) const
{
    StudyUtils *inst = StudyUtils::getUtils();
    const QList<Step> &so = inst->getStepOrder();
    return so.indexOf(*this) < so.indexOf(other);
}


bool Step::operator >(Step const& other) const
{
    StudyUtils *inst = StudyUtils::getUtils();
    const QList<Step> &so = inst->getStepOrder();
    return so.indexOf(*this) > so.indexOf(other);
}
