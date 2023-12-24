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

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <synthclone/fileselectionview.h>

#include "aboutview.h"
#include "application.h"
#include "errorview.h"
#include "mainview.h"
#include "menumanager.h"
#include "participantmanager.h"
#include "participantview.h"
#include "pluginmanager.h"
#include "progressview.h"
#include "savechangesview.h"
#include "savewarningview.h"
#include "session.h"
#include "sessionloadview.h"
#include "settings.h"

class Controller: public QObject {

    Q_OBJECT

public:

    explicit
    Controller(Application &application, QObject *parent=0);

    ~Controller();

    MainView &
    getMainView();

    MenuManager &
    getMenuManager();

    Session &
    getSession();

public slots:

    void
    quit();

    void
    reportError(const QString &message);

    void
    run(const QDir *sessionDirectory);

signals:

    void
    errorReported(const QString &message);

private slots:

    void
    handleAboutViewCloseRequest();

    void
    handleClipboardDataChange();

    void
    handleComponentViewletFocusedChangeRequest(ComponentViewlet::Type type,
                                               int index);

    void
    handleDirectoryViewCloseRequest();

    void
    handleDirectoryViewPathSelection(const QStringList &paths);

    void
    handleEffectNameChange(const QString &name);

    void
    handleEffectProgressChange(float progress);

    void
    handleEffectStatusChange(const QString &status);

    void
    handleErrorViewCloseRequest();

    void
    handleHelpViewletAboutRequest();

    void
    handleMainViewCloseRequest();

    void
    handleParticipantManagerParticipantActivation
    (const synthclone::Participant *participant,
     const synthclone::Participant *parent, const QByteArray &id);

    void
    handleParticipantManagerParticipantAddition(const synthclone::Participant *
                                                participant,
                                                const synthclone::Participant *
                                                parent,
                                                const QByteArray &id);

    void
    handleParticipantManagerParticipantDeactivation
    (const synthclone::Participant *participant,
     const synthclone::Participant *parent, const QByteArray &id);

    void
    handleParticipantManagerParticipantRemoval(const synthclone::Participant *
                                               participant,
                                               const synthclone::Participant *
                                               parent,
                                               const QByteArray &id);

    void
    handleParticipantUnregistration(QObject *obj);

    void
    handleParticipantViewCloseRequest();

    void
    handleParticipantViewletActivationChangeRequest(bool activate);

    void
    handleProgressViewCloseRequest();

    void
    handleSamplerNameChange(const QString &name);

    void
    handleSamplerProgressChange(float progress);

    void
    handleSamplerStatusChange(const QString &status);

    void
    handleSaveChangesViewCloseRequest();

    void
    handleSaveChangesViewDiscardRequest();

    void
    handleSaveChangesViewSaveRequest();

    void
    handleSaveWarningViewCloseRequest();

    void
    handleSaveWarningViewSaveRequest();

    void
    handleSessionCurrentEffectJobChange(const synthclone::EffectJob *job);

    void
    handleSessionCurrentSamplerJobChange(const synthclone::SamplerJob *job);

    void
    handleSessionEffectAddition(const synthclone::Effect *effect, int index);

    void
    handleSessionEffectMove(const synthclone::Effect *effect, int fromIndex,
                            int toIndex);

    void
    handleSessionEffectRemoval(const synthclone::Effect *effect, int index);

    void
    handleSessionFocusedComponentChange(const synthclone::Component *component);

    void
    handleSessionLoadWarning(int line, int column, const QString &message);

    void
    handleSessionProgressChange(float progress, const QString &status);

    void
    handleSessionSamplerAddition(const synthclone::Sampler *sampler);

    void
    handleSessionSamplerRemoval(const synthclone::Sampler *sampler);

    void
    handleSessionSelectedEffectChange(const synthclone::Effect *effect,
                                      int index);

    void
    handleSessionSelectedTargetChange(const synthclone::Target *target,
                                      int index);

    void
    handleSessionStateChange(synthclone::SessionState state,
                             const QDir *directory);

    void
    handleSessionTargetAddition(const synthclone::Target *target, int index);

    void
    handleSessionTargetBuild(const synthclone::Target *target);

    void
    handleSessionTargetBuildCompletion(const synthclone::Target *target);

    void
    handleSessionTargetBuildError(const synthclone::Target *target,
                                  const QString &message);

    void
    handleSessionTargetBuildOperation();

    void
    handleSessionTargetBuildOperationCompletion();

    void
    handleSessionTargetMove(const synthclone::Target *target, int fromIndex,
                            int toIndex);

    void
    handleSessionTargetRemoval(const synthclone::Target *target, int index);

    void
    handleSessionZoneAddition(synthclone::Zone *zone, int index);

    void
    handleSessionZoneMove(synthclone::Zone *zone, int fromIndex, int toIndex);

    void
    handleSessionZoneRemoval(synthclone::Zone *zone, int index);

    void
    handleSessionZoneSelectionChange(synthclone::Zone *zone, int index,
                                     bool selected);

    void
    handleSessionLoadViewCloseRequest();

    void
    handleSessionLoadViewCreationDirectoryBrowseRequest(const QString &path,
                                                        const QString &name);

    void
    handleSessionLoadViewCreationDirectoryChange(const QString &directory,
                                                 const QString &name);

    void
    handleSessionLoadViewCreationRequest(const QString &directory,
                                         const QString &name,
                                         synthclone::SampleRate sampleRate,
                                         synthclone::SampleChannelCount count);

    void
    handleSessionLoadViewOpenDirectoryBrowseRequest(const QString &path);

    void
    handleSessionLoadViewOpenDirectoryChange(const QString &directory);

    void
    handleSessionLoadViewOpenRequest(const QString &directory);

    void
    handleSessionViewletLoadRequest();

    void
    handleSessionViewletQuitRequest();

    void
    handleSessionViewletSaveAsRequest();

    void
    handleTargetBuildProgressChange(float progress);

    void
    handleTargetBuildStatusChange(const QString &status);

    void
    handleTargetBuildWarning(const QString &message);

    void
    handleTargetNameChange(const QString &name);

    void
    handleTargetProgressChange(float progress);

    void
    handleTargetStatusChange(const QString &status);

    void
    handleViewViewletParticipantEditRequest();

    void
    handleZoneAftertouchChange(synthclone::MIDIData aftertouch);

    void
    handleZoneChannelChange(synthclone::MIDIData channel);

    void
    handleZoneChannelPressureChange(synthclone::MIDIData pressure);

    void
    handleZoneControlValueChange(synthclone::MIDIData control,
                                 synthclone::MIDIData value);

    void
    handleZoneDrySampleChange(const synthclone::Sample *sample);

    void
    handleZoneDrySampleStaleChange(bool stale);

    void
    handleZoneNoteChange(synthclone::MIDIData note);

    void
    handleZoneReleaseTimeChange(synthclone::SampleTime releaseTime);

    void
    handleZoneSampleTimeChange(synthclone::SampleTime sampleTime);

    void
    handleZoneStatusChange(synthclone::Zone::Status status);

    void
    handleZoneVelocityChange(synthclone::MIDIData velocity);

    void
    handleZoneWetSampleChange(const synthclone::Sample *sample);

    void
    handleZoneWetSampleStaleChange(bool stale);

    void
    handleZoneListLoaderWarning(int line, int column, const QString &message);

    void
    handleZoneViewletAftertouchChangeRequest(int index,
                                             synthclone::MIDIData aftertouch);

    void
    handleZoneViewletAftertouchPropertySortRequest(bool ascending);

    void
    handleZoneViewletApplyEffectsRequest();

    void
    handleZoneViewletChannelChangeRequest(int index,
                                          synthclone::MIDIData channel);

    void
    handleZoneViewletChannelPressureChangeRequest
    (int index, synthclone::MIDIData pressure);

    void
    handleZoneViewletChannelPressurePropertySortRequest(bool ascending);

    void
    handleZoneViewletChannelPropertySortRequest(bool ascending);

    void
    handleZoneViewletClearEffectJobsRequest();

    void
    handleZoneViewletClearSamplerJobsRequest();

    void
    handleZoneViewletClearSelectionRequest();

    void
    handleZoneViewletControlPropertySortRequest(synthclone::MIDIData control,
                                                bool ascending);

    void
    handleZoneViewletControlValueChangeRequest(int index,
                                               synthclone::MIDIData control,
                                               synthclone::MIDIData value);

    void
    handleZoneViewletCopyRequest();

    void
    handleZoneViewletCutRequest();

    void
    handleZoneViewletDeleteRequest();

    void
    handleZoneViewletDrySamplePropertySortRequest(bool ascending);

    void
    handleZoneViewletInsertRequest();

    void
    handleZoneViewletInvertSelectionRequest();

    void
    handleZoneViewletNoteChangeRequest(int index, synthclone::MIDIData note);

    void
    handleZoneViewletNotePropertySortRequest(bool ascending);

    void
    handleZoneViewletPasteRequest();

    void
    handleZoneViewletPlayDrySampleRequest();

    void
    handleZoneViewletPlayWetSampleRequest();

    void
    handleZoneViewletReleaseTimeChangeRequest(int index,
                                              synthclone::SampleTime
                                              releaseTime);

    void
    handleZoneViewletReleaseTimePropertySortRequest(bool ascending);

    void
    handleZoneViewletRemoveEffectJobRequest();

    void
    handleZoneViewletRemoveSamplerJobRequest();

    void
    handleZoneViewletSampleRequest();

    void
    handleZoneViewletSampleTimeChangeRequest(int index,
                                             synthclone::SampleTime sampleTime);

    void
    handleZoneViewletSampleTimePropertySortRequest(bool ascending);

    void
    handleZoneViewletSelectAllRequest();

    void
    handleZoneViewletStatusPropertySortRequest(bool ascending);

    void
    handleZoneViewletVelocityChangeRequest(int index,
                                           synthclone::MIDIData velocity);

    void
    handleZoneViewletVelocityPropertySortRequest(bool ascending);

    void
    handleZoneViewletWetSamplePropertySortRequest(bool ascending);

private:

    enum PostDirectorySelectAction {
        POSTDIRECTORYSELECTACTION_CREATE_SESSION,
        POSTDIRECTORYSELECTACTION_OPEN_SESSION,
        POSTDIRECTORYSELECTACTION_SAVE_SESSION
    };

    enum PostSaveChangesAction {
        POSTSAVECHANGESACTION_LOAD,
        POSTSAVECHANGESACTION_QUIT
    };

    typedef QMap<synthclone::Participant *,
                 synthclone::IPlugin *> PluginParticipantMap;
    typedef QMap<const synthclone::Participant *,
                 ParticipantViewlet *> ParticipantViewletMap;

    void
    clearProgressView();

    void
    copySelectedZones();

    void
    executePostSaveChangesAction();

    QDir
    getCorePluginDirectory();

    bool
    loadClipboardZoneList(QDomDocument &document);

    void
    loadPlugins(const QDir &directory, QStringList &scannedPaths);

    void
    processQuitRequest();

    void
    refreshZoneBuildTargetsAction();

    void
    refreshZoneViewletActions();

    void
    removeDirectoryContents(QDir &directory);

    void
    removeSelectedZones();

    void
    setSessionLoadViewCreationDefaults();

    void
    setSessionLoadViewOpenDefaults();

    void
    showSessionLoadView();

    void
    updateZoneViewlet(const synthclone::Zone *zone, int index);

    bool
    verifySessionLoadViewCreation(const QString &path, const QString &name);

    bool
    verifySessionLoadViewOpen(const QString &path);

    Application &application;
    QString createSessionName;
    synthclone::SessionState lastSessionState;
    ParticipantViewletMap participantViewletMap;
    PluginManager pluginManager;
    PluginParticipantMap pluginParticipantMap;
    PostDirectorySelectAction postDirectorySelectAction;
    PostSaveChangesAction postSaveChangesAction;
    float sampleProfile[2048];
    QString saveAsPath;
    int sessionLoadWarningCount;
    int targetBuildWarningCount;

    ParticipantManager participantManager;

    Session session;
    Settings settings;

    AboutView aboutView;
    ErrorView errorView;
    synthclone::FileSelectionView directoryView;
    MainView mainView;
    ParticipantView participantView;
    ProgressView progressView;
    SaveChangesView saveChangesView;
    SaveWarningView saveWarningView;
    SessionLoadView sessionLoadView;

    MenuManager menuManager;

};

#endif
