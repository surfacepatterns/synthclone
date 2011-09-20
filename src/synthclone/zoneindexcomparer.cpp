#include "zoneindexcomparer.h"

ZoneIndexComparer::ZoneIndexComparer(QList<synthclone::Zone *> &zones,
                                     QObject *parent):
    synthclone::ZoneComparer(parent),
    zones(zones)
{
    // Empty
}

ZoneIndexComparer::~ZoneIndexComparer()
{
    // Empty
}

bool
ZoneIndexComparer::isLessThan(const synthclone::Zone *zone1,
                              const synthclone::Zone *zone2) const
{
    return zones.indexOf(const_cast<synthclone::Zone *>(zone1)) <
        zones.indexOf(const_cast<synthclone::Zone *>(zone2));
}
