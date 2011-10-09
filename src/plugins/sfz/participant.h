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

#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <synthclone/fileselectionview.h>
#include <synthclone/participant.h>

#include "target.h"
#include "targetview.h"

class Participant: public synthclone::Participant {

    Q_OBJECT

public:

    explicit
    Participant(QObject *parent=0);

    ~Participant();

    void
    activate(synthclone::Context &context, const QVariant &state=QVariant());

    void
    deactivate(synthclone::Context &context);

    QVariant
    getState(const synthclone::Target *target) const;

    void
    restoreTarget(const QVariant &state);

private slots:

    void
    handleControlLayerCrossfadingEnabledChange(bool enabled);

    void
    handleControlLayerDefaultValueChange(synthclone::MIDIData value);

    void
    handleControlLayerTypeChange(synthclone::ControlType type);

    void
    handleDirectoryViewCloseRequest();

    void
    handleDirectoryViewPathSelection(const QStringList &paths);

    void
    handleTargetAddition();

    void
    handleTargetConfiguration();

    void
    handleTargetControlLayerAddition(const ControlLayer *layer, int index);

    void
    handleTargetControlLayerMove(const ControlLayer *layer, int fromIndex,
                                 int toIndex);

    void
    handleTargetControlLayerRemoval(const ControlLayer *layer, int index);

    void
    handleTargetViewCloseRequest();

    void
    handleTargetViewCrossfadingEnabledChangeRequest(int index, bool enabled);

    void
    handleTargetViewDefaultValueChangeRequest(int index,
                                              synthclone::MIDIData value);

    void
    handleTargetViewTypeChangeRequest(int index, synthclone::ControlType type);

    void
    handleTargetViewPathLookupRequest();

private:

    void
    addControlLayer(int index, const ControlLayer *layer);

    Target *
    addTarget();

    void
    configureTarget(Target *target);

    CrossfadeCurve
    getCrossfadeCurveConstant(const QString &str) const;

    QString
    getCrossfadeCurveString(CrossfadeCurve curve) const;

    void
    removeControlLayer(int index, const ControlLayer *layer);

    synthclone::MenuAction addTargetAction;
    Target *configuredTarget;
    synthclone::Context *context;
    synthclone::FileSelectionView directoryView;
    TargetView targetView;

};

#endif
