#ifndef __SAMPLERJOB_H__
#define __SAMPLERJOB_H__

#include <synthclone/samplerjob.h>

#include "zone.h"

class SamplerJob: public synthclone::SamplerJob {

    Q_OBJECT

public:

    SamplerJob(Type type, Zone *zone, QObject *parent=0);

    ~SamplerJob();

    Type
    getType() const;

    Zone *
    getZone();

    const synthclone::Zone *
    getZone() const;

private:

    Type type;
    Zone *zone;

};

#endif
