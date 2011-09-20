#ifndef __ZONE_H__
#define __ZONE_H__

#include <QtCore/QDir>

#include <synthclone/zone.h>

#include "sessionsampledata.h"

class Zone: public synthclone::Zone {

    Q_OBJECT

public:

    explicit
    Zone(SessionSampleData &sessionSampleData, QObject *parent=0);

    ~Zone();

    synthclone::MIDIData
    getAftertouch() const;

    synthclone::MIDIData
    getChannel() const;

    const ControlMap &
    getControlMap() const;

    synthclone::MIDIData
    getControlValue(synthclone::MIDIData control) const;

    const synthclone::Sample *
    getDrySample() const;

    synthclone::MIDIData
    getNote() const;

    synthclone::SampleTime
    getReleaseTime() const;

    synthclone::SampleTime
    getSampleTime() const;

    Status
    getStatus() const;

    synthclone::MIDIData
    getVelocity() const;

    const synthclone::Sample *
    getWetSample() const;

    bool
    isDrySampleStale() const;

    bool
    isWetSampleStale() const;

public slots:

    void
    setAftertouch(synthclone::MIDIData aftertouch);

    void
    setChannel(synthclone::MIDIData channel);

    void
    setControlValue(synthclone::MIDIData control, synthclone::MIDIData value);

    void
    setDrySample(synthclone::Sample *sample);

    void
    setDrySample(synthclone::Sample *sample, bool copy);

    void
    setDrySampleStale(bool stale);

    void
    setNote(synthclone::MIDIData note);

    void
    setReleaseTime(synthclone::SampleTime releaseTime);

    void
    setSampleTime(synthclone::SampleTime sampleTime);

    void
    setStatus(Status status);

    void
    setVelocity(synthclone::MIDIData velocity);

    void
    setWetSample(synthclone::Sample *sample, bool copy=true);

    void
    setWetSampleStale();

    void
    setWetSampleStale(bool stale);

private slots:

    void
    handleSessionSampleDataChange();

private:

    synthclone::MIDIData aftertouch;
    synthclone::MIDIData channel;
    ControlMap controlMap;
    synthclone::Sample *drySample;
    bool drySampleStale;
    synthclone::MIDIData note;
    synthclone::SampleTime releaseTime;
    synthclone::SampleTime sampleTime;
    SessionSampleData &sessionSampleData;
    Status status;
    synthclone::MIDIData velocity;
    synthclone::Sample *wetSample;
    bool wetSampleStale;

};

#endif
