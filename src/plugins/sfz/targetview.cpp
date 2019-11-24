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

#include <QtCore/QLocale>
#include <QMenu>

#include <synthclone/util.h>

#include "targetview.h"

TargetView::TargetView(QObject *parent):
    synthclone::DesignerView(":/synthclone/plugins/sfz/targetview.ui", parent)
{
    QWidget *rootWidget = getRootWidget();

    addControlLayerButton = synthclone::getChild<QPushButton>
        (rootWidget, "addControlLayerButton");
    QAction *action;
    QMenu *controlMenu = new QMenu(addControlLayerButton);
    QLocale locale = QLocale::system();
    for (synthclone::MIDIData i = 0; i < 0x80; i += 0x10) {
        QString caption = tr("Controls %1 - %2").
            arg(locale.toString(i), locale.toString(i + 0x0f));
        QMenu *subControlMenu = controlMenu->addMenu(caption);
        for (synthclone::MIDIData j = 0; j < 0x10; j++) {
            synthclone::MIDIData control = i + j;
            caption = synthclone::getMIDIControlString(control);
            action = subControlMenu->addAction(caption);
            action->setData(static_cast<int>(control));
            connect(action, SIGNAL(triggered()),
                    SLOT(handleControlActionTrigger()));
            controlActionMap.insert(control, action);
        }
    }
    controlMenu->addSeparator();
    action = controlMenu->addAction(tr("Aftertouch"));
    action->setData(static_cast<int>(CONTROL_AFTERTOUCH));
    connect(action, SIGNAL(triggered()), SLOT(handleControlActionTrigger()));
    controlActionMap.insert(CONTROL_AFTERTOUCH, action);
    action = controlMenu->addAction(tr("Channel Pressure"));
    action->setData(static_cast<int>(CONTROL_CHANNEL_PRESSURE));
    connect(action, SIGNAL(triggered()), SLOT(handleControlActionTrigger()));
    controlActionMap.insert(CONTROL_CHANNEL_PRESSURE, action);
    addControlLayerButton->setMenu(controlMenu);

    closeButton = synthclone::getChild<QPushButton>(rootWidget, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    controlCrossfadeCurve = synthclone::getChild<QComboBox>
        (rootWidget, "controlCrossfadeCurve");
    connect(controlCrossfadeCurve, SIGNAL(currentIndexChanged(int)),
            SLOT(handleControlCrossfadeCurveIndexChange(int)));

    connect(&controlLayerTableDelegate,
            SIGNAL(crossfadingEnabledChangeRequest(int, bool)),
            SIGNAL(controlLayerCrossfadingEnabledChangeRequest(int, bool)));
    connect(&controlLayerTableDelegate,
            SIGNAL(defaultValueChangeRequest(int, synthclone::MIDIData)),
            SIGNAL(controlLayerDefaultValueChangeRequest
                   (int, synthclone::MIDIData)));
    connect(&controlLayerTableDelegate,
            SIGNAL(typeChangeRequest(int, synthclone::ControlType)),
            SIGNAL(controlLayerTypeChangeRequest(int,
                                                 synthclone::ControlType)));

    controlLayerTableModel.setColumnCount(CONTROLLAYERTABLECOLUMN_TOTAL);
    controlLayerTableModel.setHeaderData(CONTROLLAYERTABLECOLUMN_CONTROL,
                                         Qt::Horizontal, tr("Control"));
    controlLayerTableModel.setHeaderData
        (CONTROLLAYERTABLECOLUMN_CROSSFADING_ENABLED, Qt::Horizontal,
         tr("Crossfade?"));
    controlLayerTableModel.setHeaderData(CONTROLLAYERTABLECOLUMN_DEFAULT_VALUE,
                                         Qt::Horizontal, tr("Default Value"));
    controlLayerTableModel.setHeaderData(CONTROLLAYERTABLECOLUMN_TYPE,
                                         Qt::Horizontal, tr("Type"));

    controlLayerTableView = synthclone::getChild<QTableView>
        (rootWidget, "controlLayerTableView");
    controlLayerTableView->setItemDelegate(&controlLayerTableDelegate);
    controlLayerTableView->setModel(&controlLayerTableModel);
    connect(controlLayerTableView->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            SLOT(handleControlLayerTableRowChange()));

    drumKit = synthclone::getChild<QCheckBox>(rootWidget, "drumKit");
    connect(drumKit, SIGNAL(toggled(bool)),
            SIGNAL(drumKitChangeRequest(bool)));

    moveControlLayerDownButton = synthclone::getChild<QPushButton>
        (rootWidget, "moveControlLayerDownButton");
    connect(moveControlLayerDownButton, SIGNAL(clicked()),
            SLOT(handleMoveControlLayerDownButtonClick()));

    moveControlLayerUpButton = synthclone::getChild<QPushButton>
        (rootWidget, "moveControlLayerUpButton");
    connect(moveControlLayerUpButton, SIGNAL(clicked()),
            SLOT(handleMoveControlLayerUpButtonClick()));

    name = synthclone::getChild<QLineEdit>(rootWidget, "name");
    connect(name, SIGNAL(textEdited(const QString &)),
            SIGNAL(nameChangeRequest(const QString &)));

    noteCrossfadeCurve = synthclone::getChild<QComboBox>
        (rootWidget, "noteCrossfadeCurve");
    connect(noteCrossfadeCurve, SIGNAL(currentIndexChanged(int)),
            SLOT(handleNoteCrossfadeCurveIndexChange(int)));

    noteCrossfadeCurveLabel = synthclone::getChild<QLabel>
        (rootWidget, "noteCrossfadeCurveLabel");

    path = synthclone::getChild<QLineEdit>(rootWidget, "path");
    connect(path, SIGNAL(textEdited(const QString &)),
            SIGNAL(pathChangeRequest(const QString &)));

    pathLookupButton = synthclone::getChild<QPushButton>
        (rootWidget, "pathLookupButton");
    connect(pathLookupButton, SIGNAL(clicked()),
            SIGNAL(pathLookupRequest()));

    removeControlLayerButton = synthclone::getChild<QPushButton>
        (rootWidget, "removeControlLayerButton");
    connect(removeControlLayerButton, SIGNAL(clicked()),
            SLOT(handleRemoveControlLayerButtonClick()));

    sampleFormat = synthclone::getChild<QComboBox>(rootWidget, "sampleFormat");
    connect(sampleFormat, SIGNAL(currentIndexChanged(int)),
            SLOT(handleSampleFormatIndexChange(int)));

    velocityCrossfadeCurve = synthclone::getChild<QComboBox>
        (rootWidget, "velocityCrossfadeCurve");
    connect(velocityCrossfadeCurve, SIGNAL(currentIndexChanged(int)),
            SLOT(handleVelocityCrossfadeCurveIndexChange(int)));
}

TargetView::~TargetView()
{
    // Empty
}

void
TargetView::addControlLayer(int index, synthclone::MIDIData control)
{
    assert((index >= 0) && (index <= controlLayerTableModel.rowCount()));
    assert((control < 0x80) || (control == CONTROL_AFTERTOUCH) ||
           (control == CONTROL_CHANNEL_PRESSURE));
    bool inserted = controlLayerTableModel.insertRow(index);
    assert(inserted);
    QString controlStr;
    switch (control) {
    case CONTROL_AFTERTOUCH:
        controlStr = tr("Aftertouch");
        break;
    case CONTROL_CHANNEL_PRESSURE:
        controlStr = tr("Channel Pressure");
        break;
    default:
        controlStr = synthclone::getMIDIControlString(control);
    }
    setModelData(index, CONTROLLAYERTABLECOLUMN_CONTROL, controlStr,
                 Qt::DisplayRole);

    QStandardItem *item = controlLayerTableModel.itemFromIndex
        (controlLayerTableModel.index(index, CONTROLLAYERTABLECOLUMN_CONTROL));
    item->setFlags(item->flags() & (~ Qt::ItemIsEditable));

    updateControlLayerButtons();

    QAction *action = controlActionMap.value(control, 0);
    assert(action);
    action->setEnabled(false);
}

int
TargetView::getSelectedControlLayer() const
{
    QModelIndex index = controlLayerTableView->currentIndex();
    return index.isValid() ? index.row() : -1;
}

void
TargetView::handleControlActionTrigger()
{
    QAction *action = qobject_cast<QAction *>(sender());
    synthclone::MIDIData control =
        static_cast<synthclone::MIDIData>(action->data().toInt());
    emit controlLayerAddRequest(control);
}

void
TargetView::handleControlCrossfadeCurveIndexChange(int index)
{
    emit controlCrossfadeCurveChangeRequest
        (static_cast<CrossfadeCurve>(index + 1));
}

void
TargetView::handleControlLayerTableRowChange()
{
    updateControlLayerButtons();
}

void
TargetView::handleMoveControlLayerDownButtonClick()
{
    int index = getSelectedControlLayer();
    emit controlLayerMoveRequest(index, index + 1);
}

void
TargetView::handleMoveControlLayerUpButtonClick()
{
    int index = getSelectedControlLayer();
    emit controlLayerMoveRequest(index, index - 1);
}

void
TargetView::handleNoteCrossfadeCurveIndexChange(int index)
{
    emit noteCrossfadeCurveChangeRequest(static_cast<CrossfadeCurve>(index));
}

void
TargetView::handleRemoveControlLayerButtonClick()
{
    emit controlLayerRemoveRequest(getSelectedControlLayer());
}

void
TargetView::handleSampleFormatIndexChange(int index)
{
    emit sampleFormatChangeRequest(static_cast<SampleFormat>(index));
}

void
TargetView::handleVelocityCrossfadeCurveIndexChange(int index)
{
    emit velocityCrossfadeCurveChangeRequest
        (static_cast<CrossfadeCurve>(index));
}

void
TargetView::moveControlLayer(int fromIndex, int toIndex)
{
    int rowCount = controlLayerTableModel.rowCount();
    assert((fromIndex >= 0) && (fromIndex < rowCount));
    assert((toIndex >= 0) && (toIndex < rowCount));
    assert(fromIndex != toIndex);
    int selectedIndex = getSelectedControlLayer();
    controlLayerTableModel.insertRow
        (toIndex, controlLayerTableModel.takeRow(fromIndex));
    if (selectedIndex == fromIndex) {
        controlLayerTableView->selectRow(toIndex);
    }
    updateControlLayerButtons();
}

void
TargetView::removeControlLayer(int index, synthclone::MIDIData control)
{
    assert((index >= 0) && (index < controlLayerTableModel.rowCount()));
    bool removed = controlLayerTableModel.removeRow(index);
    assert(removed);
    updateControlLayerButtons();

    QAction *action = controlActionMap.value(control, 0);
    assert(action);
    action->setEnabled(true);
}

void
TargetView::setControlCrossfadeCurve(CrossfadeCurve curve)
{
    controlCrossfadeCurve->setCurrentIndex(static_cast<int>(curve));
}

void
TargetView::setControlLayerCrossfadingEnabled(int index, bool enabled)
{
    assert((index >= 0) && (index < controlLayerTableModel.rowCount()));
    setModelData(index, CONTROLLAYERTABLECOLUMN_CROSSFADING_ENABLED, enabled);
    setModelData(index, CONTROLLAYERTABLECOLUMN_CROSSFADING_ENABLED,
                 enabled ? tr("Enabled") : tr("Disabled"), Qt::DisplayRole);
}

void
TargetView::setControlLayerDefaultValue(int index, synthclone::MIDIData value)
{
    assert((index >= 0) && (index < controlLayerTableModel.rowCount()));
    int intValue = static_cast<int>(value);
    setModelData(index, CONTROLLAYERTABLECOLUMN_DEFAULT_VALUE, intValue);
    setModelData(index, CONTROLLAYERTABLECOLUMN_DEFAULT_VALUE, intValue,
                 Qt::DisplayRole);
}

void
TargetView::setControlLayerType(int index, synthclone::ControlType type)
{
    assert((index >= 0) && (index < controlLayerTableModel.rowCount()));
    assert((type == synthclone::CONTROLTYPE_CONTINUOUS) ||
           (type == synthclone::CONTROLTYPE_SWITCH));
    setModelData(index, CONTROLLAYERTABLECOLUMN_TYPE, static_cast<int>(type));
    setModelData(index, CONTROLLAYERTABLECOLUMN_TYPE,
                 type == synthclone::CONTROLTYPE_CONTINUOUS ?
                 tr("Continuous") : tr("Switch"), Qt::DisplayRole);
}

void
TargetView::setDrumKit(bool drumKit)
{
    this->drumKit->setChecked(drumKit);
}

void
TargetView::setModelData(int row, int column, const QVariant &value, int role)
{
    bool result = controlLayerTableModel.setData
        (controlLayerTableModel.index(row, column), value, role);
    assert(result);
}

void
TargetView::setName(const QString &name)
{
    this->name->setText(name);
}

void
TargetView::setNoteCrossfadeCurve(CrossfadeCurve curve)
{
    noteCrossfadeCurve->setCurrentIndex(static_cast<int>(curve));
}

void
TargetView::setPath(const QString &path)
{
    this->path->setText(path);
}

void
TargetView::setSampleFormat(SampleFormat format)
{
    sampleFormat->setCurrentIndex(static_cast<int>(format));
}

void
TargetView::setVelocityCrossfadeCurve(CrossfadeCurve curve)
{
    velocityCrossfadeCurve->setCurrentIndex(static_cast<int>(curve));
}

void
TargetView::updateControlLayerButtons()
{
    int index = getSelectedControlLayer();
    bool selected = index != -1;
    removeControlLayerButton->setEnabled(selected);
    if (! selected) {
        moveControlLayerDownButton->setEnabled(false);
        moveControlLayerUpButton->setEnabled(false);
    } else {
        moveControlLayerDownButton->
            setEnabled(index != (controlLayerTableModel.rowCount() - 1));
        moveControlLayerUpButton->setEnabled(static_cast<bool>(index));
    }
}
