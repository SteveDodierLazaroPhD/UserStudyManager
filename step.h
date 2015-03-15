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
    /**
     * @brief stepOrder
     * Steps are ordered based on this identifier. The app will compare a #Step to
     * the user's current #Step based on the #stepOrder of the user. You can reorder
     * the default steps if you want, but must ensure they map the order encoded
     * in your corresponding Web server.
     */
//    short stepOrder;

public:
//    Step(const QString &name, short stepOrder);
//    Step(short stepOrder);
    Step(const QString &name = QString());
    ~Step();

    inline QString getName() const
        { return name; }

    Step &operator = (Step const& other);

    bool operator <=(Step const& b);
    bool operator >=(Step const& b);
    bool operator ==(Step const& b);
    bool operator !=(Step const& b);
    bool operator <(Step const& b);
    bool operator >(Step const& b);

    static const Step INVALID;
    static const Step WAITING_ENROLLMENT;
    static const Step CONSENT;
    static const Step BRIEFING;
    static const Step START;
    static const Step PRIMARY_TASK;
    static const Step RUNNING;
    static const Step UPLOAD;
    static const Step DEBRIEFING;
    static const Step DONE;
};

#endif // STEP_H
