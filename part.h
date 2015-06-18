#ifndef PART_H
#define PART_H

#include <QString>
#include <QList>
#include <QHash>
#include "step.h"

class Part
{
private:
    /**
     * @brief id
     * The identifier of this #Part. Users navigate through study parts
     * in a sequential order. Special parts are defined for participants
     * who have not yet started or who have finished the study.
     */
    short id;

    /**
     * @brief steps
     * The list of steps for this part of the study.
     */
    QList<Step> steps;

    /**
     * @brief maxPartId
     * Identifies the maximum number of parts in the study. Set to 1 by
     * default, but can be changed by the application.
     */
    static short maxPartId;

 public:
    Part(short partId = -2);
    Part(const Part &other);
    ~Part();

    bool isValid() const;
    int getId() const;
    QString toString() const;
    static Part fromString(const QString &string);

    bool operator <=(Part const& b) const;
    bool operator >=(Part const& b) const;
    bool operator ==(Part const& b) const;
    bool operator !=(Part const& b) const;
    bool operator <(Part const& b) const;
    bool operator >(Part const& b) const;

    static void setMaxPart(short maxPart);

    void registerSteps(const QList<Step> &s);
    const QList<Step> &getSteps() const;

    bool hasReached(const Step &step, const Step &threshold) const;
    bool isBeyond(const Step &step, const Step &threshold) const;

    static const Part NOT_STARTED_YET;
    static const Part DONE;
    static const Part INVALID;
};

#endif // PART_H
