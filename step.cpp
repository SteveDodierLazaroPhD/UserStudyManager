#include "step.h"
#include "study.h"
#include <QMetaType>

//TODO replace "You must" strings by settingd
//TODO move StudyUtils::stepOrder in here

const Step Step::INVALID;
const Step Step::WAITING_ENROLLMENT("waiting_enrollment", "You must wait till you are enrolled in the study to proceed further.");
const Step Step::CONSENT("consent", "You must read the information sheet and consent form on the website before you can proceed further.");
const Step Step::BRIEFING("briefing", "You must meet the researchers for a briefing session before you can proceed further.");
const Step Step::INSTALL("install", "You must install the study software before you can proceed further.");
const Step Step::PRIMARY_TASK("primary_task", "You must perform the task assigned to you before you can proceed further.");
const Step Step::RUNNING("running", "You must use the study software for longer before you can proceed further.");
const Step Step::JSON_UPLOAD("json_upload", "You must upload collected data before you can proceed further.");
const Step Step::DEBRIEFING("debriefing", "You must meet the researchers for a debriefing session before you can proceed further.");
const Step Step::DONE("done", "This part of the study is completed.");

const Step Step::DefinedSteps[] = {
    Step::WAITING_ENROLLMENT,
    Step::CONSENT,
    Step::BRIEFING,
    Step::INSTALL,
    Step::PRIMARY_TASK,
    Step::RUNNING,
    Step::JSON_UPLOAD,
    Step::DEBRIEFING,
    Step::DONE,
    Step::INVALID
};

Step::Step(const QString &name, const QString &mustDoLabel) :
    name(name),
    mustDoLabel(mustDoLabel)
{
    qRegisterMetaType<Step>("Step");
}

Step::Step(const Step &other) :
    name(other.name),
    mustDoLabel(other.mustDoLabel)
{
    qRegisterMetaType<Step>("Step");
}

Step::~Step()
{
}

Step Step::fromName(const QString &name)
{
    int i=0;
    while (DefinedSteps[i] != INVALID) {
        if (DefinedSteps[i].getName() == name)
            return DefinedSteps[i];
        ++i;
    }

    return Step(INVALID);
}

bool Step::isValid() const
{
    return *this != Step::INVALID;
}

Step &Step::operator =(Step const& other)
{
    this->name = other.name;
    return *this;
}

bool Step::operator ==(Step const& other) const
{
    return this->name == other.name;
}


bool Step::operator !=(Step const& other) const
{
    return this->name != other.name;
}



