#ifndef PROGRESSBUTTON_H
#define PROGRESSBUTTON_H

#include <QPushButton>
#include "part.h"
#include "step.h"

class ProgressButton : public QPushButton
{
    Q_OBJECT
private:
    Part currentPart;
    Step currentStep;

public:
    ProgressButton(QWidget *parent = 0);

    void setCurrentPart(const Part &part);
    void setCurrentStep(const Step &step);
    void reset();

private slots:
    void onClicked(bool);

signals:
    void clicked(const Part &, const Step &);
};

#endif // PROGRESSBUTTON_H
