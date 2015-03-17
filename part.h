#ifndef PART_H
#define PART_H

#include <QString>

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
     * @brief maxPartId
     * Identifies the maximum number of parts in the study. Set to 1 by
     * default, but can be changed by the application.
     */
    static short maxPartId;

 public:
    Part(short partId);
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

    static void setMaxPart(short maxPart);

    static const Part NOT_STARTED_YET;
    static const Part DONE;
    static const Part INVALID;
};

#endif // PART_H
