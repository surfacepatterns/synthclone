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

#include <cassert>

#include <synthclone/error.h>
#include <synthclone/util.h>

#include "util.h"
#include "zone.h"

Zone::Zone(SessionSampleData &sessionSampleData, QObject *parent):
    synthclone::Zone(parent),
    sessionSampleData(sessionSampleData)
{
    connect(&sessionSampleData,
            SIGNAL(sampleChannelCountChanged(synthclone::SampleChannelCount)),
            SLOT(handleSessionSampleDataChange()));
    connect(&sessionSampleData, SIGNAL(sampleDirectoryChanged(const QDir *)),
            SLOT(handleSessionSampleDataChange()));
    connect(&sessionSampleData,
            SIGNAL(sampleRateChanged(synthclone::SampleRate)),
            SLOT(handleSessionSampleDataChange()));

    aftertouch = synthclone::MIDI_VALUE_NOT_SET;
    channel = 1;
    channelPressure = synthclone::MIDI_VALUE_NOT_SET;
    drySample = 0;
    drySampleStale = true;
    note = 60;
    releaseTime = 1.0;
    sampleTime = 5.0;
    status = STATUS_NORMAL;
    velocity = 0x7f;
    wetSample = 0;
    wetSampleStale = true;
}

Zone::~Zone()
{
    if (drySample) {
        drySample->deleteLater();
    }
    if (wetSample) {
        wetSample->deleteLater();
    }
}

synthclone::MIDIData
Zone::getAftertouch() const
{
    return aftertouch;
}

synthclone::MIDIData
Zone::getChannelPressure() const
{
    return channelPressure;
}

synthclone::MIDIData
Zone::getChannel() const
{
    return channel;
}

const Zone::ControlMap &
Zone::getControlMap() const
{
    return controlMap;
}

synthclone::MIDIData
Zone::getControlValue(synthclone::MIDIData control) const
{
    CONFIRM(control < 0x80, tr("'%1': invalid control index").arg(control));

    ControlMap::const_iterator iter = controlMap.find(control);
    return iter != controlMap.end() ? iter.value() :
        synthclone::MIDI_VALUE_NOT_SET;
}

const synthclone::Sample *
Zone::getDrySample() const
{
    return drySample;
}

synthclone::MIDIData
Zone::getNote() const
{
    return note;
}

synthclone::SampleTime
Zone::getReleaseTime() const
{
    return releaseTime;
}

synthclone::SampleTime
Zone::getSampleTime() const
{
    return sampleTime;
}

Zone::Status
Zone::getStatus() const
{
    return status;
}

synthclone::MIDIData
Zone::getVelocity() const
{
    return velocity;
}

const synthclone::Sample *
Zone::getWetSample() const
{
    return wetSample;
}

void
Zone::handleSessionSampleDataChange()
{
    synthclone::Sample *sample;
    if (drySample) {
        sample = sessionSampleData.updateSample(*drySample, this);
        if (sample != drySample) {
            setDrySample(sample, false);
        }
    }
    if (wetSample) {
        sample = sessionSampleData.updateSample(*wetSample, this);
        if (sample != wetSample) {
            setWetSample(sample, false);
        }
    }
}

bool
Zone::isDrySampleStale() const
{
    return drySampleStale;
}

bool
Zone::isWetSampleStale() const
{
    return wetSampleStale;
}

void
Zone::setAftertouch(synthclone::MIDIData aftertouch)
{
    CONFIRM((aftertouch == synthclone::MIDI_VALUE_NOT_SET) ||
            (aftertouch < 0x80),
            tr("'%1': invalid MIDI aftertouch").arg(aftertouch));
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));

    if (this->aftertouch != aftertouch) {
        this->aftertouch = aftertouch;
        emit aftertouchChanged(aftertouch);
        setDrySampleStale(true);
    }
}

void
Zone::setChannel(synthclone::MIDIData channel)
{
    CONFIRM((channel >= 1) && (channel <= 16),
            tr("'%1': invalid MIDI channel").arg(channel));
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));

    if (this->channel != channel) {
        this->channel = channel;
        emit channelChanged(channel);
        setDrySampleStale(true);
    }
}

void
Zone::setChannelPressure(synthclone::MIDIData pressure)
{
    CONFIRM((pressure == synthclone::MIDI_VALUE_NOT_SET) || (pressure < 0x80),
            tr("'%1': invalid MIDI channel pressure").arg(pressure));
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));

    if (channelPressure != pressure) {
        channelPressure = pressure;
        emit channelPressureChanged(pressure);
        setDrySampleStale(true);
    }
}

void
Zone::setControlValue(synthclone::MIDIData control, synthclone::MIDIData value)
{
    CONFIRM(control < 0x80,
            tr("'%1': invalid MIDI control index").arg(control));
    CONFIRM((value < 0x80) || (value == synthclone::MIDI_VALUE_NOT_SET),
            tr("'%1': invalid MIDI control value").arg(value));
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));

    ControlMap::iterator iter = controlMap.find(control);
    synthclone::MIDIData oldValue = iter != controlMap.end() ? iter.value() :
        synthclone::MIDI_VALUE_NOT_SET;
    if (value != oldValue) {
        if (value == synthclone::MIDI_VALUE_NOT_SET) {
            controlMap.remove(control);
        } else {
            controlMap[control] = value;
        }
        emit controlValueChanged(control, value);
        setDrySampleStale(true);
    }
}

void
Zone::setDrySample(synthclone::Sample *sample)
{
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));
    setDrySample(sample, true);
}

void
Zone::setDrySample(synthclone::Sample *sample, bool copy)
{
    if (sample && copy) {
        QString path = createUniqueFile(sessionSampleData.getSampleDirectory());
        sample = new synthclone::Sample(*sample, path, false, this);
    }
    if (this->drySample != sample) {
        if (this->drySample) {
            this->drySample->deleteLater();
        }
        this->drySample = sample;
        if (sample) {
            sample->setTemporary(false);
        }
        emit drySampleChanged(sample);
        setDrySampleStale(! static_cast<bool>(sample));
    }
}

void
Zone::setDrySampleStale(bool stale)
{
    if (stale != drySampleStale) {
        drySampleStale = stale;
        emit drySampleStaleChanged(stale);
        if (stale) {
            setWetSampleStale(true);
        }
    }
}

void
Zone::setNote(synthclone::MIDIData note)
{
    CONFIRM(note < 0x80, tr("'%1': invalid MIDI note").arg(note));
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));

    if (this->note != note) {
        this->note = note;
        emit noteChanged(note);
        setDrySampleStale(true);
    }
}

void
Zone::setReleaseTime(synthclone::SampleTime releaseTime)
{
    CONFIRM((releaseTime > 0.0) &&
            (releaseTime <= synthclone::SAMPLE_TIME_MAXIMUM),
            tr("'%1': invalid release time").arg(releaseTime));
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));

    if (this->releaseTime != releaseTime) {
        this->releaseTime = releaseTime;
        emit releaseTimeChanged(releaseTime);
        setDrySampleStale(true);
    }
}

void
Zone::setSampleTime(synthclone::SampleTime sampleTime)
{
    CONFIRM((sampleTime > 0.0) &&
            (sampleTime <= synthclone::SAMPLE_TIME_MAXIMUM),
            tr("'%1': invalid sample time").arg(sampleTime));
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));

    if (this->sampleTime != sampleTime) {
        this->sampleTime = sampleTime;
        emit sampleTimeChanged(sampleTime);
        setDrySampleStale(true);
    }
}

void
Zone::setStatus(Status status)
{
    if (status != this->status) {
        this->status = status;
        emit statusChanged(status);
    }
}

void
Zone::setVelocity(synthclone::MIDIData velocity)
{
    CONFIRM((velocity > 0) && (velocity < 0x80),
            tr("'%1': invalid MIDI velocity").arg(velocity));
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));

    if (this->velocity != velocity) {
        this->velocity = velocity;
        emit velocityChanged(velocity);
        setDrySampleStale(true);
    }
}

void
Zone::setWetSample(synthclone::Sample *sample, bool copy)
{
    if (sample && copy) {
        QString path = createUniqueFile(sessionSampleData.getSampleDirectory());
        sample = new synthclone::Sample(*sample, path, false, this);
    }
    if (this->wetSample != sample) {
        if (this->wetSample) {
            this->wetSample->deleteLater();
        }
        this->wetSample = sample;
        if (sample) {
            sample->setTemporary(false);
        }
        emit wetSampleChanged(sample);
        if (wetSample) {
            if (! drySampleStale) {
                setWetSampleStale(false);
            }
        } else {
            setWetSampleStale(true);
        }
    }
}

void
Zone::setWetSampleStale()
{
    CONFIRM(status == STATUS_NORMAL, tr("zone is being used by session"));
    setWetSampleStale(true);
}

void
Zone::setWetSampleStale(bool stale)
{
    if (stale != wetSampleStale) {
        if (! stale) {
            assert(! drySampleStale);
        }
        wetSampleStale = stale;
        emit wetSampleStaleChanged(stale);
    }
}
