#ifndef __ZONELISTLOADER_H__
#define __ZONELISTLOADER_H__

#include "session.h"

class ZoneListLoader: public QObject {

    Q_OBJECT

public:

    ZoneListLoader(Session &session, QObject *parent=0);

    ~ZoneListLoader();

    void
    loadZones(QDomElement &element, int startIndex=-1,
              const QDir *sampleDirectory=0);

signals:

    void
    loadingZone(int current, int total);

    void
    warning(int line, int column, const QString &message);

private:

    void
    emitWarning(const QDomElement &element, const QString &message);

    bool
    verifyBooleanAttribute(const QDomElement &element, const QString &name,
                           bool defaultValue);

    bool
    verifyMIDIAttribute(const QDomElement &element, const QString &name,
                        synthclone::MIDIData &value, bool required=true,
                        synthclone::MIDIData minimumValue=0,
                        synthclone::MIDIData maximumValue=0x7f);

    synthclone::Sample *
    verifySampleAttribute(const QDomElement &element, const QString &name,
                          const QDir *directory=0, QObject *parent=0);

    bool
    verifySampleTimeAttribute(const QDomElement &element, const QString &name,
                              synthclone::SampleTime &sampleTime);

    Session &session;

};

#endif
