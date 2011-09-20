#ifndef __EFFECTJOB_H__
#define __EFFECTJOB_H__

#include <synthclone/effectjob.h>

#include "zone.h"

class EffectJob: public synthclone::EffectJob {

    Q_OBJECT

public:

    explicit
    EffectJob(Zone *zone, QObject *parent=0);

    ~EffectJob();

    Zone *
    getZone();

    const synthclone::Zone *
    getZone() const;

private:

    Zone *zone;

};

#endif
