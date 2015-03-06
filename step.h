#ifndef STEP_H
#define STEP_H


class Step
{
private:
    /**
     * @brief stepOrder
     * Steps are ordered based on this identifier. The app will compare a #Step to
     * the user's current #Step based on the #stepOrder of the user. You can reorder
     * the default steps if you want, but must ensure they map the order encoded
     * in your corresponding Web server.
     */
    short stepOrder;

public:
    Step(short stepOrder);
    ~Step();

    bool operator <=(Step const& b);
    bool operator >=(Step const& b);
    bool operator ==(Step const& b);
    bool operator !=(Step const& b);
    bool operator <(Step const& b);
    bool operator >(Step const& b);

    static const Step WAITING_ENROLLMENT;
    static const Step CONSENT;
    static const Step BRIEFING;
    static const Step START;
    static const Step PRIMARY_TASK;
    static const Step RUNNING;
    static const Step UPLOAD;
    static const Step DEBRIEFING;
    static const Step DONE;
    static const Step INVALID;
};

#endif // STEP_H
