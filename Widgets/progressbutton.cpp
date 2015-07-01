/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#include "Widgets/progressbutton.h"

ProgressButton::ProgressButton(QWidget *parent) :
    QPushButton(parent),
    currentPart(Part::INVALID),
    currentStep(Step::INVALID)
{
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
}

void ProgressButton::setCurrentPart(const Part &part)
{
    currentPart = part;
}

void ProgressButton::setCurrentStep(const Step &step)
{
    currentStep = step;
}

void ProgressButton::reset()
{
    currentPart = Part::INVALID;
    currentStep = Step::INVALID;
}

void ProgressButton::onClicked(bool)
{
    if(currentPart.isValid() && currentStep.isValid())
        emit clicked(currentPart, currentStep);
}
