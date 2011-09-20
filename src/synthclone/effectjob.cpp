#include <cassert>

#include "effectjob.h"

EffectJob::EffectJob(Zone *zone, QObject *parent):
    synthclone::EffectJob(parent)
{
    assert(zone);
    this->zone = zone;
}

EffectJob::~EffectJob()
{
    // Empty
}

Zone *
EffectJob::getZone()
{
    return zone;
}

const synthclone::Zone *
EffectJob::getZone() const
{
    return zone;
}
