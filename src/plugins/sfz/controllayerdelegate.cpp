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

#include <cassert>

#include <QComboBox>
#include <QSpinBox>

#include "controllayerdelegate.h"
#include "types.h"

ControlLayerDelegate::ControlLayerDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
    // Empty
}

ControlLayerDelegate::~ControlLayerDelegate()
{
    // Empty
}

QWidget *
ControlLayerDelegate::createEditor(QWidget *parent,
                                   const QStyleOptionViewItem &/*option*/,
                                   const QModelIndex &index) const
{
    assert(index.isValid());
    QComboBox *comboBox;
    QSpinBox *spinBox;
    switch (index.column()) {
    case CONTROLLAYERTABLECOLUMN_CONTROL:
    default:
        assert(false);
    case CONTROLLAYERTABLECOLUMN_CROSSFADING_ENABLED:
        comboBox = new QComboBox(parent);
        comboBox->addItem(tr("Disabled"));
        comboBox->addItem(tr("Enabled"));
        break;
    case CONTROLLAYERTABLECOLUMN_DEFAULT_VALUE:
        spinBox = new QSpinBox(parent);
        spinBox->setRange(0, 127);
        return spinBox;
    case CONTROLLAYERTABLECOLUMN_TYPE:
        comboBox = new QComboBox(parent);
        comboBox->addItem(tr("Continuous"));
        comboBox->addItem(tr("Switch (0 - 63: off, 64 - 127: on"));
    }
    return comboBox;
}

void
ControlLayerDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    assert(index.isValid());
    synthclone::ControlType type;
    QVariant value = index.data(Qt::EditRole);
    switch (index.column()) {
    case CONTROLLAYERTABLECOLUMN_CONTROL:
    default:
        assert(false);
    case CONTROLLAYERTABLECOLUMN_CROSSFADING_ENABLED:
        qobject_cast<QComboBox *>(editor)->
            setCurrentIndex(value.toBool() ? 1 : 0);
        break;
    case CONTROLLAYERTABLECOLUMN_DEFAULT_VALUE:
        qobject_cast<QSpinBox *>(editor)->setValue(value.toInt());
        break;
    case CONTROLLAYERTABLECOLUMN_TYPE:
        type = static_cast<synthclone::ControlType>(value.toInt());
        qobject_cast<QComboBox *>(editor)->
            setCurrentIndex(type == synthclone::CONTROLTYPE_SWITCH ? 1 : 0);
    }
}

void
ControlLayerDelegate::setModelData(QWidget *editor,
                                   QAbstractItemModel */*model*/,
                                   const QModelIndex &index) const
{
    assert(index.isValid());
    bool boolValue;
    int intValue;
    int row = index.row();
    synthclone::ControlType typeValue;
    switch (index.column()) {
    case CONTROLLAYERTABLECOLUMN_CONTROL:
    default:
        assert(false);
    case CONTROLLAYERTABLECOLUMN_CROSSFADING_ENABLED:
        boolValue = qobject_cast<QComboBox *>(editor)->currentIndex() == 1;
        emit crossfadingEnabledChangeRequest(row, boolValue);
        break;
    case CONTROLLAYERTABLECOLUMN_DEFAULT_VALUE:
        intValue = qobject_cast<QSpinBox *>(editor)->value();
        emit defaultValueChangeRequest(row, intValue);
        break;
    case CONTROLLAYERTABLECOLUMN_TYPE:
        typeValue = qobject_cast<QComboBox *>(editor)->currentIndex() == 1 ?
            synthclone::CONTROLTYPE_SWITCH :
            synthclone::CONTROLTYPE_CONTINUOUS;
        emit typeChangeRequest(row, typeValue);
    }
}

void
ControlLayerDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    assert(index.isValid());
    editor->setGeometry(option.rect);
}
