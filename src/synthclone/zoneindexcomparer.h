#ifndef __ZONEINDEXCOMPARER_H__
#define __ZONEINDEXCOMPARER_H__

#include <QtCore/QList>

#include <synthclone/zonecomparer.h>

class ZoneIndexComparer: public synthclone::ZoneComparer {

    Q_OBJECT

public:

    ZoneIndexComparer(QList<synthclone::Zone *> &zones, QObject *parent=0);

    ~ZoneIndexComparer();

    bool
    isLessThan(const synthclone::Zone *zone1,
               const synthclone::Zone *zone2) const;

private:

    QList<synthclone::Zone *> &zones;

};

#endif
