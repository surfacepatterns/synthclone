/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011-2013 Devin Anderson
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

#include <synthclone/error.h>
#include <synthclone/util.h>

#include "util.h"
#include "zoneviewlet.h"

ZoneViewlet::ZoneViewlet(QMainWindow *mainWindow, QObject *parent):
    QObject(parent)
{
    // Initialize zones menu and associated actions

    zonesMenu = synthclone::getChild<QMenu>(mainWindow, "zonesMenu");

    applyEffectsAction = synthclone::getChild<QAction>
        (mainWindow, "applyEffectsZonesAction");
    connect(applyEffectsAction, SIGNAL(triggered()),
            SIGNAL(applyEffectsRequest()));

    buildTargetsAction = synthclone::getChild<QAction>
        (mainWindow, "buildTargetsAction");
    connect(buildTargetsAction, SIGNAL(triggered()),
            SIGNAL(buildTargetsRequest()));

    clearEffectJobsAction = synthclone::getChild<QAction>
        (mainWindow, "clearEffectJobsAction");
    connect(clearEffectJobsAction, SIGNAL(triggered()),
            SIGNAL(clearEffectJobsRequest()));

    clearSamplerJobsAction = synthclone::getChild<QAction>
        (mainWindow, "clearSamplerJobsAction");
    connect(clearSamplerJobsAction, SIGNAL(triggered()),
            SIGNAL(clearSamplerJobsRequest()));

    clearSamplerJobsButton = synthclone::getChild<QPushButton>
        (mainWindow, "clearSamplerJobsButton");
    connect(clearSamplerJobsButton, SIGNAL(clicked()),
            SIGNAL(clearSamplerJobsRequest()));

    clearSelectionAction = synthclone::getChild<QAction>
        (mainWindow, "clearZonesSelectionAction");
    connect(clearSelectionAction, SIGNAL(triggered()),
            SIGNAL(clearSelectionRequest()));

    copyAction = synthclone::getChild<QAction>(mainWindow, "copyZonesAction");
    connect(copyAction, SIGNAL(triggered()), SIGNAL(copyRequest()));

    cutAction = synthclone::getChild<QAction>(mainWindow, "cutZonesAction");
    connect(cutAction, SIGNAL(triggered()), SIGNAL(cutRequest()));

    deleteAction = synthclone::getChild<QAction>
        (mainWindow, "deleteZonesAction");
    connect(deleteAction, SIGNAL(triggered()), SIGNAL(deleteRequest()));

    insertAction = synthclone::getChild<QAction>
        (mainWindow, "insertZoneAction");
    connect(insertAction, SIGNAL(triggered()), SIGNAL(insertRequest()));

    invertSelectionAction = synthclone::getChild<QAction>
        (mainWindow, "invertZoneSelectionAction");
    connect(invertSelectionAction, SIGNAL(triggered()),
            SIGNAL(invertSelectionRequest()));

    pasteAction = synthclone::getChild<QAction>(mainWindow, "pasteZonesAction");
    connect(pasteAction, SIGNAL(triggered()), SIGNAL(pasteRequest()));

    playDrySampleAction = synthclone::getChild<QAction>
        (mainWindow, "playDrySampleZonesAction");
    connect(playDrySampleAction, SIGNAL(triggered()),
            SIGNAL(playDrySampleRequest()));

    playWetSampleAction =
        synthclone::getChild<QAction>(mainWindow, "playWetSampleZonesAction");
    connect(playWetSampleAction, SIGNAL(triggered()),
            SIGNAL(playWetSampleRequest()));

    removeEffectJobAction =
        synthclone::getChild<QAction>(mainWindow, "removeEffectJobZonesAction");
    connect(removeEffectJobAction, SIGNAL(triggered()),
            SIGNAL(removeEffectJobRequest()));

    removeSamplerJobAction = synthclone::getChild<QAction>
        (mainWindow, "removeSamplerJobZonesAction");
    connect(removeSamplerJobAction, SIGNAL(triggered()),
            SIGNAL(removeSamplerJobRequest()));

    sampleAction = synthclone::getChild<QAction>
        (mainWindow, "getSampleZonesAction");
    connect(sampleAction, SIGNAL(triggered()), SIGNAL(sampleRequest()));

    selectAllAction = synthclone::getChild<QAction>
        (mainWindow, "selectAllZonesAction");
    connect(selectAllAction, SIGNAL(triggered()), SIGNAL(selectAllRequest()));

    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_AFTERTOUCH,
                               "aftertouchColumnShowAction");
    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_CHANNEL,
                               "channelColumnShowAction");
    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_CHANNEL_PRESSURE,
                               "channelPressureColumnShowAction");
    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_DRY_SAMPLE,
                               "drySampleColumnShowAction");
    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_NOTE,
                               "noteColumnShowAction");
    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_RELEASE_TIME,
                               "releaseTimeColumnShowAction");
    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_SAMPLE_TIME,
                               "sampleTimeColumnShowAction");
    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_STATUS,
                               "statusColumnShowAction");
    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_VELOCITY,
                               "velocityColumnShowAction");
    initializeColumnShowAction(mainWindow, ZONETABLECOLUMN_WET_SAMPLE,
                               "wetSampleColumnShowAction");
    initializeControlColumnShowActions("controlColumnsMenu1", 0, 0x10);
    initializeControlColumnShowActions("controlColumnsMenu2", 0x10, 0x20);
    initializeControlColumnShowActions("controlColumnsMenu3", 0x20, 0x30);
    initializeControlColumnShowActions("controlColumnsMenu4", 0x30, 0x40);
    initializeControlColumnShowActions("controlColumnsMenu5", 0x40, 0x50);
    initializeControlColumnShowActions("controlColumnsMenu6", 0x50, 0x60);
    initializeControlColumnShowActions("controlColumnsMenu7", 0x60, 0x70);
    initializeControlColumnShowActions("controlColumnsMenu8", 0x70, 0x80);

    menuViewlet = new MenuViewlet(zonesMenu);

    // Setup zones table view

    connect(&tableDelegate,
            SIGNAL(aftertouchChangeRequest(int, synthclone::MIDIData)),
            SIGNAL(aftertouchChangeRequest(int, synthclone::MIDIData)));
    connect(&tableDelegate,
            SIGNAL(channelChangeRequest(int, synthclone::MIDIData)),
            SIGNAL(channelChangeRequest(int, synthclone::MIDIData)));
    connect(&tableDelegate,
            SIGNAL(channelPressureChangeRequest(int, synthclone::MIDIData)),
            SIGNAL(channelPressureChangeRequest(int, synthclone::MIDIData)));
    connect(&tableDelegate,
            SIGNAL(controlValueChangeRequest(int, synthclone::MIDIData,
                                             synthclone::MIDIData)),
            SIGNAL(controlValueChangeRequest(int, synthclone::MIDIData,
                                             synthclone::MIDIData)));
    connect(&tableDelegate,
            SIGNAL(noteChangeRequest(int, synthclone::MIDIData)),
            SIGNAL(noteChangeRequest(int, synthclone::MIDIData)));
    connect(&tableDelegate,
            SIGNAL(releaseTimeChangeRequest(int, synthclone::SampleTime)),
            SIGNAL(releaseTimeChangeRequest(int, synthclone::SampleTime)));
    connect(&tableDelegate,
            SIGNAL(sampleTimeChangeRequest(int, synthclone::SampleTime)),
            SIGNAL(sampleTimeChangeRequest(int, synthclone::SampleTime)));
    connect(&tableDelegate,
            SIGNAL(velocityChangeRequest(int, synthclone::MIDIData)),
            SIGNAL(velocityChangeRequest(int, synthclone::MIDIData)));

    itemPrototype = new StandardItem();
    connect(&tableModel, SIGNAL(sortRequest(int, bool)),
            SLOT(handleSortRequest(int, bool)));
    tableModel.setColumnCount(ZONETABLECOLUMN_BASE_TOTAL);
    tableModel.setRowCount(0);
    tableModel.setItemPrototype(itemPrototype);
    tableModel.setHeaderData(ZONETABLECOLUMN_AFTERTOUCH, Qt::Horizontal,
                             tr("Aftertouch"), Qt::DisplayRole);
    tableModel.setHeaderData(ZONETABLECOLUMN_CHANNEL, Qt::Horizontal,
                             tr("Channel"), Qt::DisplayRole);
    tableModel.setHeaderData(ZONETABLECOLUMN_CHANNEL_PRESSURE, Qt::Horizontal,
                             tr("Channel Pressure"), Qt::DisplayRole);
    tableModel.setHeaderData(ZONETABLECOLUMN_DRY_SAMPLE, Qt::Horizontal,
                             tr("Dry Sample"), Qt::DisplayRole);
    tableModel.setHeaderData(ZONETABLECOLUMN_NOTE, Qt::Horizontal,
                             tr("Note"), Qt::DisplayRole);
    tableModel.setHeaderData(ZONETABLECOLUMN_RELEASE_TIME, Qt::Horizontal,
                             tr("Release Time"), Qt::DisplayRole);
    tableModel.setHeaderData(ZONETABLECOLUMN_SAMPLE_TIME, Qt::Horizontal,
                             tr("Sample Time"), Qt::DisplayRole);
    tableModel.setHeaderData(ZONETABLECOLUMN_STATUS, Qt::Horizontal,
                             tr("Status"), Qt::DisplayRole);
    tableModel.setHeaderData(ZONETABLECOLUMN_VELOCITY, Qt::Horizontal,
                             tr("Velocity"), Qt::DisplayRole);
    tableModel.setHeaderData(ZONETABLECOLUMN_WET_SAMPLE, Qt::Horizontal,
                             tr("Wet Sample"), Qt::DisplayRole);
    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        int column = ZONETABLECOLUMN_CONTROL_0 + i;
        tableModel.setHeaderData(column, Qt::Horizontal,
                                 synthclone::getMIDIControlString(i),
                                 Qt::DisplayRole);
    }

    tableView = synthclone::getChild<QTableView>(mainWindow, "zoneTableView");
    tableView->installEventFilter(&contextMenuEventFilter);
    tableView->setItemDelegate(&tableDelegate);
    tableView->setModel(&tableModel);
    connect(tableView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            SLOT(handleSelectionChange(QItemSelection, QItemSelection)));

    connect(&contextMenuEventFilter, SIGNAL(contextMenuRequest(int, int)),
            SLOT(handleContextMenuRequest(int, int)));

    emitZoneSelectRequest = true;
}

ZoneViewlet::~ZoneViewlet()
{
    for (int i = 0; i < 0x80; i++) {
        delete coreColumnShowActions[ZONETABLECOLUMN_CONTROL_0 + i];
    }
    delete menuViewlet;
}

void
ZoneViewlet::addZone(int index)
{
    assert((index >= 0) && (index <= tableModel.rowCount()));
    bool inserted = tableModel.insertRow(index);
    assert(inserted);
    enableRow(index);
}

void
ZoneViewlet::disableRow(int index)
{
    assert((index >= 0) && (index < tableModel.rowCount()));

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    getModelItem(index, ZONETABLECOLUMN_DRY_SAMPLE)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_STATUS)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_WET_SAMPLE)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_AFTERTOUCH)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_CHANNEL)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_CHANNEL_PRESSURE)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_NOTE)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_RELEASE_TIME)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_SAMPLE_TIME)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_VELOCITY)->setFlags(flags);
    for (int i = 0; i < 0x80; i++) {
        getModelItem(index, ZONETABLECOLUMN_CONTROL_0 + i)->setFlags(flags);
    }

    QModelIndex editIndex = tableDelegate.getEditIndex();
    if (editIndex.isValid() && (editIndex.row() == index)) {
        tableView->closePersistentEditor(editIndex);
    }
}

void
ZoneViewlet::enableRow(int index)
{
    assert((index >= 0) && (index < tableModel.rowCount()));

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    getModelItem(index, ZONETABLECOLUMN_DRY_SAMPLE)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_STATUS)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_WET_SAMPLE)->setFlags(flags);

    flags |= Qt::ItemIsEditable;
    getModelItem(index, ZONETABLECOLUMN_AFTERTOUCH)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_CHANNEL)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_CHANNEL_PRESSURE)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_NOTE)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_RELEASE_TIME)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_SAMPLE_TIME)->setFlags(flags);
    getModelItem(index, ZONETABLECOLUMN_VELOCITY)->setFlags(flags);
    for (int i = 0; i < 0x80; i++) {
        getModelItem(index, ZONETABLECOLUMN_CONTROL_0 + i)->setFlags(flags);
    }
}

QVariant
ZoneViewlet::generateSampleProfile(const SampleProfile *profile)
{
    QVariant data;
    if (profile) {
        const float *peaks = profile->getPeaks();
        QVariantList variants;
        for (int i = 0; i < 1024; i++) {
            variants.append(peaks[i] < -128.0 ? -128.0 : peaks[i]);
        }
        QVariantMap map;
        map["peaks"] = variants;
        map["time"] = profile->getTime();
        data.setValue(map);
    }
    return data;
}

MenuViewlet *
ZoneViewlet::getMenuViewlet()
{
    return menuViewlet;
}

QStandardItem *
ZoneViewlet::getModelItem(int row, int column)
{
    QStandardItem *item =
        tableModel.itemFromIndex(tableModel.index(row, column));
    assert(item);
    return item;
}

void
ZoneViewlet::handleColumnShowAction()
{
    QAction *action = qobject_cast<QAction *>(sender());
    assert(action);
    int column = action->data().toInt();
    synthclone::MIDIData control;
    bool visible = tableView->isColumnHidden(column);
    switch (column) {
    case ZONETABLECOLUMN_AFTERTOUCH:
        emit aftertouchPropertyVisibilityChangeRequest(visible);
        break;
    case ZONETABLECOLUMN_CHANNEL:
        emit channelPropertyVisibilityChangeRequest(visible);
        break;
    case ZONETABLECOLUMN_CHANNEL_PRESSURE:
        emit channelPressurePropertyVisibilityChangeRequest(visible);
        break;
    case ZONETABLECOLUMN_DRY_SAMPLE:
        emit drySamplePropertyVisibilityChangeRequest(visible);
        break;
    case ZONETABLECOLUMN_NOTE:
        emit notePropertyVisibilityChangeRequest(visible);
        break;
    case ZONETABLECOLUMN_RELEASE_TIME:
        emit releaseTimePropertyVisibilityChangeRequest(visible);
        break;
    case ZONETABLECOLUMN_SAMPLE_TIME:
        emit sampleTimePropertyVisibilityChangeRequest(visible);
        break;
    case ZONETABLECOLUMN_STATUS:
        emit statusPropertyVisibilityChangeRequest(visible);
        break;
    case ZONETABLECOLUMN_VELOCITY:
        emit velocityPropertyVisibilityChangeRequest(visible);
        break;
    case ZONETABLECOLUMN_WET_SAMPLE:
        emit wetSamplePropertyVisibilityChangeRequest(visible);
        break;
    default:
        control = static_cast<synthclone::MIDIData>
            (column - ZONETABLECOLUMN_CONTROL_0);
        emit controlPropertyVisibilityChangeRequest(control, visible);
    }
}

void
ZoneViewlet::handleContextMenuRequest(int x, int y)
{
    zonesMenu->popup(tableView->mapToGlobal(QPoint(x, y)));
}

void
ZoneViewlet::handleSelectionChange(const QItemSelection &selectedItems,
                                   const QItemSelection &deselectedItems)
{
    if (emitZoneSelectRequest) {
        QList<int> rowIndexes;
        QModelIndexList tableIndexes = selectedItems.indexes();
        for (int i = tableIndexes.count() - 1; i >= 0; i--) {
            int row = tableIndexes[i].row();
            if (! rowIndexes.contains(row)) {
                emit selectionChangeRequest(row, true);
                rowIndexes.append(row);
            }
        }
        rowIndexes.clear();
        tableIndexes = deselectedItems.indexes();
        for (int i = tableIndexes.count() - 1; i >= 0; i--) {
            int row = tableIndexes[i].row();
            if (! rowIndexes.contains(row)) {
                emit selectionChangeRequest(row, false);
                rowIndexes.append(row);
            }
        }
    }
}

void
ZoneViewlet::handleSortRequest(int column, bool ascending)
{
    assert((column >= 0) && (column < tableModel.columnCount()));
    synthclone::MIDIData control;
    switch (column) {
    case ZONETABLECOLUMN_AFTERTOUCH:
        emit aftertouchPropertySortRequest(ascending);
        break;
    case ZONETABLECOLUMN_CHANNEL:
        emit channelPropertySortRequest(ascending);
        break;
    case ZONETABLECOLUMN_CHANNEL_PRESSURE:
        emit channelPressurePropertySortRequest(ascending);
        break;
    case ZONETABLECOLUMN_DRY_SAMPLE:
        emit drySamplePropertySortRequest(ascending);
        break;
    case ZONETABLECOLUMN_NOTE:
        emit notePropertySortRequest(ascending);
        break;
    case ZONETABLECOLUMN_RELEASE_TIME:
        emit releaseTimePropertySortRequest(ascending);
        break;
    case ZONETABLECOLUMN_SAMPLE_TIME:
        emit sampleTimePropertySortRequest(ascending);
        break;
    case ZONETABLECOLUMN_STATUS:
        emit statusPropertySortRequest(ascending);
        break;
    case ZONETABLECOLUMN_VELOCITY:
        emit velocityPropertySortRequest(ascending);
        break;
    case ZONETABLECOLUMN_WET_SAMPLE:
        emit wetSamplePropertySortRequest(ascending);
        break;
    default:
        control = static_cast<synthclone::MIDIData>
            (column - ZONETABLECOLUMN_CONTROL_0);
        emit controlPropertySortRequest(control, ascending);
    }
}

void
ZoneViewlet::initializeColumnShowAction(QWidget *widget, int column,
                                        const QString &actionId)
{
    QAction *action = synthclone::getChild<QAction>(widget, actionId);
    action->setData(column);
    connect(action, SIGNAL(triggered()), SLOT(handleColumnShowAction()));
    coreColumnShowActions[column] = action;
}

void
ZoneViewlet::initializeControlColumnShowActions(const QString &menuId,
                                                synthclone::MIDIData start,
                                                synthclone::MIDIData end)
{
    QMenu *menu = synthclone::getChild<QMenu>(zonesMenu, menuId);
    for (synthclone::MIDIData i = start; i < end; i++) {
        QAction *action =
            new QAction(synthclone::getMIDIControlString(i), menu);
        int column = ZONETABLECOLUMN_CONTROL_0 + static_cast<int>(i);
        action->setCheckable(true);
        action->setData(column);
        connect(action, SIGNAL(triggered()), SLOT(handleColumnShowAction()));
        coreColumnShowActions[column] = action;
        menu->insertAction(0, action);
    }
}

void
ZoneViewlet::moveZone(int fromIndex, int toIndex)
{
    int rowCount = tableModel.rowCount();
    assert((fromIndex >= 0) && (fromIndex < rowCount));
    assert((toIndex >= 0) && (toIndex < rowCount));
    assert(fromIndex != toIndex);
    bool selected = tableView->selectionModel()->
        isRowSelected(fromIndex, QModelIndex());
    tableModel.insertRow(toIndex, tableModel.takeRow(fromIndex));
    if (selected) {
        int columnCount = tableModel.columnCount();
        QItemSelection selection(tableModel.index(toIndex, 0),
                                 tableModel.index(toIndex, columnCount - 1));
        emitZoneSelectRequest = false;
        tableView->selectionModel()->
            select(selection, QItemSelectionModel::Select);
        emitZoneSelectRequest = true;
    }
}

void
ZoneViewlet::removeZone(int index)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    bool removed = tableModel.removeRow(index);
    assert(removed);
}

void
ZoneViewlet::setAftertouch(int index, synthclone::MIDIData aftertouch)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    int intAftertouch = static_cast<int>(aftertouch);
    setModelData(index, ZONETABLECOLUMN_AFTERTOUCH, intAftertouch);
    QVariant value;
    if (aftertouch == synthclone::MIDI_VALUE_NOT_SET) {
        value = tr("(not set)");
    } else {
        value = intAftertouch;
    }
    setModelData(index, ZONETABLECOLUMN_AFTERTOUCH, value, Qt::DisplayRole);
}

void
ZoneViewlet::setAftertouchPropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_AFTERTOUCH, visible);
}

void
ZoneViewlet::setApplyEffectsEnabled(bool enabled)
{
    applyEffectsAction->setEnabled(enabled);
}

void
ZoneViewlet::setBuildTargetsEnabled(bool enabled)
{
    buildTargetsAction->setEnabled(enabled);
}

void
ZoneViewlet::setChannel(int index, synthclone::MIDIData channel)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    int intChannel = static_cast<int>(channel);
    setModelData(index, ZONETABLECOLUMN_CHANNEL, intChannel);
    setModelData(index, ZONETABLECOLUMN_CHANNEL, intChannel, Qt::DisplayRole);
}

void
ZoneViewlet::setChannelPressure(int index, synthclone::MIDIData pressure)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    int intPressure = static_cast<int>(pressure);
    setModelData(index, ZONETABLECOLUMN_CHANNEL_PRESSURE, intPressure);
    QVariant value;
    if (pressure == synthclone::MIDI_VALUE_NOT_SET) {
        value = tr("(not set)");
    } else {
        value = intPressure;
    }
    setModelData(index, ZONETABLECOLUMN_CHANNEL_PRESSURE, value,
                 Qt::DisplayRole);
}

void
ZoneViewlet::setChannelPressurePropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_CHANNEL_PRESSURE, visible);
}

void
ZoneViewlet::setChannelPropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_CHANNEL, visible);
}

void
ZoneViewlet::setClearEffectJobsEnabled(bool enabled)
{
    clearEffectJobsAction->setEnabled(enabled);
}

void
ZoneViewlet::setClearSamplerJobsEnabled(bool enabled)
{
    clearSamplerJobsAction->setEnabled(enabled);
    clearSamplerJobsButton->setEnabled(enabled);
}

void
ZoneViewlet::setClearSelectionEnabled(bool enabled)
{
    clearSelectionAction->setEnabled(enabled);
}

void
ZoneViewlet::setColumnVisible(int column, bool visible)
{
    tableView->setColumnHidden(column, ! visible);
    coreColumnShowActions[column]->setChecked(visible);
    if (visible) {
        tableView->resizeColumnToContents(column);
    }
}

void
ZoneViewlet::setControlPropertyVisible(synthclone::MIDIData control,
                                       bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_CONTROL_0 + static_cast<int>(control),
                     visible);
}

void
ZoneViewlet::setControlValue(int index, synthclone::MIDIData control,
                             synthclone::MIDIData value)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    assert(control < 0x80);
    assert((value < 0x80) || (value == synthclone::MIDI_VALUE_NOT_SET));
    int column = ZONETABLECOLUMN_CONTROL_0 + static_cast<int>(control);
    int intValue = static_cast<int>(value);
    setModelData(index, column, intValue);
    QVariant varValue;
    if (value == synthclone::MIDI_VALUE_NOT_SET) {
        varValue = tr("(not set)");
    } else {
        varValue = intValue;
    }
    setModelData(index, column, varValue, Qt::DisplayRole);
}

void
ZoneViewlet::setCopyEnabled(bool enabled)
{
    copyAction->setEnabled(enabled);
}

void
ZoneViewlet::setCutEnabled(bool enabled)
{
    cutAction->setEnabled(enabled);
}

void
ZoneViewlet::setDeleteEnabled(bool enabled)
{
    deleteAction->setEnabled(enabled);
}

void
ZoneViewlet::setDrySampleProfile(int index, const SampleProfile *profile)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    setModelData(index, ZONETABLECOLUMN_DRY_SAMPLE,
                 generateSampleProfile(profile), Qt::UserRole);
}

void
ZoneViewlet::setDrySamplePropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_DRY_SAMPLE, visible);
}

void
ZoneViewlet::setDrySampleStale(int index, bool stale)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    const QPalette &palette = tableView->palette();
    setModelData(index, ZONETABLECOLUMN_DRY_SAMPLE,
                 stale ? palette.alternateBase() : palette.base(),
                 Qt::BackgroundRole);
}

void
ZoneViewlet::setInsertEnabled(bool enabled)
{
    insertAction->setEnabled(enabled);
}

void
ZoneViewlet::setInvertSelectionEnabled(bool enabled)
{
    invertSelectionAction->setEnabled(enabled);
}

void
ZoneViewlet::setModelData(int row, int column, const QVariant &value, int role)
{
    bool result = tableModel.setData(tableModel.index(row, column), value,
                                     role);
    assert(result);
}

void
ZoneViewlet::setNote(int index, synthclone::MIDIData note)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    setModelData(index, ZONETABLECOLUMN_NOTE, static_cast<int>(note));
    setModelData(index, ZONETABLECOLUMN_NOTE,
                 synthclone::getMIDINoteString(note), Qt::DisplayRole);
}

void
ZoneViewlet::setNotePropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_NOTE, visible);
}

void
ZoneViewlet::setPasteEnabled(bool enabled)
{
    pasteAction->setEnabled(enabled);
}

void
ZoneViewlet::setPlayDrySampleEnabled(bool enabled)
{
    playDrySampleAction->setEnabled(enabled);
}

void
ZoneViewlet::setPlayWetSampleEnabled(bool enabled)
{
    playWetSampleAction->setEnabled(enabled);
}

void
ZoneViewlet::setReleaseTime(int index, synthclone::SampleTime releaseTime)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    setModelData(index, ZONETABLECOLUMN_RELEASE_TIME, releaseTime);
    setModelData(index, ZONETABLECOLUMN_RELEASE_TIME,
                 tr("%1 seconds").arg(releaseTime), Qt::DisplayRole);
}

void
ZoneViewlet::setReleaseTimePropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_RELEASE_TIME, visible);
}

void
ZoneViewlet::setRemoveEffectJobEnabled(bool enabled)
{
    removeEffectJobAction->setEnabled(enabled);
}

void
ZoneViewlet::setRemoveSamplerJobEnabled(bool enabled)
{
    removeSamplerJobAction->setEnabled(enabled);
}

void
ZoneViewlet::setSampleEnabled(bool enabled)
{
    sampleAction->setEnabled(enabled);
}

void
ZoneViewlet::setSampleTime(int index, synthclone::SampleTime sampleTime)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    setModelData(index, ZONETABLECOLUMN_SAMPLE_TIME, sampleTime);
    setModelData(index, ZONETABLECOLUMN_SAMPLE_TIME,
                 tr("%1 seconds").arg(sampleTime), Qt::DisplayRole);
}

void
ZoneViewlet::setSampleTimePropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_SAMPLE_TIME, visible);
}

void
ZoneViewlet::setSelectAllEnabled(bool enabled)
{
    selectAllAction->setEnabled(enabled);
}

void
ZoneViewlet::setSelected(int index, bool selected)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    int columnCount = tableModel.columnCount();
    QItemSelection selection(tableModel.index(index, 0),
                             tableModel.index(index, columnCount - 1));
    QItemSelectionModel::SelectionFlag flag =
        selected ? QItemSelectionModel::Select : QItemSelectionModel::Deselect;
    emitZoneSelectRequest = false;
    tableView->selectionModel()->select(selection, flag);
    emitZoneSelectRequest = true;
}

void
ZoneViewlet::setStatus(int index, synthclone::Zone::Status status)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    bool disabled = true;
    QString statusStr;

    switch (status) {
    case synthclone::Zone::STATUS_EFFECT_JOB_QUEUE:
        statusStr = tr("In effect job queue ...");
        break;
    case synthclone::Zone::STATUS_EFFECTS:
        statusStr = tr("Applying effects ...");
        break;
    case synthclone::Zone::STATUS_NORMAL:
        disabled = false;
        statusStr = "";
        break;
    case synthclone::Zone::STATUS_SAMPLER_PLAYING_DRY_SAMPLE:
        statusStr = tr("Playing dry sample ...");
        break;
    case synthclone::Zone::STATUS_SAMPLER_PLAYING_WET_SAMPLE:
        statusStr = tr("Playing wet sample ...");
        break;
    case synthclone::Zone::STATUS_SAMPLER_SAMPLING:
        statusStr = tr("Sampling ...");
        break;
    case synthclone::Zone::STATUS_SAMPLER_JOB_QUEUE:
        statusStr = tr("In sampler job queue ...");
        break;
    case synthclone::Zone::STATUS_TARGETS:
        statusStr = tr("Building targets ...");
    }
    setModelData(index, ZONETABLECOLUMN_STATUS, statusStr, Qt::DisplayRole);
    if (disabled) {
        disableRow(index);
        setModelData(index, ZONETABLECOLUMN_STATUS,
                     QPixmap(":/synthclone/images/16x16/locked.png"),
                     Qt::DecorationRole);
    } else {
        enableRow(index);
        setModelData(index, ZONETABLECOLUMN_STATUS, QVariant(),
                     Qt::DecorationRole);
    }
}

void
ZoneViewlet::setStatusPropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_STATUS, visible);
}

void
ZoneViewlet::setVelocity(int index, synthclone::MIDIData velocity)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    int intVelocity = static_cast<int>(velocity);
    setModelData(index, ZONETABLECOLUMN_VELOCITY, intVelocity);
    setModelData(index, ZONETABLECOLUMN_VELOCITY, intVelocity, Qt::DisplayRole);
}

void
ZoneViewlet::setVelocityPropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_VELOCITY, visible);
}

void
ZoneViewlet::setWetSampleProfile(int index, const SampleProfile *profile)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    setModelData(index, ZONETABLECOLUMN_WET_SAMPLE,
                 generateSampleProfile(profile), Qt::UserRole);
}

void
ZoneViewlet::setWetSamplePropertyVisible(bool visible)
{
    setColumnVisible(ZONETABLECOLUMN_WET_SAMPLE, visible);
}

void
ZoneViewlet::setWetSampleStale(int index, bool stale)
{
    assert((index >= 0) && (index < tableModel.rowCount()));
    const QPalette &palette = tableView->palette();
    setModelData(index, ZONETABLECOLUMN_WET_SAMPLE,
                 stale ? palette.alternateBase() : palette.base(),
                 Qt::BackgroundRole);
}
