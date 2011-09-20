#ifndef __ZONECOMPARERPROXY_H__
#define __ZONECOMPARERPROXY_H__

#include <synthclone/zonecomparer.h>

class ZoneComparerProxy: public QObject {

    Q_OBJECT

public:

    ZoneComparerProxy(const ZoneComparerProxy &proxy, QObject *parent=0);

    explicit
    ZoneComparerProxy(const synthclone::ZoneComparer &comparer,
                      QObject *parent=0);

    ~ZoneComparerProxy();

    bool
    operator()(const synthclone::Zone *zone1,
               const synthclone::Zone *zone2) const;

private:

    const synthclone::ZoneComparer &comparer;

};

#endif
