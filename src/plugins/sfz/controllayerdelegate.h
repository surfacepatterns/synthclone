/*
 * libsynthclone_sfz - SFZ target plugin for `synthclone`
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

#ifndef __CONTROLLAYERDELEGATE_H__
#define __CONTROLLAYERDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>

#include <synthclone/types.h>

class ControlLayerDelegate: public QStyledItemDelegate {

    Q_OBJECT

public:

    explicit
    ControlLayerDelegate(QObject *parent=0);

    ~ControlLayerDelegate();

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

signals:

    void
    crossfadingEnabledChangeRequest(int index, bool enabled) const;

    void
    defaultValueChangeRequest(int index,
                              synthclone::MIDIData defaultValue) const;

    void
    typeChangeRequest(int index, synthclone::ControlType type) const;

};

#endif
