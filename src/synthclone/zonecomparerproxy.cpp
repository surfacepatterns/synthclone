#include <cassert>

#include "zonecomparerproxy.h"

ZoneComparerProxy::ZoneComparerProxy(const ZoneComparerProxy &proxy,
                                     QObject *parent):
    QObject(parent),
    comparer(proxy.comparer)
{
    // Empty
}

ZoneComparerProxy::ZoneComparerProxy(const synthclone::ZoneComparer &comparer,
                                     QObject *parent):
    QObject(parent),
    comparer(comparer)
{
    // Empty
}

ZoneComparerProxy::~ZoneComparerProxy()
{
    // Empty
}

bool
ZoneComparerProxy::operator()(const synthclone::Zone *zone1,
                              const synthclone::Zone *zone2) const
{
    assert(zone1);
    assert(zone2);
    return comparer.isLessThan(zone1, zone2);
}
