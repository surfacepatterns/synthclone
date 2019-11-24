/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone`
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
#include <QComboBox>

#include <cassert>

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

void
ChannelMapDelegate::addAudioInputPort(const QString &name)
{
    audioInputPortNames.append(name);
}

void
ChannelMapDelegate::addAudioOutputPort(const QString &name)
{
    audioOutputPortNames.append(name);
}

void
ChannelMapDelegate::clearAudioInputPorts()
{
    audioInputPortNames.clear();
}

void
ChannelMapDelegate::clearAudioOutputPorts()
{
    audioOutputPortNames.clear();
}

QWidget *
ChannelMapDelegate::createEditor(QWidget *parent,
                                 const QStyleOptionViewItem &/*option*/,
                                 const QModelIndex &index) const
{
    assert(index.isValid());
    const QStringList *names;
    switch (index.column()) {
    case CHANNELMAPTABLECOLUMN_INPUT_CHANNEL:
        names = &audioInputPortNames;
        break;
    case CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL:
        names = &audioOutputPortNames;
        break;
    default:
        assert(false);
    }
    QComboBox *comboBox = new QComboBox(parent);
    int count = names->count();
    for (int i = 0; i < count; i++) {
        comboBox->addItem(names->at(i));
    }
    return comboBox;
}

void
ChannelMapDelegate::removeAudioInputPort()
{
    assert(audioInputPortNames.count());
    audioInputPortNames.removeLast();
}

void
ChannelMapDelegate::removeAudioOutputPort()
{
    assert(audioOutputPortNames.count());
    audioOutputPortNames.removeLast();
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
            setCurrentIndex(index.data(Qt::UserRole).toInt());
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
    int effectChannel = qobject_cast<QComboBox *>(editor)->currentIndex();
    switch (index.column()) {
    case CHANNELMAPTABLECOLUMN_INPUT_CHANNEL:
        emit audioInputChannelChanged(channel, effectChannel);
        break;
    case CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL:
        emit audioOutputChannelChanged(channel, effectChannel);
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
