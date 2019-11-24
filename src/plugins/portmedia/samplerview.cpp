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

#include <cassert>

#include <QtCore/QLocale>
#include <QHeaderView>

#include <synthclone/util.h>

#include "samplerview.h"
#include "types.h"

SamplerView::SamplerView(QObject *parent):
    synthclone::DesignerView(":/synthclone/plugins/portmedia/samplerview.ui",
                             parent)
{
    QWidget *rootWidget = getRootWidget();

    addButton = synthclone::getChild<QPushButton>(rootWidget, "addButton");
    connect(addButton, SIGNAL(clicked()), SIGNAL(addRequest()));

    audioDriver = synthclone::getChild<QComboBox>(rootWidget, "audioDriver");
    connect(audioDriver, SIGNAL(activated(int)),
            SIGNAL(audioAPIChangeRequest(int)));

    audioInputDevice = synthclone::getChild<QComboBox>
        (rootWidget, "audioInputDevice");
    connect(audioInputDevice, SIGNAL(activated(int)),
            SIGNAL(audioInputDeviceChangeRequest(int)));

    audioOutputDevice = synthclone::getChild<QComboBox>
        (rootWidget, "audioOutputDevice");
    connect(audioOutputDevice, SIGNAL(activated(int)),
            SIGNAL(audioOutputDeviceChangeRequest(int)));

    connect(&channelMapTableDelegate,
            SIGNAL(audioInputChannelChangeRequest
                   (synthclone::SampleChannelCount,
                    synthclone::SampleChannelCount)),
            SIGNAL(audioInputChannelChangeRequest
                   (synthclone::SampleChannelCount,
                    synthclone::SampleChannelCount)));
    connect(&channelMapTableDelegate,
            SIGNAL(audioOutputChannelChangeRequest
                   (synthclone::SampleChannelCount,
                    synthclone::SampleChannelCount)),
            SIGNAL(audioOutputChannelChangeRequest
                   (synthclone::SampleChannelCount,
                    synthclone::SampleChannelCount)));

    channelMapTableModel.setColumnCount(CHANNELMAPTABLECOLUMN_TOTAL);
    channelMapTableModel.setHeaderData(CHANNELMAPTABLECOLUMN_INPUT_CHANNEL,
                                       Qt::Horizontal,
                                       tr("Device Input Channel"));
    channelMapTableModel.setHeaderData(CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL,
                                       Qt::Horizontal,
                                       tr("Device Output Channel"));

    channelMapTableView = synthclone::getChild<QTableView>
        (rootWidget, "channelMap");
    //channelMapTableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    channelMapTableView->setHorizontalScrollMode
        (QAbstractItemView::ScrollPerPixel);
    channelMapTableView->setVerticalScrollMode
        (QAbstractItemView::ScrollPerPixel);
    channelMapTableView->setItemDelegate(&channelMapTableDelegate);
    channelMapTableView->setModel(&channelMapTableModel);

    closeButton = synthclone::getChild<QPushButton>(rootWidget, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    midiDevice = synthclone::getChild<QComboBox>(rootWidget, "midiDevice");
    connect(midiDevice, SIGNAL(activated(int)),
            SIGNAL(midiDeviceChangeRequest(int)));

    name = synthclone::getChild<QLineEdit>(rootWidget, "name");
    connect(name, SIGNAL(textEdited(const QString &)),
            SIGNAL(nameChangeRequest(const QString &)));
}

SamplerView::~SamplerView()
{
    // Empty
}

void
SamplerView::addAudioAPI(int index, const QString &name)
{
    audioDriver->insertItem(index, name);
}

void
SamplerView::addAudioInputDevice(int index, const QString &name)
{
    audioInputDevice->insertItem(index, name);
}

void
SamplerView::addAudioOutputDevice(int index, const QString &name)
{
    audioOutputDevice->insertItem(index, name);
}

void
SamplerView::addMIDIDevice(int index, const QString &name)
{
    midiDevice->insertItem(index, name);
}

void
SamplerView::clearAudioAPIs()
{
    audioDriver->clear();
}

void
SamplerView::clearAudioInputDevices()
{
    audioInputDevice->clear();
}

void
SamplerView::clearAudioOutputDevices()
{
    audioOutputDevice->clear();
}

void
SamplerView::clearMIDIDevices()
{
    midiDevice->clear();
}

void
SamplerView::setAudioAPI(int index)
{
    audioDriver->setCurrentIndex(index);
}

void
SamplerView::setAudioInputChannel(synthclone::SampleChannelCount sampleChannel,
                                  synthclone::SampleChannelCount inputChannel)
{
    QString channel = QLocale::system().toString(inputChannel + 1);
    int row = static_cast<int>(sampleChannel);
    setModelData(row, CHANNELMAPTABLECOLUMN_INPUT_CHANNEL, inputChannel);
    setModelData(row, CHANNELMAPTABLECOLUMN_INPUT_CHANNEL,
                 tr("%1 - Channel %2").arg(audioInputDevice->currentText(),
                                           channel), Qt::DisplayRole);
    channelMapTableView->resizeColumnToContents
        (CHANNELMAPTABLECOLUMN_INPUT_CHANNEL);
}

void
SamplerView::setAudioInputDevice(int index)
{
    audioInputDevice->setCurrentIndex(index);
    QLocale locale = QLocale::system();
    const QString &name = audioInputDevice->currentText();
    int rows = channelMapTableModel.rowCount();
    for (int i = 0; i < rows; i++) {
        QModelIndex index = channelMapTableModel.index
            (i, CHANNELMAPTABLECOLUMN_INPUT_CHANNEL);
        QString channel =
            locale.toString(channelMapTableModel.data(index).toInt() + 1);
        setModelData(i, CHANNELMAPTABLECOLUMN_INPUT_CHANNEL,
                     tr("%1 - Channel %2").arg(name, channel),
                     Qt::DisplayRole);
    }
    channelMapTableDelegate.setAudioInputDeviceName(name);
    channelMapTableView->resizeColumnToContents
        (CHANNELMAPTABLECOLUMN_INPUT_CHANNEL);
}

void
SamplerView::
setAudioInputDeviceChannelCount(synthclone::SampleChannelCount channels)
{
    channelMapTableDelegate.setAudioInputDeviceChannelCount(channels);
}

void
SamplerView::
setAudioOutputChannel(synthclone::SampleChannelCount sampleChannel,
                      synthclone::SampleChannelCount outputChannel)
{
    QString channel = QLocale::system().toString(outputChannel + 1);
    int row = static_cast<int>(sampleChannel);
    setModelData(row, CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL, outputChannel);
    setModelData(row, CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL,
                 tr("%1 - Channel %2").arg(audioOutputDevice->currentText(),
                                           channel), Qt::DisplayRole);
    channelMapTableView->resizeColumnToContents
        (CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL);
}

void
SamplerView::setAudioOutputDevice(int index)
{
    audioOutputDevice->setCurrentIndex(index);
    QLocale locale = QLocale::system();
    const QString &name = audioOutputDevice->currentText();
    int rows = channelMapTableModel.rowCount();
    for (int i = 0; i < rows; i++) {
        QModelIndex index = channelMapTableModel.index
            (i, CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL);
        QString channel =
            locale.toString(channelMapTableModel.data(index).toInt() + 1);
        setModelData(i, CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL,
                     tr("%1 - Channel %2").arg(name, channel),
                     Qt::DisplayRole);
    }
    channelMapTableDelegate.setAudioOutputDeviceName(name);
    channelMapTableView->resizeColumnToContents
        (CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL);
}

void
SamplerView::
setAudioOutputDeviceChannelCount(synthclone::SampleChannelCount channels)
{
    channelMapTableDelegate.setAudioOutputDeviceChannelCount(channels);
}

void
SamplerView::setMIDIDevice(int index)
{
    midiDevice->setCurrentIndex(index);
}

void
SamplerView::setModelData(int row, int column, const QVariant &value, int role)
{
    bool result = channelMapTableModel.setData
        (channelMapTableModel.index(row, column), value, role);
    assert(result);
}

void
SamplerView::setName(const QString &name)
{
    this->name->setText(name);
}

void
SamplerView::setSampleChannelCount(synthclone::SampleChannelCount channels)
{
    channelMapTableModel.setRowCount(static_cast<int>(channels));
}
