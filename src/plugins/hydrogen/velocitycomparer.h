#ifndef __VELOCITYCOMPARER_H__
#define __VELOCITYCOMPARER_H__

#include <synthclone/zone.h>

class VelocityComparer: public QObject {

    Q_OBJECT

public:

    explicit
    VelocityComparer(QObject *parent=0);

    VelocityComparer(const VelocityComparer &comparer, QObject *parent=0);

    ~VelocityComparer();

    bool
    operator()(const synthclone::Zone *zone1,
               const synthclone::Zone *zone2) const;

};

#endif
