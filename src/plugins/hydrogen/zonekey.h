#ifndef __ZONEKEY_H__
#define __ZONEKEY_H__

#include <synthclone/zone.h>

class ZoneKey: public QObject {

    Q_OBJECT

public:

    explicit
    ZoneKey(const synthclone::Zone &zone, QObject *parent=0);

    ZoneKey(const ZoneKey &key, QObject *parent=0);

    ZoneKey &
    operator=(const ZoneKey &key);

    bool
    operator<(const ZoneKey &key) const;

private:

    synthclone::MIDIData aftertouch;
    synthclone::MIDIData channel;
    quint64 controlBits1;
    quint64 controlBits2;
    synthclone::MIDIData controlValues[0x80];
    synthclone::MIDIData note;

};

#endif
