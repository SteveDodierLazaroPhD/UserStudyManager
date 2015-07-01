/*
 * 2015 © Steve Dodier-Lazaro <sidnioulz@gmail.com>
 * Under the GNU Affero GPL3 License
 */
#ifndef PROGRESSBUTTON_H
#define PROGRESSBUTTON_H

#include "Model/part.h"
#include "Model/step.h"
#include <QPushButton>

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
