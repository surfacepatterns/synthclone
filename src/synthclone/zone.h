/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

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

    synthclone::MIDIData
    getChannelPressure() const;

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
    setChannelPressure(synthclone::MIDIData pressure);

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
    synthclone::MIDIData channelPressure;
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
