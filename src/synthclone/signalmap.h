#ifndef __SIGNALMAP_H__
#define __SIGNALMAP_H__

#include <QtCore/QMap>

#include "signalpair.h"

class SignalMap: public QMap<QLatin1String, const char *> {

public:

    SignalMap &
    operator<<(const SignalPair &pair);

};

#endif
