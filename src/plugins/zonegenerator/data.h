/*
 * libsynthclone_zonegenerator - Zone generation plugin for `synthclone`
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

#ifndef __DATA_H__
#define __DATA_H__

#include <QtCore/QObject>

#include <synthclone/types.h>

class Data: public QObject {

    Q_OBJECT

public:

    explicit
    Data(QObject *parent=0);

    ~Data();

    synthclone::MIDIData
    getAftertouchLayers() const;

    synthclone::MIDIData
    getFirstNote() const;

    synthclone::MIDIData
    getLastNote() const;

    synthclone::MIDIData
    getMIDIChannel() const;

    synthclone::SampleTime
    getReleaseTime() const;

    synthclone::SampleTime
    getSampleTime() const;

    synthclone::MIDIData
    getTotalNotes() const;

    synthclone::MIDIData
    getVelocityLayers() const;

public slots:

    void
    reset();

    void
    setAftertouchLayers(synthclone::MIDIData layers);

    void
    setFirstNote(synthclone::MIDIData firstNote);

    void
    setLastNote(synthclone::MIDIData lastNote);

    void
    setMIDIChannel(synthclone::MIDIData midiChannel);

    void
    setReleaseTime(synthclone::SampleTime releaseTime);

    void
    setSampleTime(synthclone::SampleTime sampleTime);

    void
    setTotalNotes(synthclone::MIDIData totalNotes);

    void
    setVelocityLayers(synthclone::MIDIData layers);

signals:

    void
    aftertouchLayersChanged(synthclone::MIDIData layers);

    void
    firstNoteChanged(synthclone::MIDIData firstNote);

    void
    lastNoteChanged(synthclone::MIDIData lastNote);

    void
    midiChannelChanged(synthclone::MIDIData midiChannel);

    void
    releaseTimeChanged(synthclone::SampleTime releaseTime);

    void
    sampleTimeChanged(synthclone::SampleTime sampleTime);

    void
    totalNotesChanged(synthclone::MIDIData totalNotes);

    void
    velocityLayersChanged(synthclone::MIDIData layers);

private:

    void
    updateTotalNotes();

    synthclone::MIDIData aftertouchLayers;
    synthclone::MIDIData firstNote;
    synthclone::MIDIData lastNote;
    synthclone::MIDIData midiChannel;
    synthclone::SampleTime releaseTime;
    synthclone::SampleTime sampleTime;
    synthclone::MIDIData totalNotes;
    synthclone::MIDIData velocityLayers;

};

#endif
