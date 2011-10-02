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

#include <cassert>

#include <QtCore/QLocale>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>
#include <QtGui/QSpinBox>

#include <synthclone/util.h>

#include "zonetablecolumn.h"
#include "zonetabledelegate.h"

ZoneTableDelegate::ZoneTableDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
    // Empty
}

ZoneTableDelegate::~ZoneTableDelegate()
{
    // Empty
}

QWidget *
ZoneTableDelegate::createEditor(QWidget *parent,
                                const QStyleOptionViewItem &/*option*/,
                                const QModelIndex &index) const
{
    assert(index.isValid());
    QComboBox *comboBox;
    QDoubleSpinBox *doubleSpinBox;
    QSpinBox *spinBox;
    switch (index.column()) {
    default:
    case ZONETABLECOLUMN_AFTERTOUCH:
    case ZONETABLECOLUMN_CHANNEL_PRESSURE:
        spinBox = new QSpinBox(parent);
        spinBox->setRange(-1, 127);
        spinBox->setSpecialValueText(tr("(not set)"));
        return spinBox;
    case ZONETABLECOLUMN_CHANNEL:
        spinBox = new QSpinBox(parent);
        spinBox->setRange(1, 16);
        return spinBox;
    case ZONETABLECOLUMN_DRY_SAMPLE:
    case ZONETABLECOLUMN_STATUS:
    case ZONETABLECOLUMN_WET_SAMPLE:
        // This shouldn't happen.
        assert(false);
    case ZONETABLECOLUMN_NOTE:
        comboBox = new QComboBox(parent);
        for (synthclone::MIDIData i = 0; i < 0x80; i++) {
            comboBox->addItem(synthclone::getMIDINoteString(i));
        }
        return comboBox;
    case ZONETABLECOLUMN_RELEASE_TIME:
    case ZONETABLECOLUMN_SAMPLE_TIME:
        doubleSpinBox = new QDoubleSpinBox(parent);
        doubleSpinBox->setDecimals(2);
        doubleSpinBox->setRange
            (0.01, static_cast<float>(synthclone::SAMPLE_TIME_MAXIMUM));
        doubleSpinBox->setSuffix(tr(" seconds"));
        return doubleSpinBox;
    case ZONETABLECOLUMN_VELOCITY:
        spinBox = new QSpinBox(parent);
        spinBox->setRange(1, 127);
    }
    return spinBox;
}

QModelIndex
ZoneTableDelegate::getEditIndex() const
{
    return editIndex;
}

void
ZoneTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    assert(index.isValid());
    QStyledItemDelegate::paint(painter, option, index);
    QVariant variant;
    switch (index.column()) {
    case ZONETABLECOLUMN_DRY_SAMPLE:
    case ZONETABLECOLUMN_WET_SAMPLE:
        variant = index.data(Qt::UserRole);
        if (variant.isValid()) {
            QRect rectangle = option.rect;
            int height = rectangle.height();
            int width = rectangle.width();
            if (height && width) {
                float midHeight = height / 2.0;
                QVariantMap variantMap = variant.toMap();
                QVariantList peaks = variantMap.value("peaks").toList();
                assert(peaks.count() == 1024);
                float time = variantMap.value("time").toFloat();

                // Set the operations for drawing the sample.
                float dBFSFloor = -64.0;
                float maximumDBFS = -64.0;
                QPainterPath maximumPath(QPointF(0.0, midHeight));
                QPainterPath minimumPath(QPointF(0.0, midHeight));
                for (int i = 0; i < 1024; i++) {
                    float pixelX = (i / 1023.0) * (width - 1);
                    float value = peaks[i].toFloat();
                    if (value > 0.0) {
                        value = 0.0;
                    } else if (value < dBFSFloor) {
                        value = dBFSFloor;
                    }
                    float sampleHeight = ((value + (-dBFSFloor)) /
                                          (-dBFSFloor)) * midHeight;
                    maximumPath.lineTo(pixelX, midHeight - sampleHeight);
                    minimumPath.lineTo(pixelX, midHeight + sampleHeight);
                    if (value > maximumDBFS) {
                        maximumDBFS = value;
                    }
                }
                maximumPath.lineTo(width - 1, midHeight);
                minimumPath.lineTo(width - 1, midHeight);
                maximumPath.addPath(minimumPath.toReversed());

                QPixmap pixmap(width, height);
                QPainter pixmapPainter;
                QPalette palette = option.palette;
                pixmap.fill(Qt::transparent);
                pixmapPainter.begin(&pixmap);

                QLinearGradient gradient(0, 0, 0, height);
                QColor baseColor = palette.color(QPalette::Base);
                QColor textColor = palette.color(QPalette::Text);
                baseColor.setAlpha(0x80);
                textColor.setAlpha(0x80);
                float maximumHeight = ((maximumDBFS + (-dBFSFloor)) /
                                       (-dBFSFloor)) * midHeight;
                gradient.setColorAt((midHeight - maximumHeight) / height,
                                    textColor);
                gradient.setColorAt(0.5, baseColor);
                gradient.setColorAt((midHeight + maximumHeight) / height,
                                    textColor);
                gradient.setSpread(QLinearGradient::ReflectSpread);
                pixmapPainter.fillPath(maximumPath, gradient);

                // Write the sample time.
                QString timeString = tr("%1 seconds").
                    arg(QLocale::system().toString(time));
                QRect pixmapRectangle = pixmap.rect();
                int flags = Qt::AlignHCenter | Qt::AlignVCenter;
                QFont font(pixmapPainter.font());
                pixmapPainter.setFont(font);
                QRect textRectangle =
                    pixmapPainter.boundingRect(pixmapRectangle, flags,
                                               timeString);
                baseColor.setAlpha(0xd0);
                pixmapPainter.fillRect(textRectangle, baseColor);
                pixmapPainter.drawText(pixmapRectangle, flags, timeString);
                pixmapPainter.drawRect(textRectangle);

                pixmapPainter.end();

                painter->save();
                painter->drawPixmap(rectangle.topLeft(), pixmap);
                painter->restore();
            }
        }
    }
}

void
ZoneTableDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    assert(index.isValid());
    editIndex = index;
    int intValue;
    QVariant value = index.data(Qt::EditRole);
    switch (index.column()) {
    case ZONETABLECOLUMN_AFTERTOUCH:
    case ZONETABLECOLUMN_CHANNEL_PRESSURE:
    default:
        intValue = value.toInt();
        if (intValue == synthclone::MIDI_VALUE_NOT_SET) {
            intValue = -1;
        }
        qobject_cast<QSpinBox *>(editor)->setValue(intValue);
        break;
    case ZONETABLECOLUMN_CHANNEL:
    case ZONETABLECOLUMN_VELOCITY:
        qobject_cast<QSpinBox *>(editor)->setValue(value.toInt());
        break;
    case ZONETABLECOLUMN_DRY_SAMPLE:
    case ZONETABLECOLUMN_STATUS:
    case ZONETABLECOLUMN_WET_SAMPLE:
        // This shouldn't happen.
        assert(false);
    case ZONETABLECOLUMN_NOTE:
        qobject_cast<QComboBox *>(editor)->setCurrentIndex(value.toInt());
        break;
    case ZONETABLECOLUMN_RELEASE_TIME:
    case ZONETABLECOLUMN_SAMPLE_TIME:
        qobject_cast<QDoubleSpinBox *>(editor)->setValue(value.toDouble());
    }
}

void
ZoneTableDelegate::setModelData(QWidget *editor, QAbstractItemModel */*model*/,
                                const QModelIndex &index) const
{
    assert(index.isValid());
    if (index == editIndex) {
        editIndex = QModelIndex();
    }
    int column = index.column();
    int intValue;
    synthclone::MIDIData midiValue;
    int row = index.row();
    synthclone::SampleTime sampleTimeValue;
    switch (column) {
    case ZONETABLECOLUMN_AFTERTOUCH:
        intValue = qobject_cast<QSpinBox *>(editor)->value();
        midiValue = (intValue == -1) ? synthclone::MIDI_VALUE_NOT_SET :
            static_cast<synthclone::MIDIData>(intValue);
        emit aftertouchChangeRequest(row, midiValue);
        break;
    case ZONETABLECOLUMN_CHANNEL:
        intValue = qobject_cast<QSpinBox *>(editor)->value();
        emit channelChangeRequest
            (row, static_cast<synthclone::MIDIData>(intValue));
        break;
    case ZONETABLECOLUMN_CHANNEL_PRESSURE:
        intValue = qobject_cast<QSpinBox *>(editor)->value();
        midiValue = (intValue == -1) ? synthclone::MIDI_VALUE_NOT_SET :
            static_cast<synthclone::MIDIData>(intValue);
        emit channelPressureChangeRequest(row, midiValue);
        break;
    case ZONETABLECOLUMN_DRY_SAMPLE:
    case ZONETABLECOLUMN_STATUS:
    case ZONETABLECOLUMN_WET_SAMPLE:
        // This shouldn't happen.
        assert(false);
    case ZONETABLECOLUMN_NOTE:
        intValue = qobject_cast<QComboBox *>(editor)->currentIndex();
        emit noteChangeRequest(row,
                               static_cast<synthclone::MIDIData>(intValue));
        break;
    case ZONETABLECOLUMN_RELEASE_TIME:
        sampleTimeValue = static_cast<synthclone::SampleTime>
            (qobject_cast<QDoubleSpinBox *>(editor)->value());
        emit releaseTimeChangeRequest(row, sampleTimeValue);
        break;
    case ZONETABLECOLUMN_SAMPLE_TIME:
        sampleTimeValue = static_cast<synthclone::SampleTime>
            (qobject_cast<QDoubleSpinBox *>(editor)->value());
        emit sampleTimeChangeRequest(row, sampleTimeValue);
        break;
    case ZONETABLECOLUMN_VELOCITY:
        intValue = qobject_cast<QSpinBox *>(editor)->value();
        emit velocityChangeRequest
            (row, static_cast<synthclone::MIDIData>(intValue));
        break;
    default:
        intValue = qobject_cast<QSpinBox *>(editor)->value();
        midiValue = (intValue == -1) ? synthclone::MIDI_VALUE_NOT_SET :
            static_cast<synthclone::MIDIData>(intValue);
        emit controlValueChangeRequest
            (row, column - ZONETABLECOLUMN_CONTROL_0, midiValue);
        break;
    }
}

void
ZoneTableDelegate::updateEditorGeometry(QWidget *editor,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    assert(index.isValid());
    editor->setGeometry(option.rect);
}
