#ifndef STEP_H
#define STEP_H

#include <QString>

class Step
{
protected:
    /**
     * @brief name Name of the #Step, used to communicate with the server
     */
    QString name;
//    /**
//     * @brief stepOrder
//     * Steps are ordered based on this identifier. The app will compare a #Step to
//     * the user's current #Step based on the #stepOrder of the user. You can reorder
//     * the default steps if you want, but must ensure they map the order encoded
//     * in your corresponding Web server.
//     */
//    short stepOrder;
    /**
     * @brief mustDoLabel Label to display when the Step must be completed before moving on (also used to indicate it's not managed by the app)
     */
    QString mustDoLabel;

public:
//    Step(const QString &name, short stepOrder);
//    Step(short stepOrder);
    Step(const QString &name = QString(), const QString &mustDoLabel="You must visit the study website to progress through the next step.");
    ~Step();

    static Step fromName(const QString &name);

    inline QString getName() const
        { return name; }
    inline QString getMustDoLabel() const
        { return mustDoLabel; }

    bool isValid() const;

    Step &operator = (Step const& other);

    bool operator <=(Step const& b) const;
    bool operator >=(Step const& b) const;
    bool operator ==(Step const& b) const;
    bool operator !=(Step const& b) const;
    bool operator <(Step const& b) const;
    bool operator >(Step const& b) const;

    static const Step INVALID;
    static const Step WAITING_ENROLLMENT;
    static const Step CONSENT;
    static const Step BRIEFING;
    static const Step INSTALL;
    static const Step PRIMARY_TASK;
    static const Step RUNNING;
    static const Step UPLOAD;
    static const Step JSON_UPLOAD;
    static const Step DEBRIEFING;
    static const Step DONE;

    static const Step DefinedSteps[];
};

#endif // STEP_H
