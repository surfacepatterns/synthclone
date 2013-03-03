/*
 * libsynthclone_hydrogen - Hydrogen target plugin for `synthclone`
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

#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <QtCore/QMap>

#include <synthclone/fileselectionview.h>
#include <synthclone/participant.h>

#include "importer.h"
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
    handleDirectoryViewCloseRequest();

    void
    handleDirectoryViewPathSelection(const QStringList &paths);

    void
    handleImportArchiveRequest();

    void
    handleImportKitRequest();

    void
    handleImportViewCloseRequest();

    void
    handleImportViewPathSelection(const QStringList &paths);

    void
    handleLayerImport(synthclone::MIDIData note, synthclone::MIDIData velocity,
                      synthclone::SampleTime time, synthclone::Sample &sample);

    void
    handleTargetAddition();

    void
    handleTargetConfiguration();

    void
    handleTargetViewCloseRequest();

    void
    handleTargetViewPathLookupRequest();

    void
    handleUnregistration(QObject *obj);

private:

    Target *
    addTarget();

    void
    configureTarget(Target *target);

    synthclone::MenuAction addTargetAction;
    Target *configuredTarget;
    synthclone::Context *context;
    synthclone::FileSelectionView directoryView;
    synthclone::MenuAction importArchiveAction;
    synthclone::MenuAction importKitAction;
    Importer importer;
    synthclone::FileSelectionView importView;
    TargetView targetView;

};

#endif
