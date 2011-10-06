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

#include <synthclone/util.h>

#include "componentviewlet.h"

ComponentViewlet::ComponentViewlet(QMainWindow *mainWindow, QObject *parent):
    QObject(parent)
{
    componentMenu = synthclone::getChild<QMenu>(mainWindow, "componentsMenu");
    effectRootMenu = synthclone::getChild<QMenu>(mainWindow, "effectMenu");
    samplerRootMenu = synthclone::getChild<QMenu>(mainWindow, "samplerMenu");
    targetRootMenu = synthclone::getChild<QMenu>(mainWindow, "targetMenu");

    QMenu *effectAddMenu =
        synthclone::getChild<QMenu>(mainWindow, "addEffectMenu");

    effectAddButton = synthclone::getChild<QPushButton>
        (mainWindow, "effectAddButton");
    effectAddButton->setMenu(effectAddMenu);

    effectAddMenuViewlet = new MenuViewlet(effectAddMenu);

    effectMoveDownButton =
        synthclone::getChild<QPushButton>(mainWindow, "effectMoveDownButton");
    connect(effectMoveDownButton, SIGNAL(clicked()),
            SLOT(handleEffectMoveDownButtonClick()));

    effectMoveUpButton =
        synthclone::getChild<QPushButton>(mainWindow, "effectMoveUpButton");
    connect(effectMoveUpButton, SIGNAL(clicked()),
            SLOT(handleEffectMoveUpButtonClick()));

    effectRemoveButton =
        synthclone::getChild<QPushButton>(mainWindow, "effectRemoveButton");
    connect(effectRemoveButton, SIGNAL(clicked()),
            SLOT(handleEffectRemoveButtonClick()));

    effectTableModel.setColumnCount(COMPONENTTABLECOLUMN_TOTAL);
    effectTableModel.setHeaderData(COMPONENTTABLECOLUMN_NAME,
                                   Qt::Horizontal, tr("Name"));
    effectTableModel.setHeaderData(COMPONENTTABLECOLUMN_PROGRESS,
                                   Qt::Horizontal, tr("Progress"));
    effectTableModel.setHeaderData(COMPONENTTABLECOLUMN_STATUS,
                                   Qt::Horizontal, tr("Status"));

    effectTableView =
        synthclone::getChild<QTableView>(mainWindow, "effectTableView");
    effectTableView->setItemDelegateForColumn(COMPONENTTABLECOLUMN_PROGRESS,
                                              &effectProgressBarDelegate);
    effectTableView->setModel(&effectTableModel);
    connect(effectTableView->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            SLOT(handleEffectTableRowChange(QModelIndex)));

    samplerAddMenu =
        synthclone::getChild<QMenu>(mainWindow, "addSamplerMenu");

    samplerAddButton =
        synthclone::getChild<QPushButton>(mainWindow, "samplerAddButton");
    samplerAddButton->setMenu(samplerAddMenu);

    samplerAddMenuViewlet = new MenuViewlet(samplerAddMenu);

    samplerJobAbortButton =
        synthclone::getChild<QPushButton>(mainWindow, "samplerJobAbortButton");
    connect(samplerJobAbortButton, SIGNAL(clicked()),
            SIGNAL(samplerJobAbortRequest()));

    samplerMenuViewlet = new MenuViewlet(samplerRootMenu);

    samplerName = synthclone::getChild<QLabel>(mainWindow, "samplerName");

    samplerProgressBar =
        synthclone::getChild<QProgressBar>(mainWindow, "samplerProgressBar");
    samplerProgressBar->setValue(0);

    samplerStatus = synthclone::getChild<QLabel>(mainWindow, "samplerStatus");

    tabWidget = synthclone::getChild<QTabWidget>
        (mainWindow, "componentTabWidget");
    connect(&effectTabContextMenuEventFilter,
            SIGNAL(contextMenuRequest(int, int)),
            SLOT(handleEffectTabContextMenuRequest(int, int)));
    connect(&samplerTabContextMenuEventFilter,
            SIGNAL(contextMenuRequest(int, int)),
            SLOT(handleSamplerTabContextMenuRequest(int, int)));
    connect(&targetTabContextMenuEventFilter,
            SIGNAL(contextMenuRequest(int, int)),
            SLOT(handleTargetTabContextMenuRequest(int, int)));
    connect(tabWidget, SIGNAL(currentChanged(int)),
            SLOT(handleTabChange(int)));
    tabWidget->widget(TYPE_EFFECT)->
        installEventFilter(&effectTabContextMenuEventFilter);
    tabWidget->widget(TYPE_SAMPLER)->
        installEventFilter(&samplerTabContextMenuEventFilter);
    tabWidget->widget(TYPE_TARGET)->
        installEventFilter(&targetTabContextMenuEventFilter);

    QMenu *targetAddMenu =
        synthclone::getChild<QMenu>(mainWindow, "addTargetMenu");

    targetAddButton =
        synthclone::getChild<QPushButton>(mainWindow, "targetAddButton");
    targetAddButton->setMenu(targetAddMenu);

    targetAddMenuViewlet = new MenuViewlet(targetAddMenu);

    targetMoveDownButton =
        synthclone::getChild<QPushButton>(mainWindow, "targetMoveDownButton");
    connect(targetMoveDownButton, SIGNAL(clicked()),
            SLOT(handleTargetMoveDownButtonClick()));

    targetMoveUpButton =
        synthclone::getChild<QPushButton>(mainWindow, "targetMoveUpButton");
    connect(targetMoveUpButton, SIGNAL(clicked()),
            SLOT(handleTargetMoveUpButtonClick()));

    targetRemoveButton =
        synthclone::getChild<QPushButton>(mainWindow, "targetRemoveButton");
    connect(targetRemoveButton, SIGNAL(clicked()),
            SLOT(handleTargetRemoveButtonClick()));

    targetTableModel.setColumnCount(COMPONENTTABLECOLUMN_TOTAL);
    targetTableModel.setHeaderData(COMPONENTTABLECOLUMN_NAME,
                                   Qt::Horizontal, tr("Name"));
    targetTableModel.setHeaderData(COMPONENTTABLECOLUMN_PROGRESS,
                                   Qt::Horizontal, tr("Progress"));
    targetTableModel.setHeaderData(COMPONENTTABLECOLUMN_STATUS,
                                   Qt::Horizontal, tr("Status"));

    targetTableView =
        synthclone::getChild<QTableView>(mainWindow, "targetTableView");
    targetTableView->setItemDelegateForColumn(COMPONENTTABLECOLUMN_PROGRESS,
                                              &targetProgressBarDelegate);
    targetTableView->setModel(&targetTableModel);
    connect(targetTableView->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            SLOT(handleTargetTableRowChange(QModelIndex)));

    emitSelectedEffectChange = true;
    emitSelectedTargetChange = true;
    samplerActive = false;
}

ComponentViewlet::~ComponentViewlet()
{
    // Empty
}

void
ComponentViewlet::addEffect(int index)
{
    int count = effectMenuViewlets.count();
    assert((index >= 0) && (index <= count));
    bool inserted = effectTableModel.insertRow(index);
    assert(inserted);
    setupColumn(effectTableModel, index, COMPONENTTABLECOLUMN_NAME);
    setupColumn(effectTableModel, index, COMPONENTTABLECOLUMN_PROGRESS);
    setupColumn(effectTableModel, index, COMPONENTTABLECOLUMN_STATUS);
    updateEffectButtons();

    QMenu *effectMenu = new QMenu();
    MenuViewlet *effectMenuViewlet = new MenuViewlet(effectMenu);
    QAction *beforeAction = index == count ? 0 :
        menuViewletMap.value(effectMenuViewlets[index], 0)->menuAction();
    effectRootMenu->insertMenu(beforeAction, effectMenu);
    effectMenuViewlets.insert(index, effectMenuViewlet);
    menuViewletMap.insert(effectMenuViewlet, effectMenu);
}

void
ComponentViewlet::addSampler()
{
    samplerName->setText("");
    samplerProgressBar->setValue(0);
    samplerStatus->setText(tr("Idle."));
    samplerAddButton->setIcon(QIcon(":/synthclone/images/16x16/remove.png"));
    samplerAddButton->setMenu(0);
    samplerAddButton->setText(tr("Remove"));
    connect(samplerAddButton, SIGNAL(clicked()),
            SIGNAL(samplerRemoveRequest()));
    samplerActive = true;
    samplerMenuViewlet->setText("");
}

void
ComponentViewlet::addTarget(int index)
{
    int count = targetMenuViewlets.count();
    assert((index >= 0) && (index <= count));
    bool inserted = targetTableModel.insertRow(index);
    assert(inserted);
    setupColumn(targetTableModel, index, COMPONENTTABLECOLUMN_NAME);
    setupColumn(targetTableModel, index, COMPONENTTABLECOLUMN_PROGRESS);
    setupColumn(targetTableModel, index, COMPONENTTABLECOLUMN_STATUS);
    updateTargetButtons();

    QMenu *targetMenu = new QMenu();
    MenuViewlet *targetMenuViewlet = new MenuViewlet(targetMenu);
    QAction *beforeAction = index == count ? 0 :
        menuViewletMap.value(targetMenuViewlets[index], 0)->menuAction();
    targetRootMenu->insertMenu(beforeAction, targetMenu);
    targetMenuViewlets.insert(index, targetMenuViewlet);
    menuViewletMap.insert(targetMenuViewlet, targetMenu);
}

MenuViewlet *
ComponentViewlet::getEffectAddMenuViewlet()
{
    return effectAddMenuViewlet;
}

MenuViewlet *
ComponentViewlet::getEffectMenuViewlet(int index)
{
    return effectMenuViewlets[index];
}

MenuViewlet *
ComponentViewlet::getSamplerAddMenuViewlet()
{
    return samplerAddMenuViewlet;
}

MenuViewlet *
ComponentViewlet::getSamplerMenuViewlet()
{
    return samplerMenuViewlet;
}

int
ComponentViewlet::getSelectedEffect()
{
    QModelIndex index = effectTableView->currentIndex();
    return index.isValid() ? index.row() : -1;
}

int
ComponentViewlet::getSelectedTarget()
{
    QModelIndex index = targetTableView->currentIndex();
    return index.isValid() ? index.row() : -1;
}

MenuViewlet *
ComponentViewlet::getTargetAddMenuViewlet()
{
    return targetAddMenuViewlet;
}

MenuViewlet *
ComponentViewlet::getTargetMenuViewlet(int index)
{
    return targetMenuViewlets[index];
}

void
ComponentViewlet::handleEffectMoveDownButtonClick()
{
    int currentRow = getSelectedEffect();
    emit effectMoveRequest(currentRow, currentRow + 1);
}

void
ComponentViewlet::handleEffectMoveUpButtonClick()
{
    int currentRow = getSelectedEffect();
    emit effectMoveRequest(currentRow, currentRow - 1);
}

void
ComponentViewlet::handleEffectRemoveButtonClick()
{
    emit effectRemoveRequest(getSelectedEffect());
}

void
ComponentViewlet::handleEffectTabContextMenuRequest(int x, int y)
{
    effectRootMenu->
        popup(tabWidget->widget(TYPE_EFFECT)->mapToGlobal(QPoint(x, y)));
}

void
ComponentViewlet::handleEffectTableRowChange(const QModelIndex &index)
{
    if (emitSelectedEffectChange) {
        emit selectedEffectChangeRequest(index.isValid() ? index.row() : -1);
    }
}

void
ComponentViewlet::handleSamplerTabContextMenuRequest(int x, int y)
{
    samplerRootMenu->
        popup(tabWidget->widget(TYPE_SAMPLER)->mapToGlobal(QPoint(x, y)));
}

void
ComponentViewlet::handleTabChange(int tabIndex)
{
    int index;
    Type type = static_cast<Type>(tabIndex);
    switch (type) {
    case TYPE_EFFECT:
        index = getSelectedEffect();
        break;
    case TYPE_SAMPLER:
        index = samplerActive ? 0 : -1;
        break;
    case TYPE_TARGET:
        index = getSelectedTarget();
        break;
    default:
        assert(false);
    }
    if (index == -1) {
        type = TYPE_NONE;
    }
    emit focusedComponentChangeRequest(type, index);
}

void
ComponentViewlet::handleTargetMoveDownButtonClick()
{
    int currentRow = getSelectedTarget();
    emit targetMoveRequest(currentRow, currentRow + 1);
}

void
ComponentViewlet::handleTargetMoveUpButtonClick()
{
    int currentRow = getSelectedTarget();
    emit targetMoveRequest(currentRow, currentRow - 1);
}

void
ComponentViewlet::handleTargetRemoveButtonClick()
{
    emit targetRemoveRequest(getSelectedTarget());
}

void
ComponentViewlet::handleTargetTabContextMenuRequest(int x, int y)
{
    targetRootMenu->
        popup(tabWidget->widget(TYPE_TARGET)->mapToGlobal(QPoint(x, y)));
}


void
ComponentViewlet::handleTargetTableRowChange(const QModelIndex &index)
{
    if (emitSelectedTargetChange) {
        emit selectedTargetChangeRequest(index.isValid() ? index.row() : -1);
    }
}

void
ComponentViewlet::moveEffect(int fromIndex, int toIndex)
{
    int selectedIndex = getSelectedEffect();
    int count = effectMenuViewlets.count();
    effectTableModel.insertRow(toIndex, effectTableModel.takeRow(fromIndex));
    if (selectedIndex == fromIndex) {
        emitSelectedEffectChange = false;
        setSelectedEffect(toIndex);
        emitSelectedEffectChange = true;
    }
    updateEffectButtons();

    MenuViewlet *effectMenuViewlet = effectMenuViewlets.takeAt(fromIndex);
    QMenu *effectMenu = menuViewletMap.value(effectMenuViewlet);
    effectRootMenu->removeAction(effectMenu->menuAction());
    QAction *beforeAction = toIndex == count ? 0 :
        menuViewletMap.value(effectMenuViewlets[toIndex], 0)->menuAction();
    effectRootMenu->insertMenu(beforeAction, effectMenu);
    effectMenuViewlets.insert(toIndex, effectMenuViewlet);
}

void
ComponentViewlet::moveTarget(int fromIndex, int toIndex)
{
    int selectedIndex = getSelectedTarget();
    int count = targetMenuViewlets.count();
    targetTableModel.insertRow(toIndex, targetTableModel.takeRow(fromIndex));
    if (selectedIndex == fromIndex) {
        emitSelectedTargetChange = false;
        setSelectedTarget(toIndex);
        emitSelectedTargetChange = true;
    }
    updateTargetButtons();

    MenuViewlet *targetMenuViewlet = targetMenuViewlets.takeAt(fromIndex);
    QMenu *targetMenu = menuViewletMap.value(targetMenuViewlet);
    targetRootMenu->removeAction(targetMenu->menuAction());
    QAction *beforeAction = toIndex == count ? 0 :
        menuViewletMap.value(targetMenuViewlets[toIndex], 0)->menuAction();
    targetRootMenu->insertMenu(beforeAction, targetMenu);
    targetMenuViewlets.insert(toIndex, targetMenuViewlet);
}

void
ComponentViewlet::removeEffect(int index)
{
    bool removed = effectTableModel.removeRow(index);
    assert(removed);
    updateEffectButtons();

    MenuViewlet *effectMenuViewlet = effectMenuViewlets.takeAt(index);
    QMenu *effectMenu = menuViewletMap.take(effectMenuViewlet);
    effectRootMenu->removeAction(effectMenu->menuAction());
    effectMenuViewlet->deleteLater();
    effectMenu->deleteLater();
}

void
ComponentViewlet::removeSampler()
{
    samplerName->setText("(sampler not set)");
    samplerProgressBar->setValue(0);
    samplerStatus->setText("");
    samplerAddButton->setIcon(QIcon(":/synthclone/images/16x16/add.png"));
    samplerAddButton->setMenu(samplerAddMenu);
    samplerAddButton->setText(tr("Add"));
    disconnect(samplerAddButton, SIGNAL(clicked()),
               this, SIGNAL(samplerRemoveRequest()));
    samplerActive = false;
}

void
ComponentViewlet::removeTarget(int index)
{
    bool removed = targetTableModel.removeRow(index);
    assert(removed);
    updateTargetButtons();

    MenuViewlet *targetMenuViewlet = targetMenuViewlets.takeAt(index);
    QMenu *targetMenu = menuViewletMap.take(targetMenuViewlet);
    targetRootMenu->removeAction(targetMenu->menuAction());
    targetMenuViewlet->deleteLater();
    targetMenu->deleteLater();
}

void
ComponentViewlet::setCurrentSamplerJob(bool enabled)
{
    samplerJobAbortButton->setEnabled(enabled);
}

void
ComponentViewlet::setEffectEditingEnabled(bool enabled)
{
    effectAddButton->setEnabled(enabled);
    effectRootMenu->setEnabled(enabled);
    if (! enabled) {
        effectMoveDownButton->setEnabled(false);
        effectMoveUpButton->setEnabled(false);
        effectRemoveButton->setEnabled(false);
    } else {
        updateEffectButtons();
    }
}

void
ComponentViewlet::setEffectName(int index, const QString &name)
{
    setModelData(effectTableModel, index, COMPONENTTABLECOLUMN_NAME, name);
    effectMenuViewlets[index]->setText(name);
}

void
ComponentViewlet::setEffectProgress(int index, float value)
{
    setModelData(effectTableModel, index, COMPONENTTABLECOLUMN_PROGRESS,
                 value == -1 ? QVariant() : value, Qt::EditRole);
}

void
ComponentViewlet::setEffectStatus(int index, const QString &status)
{
    setModelData(effectTableModel, index, COMPONENTTABLECOLUMN_STATUS, status);
}

void
ComponentViewlet::setFocusedComponent(Type type, int index)
{
    switch (type) {
    case TYPE_EFFECT:
        effectTableView->selectRow(index);
        // Fallthrough on purpose
    case TYPE_SAMPLER:
        break;
    case TYPE_TARGET:
        targetTableView->selectRow(index);
        break;
    case TYPE_NONE:
        return;
    }
    tabWidget->setCurrentIndex(type);
}

void
ComponentViewlet::setModelData(QStandardItemModel &model, int row, int column,
                               const QVariant &value, int role)
{
    QModelIndex index = model.index(row, column);
    assert(index.isValid());
    bool result = model.setData(index, value, role);
    assert(result);
}

void
ComponentViewlet::setSamplerEditingEnabled(bool enabled)
{
    samplerAddButton->setEnabled(enabled);
    samplerRootMenu->setEnabled(enabled);
}

void
ComponentViewlet::setSamplerName(const QString &name)
{
    samplerName->setText(name);
    samplerMenuViewlet->setText(name);
}

void
ComponentViewlet::setSamplerProgress(float value)
{
    samplerProgressBar->setValue(static_cast<int>(value * 10000.0));
}

void
ComponentViewlet::setSamplerStatus(const QString &status)
{
    samplerStatus->setText(status);
}

void
ComponentViewlet::setSelectedEffect(int index)
{
    effectTableView->selectRow(index);
    updateEffectButtons();
}

void
ComponentViewlet::setSelectedTarget(int index)
{
    targetTableView->selectRow(index);
    updateTargetButtons();
}

void
ComponentViewlet::setTargetName(int index, const QString &name)
{
    setModelData(targetTableModel, index, COMPONENTTABLECOLUMN_NAME, name);
    targetMenuViewlets[index]->setText(name);
}

void
ComponentViewlet::setTargetProgress(int index, float value)
{
    setModelData(targetTableModel, index, COMPONENTTABLECOLUMN_PROGRESS,
                 value == -1 ? QVariant() : value, Qt::EditRole);
}

void
ComponentViewlet::setTargetStatus(int index, const QString &status)
{
    setModelData(targetTableModel, index, COMPONENTTABLECOLUMN_STATUS, status);
}

void
ComponentViewlet::setupColumn(QStandardItemModel &model, int row, int column)
{
    QModelIndex index = model.index(row, column);
    QStandardItem *item = model.itemFromIndex(index);
    assert(item);
    item->setFlags(item->flags() & (~ Qt::ItemIsEditable));
}

void
ComponentViewlet::updateEffectButtons()
{
    int index = getSelectedEffect();
    bool selected = index != -1;
    effectRemoveButton->setEnabled(selected);
    if (! selected) {
        effectMoveDownButton->setEnabled(false);
        effectMoveUpButton->setEnabled(false);
    } else {
        effectMoveDownButton->
            setEnabled(index != (effectTableModel.rowCount() - 1));
        effectMoveUpButton->setEnabled(static_cast<bool>(index));
    }
}

void
ComponentViewlet::updateTargetButtons()
{
    int index = getSelectedTarget();
    bool selected = index != -1;
    targetRemoveButton->setEnabled(selected);
    if (! selected) {
        targetMoveDownButton->setEnabled(false);
        targetMoveUpButton->setEnabled(false);
    } else {
        targetMoveDownButton->
            setEnabled(index != (targetTableModel.rowCount() - 1));
        targetMoveUpButton->setEnabled(static_cast<bool>(index));
    }
}
