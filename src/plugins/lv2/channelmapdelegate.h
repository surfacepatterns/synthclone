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

#ifndef __CHANNELMAPDELEGATE_H__
#define __CHANNELMAPDELEGATE_H__

#include <QtGui/QStyledItemDelegate>

#include <synthclone/types.h>

class ChannelMapDelegate: public QStyledItemDelegate {

    Q_OBJECT

public:

    explicit
    ChannelMapDelegate(QObject *parent=0);

    ~ChannelMapDelegate();

    QWidget *
    createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                 const QModelIndex &index) const;

    void
    setEditorData(QWidget *editor, const QModelIndex &index) const;

    void
    setModelData(QWidget *editor, QAbstractItemModel *model,
                 const QModelIndex &index) const;

    void
    updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

public slots:

    void
    addAudioInputPort(const QString &name);

    void
    addAudioOutputPort(const QString &name);

    void
    clearAudioInputPorts();

    void
    clearAudioOutputPorts();

    void
    removeAudioInputPort();

    void
    removeAudioOutputPort();

signals:

    void
    audioInputChannelChanged(synthclone::SampleChannelCount channel,
                             int effectChannel) const;

    void
    audioOutputChannelChanged(synthclone::SampleChannelCount channel,
                              int effectChannel) const;

private:

    QStringList audioInputPortNames;
    QStringList audioOutputPortNames;
    synthclone::SampleChannelCount channels;

};

#endif
