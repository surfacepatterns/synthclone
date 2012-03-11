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
#include <QtGui/QComboBox>

#include "channelmapdelegate.h"
#include "types.h"

ChannelMapDelegate::ChannelMapDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
    // Empty
}

ChannelMapDelegate::~ChannelMapDelegate()
{
    // Empty
}

QWidget *
ChannelMapDelegate::createEditor(QWidget *parent,
                                 const QStyleOptionViewItem &/*option*/,
                                 const QModelIndex &index) const
{
    assert(index.isValid());
    int channelCount;
    QString name;
    switch (index.column()) {
    case CHANNELMAPTABLECOLUMN_INPUT_CHANNEL:
        channelCount = inputChannelCount;
        name = inputName;
        break;
    case CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL:
        channelCount = outputChannelCount;
        name = outputName;
        break;
    default:
        assert(false);
    }
    QComboBox *comboBox = new QComboBox(parent);
    QLocale locale = QLocale::system();
    for (int i = 1; i <= channelCount; i++) {
        comboBox->addItem(tr("%1 - Channel %2").arg(name, locale.toString(i)));
    }
    return comboBox;
}

void
ChannelMapDelegate::
setAudioInputDeviceChannelCount(synthclone::SampleChannelCount channels)
{
    inputChannelCount = channels;
}

void
ChannelMapDelegate::setAudioInputDeviceName(const QString &name)
{
    inputName = name;
}

void
ChannelMapDelegate::
setAudioOutputDeviceChannelCount(synthclone::SampleChannelCount channels)
{
    outputChannelCount = channels;
}

void
ChannelMapDelegate::setAudioOutputDeviceName(const QString &name)
{
    outputName = name;
}

void
ChannelMapDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    assert(index.isValid());
    switch (index.column()) {
    case CHANNELMAPTABLECOLUMN_INPUT_CHANNEL:
    case CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL:
        qobject_cast<QComboBox *>(editor)->
            setCurrentIndex(index.data(Qt::EditRole).toInt());
        break;
    default:
        assert(false);
    }
}

void
ChannelMapDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel */*model*/,
                                 const QModelIndex &index) const
{
    assert(index.isValid());
    synthclone::SampleChannelCount channel =
        static_cast<synthclone::SampleChannelCount>(index.row());
    synthclone::SampleChannelCount deviceChannel =
        static_cast<synthclone::SampleChannelCount>
        (qobject_cast<QComboBox *>(editor)->currentIndex());
    switch (index.column()) {
    case CHANNELMAPTABLECOLUMN_INPUT_CHANNEL:
        emit audioInputChannelChangeRequest(channel, deviceChannel);
        break;
    case CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL:
        emit audioOutputChannelChangeRequest(channel, deviceChannel);
        break;
    default:
        assert(false);
    }
}

void
ChannelMapDelegate::updateEditorGeometry(QWidget *editor,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    assert(index.isValid());
    editor->setGeometry(option.rect);
}
