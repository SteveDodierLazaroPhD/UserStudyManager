#ifndef PART_H
#define PART_H

#include <QString>
#include <QList>
#include <QHash>
#include <QSharedPointer>
#include "step.h"

class Part
{
    friend class StudyUtils;

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

    /**
     * @brief parts
     * Stores the study's parts once they're properly initialised by the
     * StudyUtils. Those parts are then copied by the static Part accessors.
     */
    static QHash<short, Part> parts;

protected:
    static void setMaxPart(short maxPart);
    static void registerPart(const Part &part);
    void registerSteps(const QList<Step> &s);

public:
    static Part fromId(const short &id);
    static Part fromString(const QString &string);

    Part(short partId = -2);
    Part(const Part &other);
    ~Part();

    bool isValid() const;
    int getId() const;
    QString toString() const;

    bool operator <=(Part const& b) const;
    bool operator >=(Part const& b) const;
    bool operator ==(Part const& b) const;
    bool operator !=(Part const& b) const;
    bool operator <(Part const& b) const;
    bool operator >(Part const& b) const;

    const QList<Step> &getSteps() const;

    bool hasReached(const Step &step, const Step &threshold) const;
    bool isBeyond(const Step &step, const Step &threshold) const;

    static const Part NOT_STARTED_YET;
    static const Part DONE;
    static const Part INVALID;
};

#endif // PART_H
