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

#ifndef __VIEW_H__
#define __VIEW_H__

#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

#include <synthclone/designerview.h>
#include <synthclone/types.h>

class View: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    View(QObject *parent=0);

    ~View();

public slots:

    void
    setAftertouchLayers(synthclone::MIDIData layers);

    void
    setChannelPressureLayers(synthclone::MIDIData layers);

    void
    setFirstNote(synthclone::MIDIData firstNote);

    void
    setLastNote(synthclone::MIDIData lastNote);

    void
    setLastNoteEditingEnabled(bool lastNoteEditingEnabled);

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
    channelPressureLayersChanged(synthclone::MIDIData layers);

    void
    firstNoteChanged(synthclone::MIDIData firstNote);

    void
    generateRequest();

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

private slots:

    void
    handleAftertouchLayersChange(int layers);

    void
    handleChannelPressureLayersChange(int layers);

    void
    handleFirstNoteChange(int firstNoteIndex);

    void
    handleLastNoteChange(int lastNoteIndex);

    void
    handleMIDIChannelChange(int midiChannel);

    void
    handleReleaseTimeChange(double releaseTime);

    void
    handleSampleTimeChange(double sampleTime);

    void
    handleTotalNotesChange(int totalNotes);

    void
    handleVelocityLayersChange(int layers);

private:

    QSpinBox *aftertouchLayers;
    QPushButton *cancelButton;
    QSpinBox *channelPressureLayers;
    QComboBox *firstNote;
    QPushButton *generateButton;
    QComboBox *lastNote;
    QLabel *lastNoteLabel;
    QSpinBox *midiChannel;
    QDoubleSpinBox *releaseTime;
    QDoubleSpinBox *sampleTime;
    QSpinBox *totalNotes;
    QSpinBox *velocityLayers;

};

#endif
