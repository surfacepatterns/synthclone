#include <cassert>

#include "samplerjob.h"

SamplerJob::SamplerJob(Type type, Zone *zone, QObject *parent):
    synthclone::SamplerJob(parent)
{
    assert(zone);
    this->type = type;
    this->zone = zone;
}

SamplerJob::~SamplerJob()
{
    // Empty
}

synthclone::SamplerJob::Type
SamplerJob::getType() const
{
    return type;
}

Zone *
SamplerJob::getZone()
{
    return zone;
}

const synthclone::Zone *
SamplerJob::getZone() const
{
    return zone;
}
