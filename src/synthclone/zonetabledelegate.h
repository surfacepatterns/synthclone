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

#ifndef __ZONETABLEDELEGATE_H__
#define __ZONETABLEDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>

#include <synthclone/types.h>

class ZoneTableDelegate: public QStyledItemDelegate {

    Q_OBJECT

public:

    explicit
    ZoneTableDelegate(QObject *parent=0);

    ~ZoneTableDelegate();

    QWidget *
    createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                 const QModelIndex &index) const;

    QModelIndex
    getEditIndex() const;

    void
    paint(QPainter *painter, const QStyleOptionViewItem &option,
          const QModelIndex &index) const;

    void
    setEditorData(QWidget *editor, const QModelIndex &index) const;

    void
    setModelData(QWidget *editor, QAbstractItemModel *model,
                 const QModelIndex &index) const;

    void
    updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

signals:

    void
    aftertouchChangeRequest(int index, synthclone::MIDIData aftertouch) const;

    void
    channelChangeRequest(int index, synthclone::MIDIData channel) const;

    void
    channelPressureChangeRequest(int index,
                                 synthclone::MIDIData pressure) const;

    void
    controlValueChangeRequest(int index, synthclone::MIDIData control,
                              synthclone::MIDIData value) const;

    void
    noteChangeRequest(int index, synthclone::MIDIData note) const;

    void
    releaseTimeChangeRequest(int index,
                             synthclone::SampleTime releaseTime) const;

    void
    sampleTimeChangeRequest(int index, synthclone::SampleTime sampleTime) const;

    void
    velocityChangeRequest(int index, synthclone::MIDIData velocity) const;

private:

    mutable QModelIndex editIndex;

};

#endif
