/*
 * libsynthclone_portmedia - PortAudio/PortMIDI sampler plugin for `synthclone`
 * Copyright (C) 2012 Devin Anderson
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

#ifndef __SAMPLERVIEW_H__
#define __SAMPLERVIEW_H__

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTableView>

#include <synthclone/designerview.h>
#include <synthclone/types.h>

#include "channelmapdelegate.h"

class SamplerView: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    SamplerView(QObject *parent=0);

    ~SamplerView();

public slots:

    void
    addAudioAPI(int index, const QString &name);

    void
    addAudioInputDevice(int index, const QString &name);

    void
    addAudioOutputDevice(int index, const QString &name);

    void
    addMIDIDevice(int index, const QString &name);

    void
    clearAudioAPIs();

    void
    clearAudioInputDevices();

    void
    clearAudioOutputDevices();

    void
    clearMIDIDevices();

    void
    setAudioAPI(int index);

    void
    setAudioInputChannel(synthclone::SampleChannelCount sampleChannel,
                         synthclone::SampleChannelCount inputChannel);

    void
    setAudioInputDevice(int index);

    void
    setAudioInputDeviceChannelCount(synthclone::SampleChannelCount channels);

    void
    setAudioOutputChannel(synthclone::SampleChannelCount sampleChannel,
                          synthclone::SampleChannelCount outputChannel);

    void
    setAudioOutputDevice(int index);

    void
    setAudioOutputDeviceChannelCount(synthclone::SampleChannelCount channels);

    void
    setMIDIDevice(int index);

    void
    setName(const QString &name);

    void
    setSampleChannelCount(synthclone::SampleChannelCount channels);

signals:

    void
    addRequest();

    void
    audioAPIChangeRequest(int index);

    void
    audioInputChannelChangeRequest(synthclone::SampleChannelCount sampleIndex,
                                   synthclone::SampleChannelCount outputIndex);

    void
    audioInputDeviceChangeRequest(int index);

    void
    audioOutputChannelChangeRequest(synthclone::SampleChannelCount sampleIndex,
                                    synthclone::SampleChannelCount outputIndex);

    void
    audioOutputDeviceChangeRequest(int index);

    void
    midiDeviceChangeRequest(int index);

    void
    nameChangeRequest(const QString &name);

private:

    void
    setModelData(int row, int column, const QVariant &value,
                 int role=Qt::UserRole);

    QPushButton *addButton;
    QComboBox *audioDriver;
    QComboBox *audioInputDevice;
    QComboBox *audioOutputDevice;
    ChannelMapDelegate channelMapTableDelegate;
    QStandardItemModel channelMapTableModel;
    QTableView *channelMapTableView;
    QPushButton *closeButton;
    QComboBox *midiDevice;
    QLineEdit *name;

};

#endif
