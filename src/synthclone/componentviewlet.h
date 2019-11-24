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

#ifndef __COMPONENTVIEWLET_H__
#define __COMPONENTVIEWLET_H__

#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QTabWidget>

#include "contextmenueventfilter.h"
#include "menuviewlet.h"
#include "progressbardelegate.h"

class ComponentViewlet: public QObject {

    Q_OBJECT

public:

    // Constants apply to tab indexes.
    enum Type {
        TYPE_NONE = -1,
        TYPE_EFFECT = 1,
        TYPE_SAMPLER = 0,
        TYPE_TARGET = 2
    };

    explicit
    ComponentViewlet(QMainWindow *mainWindow, QObject *parent=0);

    ~ComponentViewlet();

    MenuViewlet *
    getEffectAddMenuViewlet();

    MenuViewlet *
    getEffectMenuViewlet(int index);

    MenuViewlet *
    getSamplerAddMenuViewlet();

    MenuViewlet *
    getSamplerMenuViewlet();

    MenuViewlet *
    getTargetAddMenuViewlet();

    MenuViewlet *
    getTargetMenuViewlet(int index);

public slots:

    void
    addEffect(int index);

    void
    addSampler();

    void
    addTarget(int index);

    void
    moveEffect(int fromIndex, int toIndex);

    void
    moveTarget(int fromIndex, int toIndex);

    void
    removeEffect(int index);

    void
    removeSampler();

    void
    removeTarget(int index);

    void
    setCurrentSamplerJob(bool enabled);

    void
    setEffectEditingEnabled(bool enabled);

    void
    setEffectName(int index, const QString &name);

    void
    setEffectProgress(int index, float value);

    void
    setEffectStatus(int index, const QString &status);

    void
    setFocusedComponent(Type type, int index);

    void
    setSamplerEditingEnabled(bool enabled);

    void
    setSamplerName(const QString &name);

    void
    setSamplerProgress(float value);

    void
    setSamplerStatus(const QString &status);

    void
    setSelectedEffect(int index);

    void
    setSelectedTarget(int index);

    void
    setTargetName(int index, const QString &name);

    void
    setTargetProgress(int index, float value);

    void
    setTargetStatus(int index, const QString &status);

signals:

    void
    effectMoveRequest(int fromIndex, int toIndex);

    void
    effectRemoveRequest(int index);

    void
    focusedComponentChangeRequest(ComponentViewlet::Type type, int index);

    void
    samplerJobAbortRequest();

    void
    samplerRemoveRequest();

    void
    selectedEffectChangeRequest(int index=-1);

    void
    selectedTargetChangeRequest(int index=-1);

    void
    targetMoveRequest(int fromIndex, int toIndex);

    void
    targetRemoveRequest(int index);

private slots:

    void
    handleEffectMoveDownButtonClick();

    void
    handleEffectMoveUpButtonClick();

    void
    handleEffectRemoveButtonClick();

    void
    handleEffectTabContextMenuRequest(int x, int y);

    void
    handleEffectTableRowChange(const QModelIndex &index);

    void
    handleSamplerTabContextMenuRequest(int x, int y);

    void
    handleTabChange(int index);

    void
    handleTargetMoveDownButtonClick();

    void
    handleTargetMoveUpButtonClick();

    void
    handleTargetRemoveButtonClick();

    void
    handleTargetTabContextMenuRequest(int x, int y);

    void
    handleTargetTableRowChange(const QModelIndex &index);

private:

    typedef QMap<MenuViewlet *, QMenu *> MenuViewletMap;

    enum ComponentTableColumn {
        COMPONENTTABLECOLUMN_NAME = 0,
        COMPONENTTABLECOLUMN_PROGRESS = 1,
        COMPONENTTABLECOLUMN_STATUS = 2,

        COMPONENTTABLECOLUMN_TOTAL = 3
    };

    int
    getSelectedEffect();

    int
    getSelectedTarget();

    void
    setModelData(QStandardItemModel &model, int row, int column,
                 const QVariant &value, int role=Qt::DisplayRole);

    void
    setupColumn(QStandardItemModel &model, int row, int column);

    void
    updateEffectButtons();

    void
    updateTargetButtons();

    QMenu *componentMenu;
    MenuViewletMap menuViewletMap;
    QTabWidget *tabWidget;

    QPushButton *effectAddButton;
    MenuViewlet *effectAddMenuViewlet;
    QList<MenuViewlet *> effectMenuViewlets;
    QPushButton *effectMoveDownButton;
    QPushButton *effectMoveUpButton;
    ProgressBarDelegate effectProgressBarDelegate;
    QPushButton *effectRemoveButton;
    QMenu *effectRootMenu;
    ContextMenuEventFilter effectTabContextMenuEventFilter;
    QStandardItemModel effectTableModel;
    QTableView *effectTableView;

    bool samplerActive;
    QPushButton *samplerAddButton;
    QMenu *samplerAddMenu;
    MenuViewlet *samplerAddMenuViewlet;
    QPushButton *samplerJobAbortButton;
    MenuViewlet *samplerMenuViewlet;
    QLabel *samplerName;
    QProgressBar *samplerProgressBar;
    QMenu *samplerRootMenu;
    QLabel *samplerStatus;
    ContextMenuEventFilter samplerTabContextMenuEventFilter;

    QPushButton *targetAddButton;
    MenuViewlet *targetAddMenuViewlet;
    QList<MenuViewlet *> targetMenuViewlets;
    QPushButton *targetMoveDownButton;
    QPushButton *targetMoveUpButton;
    ProgressBarDelegate targetProgressBarDelegate;
    QPushButton *targetRemoveButton;
    QMenu *targetRootMenu;
    ContextMenuEventFilter targetTabContextMenuEventFilter;
    QStandardItemModel targetTableModel;
    QTableView *targetTableView;

    bool emitSelectedEffectChange;
    bool emitSelectedTargetChange;

};

#endif
