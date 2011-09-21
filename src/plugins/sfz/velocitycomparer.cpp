#include <cassert>

#include "velocitycomparer.h"

VelocityComparer::VelocityComparer(QObject *parent):
    QObject(parent)
{
    // Empty
}

VelocityComparer::VelocityComparer(const VelocityComparer &/*comparer*/,
                                   QObject *parent):
    QObject(parent)
{
    // Empty
}

VelocityComparer::~VelocityComparer()
{
    // Empty
}

bool
VelocityComparer::operator()(const synthclone::Zone *zone1,
                             const synthclone::Zone *zone2) const
{
    assert(zone1);
    assert(zone2);
    return zone1->getVelocity() < zone2->getVelocity();
}
