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

#ifndef __TARGETVIEW_H__
#define __TARGETVIEW_H__

#include <QtCore/QMap>
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>

#include <synthclone/designerview.h>

#include "controllayerdelegate.h"
#include "types.h"

class TargetView: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    TargetView(QObject *parent=0);

    ~TargetView();

public slots:

    void
    addControlLayer(int index, synthclone::MIDIData control);

    void
    moveControlLayer(int fromIndex, int toIndex);

    void
    removeControlLayer(int index, synthclone::MIDIData control);

    void
    setControlCrossfadeCurve(CrossfadeCurve curve);

    void
    setControlLayerCrossfadingEnabled(int index, bool enabled);

    void
    setControlLayerDefaultValue(int index, synthclone::MIDIData value);

    void
    setControlLayerType(int index, synthclone::ControlType type);

    void
    setDrumKit(bool drumKit);

    void
    setName(const QString &name);

    void
    setNoteCrossfadeCurve(CrossfadeCurve curve);

    void
    setPath(const QString &path);

    void
    setSampleFormat(SampleFormat format);

    void
    setVelocityCrossfadeCurve(CrossfadeCurve curve);

signals:

    void
    controlCrossfadeCurveChangeRequest(CrossfadeCurve curve);

    void
    controlLayerAddRequest(synthclone::MIDIData control);

    void
    controlLayerCrossfadingEnabledChangeRequest(int index, bool enabled);

    void
    controlLayerDefaultValueChangeRequest(int index,
                                          synthclone::MIDIData value);

    void
    controlLayerMoveRequest(int fromIndex, int toIndex);

    void
    controlLayerRemoveRequest(int index);

    void
    controlLayerTypeChangeRequest(int index, synthclone::ControlType type);

    void
    drumKitChangeRequest(bool drumKit);

    void
    nameChangeRequest(const QString &name);

    void
    noteCrossfadeCurveChangeRequest(CrossfadeCurve curve);

    void
    pathChangeRequest(const QString &path);

    void
    pathLookupRequest();

    void
    sampleFormatChangeRequest(SampleFormat format);

    void
    velocityCrossfadeCurveChangeRequest(CrossfadeCurve curve);

private slots:

    void
    handleControlActionTrigger();

    void
    handleControlCrossfadeCurveIndexChange(int index);

    void
    handleControlLayerTableRowChange();

    void
    handleMoveControlLayerDownButtonClick();

    void
    handleMoveControlLayerUpButtonClick();

    void
    handleNoteCrossfadeCurveIndexChange(int index);

    void
    handleRemoveControlLayerButtonClick();

    void
    handleSampleFormatIndexChange(int index);

    void
    handleVelocityCrossfadeCurveIndexChange(int index);

private:

    int
    getSelectedControlLayer() const;

    void
    setModelData(int row, int column, const QVariant &value,
                 int role=Qt::EditRole);

    void
    updateControlLayerButtons();

    QPushButton *addControlLayerButton;
    QPushButton *closeButton;
    QMap<synthclone::MIDIData, QAction *> controlActionMap;
    QComboBox *controlCrossfadeCurve;
    ControlLayerDelegate controlLayerTableDelegate;
    QStandardItemModel controlLayerTableModel;
    QTableView *controlLayerTableView;
    QCheckBox *drumKit;
    QPushButton *moveControlLayerDownButton;
    QPushButton *moveControlLayerUpButton;
    QLineEdit *name;
    QComboBox *noteCrossfadeCurve;
    QLabel *noteCrossfadeCurveLabel;
    QLineEdit *path;
    QPushButton *pathLookupButton;
    QPushButton *removeControlLayerButton;
    QComboBox *sampleFormat;
    QComboBox *velocityCrossfadeCurve;

};

#endif
