#ifndef __ZONECOMPARER_H__
#define __ZONECOMPARER_H__

#include <synthclone/sample.h>
#include <synthclone/zonecomparer.h>

#include "zonetablecolumn.h"

class ZoneComparer: public synthclone::ZoneComparer {

    Q_OBJECT

public:

    enum Property {
        PROPERTY_AFTERTOUCH = 0,
        PROPERTY_CHANNEL = 1,
        PROPERTY_DRY_SAMPLE = 2,
        PROPERTY_NOTE = 3,
        PROPERTY_RELEASE_TIME = 4,
        PROPERTY_SAMPLE_TIME = 5,
        PROPERTY_STATUS = 6,
        PROPERTY_VELOCITY = 7,
        PROPERTY_WET_SAMPLE = 8,
        PROPERTY_CONTROL_0 = 9,
        PROPERTY_CONTROL_127 = 136
    };

    ZoneComparer(int property, QObject *parent=0);

    ~ZoneComparer();

    bool
    isLessThan(const synthclone::Zone *zone1,
               const synthclone::Zone *zone2) const;

private:

    bool
    isLessThan(synthclone::MIDIData n1, synthclone::MIDIData n2) const;

    bool
    isLessThan(const synthclone::Sample *s1,
               const synthclone::Sample *s2) const;

    int property;

};

#endif
