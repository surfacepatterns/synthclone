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

#include <QtCore/QDebug>
#include <QtCore/QDirIterator>
#include <QtCore/QFSFileEngine>
#include <QtCore/QMimeData>
#include <QtGui/QClipboard>

#include <synthclone/error.h>

#include "controller.h"
#include "samplerateconverter.h"
#include "util.h"
#include "zonecomparer.h"
#include "zonelistloader.h"

#define STRINGIFY(s) #s
#define XSTRINGIFY(s) STRINGIFY(s)

static const char *PLUGIN_PATH = XSTRINGIFY(SYNTHCLONE_PLUGIN_PATH);

Controller::Controller(Application &application, QObject *parent):
    QObject(parent),
    application(application),
    participantManager(*this),
    session(participantManager),
    settings(session),
    menuManager(mainView, session)
{
    // Setup views

    connect(&aboutView, SIGNAL(closeRequest()),
            SLOT(handleAboutViewCloseRequest()));
    aboutView.setMajorVersion(session.getMajorVersion());
    aboutView.setMinorVersion(session.getMinorVersion());
    aboutView.setRevision(session.getRevision());

    directoryView.setFilesVisible(false);
    connect(&directoryView, SIGNAL(closeRequest()),
            SLOT(handleDirectoryViewCloseRequest()));
    connect(&directoryView, SIGNAL(pathsSelected(QStringList)),
            SLOT(handleDirectoryViewPathSelection(QStringList)));

    connect(&errorView, SIGNAL(closeRequest()),
            SLOT(handleErrorViewCloseRequest()));

    connect(&mainView, SIGNAL(closeRequest()),
            SLOT(handleMainViewCloseRequest()));

    connect(&participantView, SIGNAL(closeRequest()),
            SLOT(handleParticipantViewCloseRequest()));

    connect(&progressView, SIGNAL(closeRequest()),
            SLOT(handleProgressViewCloseRequest()));

    connect(&saveChangesView, SIGNAL(closeRequest()),
            SLOT(handleSaveChangesViewCloseRequest()));
    connect(&saveChangesView, SIGNAL(discardRequest()),
            SLOT(handleSaveChangesViewDiscardRequest()));
    connect(&saveChangesView, SIGNAL(saveRequest()),
            SLOT(handleSaveChangesViewSaveRequest()));

    connect(&saveWarningView, SIGNAL(closeRequest()),
            SLOT(handleSaveWarningViewCloseRequest()));
    connect(&saveWarningView, SIGNAL(saveRequest()),
            SLOT(handleSaveWarningViewSaveRequest()));

    connect(&sessionLoadView, SIGNAL(closeRequest()), SLOT(quit()));
    connect(&sessionLoadView,
            SIGNAL(creationDirectoryBrowseRequest(QString, QString)),
            SLOT(handleSessionLoadViewCreationDirectoryBrowseRequest
                 (QString, QString)));
    connect(&sessionLoadView,
            SIGNAL(creationDirectoryChanged(QString, QString)),
            SLOT(handleSessionLoadViewCreationDirectoryChange(QString,
                                                              QString)));
    connect(&sessionLoadView,
            SIGNAL(creationRequest(QString, QString, synthclone::SampleRate,
                                   synthclone::SampleChannelCount)),
            SLOT(handleSessionLoadViewCreationRequest
                 (QString, QString, synthclone::SampleRate,
                  synthclone::SampleChannelCount)));
    connect(&sessionLoadView, SIGNAL(error(QString)),
            SLOT(reportError(QString)));
    connect(&sessionLoadView, SIGNAL(openDirectoryBrowseRequest(QString)),
            SLOT(handleSessionLoadViewOpenDirectoryBrowseRequest(QString)));
    connect(&sessionLoadView, SIGNAL(openDirectoryChanged(QString)),
            SLOT(handleSessionLoadViewOpenDirectoryChange(QString)));
    connect(&sessionLoadView, SIGNAL(openRequest(QString)),
            SLOT(handleSessionLoadViewOpenRequest(QString)));

    // Setup viewlets

    ComponentViewlet *componentViewlet = mainView.getComponentViewlet();
    connect(componentViewlet, SIGNAL(effectMoveRequest(int, int)),
            &session, SLOT(moveEffect(int, int)));
    connect(componentViewlet, SIGNAL(effectRemoveRequest(int)),
            &session, SLOT(removeEffect(int)));
    connect(componentViewlet, SIGNAL(samplerJobAbortRequest()),
            &session, SLOT(abortCurrentSamplerJob()));
    connect(componentViewlet, SIGNAL(samplerRemoveRequest()),
            &session, SLOT(removeSampler()));
    connect(componentViewlet, SIGNAL(targetMoveRequest(int, int)),
            &session, SLOT(moveTarget(int, int)));
    connect(componentViewlet, SIGNAL(targetRemoveRequest(int)),
            &session, SLOT(removeTarget(int)));

    connect(componentViewlet, SIGNAL(selectedEffectChangeRequest(int)),
            &session, SLOT(setSelectedEffect(int)));
    connect(componentViewlet, SIGNAL(selectedTargetChangeRequest(int)),
            &session, SLOT(setSelectedTarget(int)));

    connect(componentViewlet,
            SIGNAL(focusedComponentChangeRequest(ComponentViewlet::Type, int)),
            SLOT(handleComponentViewletFocusedChangeRequest
                 (ComponentViewlet::Type, int)));

    componentViewlet->getEffectAddMenuViewlet()->setVisible(false);
    componentViewlet->getSamplerAddMenuViewlet()->setVisible(false);
    componentViewlet->getTargetAddMenuViewlet()->setVisible(false);

    HelpViewlet *helpViewlet = mainView.getHelpViewlet();
    connect(helpViewlet, SIGNAL(aboutRequest()),
            SLOT(handleHelpViewletAboutRequest()));

    SessionViewlet *sessionViewlet = mainView.getSessionViewlet();
    connect(sessionViewlet, SIGNAL(loadRequest()),
            SLOT(handleSessionViewletLoadRequest()));
    connect(sessionViewlet, SIGNAL(quitRequest()),
            SLOT(handleSessionViewletQuitRequest()));
    connect(sessionViewlet, SIGNAL(saveAsRequest()),
            SLOT(handleSessionViewletSaveAsRequest()));
    connect(sessionViewlet, SIGNAL(saveRequest()), &session, SLOT(save()));

    // The tool viewlet doesn't require any action right now.

    ViewViewlet *viewViewlet = mainView.getViewViewlet();
    connect(viewViewlet, SIGNAL(participantEditRequest()),
            SLOT(handleViewViewletParticipantEditRequest()));

    ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();

    connect(zoneViewlet,
            SIGNAL(aftertouchPropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setAftertouchPropertyVisible(bool)));
    connect(zoneViewlet,
            SIGNAL(channelPressurePropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setChannelPressurePropertyVisible(bool)));
    connect(zoneViewlet, SIGNAL(channelPropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setChannelPropertyVisible(bool)));
    connect(zoneViewlet,
            SIGNAL(controlPropertyVisibilityChangeRequest(synthclone::MIDIData,
                                                          bool)),
            &session,
            SLOT(setControlPropertyVisible(synthclone::MIDIData, bool)));
    connect(zoneViewlet, SIGNAL(drySamplePropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setDrySamplePropertyVisible(bool)));
    connect(zoneViewlet, SIGNAL(notePropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setNotePropertyVisible(bool)));
    connect(zoneViewlet,
            SIGNAL(releaseTimePropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setReleaseTimePropertyVisible(bool)));
    connect(zoneViewlet,
            SIGNAL(sampleTimePropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setSampleTimePropertyVisible(bool)));
    connect(zoneViewlet, SIGNAL(statusPropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setStatusPropertyVisible(bool)));
    connect(zoneViewlet, SIGNAL(velocityPropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setVelocityPropertyVisible(bool)));
    connect(zoneViewlet, SIGNAL(wetSamplePropertyVisibilityChangeRequest(bool)),
            &session, SLOT(setWetSamplePropertyVisible(bool)));

    connect(zoneViewlet, SIGNAL(aftertouchPropertySortRequest(bool)),
            SLOT(handleZoneViewletAftertouchPropertySortRequest(bool)));
    connect(zoneViewlet, SIGNAL(channelPressurePropertySortRequest(bool)),
            SLOT(handleZoneViewletChannelPressurePropertySortRequest(bool)));
    connect(zoneViewlet, SIGNAL(channelPropertySortRequest(bool)),
            SLOT(handleZoneViewletChannelPropertySortRequest(bool)));
    connect(zoneViewlet,
            SIGNAL(controlPropertySortRequest(synthclone::MIDIData, bool)),
            SLOT(handleZoneViewletControlPropertySortRequest
                 (synthclone::MIDIData, bool)));
    connect(zoneViewlet, SIGNAL(drySamplePropertySortRequest(bool)),
            SLOT(handleZoneViewletDrySamplePropertySortRequest(bool)));
    connect(zoneViewlet, SIGNAL(notePropertySortRequest(bool)),
            SLOT(handleZoneViewletNotePropertySortRequest(bool)));
    connect(zoneViewlet, SIGNAL(releaseTimePropertySortRequest(bool)),
            SLOT(handleZoneViewletReleaseTimePropertySortRequest(bool)));
    connect(zoneViewlet, SIGNAL(sampleTimePropertySortRequest(bool)),
            SLOT(handleZoneViewletSampleTimePropertySortRequest(bool)));
    connect(zoneViewlet, SIGNAL(statusPropertySortRequest(bool)),
            SLOT(handleZoneViewletStatusPropertySortRequest(bool)));
    connect(zoneViewlet, SIGNAL(velocityPropertySortRequest(bool)),
            SLOT(handleZoneViewletVelocityPropertySortRequest(bool)));
    connect(zoneViewlet, SIGNAL(wetSamplePropertySortRequest(bool)),
            SLOT(handleZoneViewletWetSamplePropertySortRequest(bool)));

    connect(zoneViewlet,
            SIGNAL(aftertouchChangeRequest(int, synthclone::MIDIData)),
            SLOT(handleZoneViewletAftertouchChangeRequest
                 (int, synthclone::MIDIData)));
    connect(zoneViewlet,
            SIGNAL(channelChangeRequest(int, synthclone::MIDIData)),
            SLOT(handleZoneViewletChannelChangeRequest(int,
                                                       synthclone::MIDIData)));
    connect(zoneViewlet,
            SIGNAL(channelPressureChangeRequest(int, synthclone::MIDIData)),
            SLOT(handleZoneViewletChannelPressureChangeRequest
                 (int, synthclone::MIDIData)));
    connect(zoneViewlet,
            SIGNAL(controlValueChangeRequest(int, synthclone::MIDIData,
                                             synthclone::MIDIData)),
            SLOT(handleZoneViewletControlValueChangeRequest
                 (int, synthclone::MIDIData, synthclone::MIDIData)));
    connect(zoneViewlet, SIGNAL(noteChangeRequest(int, synthclone::MIDIData)),
            SLOT(handleZoneViewletNoteChangeRequest(int,
                                                    synthclone::MIDIData)));
    connect(zoneViewlet,
            SIGNAL(releaseTimeChangeRequest(int, synthclone::SampleTime)),
            SLOT(handleZoneViewletReleaseTimeChangeRequest
                 (int, synthclone::SampleTime)));
    connect(zoneViewlet,
            SIGNAL(sampleTimeChangeRequest(int, synthclone::SampleTime)),
            SLOT(handleZoneViewletSampleTimeChangeRequest
                 (int, synthclone::SampleTime)));
    connect(zoneViewlet,
            SIGNAL(velocityChangeRequest(int, synthclone::MIDIData)),
            SLOT(handleZoneViewletVelocityChangeRequest(int,
                                                        synthclone::MIDIData)));

    connect(zoneViewlet, SIGNAL(applyEffectsRequest()),
            SLOT(handleZoneViewletApplyEffectsRequest()));
    connect(zoneViewlet, SIGNAL(clearEffectJobsRequest()),
            SLOT(handleZoneViewletClearEffectJobsRequest()));
    connect(zoneViewlet, SIGNAL(clearSamplerJobsRequest()),
            SLOT(handleZoneViewletClearSamplerJobsRequest()));
    connect(zoneViewlet, SIGNAL(clearSelectionRequest()),
            SLOT(handleZoneViewletClearSelectionRequest()));
    connect(zoneViewlet, SIGNAL(copyRequest()),
            SLOT(handleZoneViewletCopyRequest()));
    connect(zoneViewlet, SIGNAL(cutRequest()),
            SLOT(handleZoneViewletCutRequest()));
    connect(zoneViewlet, SIGNAL(deleteRequest()),
            SLOT(handleZoneViewletDeleteRequest()));
    connect(zoneViewlet, SIGNAL(insertRequest()),
            SLOT(handleZoneViewletInsertRequest()));
    connect(zoneViewlet, SIGNAL(invertSelectionRequest()),
            SLOT(handleZoneViewletInvertSelectionRequest()));
    connect(zoneViewlet, SIGNAL(pasteRequest()),
            SLOT(handleZoneViewletPasteRequest()));
    connect(zoneViewlet, SIGNAL(playDrySampleRequest()),
            SLOT(handleZoneViewletPlayDrySampleRequest()));
    connect(zoneViewlet, SIGNAL(playWetSampleRequest()),
            SLOT(handleZoneViewletPlayWetSampleRequest()));
    connect(zoneViewlet, SIGNAL(removeEffectJobRequest()),
            SLOT(handleZoneViewletRemoveEffectJobRequest()));
    connect(zoneViewlet, SIGNAL(removeSamplerJobRequest()),
            SLOT(handleZoneViewletRemoveSamplerJobRequest()));
    connect(zoneViewlet, SIGNAL(sampleRequest()),
            SLOT(handleZoneViewletSampleRequest()));
    connect(zoneViewlet, SIGNAL(selectAllRequest()),
            SLOT(handleZoneViewletSelectAllRequest()));

    connect(zoneViewlet, SIGNAL(buildTargetsRequest()),
            &session, SLOT(buildTargets()));
    connect(zoneViewlet, SIGNAL(selectionChangeRequest(int, bool)),
            &session, SLOT(setZoneSelected(int, bool)));

    zoneViewlet->setApplyEffectsEnabled(false);
    zoneViewlet->setBuildTargetsEnabled(false);
    zoneViewlet->setClearEffectJobsEnabled(false);
    zoneViewlet->setClearSamplerJobsEnabled(false);
    zoneViewlet->setClearSelectionEnabled(false);
    zoneViewlet->setCopyEnabled(false);
    zoneViewlet->setCutEnabled(false);
    zoneViewlet->setDeleteEnabled(false);
    zoneViewlet->setInsertEnabled(true);
    zoneViewlet->setInvertSelectionEnabled(false);
    zoneViewlet->setPasteEnabled(false);
    zoneViewlet->setPlayDrySampleEnabled(false);
    zoneViewlet->setPlayWetSampleEnabled(false);
    zoneViewlet->setRemoveEffectJobEnabled(false);
    zoneViewlet->setRemoveSamplerJobEnabled(false);
    zoneViewlet->setSampleEnabled(false);
    zoneViewlet->setSelectAllEnabled(false);

    zoneViewlet->setAftertouchPropertyVisible
        (session.isAftertouchPropertyVisible());
    zoneViewlet->setChannelPressurePropertyVisible
        (session.isChannelPressurePropertyVisible());
    zoneViewlet->setChannelPropertyVisible(session.isChannelPropertyVisible());
    zoneViewlet->setDrySamplePropertyVisible
        (session.isDrySamplePropertyVisible());
    zoneViewlet->setNotePropertyVisible(session.isNotePropertyVisible());
    zoneViewlet->setReleaseTimePropertyVisible
        (session.isReleaseTimePropertyVisible());
    zoneViewlet->setSampleTimePropertyVisible
        (session.isSampleTimePropertyVisible());
    zoneViewlet->setStatusPropertyVisible(session.isStatusPropertyVisible());
    zoneViewlet->setVelocityPropertyVisible
        (session.isVelocityPropertyVisible());
    zoneViewlet->setWetSamplePropertyVisible
        (session.isWetSamplePropertyVisible());
    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        zoneViewlet->
            setControlPropertyVisible(i, session.isControlPropertyVisible(i));
    }

    // Setup controller objects

    connect(&participantManager,
            SIGNAL(participantActivated(const synthclone::Participant *,
                                        const synthclone::Participant *,
                                        const QByteArray &)),
            SLOT(handleParticipantManagerParticipantActivation
                 (const synthclone::Participant *,
                  const synthclone::Participant *, const QByteArray &)));
    connect(&participantManager,
            SIGNAL(participantAdded(const synthclone::Participant *,
                                    const synthclone::Participant *,
                                    const QByteArray &)),
            SLOT(handleParticipantManagerParticipantAddition
                 (const synthclone::Participant *,
                  const synthclone::Participant *, const QByteArray &)));
    connect(&participantManager,
            SIGNAL(participantDeactivated(const synthclone::Participant *,
                                          const synthclone::Participant *,
                                          const QByteArray &)),
            SLOT(handleParticipantManagerParticipantDeactivation
                 (const synthclone::Participant *,
                  const synthclone::Participant *, const QByteArray &)));
    connect(&participantManager,
            SIGNAL(participantRemoved(const synthclone::Participant *,
                                      const synthclone::Participant *,
                                      const QByteArray &)),
            SLOT(handleParticipantManagerParticipantRemoval
                 (const synthclone::Participant *,
                  const synthclone::Participant *, const QByteArray &)));

    // Setup models

    connect(&session, SIGNAL(aftertouchPropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setAftertouchPropertyVisible(bool)));
    connect(&session, SIGNAL(channelPressurePropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setChannelPressurePropertyVisible(bool)));
    connect(&session, SIGNAL(channelPropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setChannelPropertyVisible(bool)));
    connect(&session,
            SIGNAL(controlPropertyVisibilityChanged(synthclone::MIDIData,
                                                    bool)),
            zoneViewlet,
            SLOT(setControlPropertyVisible(synthclone::MIDIData, bool)));
    connect(&session, SIGNAL(drySamplePropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setDrySamplePropertyVisible(bool)));
    connect(&session, SIGNAL(notePropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setNotePropertyVisible(bool)));
    connect(&session, SIGNAL(releaseTimePropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setReleaseTimePropertyVisible(bool)));
    connect(&session, SIGNAL(sampleTimePropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setSampleTimePropertyVisible(bool)));
    connect(&session, SIGNAL(statusPropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setStatusPropertyVisible(bool)));
    connect(&session, SIGNAL(velocityPropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setVelocityPropertyVisible(bool)));
    connect(&session, SIGNAL(wetSamplePropertyVisibilityChanged(bool)),
            zoneViewlet, SLOT(setWetSamplePropertyVisible(bool)));

    connect(&session,
            SIGNAL(selectedEffectChanged(const synthclone::Effect *, int)),
            SLOT(handleSessionSelectedEffectChange(const synthclone::Effect *,
                                                   int)));
    connect(&session,
            SIGNAL(currentEffectJobChanged(const synthclone::EffectJob *)),
            SLOT(handleSessionCurrentEffectJobChange
                 (const synthclone::EffectJob *)));
    connect(&session, SIGNAL(effectAdded(const synthclone::Effect *, int)),
            SLOT(handleSessionEffectAddition(const synthclone::Effect *, int)));
    connect(&session, SIGNAL(effectMoved(const synthclone::Effect *, int, int)),
            SLOT(handleSessionEffectMove(const synthclone::Effect *, int,
                                         int)));
    connect(&session, SIGNAL(removingEffect(const synthclone::Effect *, int)),
            SLOT(handleSessionEffectRemoval(const synthclone::Effect *, int)));
    connect(&session, SIGNAL(effectJobError(const QString &)),
            SLOT(reportError(const QString &)));

    connect(&session,
            SIGNAL(currentSamplerJobChanged(const synthclone::SamplerJob *)),
            SLOT(handleSessionCurrentSamplerJobChange
                 (const synthclone::SamplerJob *)));
    connect(&session, SIGNAL(samplerAdded(const synthclone::Sampler *)),
            SLOT(handleSessionSamplerAddition(const synthclone::Sampler *)));
    connect(&session, SIGNAL(removingSampler(const synthclone::Sampler *)),
            SLOT(handleSessionSamplerRemoval(const synthclone::Sampler *)));
    connect(&session, SIGNAL(samplerJobError(const QString &)),
            SLOT(reportError(const QString &)));


    connect(&session,
            SIGNAL(selectedTargetChanged(const synthclone::Target *, int)),
            SLOT(handleSessionSelectedTargetChange(const synthclone::Target *,
                                                   int)));
    connect(&session, SIGNAL(targetAdded(const synthclone::Target *, int)),
            SLOT(handleSessionTargetAddition(const synthclone::Target *, int)));
    connect(&session, SIGNAL(targetMoved(const synthclone::Target *, int, int)),
            SLOT(handleSessionTargetMove(const synthclone::Target *, int,
                                         int)));
    connect(&session, SIGNAL(removingTarget(const synthclone::Target *, int)),
            SLOT(handleSessionTargetRemoval(const synthclone::Target *, int)));

    connect(&session, SIGNAL(zoneAdded(synthclone::Zone *, int)),
            SLOT(handleSessionZoneAddition(synthclone::Zone *, int)));
    connect(&session, SIGNAL(zoneMoved(synthclone::Zone *, int, int)),
            SLOT(handleSessionZoneMove(synthclone::Zone *, int, int)));
    connect(&session, SIGNAL(removingZone(synthclone::Zone *, int)),
            SLOT(handleSessionZoneRemoval(synthclone::Zone *, int)));
    connect(&session,
            SIGNAL(zoneSelectionChanged(synthclone::Zone *, int, bool)),
            SLOT(handleSessionZoneSelectionChange(synthclone::Zone *, int,
                                                  bool)));

    connect(&session, SIGNAL(buildingTarget(const synthclone::Target *)),
            SLOT(handleSessionTargetBuild(const synthclone::Target *)));
    connect(&session, SIGNAL(buildingTargets()),
            SLOT(handleSessionTargetBuildOperation()));
    connect(&session,
            SIGNAL(targetBuildError(const synthclone::Target *, QString)),
            SLOT(handleSessionTargetBuildError(const synthclone::Target *,
                                               QString)));
    connect(&session, SIGNAL(targetBuilt(const synthclone::Target *)),
            SLOT(handleSessionTargetBuildCompletion
                 (const synthclone::Target *)));
    connect(&session, SIGNAL(targetsBuilt()),
            SLOT(handleSessionTargetBuildOperationCompletion()));

    connect(&session, SIGNAL(loadWarning(int, int, QString)),
            SLOT(handleSessionLoadWarning(int, int, QString)));

    connect(&session, SIGNAL(progressChanged(float, QString)),
            SLOT(handleSessionProgressChange(float, QString)));
    connect(&session,
            SIGNAL(stateChanged(synthclone::SessionState, const QDir *)),
            SLOT(handleSessionStateChange(synthclone::SessionState,
                                          const QDir *)));

    connect(&application, SIGNAL(eventError(QString)),
            SLOT(reportError(QString)));

    connect(QApplication::clipboard(), SIGNAL(dataChanged()),
            SLOT(handleClipboardDataChange()));

    lastSessionState = synthclone::SESSIONSTATE_CURRENT;

    // Load plugins
    QStringList scannedPaths;
    loadPlugins(getCorePluginDirectory(), scannedPaths);
    QStringList paths = settings.getPluginPaths();
    for (int i = paths.count() - 1; i >= 0; i--) {
        loadPlugins(QDir(paths[i]), scannedPaths);
    }
}

Controller::~Controller()
{
    session.unload();
    QList<synthclone::Participant *> participants = pluginParticipantMap.keys();
    for (int i = participants.count() - 1; i >= 0; i--) {
        participantManager.removeParticipant(participants[i]);
    }
    assert(! participantViewletMap.count());
    assert(! pluginParticipantMap.count());
}

void
Controller::clearProgressView()
{
    progressView.clearMessages();
    progressView.setCloseEnabled(false);
    progressView.setProgress(0.0);
    progressView.setStatus("");
}

void
Controller::copySelectedZones()
{
    QByteArray data;
    QXmlStreamWriter writer(&data);
    writer.writeStartDocument();
    writer.writeStartElement("synthclone-zone-list");
    int count = session.getSelectedZoneCount();
    for (int i = 0; i < count; i++) {
        writeZone(writer, qobject_cast<Zone *>(session.getSelectedZone(i)));
    }
    writer.writeEndElement();

    writer.writeEndDocument();

    // The clipboard will take ownership of the mime data.
    QMimeData *mimeData = new QMimeData();
    mimeData->setData("text/xml", data);
    application.clipboard()->setMimeData(mimeData);
}

void
Controller::executePostSaveChangesAction()
{
    switch (postSaveChangesAction) {
    case POSTSAVECHANGESACTION_LOAD:
        session.unload();
        break;
    case POSTSAVECHANGESACTION_QUIT:
        quit();
    }
}

QDir
Controller::getCorePluginDirectory()
{
    QDir pluginDirectory;

#ifdef SYNTHCLONE_DEBUG
    pluginDirectory.setPath(application.applicationDirPath());
    if (pluginDirectory.cdUp()) {
        if (pluginDirectory.cd("lib")) {
            if (pluginDirectory.cd("synthclone")) {
                if (pluginDirectory.cd("plugins")) {
                    return pluginDirectory;
                }
            }
        }
    }
#endif

    pluginDirectory.setPath(PLUGIN_PATH);
    if (pluginDirectory.exists()) {
        return pluginDirectory;
    }
    throw synthclone::Error(tr("core plugin directory '%1' is not accessible").
                            arg(PLUGIN_PATH));
}

MainView &
Controller::getMainView()
{
    return mainView;
}

MenuManager &
Controller::getMenuManager()
{
    return menuManager;
}

Session &
Controller::getSession()
{
    return session;
}

bool
Controller::loadClipboardZoneList(QDomDocument &document)
{
    const QMimeData *mimeData = application.clipboard()->mimeData();
    bool loaded = mimeData->formats().contains("text/xml");
    if (loaded) {
        QByteArray data = mimeData->data("text/xml");
        loaded = document.setContent(data);
        if (loaded) {
            loaded = document.documentElement().tagName() ==
                "synthclone-zone-list";
        }
    }
    return loaded;
}

void
Controller::loadPlugins(const QDir &directory, QStringList &scannedPaths)
{
    QString path = directory.absolutePath();
    qDebug() << tr("Loading plugins from '%1' ...").arg(path);
    QDirIterator iter(path, QDir::AllEntries | QDir::NoDotAndDotDot,
                      QDirIterator::Subdirectories);
    while (iter.hasNext()) {
        iter.next();
        QFileInfo fileInfo = iter.fileInfo();
        if (! fileInfo.isFile()) {
            continue;
        }
        path = fileInfo.absoluteFilePath();
        if (scannedPaths.contains(path)) {
            continue;
        }
        scannedPaths.append(path);
        qDebug() << tr("Attempting to load plugin from '%1' ...").arg(path);
        synthclone::IPlugin *plugin;
        try {
            plugin = pluginManager.loadPlugin(path);
        } catch (synthclone::Error &e) {
            qDebug() << tr("Error loading plugin from '%1': %2").
                arg(path).arg(e.getMessage());
            continue;
        }
        if (pluginParticipantMap.key(plugin, 0)) {
            continue;
        }
        synthclone::Participant *participant = plugin->getParticipant();
        pluginParticipantMap.insert(participant, plugin);
        const synthclone::Registration &registration =
            participantManager.addParticipant(participant, plugin->getId());
        connect(&registration, SIGNAL(unregistered(QObject*)),
                SLOT(handleParticipantUnregistration(QObject *)));
    }
}

void
Controller::processQuitRequest()
{
    switch (session.getState()) {
    case synthclone::SESSIONSTATE_CURRENT:
        quit();
        break;
    case synthclone::SESSIONSTATE_MODIFIED:
        postSaveChangesAction = POSTSAVECHANGESACTION_QUIT;
        saveChangesView.setVisible(true);
        break;
    default:
        assert(false);
    }
}

void
Controller::quit()
{
    session.unload();
    application.quit();
}

void
Controller::refreshZoneBuildTargetsAction()
{
    bool enabled = static_cast<bool>(session.getTargetCount());
    if (enabled) {
        for (int i = session.getZoneCount() - 1; i >= 0; i--) {
            if (session.getZone(i)->getStatus() !=
                synthclone::Zone::STATUS_NORMAL) {
                enabled = false;
                break;
            }
        }
    }
    mainView.getZoneViewlet()->setBuildTargetsEnabled(enabled);
}

void
Controller::refreshZoneViewletActions()
{
    int selectCount = session.getSelectedZoneCount();
    ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();
    if (! selectCount) {
        zoneViewlet->setApplyEffectsEnabled(false);
        zoneViewlet->setClearEffectJobsEnabled(false);
        zoneViewlet->setClearSamplerJobsEnabled(false);
        zoneViewlet->setClearSelectionEnabled(false);
        zoneViewlet->setCopyEnabled(false);
        zoneViewlet->setCutEnabled(false);
        zoneViewlet->setDeleteEnabled(false);
        zoneViewlet->setPlayDrySampleEnabled(false);
        zoneViewlet->setPlayWetSampleEnabled(false);
        zoneViewlet->setRemoveEffectJobEnabled(false);
        zoneViewlet->setRemoveSamplerJobEnabled(false);
        zoneViewlet->setSampleEnabled(false);
    } else {
        zoneViewlet->setClearSelectionEnabled(true);
        zoneViewlet->setCopyEnabled(true);
        bool dryEnabled = true;
        bool modifyEnabled = true;
        bool removeEffectJobEnabled = true;
        bool removeSamplerJobEnabled = true;
        bool wetEnabled = true;
        for (int i = selectCount - 1; i >= 0; i--) {
            synthclone::Zone *zone = session.getSelectedZone(i);
            switch (zone->getStatus()) {
            case synthclone::Zone::STATUS_NORMAL:
                removeEffectJobEnabled = false;
                removeSamplerJobEnabled = false;
                if (! zone->getDrySample()) {
                    dryEnabled = false;
                }
                if (! zone->getWetSample()) {
                    wetEnabled = false;
                }
                break;
            case synthclone::Zone::STATUS_EFFECT_JOB_QUEUE:
                dryEnabled = false;
                modifyEnabled = false;
                removeSamplerJobEnabled = false;
                wetEnabled = false;
                break;
            case synthclone::Zone::STATUS_SAMPLER_JOB_QUEUE:
                dryEnabled = false;
                modifyEnabled = false;
                removeEffectJobEnabled = false;
                wetEnabled = false;
                break;
            default:
                dryEnabled = false;
                modifyEnabled = false;
                removeEffectJobEnabled = false;
                removeSamplerJobEnabled = false;
                wetEnabled = false;
                goto applyFlags;
            }
        }
    applyFlags:
        zoneViewlet->
            setApplyEffectsEnabled(dryEnabled && session.getEffectCount());
        zoneViewlet->setClearEffectJobsEnabled(session.getEffectJobCount());
        zoneViewlet->setClearSamplerJobsEnabled(session.getSamplerJobCount());
        zoneViewlet->setCutEnabled(modifyEnabled);
        zoneViewlet->setDeleteEnabled(modifyEnabled);
        bool samplerSet = static_cast<bool>(session.getSampler());
        zoneViewlet->setPlayDrySampleEnabled(samplerSet && dryEnabled);
        zoneViewlet->setPlayWetSampleEnabled(samplerSet && wetEnabled);
        zoneViewlet->setRemoveEffectJobEnabled(removeEffectJobEnabled);
        zoneViewlet->setRemoveSamplerJobEnabled(removeSamplerJobEnabled);
        zoneViewlet->setSampleEnabled(samplerSet && modifyEnabled);
        zoneViewlet->setSelectAllEnabled(selectCount != session.getZoneCount());
    }
}

void
Controller::removeDirectoryContents(QDir &directory)
{
    QFSFileEngine fileEngine;
    QList<QFileInfo> fileInfoList =
        directory.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (int i = fileInfoList.count() - 1; i >= 0; i--) {
        QFileInfo fileInfo = fileInfoList[i];
        QString path = fileInfo.absoluteFilePath();
        if (fileInfo.isDir()) {
            QDir subDirectory(path);
            removeDirectoryContents(subDirectory);
            if (! fileEngine.rmdir(path, false)) {
                throw synthclone::Error(fileEngine.errorString());
            }
        } else {
            QFile file(path);
            if (! file.remove()) {
                throw synthclone::Error(file.errorString());
            }
        }
    }
}

void
Controller::removeSelectedZones()
{
    for (int i = session.getSelectedZoneCount() - 1; i >= 0; i--) {
        session.removeZone(session.getSelectedZone(i));
    }
}

void
Controller::reportError(const QString &message)
{
    errorView.setMessage(message);
    errorView.setVisible(true);
    emit errorReported(message);
}

void
Controller::run(const QDir *sessionDirectory)
{
    bool loaded = false;
    if (sessionDirectory) {
        try {
            session.load(*sessionDirectory);
            loaded = true;
        } catch (synthclone::Error &e) {
            QString message = tr("Error loading session from '%1': %2").
                arg(sessionDirectory->absolutePath(), e.getMessage());
            reportError(message);
        }
    }
    mainView.setVisible(true);
    if (! loaded) {
        showSessionLoadView();
    }
    application.exec();
}

void
Controller::setSessionLoadViewCreationDefaults()
{
    QString message = tr("Select the directory where your new session will be "
                         "created.");
    sessionLoadView.setCreationEnabled(false);
    sessionLoadView.setCreationMessage(message);
    sessionLoadView.setCreationStatus(SessionLoadView::STATUS_INFO);
}

void
Controller::setSessionLoadViewOpenDefaults()
{
    sessionLoadView.setOpenEnabled(false);
    sessionLoadView.setOpenMessage(tr("Select the session you want to open."));
    sessionLoadView.setOpenStatus(SessionLoadView::STATUS_INFO);
}

void
Controller::showSessionLoadView()
{
    sessionLoadView.clearRecentSessions();
    QStringList paths = settings.getRecentSessionPaths();
    int count = paths.count();
    for (int i = 0; i < count; i++) {
        sessionLoadView.addRecentSession(paths[i]);
    }
    sessionLoadView.setCreationName("");
    sessionLoadView.setCreationPath("");
    sessionLoadView.setOpenPath("");
    setSessionLoadViewCreationDefaults();
    setSessionLoadViewOpenDefaults();
    sessionLoadView.setVisible(true);
}

void
Controller::updateZoneViewlet(const synthclone::Zone *zone, int index)
{
    assert((index >= 0) && (index < session.getZoneCount()));
    assert(zone);
    ZoneViewlet *viewlet = mainView.getZoneViewlet();
    viewlet->setAftertouch(index, zone->getAftertouch());
    viewlet->setChannel(index, zone->getChannel());
    viewlet->setChannelPressure(index, zone->getChannelPressure());
    viewlet->setDrySampleStale(index, zone->isDrySampleStale());
    viewlet->setNote(index, zone->getNote());
    viewlet->setReleaseTime(index, zone->getReleaseTime());
    viewlet->setSampleTime(index, zone->getSampleTime());
    viewlet->setStatus(index, zone->getStatus());
    viewlet->setVelocity(index, zone->getVelocity());
    viewlet->setWetSampleStale(index, zone->isWetSampleStale());

    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        viewlet->setControlValue(index, i, zone->getControlValue(i));
    }

    const synthclone::Sample *sample = zone->getDrySample();
    if (sample) {
        SampleProfile profile(*sample);
        viewlet->setDrySampleProfile(index, &profile);
    } else {
        viewlet->setDrySampleProfile(index, 0);
    }
    sample = zone->getWetSample();
    if (sample) {
        SampleProfile profile(*sample);
        viewlet->setWetSampleProfile(index, &profile);
    } else {
        viewlet->setWetSampleProfile(index, 0);
    }
}

bool
Controller::verifySessionLoadViewCreation(const QString &path,
                                          const QString &name)
{
    if (! path.count()) {
        setSessionLoadViewCreationDefaults();
        return false;
    }
    QDir directory(path);
    bool enabled = false;
    QFileInfo info(directory.absolutePath());
    QString message;
    SessionLoadView::Status status;
    if (! info.exists()) {
        message = tr("The path '%1' does not point to a valid directory.").
            arg(directory.absolutePath());
        status = SessionLoadView::STATUS_ERROR;
    } else if (! info.isReadable()) {
        message = tr("The path '%1' does not point to a readable directory.").
            arg(directory.absolutePath());
        status = SessionLoadView::STATUS_ERROR;
    } else if (! info.isWritable()) {
        message = tr("The path '%1' does not point to a writable directory.").
            arg(directory.absolutePath());
        status = SessionLoadView::STATUS_ERROR;
    } else if (! name.count()) {
        message = tr("Choose a name for your new session.");
        status = SessionLoadView::STATUS_INFO;
    } else if ((name == ".") || (name == "..") ||
               (name != QFileInfo(name).fileName())) {
        message = tr("'%1' is not a valid session name.").arg(name);
        status = SessionLoadView::STATUS_ERROR;
    } else {
        QFileInfo info(directory.filePath(name));
        QString path = info.absoluteFilePath();
        if (info.exists()) {
            message = tr("The path '%1' points to an existing filesystem "
                         "object.").arg(path);
            status = SessionLoadView::STATUS_ERROR;
        } else {
            enabled = true;
            message = tr("The path '%1' is an acceptable new session "
                         "directory.").arg(path);
            status = SessionLoadView::STATUS_VALID;
        }
    }
    sessionLoadView.setCreationEnabled(enabled);
    sessionLoadView.setCreationMessage(message);
    sessionLoadView.setCreationName(name);
    sessionLoadView.setCreationPath(path);
    sessionLoadView.setCreationStatus(status);
    return enabled;
}

bool
Controller::verifySessionLoadViewOpen(const QString &path)
{
    if (! path.count()) {
        setSessionLoadViewOpenDefaults();
        return false;
    }
    QDir dir(path);
    bool enabled = dir.exists();
    QString message;
    QString absPath = dir.absolutePath();
    SessionLoadView::Status status;
    if (! enabled) {
        message = tr("The path '%1' does not point to a valid directory.").
            arg(absPath);
        status = SessionLoadView::STATUS_ERROR;
    } else if (! dir.isReadable()) {
        enabled = false;
        message = tr("The path '%1' does not point to a readable directory.").
            arg(absPath);
        status = SessionLoadView::STATUS_ERROR;
    } else {
        bool isSession;
        try {
            isSession = Session::isDirectory(dir);
        } catch (synthclone::Error &e) {
            enabled = false;
            message = tr("An error occurred while validating the session "
                         "directory: %1").arg(e.getMessage());
            status = SessionLoadView::STATUS_ERROR;
            goto setViewData;
        }
        if (! isSession) {
            enabled = false;
            message = tr("The path '%1' does not point to a session "
                         "directory.").arg(absPath);
            status = SessionLoadView::STATUS_ERROR;
        } else {
            message = tr("The path '%1' points to an acceptable session "
                         "directory.").arg(absPath);
            status = SessionLoadView::STATUS_VALID;
        }
    }
setViewData:
    sessionLoadView.setOpenEnabled(enabled);
    sessionLoadView.setOpenMessage(message);
    sessionLoadView.setOpenPath(path);
    sessionLoadView.setOpenStatus(status);
    return enabled;
}

////////////////////////////////////////////////////////////////////////////////
// AboutViewlet signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleAboutViewCloseRequest()
{
    aboutView.setVisible(false);
}

////////////////////////////////////////////////////////////////////////////////
// ComponentViewlet signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::
handleComponentViewletFocusedChangeRequest(ComponentViewlet::Type type,
                                           int index)
{
    const synthclone::Component *component;
    switch (type) {
    case ComponentViewlet::TYPE_EFFECT:
        component = session.getEffect(index);
        break;
    case ComponentViewlet::TYPE_NONE:
        component = 0;
        break;
    case ComponentViewlet::TYPE_SAMPLER:
        component = session.getSampler();
        break;
    case ComponentViewlet::TYPE_TARGET:
        component = session.getTarget(index);
        break;
    default:
        assert(false);
    }
    session.setFocusedComponent(component);
}

////////////////////////////////////////////////////////////////////////////////
// DirectoryView signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleDirectoryViewCloseRequest()
{
    directoryView.setVisible(false);
}

void
Controller::handleDirectoryViewPathSelection(const QStringList &paths)
{
    assert(paths.count() == 1);
    QDir directory;
    QString path = paths[0];
    directoryView.setVisible(false);
    switch (postDirectorySelectAction) {
    case POSTDIRECTORYSELECTACTION_CREATE_SESSION:
        verifySessionLoadViewCreation(path, createSessionName);
        break;
    case POSTDIRECTORYSELECTACTION_OPEN_SESSION:
        verifySessionLoadViewOpen(path);
        break;
    case POSTDIRECTORYSELECTACTION_SAVE_SESSION:
        directory = QDir(path);
        directory.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        if (! directory.count()) {
            session.save(directory);
        } else if (directory.absolutePath() ==
                   session.getDirectory()->absolutePath()) {
            session.save();
        } else {
            saveAsPath = path;
            saveWarningView.setVisible(true);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Effect signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleEffectNameChange(const QString &name)
{
    const synthclone::Effect *effect =
        qobject_cast<const synthclone::Effect *>(sender());
    int index = session.getEffectIndex(effect);
    mainView.getComponentViewlet()->setEffectName(index, name);
}

void
Controller::handleEffectProgressChange(float progress)
{
    const synthclone::Effect *effect =
        qobject_cast<const synthclone::Effect *>(sender());
    int index = session.getEffectIndex(effect);
    mainView.getComponentViewlet()->setEffectProgress(index, progress);
}

void
Controller::handleEffectStatusChange(const QString &status)
{
    const synthclone::Effect *effect =
        qobject_cast<const synthclone::Effect *>(sender());
    int index = session.getEffectIndex(effect);
    mainView.getComponentViewlet()->setEffectStatus(index, status);
}

////////////////////////////////////////////////////////////////////////////////
// ErrorView signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleErrorViewCloseRequest()
{
    errorView.setVisible(false);
}

////////////////////////////////////////////////////////////////////////////////
// HelpViewlet signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleHelpViewletAboutRequest()
{
    aboutView.setVisible(true);
}

////////////////////////////////////////////////////////////////////////////////
// MainView signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleMainViewCloseRequest()
{
    processQuitRequest();
}

////////////////////////////////////////////////////////////////////////////////
// Registration signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleParticipantUnregistration(QObject *obj)
{
    synthclone::Participant *participant =
        qobject_cast<synthclone::Participant *>(obj);
    assert(participant);
    synthclone::IPlugin *plugin = pluginParticipantMap.take(participant);
    pluginManager.unloadPlugin(plugin);
}

////////////////////////////////////////////////////////////////////////////////
// ParticipantManager signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::
handleParticipantManagerParticipantActivation(const synthclone::Participant *
                                              participant,
                                              const synthclone::Participant *
                                              /*parent*/,
                                              const QByteArray &/*id*/)
{
    ParticipantViewlet *viewlet = participantViewletMap.value(participant, 0);
    assert(viewlet);
    viewlet->setActivated(true);

    qDebug() << tr("Participant activated: %1 %2.%3-%4").
        arg(participant->getName()).arg(participant->getMajorVersion()).
        arg(participant->getMinorVersion()).arg(participant->getRevision());
}

void
Controller::
handleParticipantManagerParticipantAddition(const synthclone::Participant *
                                            participant,
                                            const synthclone::Participant *
                                            parent,
                                            const QByteArray &/*id*/)
{
    ParticipantViewlet *viewlet = participantView.createParticipantViewlet();
    if (! parent) {
        participantView.add(viewlet);
    } else {
        ParticipantViewlet *parentViewlet =
            participantViewletMap.value(parent, 0);
        assert(parentViewlet);
        parentViewlet->add(viewlet);
    }
    viewlet->setActivated(false);
    viewlet->setAuthor(participant->getAuthor());
    viewlet->setMajorVersion(participant->getMajorVersion());
    viewlet->setMinorVersion(participant->getMinorVersion());
    viewlet->setName(participant->getName());
    viewlet->setRevision(participant->getRevision());
    viewlet->setSummary(participant->getSummary());
    connect(viewlet, SIGNAL(activationChangeRequest(bool)),
            SLOT(handleParticipantViewletActivationChangeRequest(bool)));
    participantViewletMap.insert(participant, viewlet);

    qDebug() << tr("Participant added to session manager: %1 %2.%3-%4").
        arg(participant->getName()).arg(participant->getMajorVersion()).
        arg(participant->getMinorVersion()).arg(participant->getRevision());
}

void
Controller::
handleParticipantManagerParticipantDeactivation(const synthclone::Participant *
                                                participant,
                                                const synthclone::Participant *
                                                /*parent*/,
                                                const QByteArray &/*id*/)
{
    ParticipantViewlet *viewlet = participantViewletMap.value(participant, 0);
    assert(viewlet);
    viewlet->setActivated(false);

    qDebug() << tr("Participant deactivated: %1 %2.%3-%4").
        arg(participant->getName()).arg(participant->getMajorVersion()).
        arg(participant->getMinorVersion()).arg(participant->getRevision());
}

void
Controller::
handleParticipantManagerParticipantRemoval(const synthclone::Participant *
                                           participant,
                                           const synthclone::Participant *
                                           parent,
                                           const QByteArray &/*id*/)
{
    ParticipantViewlet *viewlet = participantViewletMap.take(participant);
    assert(viewlet);
    if (! parent) {
        participantView.remove(viewlet);
    } else {
        ParticipantViewlet *parentViewlet =
            participantViewletMap.value(parent, 0);
        assert(parentViewlet);
        parentViewlet->remove(viewlet);
    }
    participantView.destroyParticipantViewlet(viewlet);

    qDebug() << tr("Participant removed from session manager: %1 %2.%3.%4").
        arg(participant->getName()).arg(participant->getMajorVersion()).
        arg(participant->getMinorVersion()).arg(participant->getRevision());
}

////////////////////////////////////////////////////////////////////////////////
// ParticipantView signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleParticipantViewCloseRequest()
{
    participantView.setVisible(false);
}

////////////////////////////////////////////////////////////////////////////////
// ParticipantViewlet signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleParticipantViewletActivationChangeRequest(bool activate)
{
    ParticipantViewlet *viewlet = qobject_cast<ParticipantViewlet *>(sender());
    assert(viewlet);
    const synthclone::Participant *participant =
        participantViewletMap.key(viewlet, 0);
    assert(participant);
    if (activate) {
        participantManager.activateParticipant(participant);
    } else {
        participantManager.deactivateParticipant(participant);
    }
}

////////////////////////////////////////////////////////////////////////////////
// ProgressView signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleProgressViewCloseRequest()
{
    progressView.setVisible(false);
    clearProgressView();
}

////////////////////////////////////////////////////////////////////////////////
// QClipboard signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleClipboardDataChange()
{
    QDomDocument document;
    mainView.getZoneViewlet()->setPasteEnabled(loadClipboardZoneList(document));
}

////////////////////////////////////////////////////////////////////////////////
// Sampler signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleSamplerNameChange(const QString &name)
{
    if (session.getSampler()) {
        mainView.getComponentViewlet()->setSamplerName(name);
    }
}

void
Controller::handleSamplerProgressChange(float progress)
{
    if (session.getSampler()) {
        mainView.getComponentViewlet()->setSamplerProgress(progress);
    }
}

void
Controller::handleSamplerStatusChange(const QString &status)
{
    if (session.getSampler()) {
        mainView.getComponentViewlet()->setSamplerStatus(status);
    }
}

////////////////////////////////////////////////////////////////////////////////
// SaveChangesView signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleSaveChangesViewCloseRequest()
{
    saveChangesView.setVisible(false);
}

void
Controller::handleSaveChangesViewDiscardRequest()
{
    saveChangesView.setVisible(false);
    executePostSaveChangesAction();
}

void
Controller::handleSaveChangesViewSaveRequest()
{
    saveChangesView.setVisible(false);
    session.save();
    executePostSaveChangesAction();
}

////////////////////////////////////////////////////////////////////////////////
// SaveWarningView signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleSaveWarningViewCloseRequest()
{
    saveWarningView.setVisible(false);
}

void
Controller::handleSaveWarningViewSaveRequest()
{
    saveWarningView.setVisible(false);
    QDir directory(saveAsPath);
    removeDirectoryContents(directory);
    session.save(directory);
}

////////////////////////////////////////////////////////////////////////////////
// Session signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::
handleSessionCurrentEffectJobChange(const synthclone::EffectJob *job)
{
    mainView.getComponentViewlet()->
        setEffectEditingEnabled(! static_cast<bool>(job));
}

void
Controller::
handleSessionCurrentSamplerJobChange(const synthclone::SamplerJob *job)
{
    ComponentViewlet *viewlet = mainView.getComponentViewlet();
    bool hasJob = static_cast<bool>(job);
    viewlet->setCurrentSamplerJob(hasJob);
    viewlet->setSamplerEditingEnabled(! hasJob);
}

void
Controller::handleSessionEffectAddition(const synthclone::Effect *effect,
                                        int index)
{
    ComponentViewlet *viewlet = mainView.getComponentViewlet();
    viewlet->addEffect(index);
    viewlet->setEffectName(index, effect->getName());
    viewlet->setEffectProgress(index, 0.0);
    viewlet->setEffectStatus(index, "");
    connect(effect, SIGNAL(nameChanged(const QString)),
            SLOT(handleEffectNameChange(const QString)));
    connect(effect, SIGNAL(progressChanged(float)),
            SLOT(handleEffectProgressChange(float)));
    connect(effect, SIGNAL(statusChanged(QString)),
            SLOT(handleEffectStatusChange(QString)));
    if (session.getEffectCount() == 1) {
        int count = session.getSelectedZoneCount();
        bool enabled = static_cast<bool>(count);
        synthclone::Zone *zone;
        if (enabled) {
            for (int i = count - 1; i >= 0; i--) {
                zone = session.getSelectedZone(i);
                enabled = zone->getDrySample() &&
                    (zone->getStatus() == synthclone::Zone::STATUS_NORMAL);
                if (! enabled) {
                    break;
                }
            }
        }
        mainView.getZoneViewlet()->setApplyEffectsEnabled(enabled);
    }
}

void
Controller::handleSessionEffectMove(const synthclone::Effect */*effect*/,
                                    int fromIndex, int toIndex)
{
    mainView.getComponentViewlet()->moveEffect(fromIndex, toIndex);
}

void
Controller::handleSessionEffectRemoval(const synthclone::Effect *effect,
                                       int index)
{
    disconnect(effect, SIGNAL(nameChanged(const QString)),
               this, SLOT(handleEffectNameChange(QString)));
    disconnect(effect, SIGNAL(progressChanged(float)),
               this, SLOT(handleEffectProgressChange(float)));
    disconnect(effect, SIGNAL(statusChanged(QString)),
               this, SLOT(handleEffectStatusChange(QString)));
    mainView.getComponentViewlet()->removeEffect(index);

    // The effect is *being* removed, but isn't removed yet.
    if (session.getEffectCount() == 1) {
        mainView.getZoneViewlet()->setApplyEffectsEnabled(false);
    }
}

void
Controller::
handleSessionFocusedComponentChange(const synthclone::Component *component)
{
    int index;
    ComponentViewlet::Type type;
    if (! component) {
        index = -1;
        type = ComponentViewlet::TYPE_NONE;
    } else {
        const synthclone::Effect *effect =
            qobject_cast<const synthclone::Effect *>(component);
        if (effect) {
            index = session.getEffectIndex(effect);
            type = ComponentViewlet::TYPE_EFFECT;
        } else {
            const synthclone::Sampler *sampler =
                qobject_cast<const synthclone::Sampler *>(component);
            if (sampler) {
                index = 0;
                type = ComponentViewlet::TYPE_SAMPLER;
            } else {
                const synthclone::Target *target =
                    qobject_cast<const synthclone::Target *>(component);
                assert(target);
                index = session.getTargetIndex(target);
                type = ComponentViewlet::TYPE_TARGET;
            }
        }
    }
    mainView.getComponentViewlet()->setFocusedComponent(type, index);
}

void
Controller::handleSessionLoadWarning(int line, int column,
                                     const QString &message)
{
    QLocale locale = QLocale::system();
    QString msg = tr("Warning: line %1, column %2: %3").
        arg(locale.toString(line), locale.toString(column), message);
    progressView.addMessage(msg);
    sessionLoadWarningCount++;
    application.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void
Controller::handleSessionProgressChange(float progress, const QString &status)
{
    progressView.setProgress(progress);
    progressView.setStatus(status);
    application.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void
Controller::handleSessionSamplerAddition(const synthclone::Sampler *sampler)
{
    ComponentViewlet *viewlet = mainView.getComponentViewlet();
    viewlet->addSampler();
    viewlet->setSamplerName(sampler->getName());
    viewlet->setSamplerProgress(0.0);
    viewlet->setSamplerStatus("");

    connect(sampler, SIGNAL(nameChanged(QString)),
            SLOT(handleSamplerNameChange(QString)));
    connect(sampler, SIGNAL(progressChanged(float)),
            SLOT(handleSamplerProgressChange(float)));
    connect(sampler, SIGNAL(statusChanged(QString)),
            SLOT(handleSamplerStatusChange(QString)));

    int count = session.getSelectedZoneCount();
    synthclone::Zone *zone;
    if (count) {
        bool dryEnabled = true;
        bool sampleEnabled = true;
        bool wetEnabled = true;
        for (int i = count - 1; i >= 0; i--) {
            zone = session.getSelectedZone(i);
            if (zone->getStatus() != synthclone::Zone::STATUS_NORMAL) {
                dryEnabled = false;
                sampleEnabled = false;
                wetEnabled = false;
                break;
            }
            if (! zone->getDrySample()) {
                dryEnabled = false;
            }
            if (! zone->getWetSample()) {
                wetEnabled = false;
            }
        }
        ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();
        zoneViewlet->setPlayDrySampleEnabled(dryEnabled);
        zoneViewlet->setPlayWetSampleEnabled(wetEnabled);
        zoneViewlet->setSampleEnabled(sampleEnabled);
    }
}

void
Controller::handleSessionSamplerRemoval(const synthclone::Sampler *sampler)
{
    ComponentViewlet *viewlet = mainView.getComponentViewlet();

    disconnect(sampler, SIGNAL(nameChanged(const QString)),
               this, SLOT(handleSamplerNameChange(const QString)));
    disconnect(sampler, SIGNAL(progressChanged(float)),
               this, SLOT(handleSamplerProgressChange(float)));
    disconnect(sampler, SIGNAL(statusChanged(QString)),
               this, SLOT(handleSamplerStatusChange(QString)));

    viewlet->removeSampler();
    ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();
    zoneViewlet->setPlayDrySampleEnabled(false);
    zoneViewlet->setPlayWetSampleEnabled(false);
    zoneViewlet->setSampleEnabled(false);
}

void
Controller::handleSessionSelectedEffectChange(const synthclone::Effect *effect,
                                              int index)
{
    mainView.getComponentViewlet()->setSelectedEffect(index);
    const synthclone::Effect *effectComponent =
        qobject_cast<const synthclone::Effect *>(session.getFocusedComponent());
    if (effectComponent) {
        session.setFocusedComponent(effect);
    }
}

void
Controller::handleSessionSelectedTargetChange(const synthclone::Target *target,
                                              int index)
{
    mainView.getComponentViewlet()->setSelectedTarget(index);
    const synthclone::Target *targetComponent =
        qobject_cast<const synthclone::Target *>(session.getFocusedComponent());
    if (targetComponent) {
        session.setFocusedComponent(target);
    }
}

void
Controller::handleSessionStateChange(synthclone::SessionState state,
                                     const QDir *directory)
{
    bool enabled;
    SessionViewlet *viewlet = mainView.getSessionViewlet();
    switch (state) {
    case synthclone::SESSIONSTATE_CURRENT:
        if (! directory) {
            showSessionLoadView();
        } else if (lastSessionState == synthclone::SESSIONSTATE_LOADING) {
            participantView.setVisible(true);
        }
        viewlet->setSaveEnabled(false);
        enabled = true;
        if (lastSessionState != synthclone::SESSIONSTATE_MODIFIED) {
            progressView.setCloseEnabled(true);
            progressView.setStatus("Done.");
            if (! sessionLoadWarningCount) {
                progressView.setVisible(false);
                clearProgressView();
            }
        }
        break;
    case synthclone::SESSIONSTATE_MODIFIED:
        if (lastSessionState == synthclone::SESSIONSTATE_SAVING) {
            // This transition can happen if an error occurs while trying to
            // save the session.
            progressView.setCloseEnabled(true);
            progressView.setVisible(false);
        }
        viewlet->setSaveEnabled(true);
        enabled = true;
        break;
    case synthclone::SESSIONSTATE_LOADING:
        sessionLoadView.setVisible(false);
        sessionLoadWarningCount = 0;
        // Fallthrough on purpose.
    default:
        viewlet->setSaveEnabled(false);
        enabled = false;
        progressView.setCloseEnabled(false);
        progressView.setVisible(true);
    }
    viewlet->setLoadEnabled(enabled);
    viewlet->setQuitEnabled(enabled);
    viewlet->setSaveAsEnabled(enabled);
    lastSessionState = state;
}

void
Controller::handleSessionTargetAddition(const synthclone::Target *target,
                                        int index)
{
    ComponentViewlet *viewlet = mainView.getComponentViewlet();
    viewlet->addTarget(index);
    viewlet->setTargetName(index, target->getName());
    viewlet->setTargetProgress(index, 0.0);
    viewlet->setTargetStatus(index, "");
    connect(target, SIGNAL(nameChanged(QString)),
            SLOT(handleTargetNameChange(QString)));
    connect(target, SIGNAL(progressChanged(float)),
            SLOT(handleTargetProgressChange(float)));
    connect(target, SIGNAL(statusChanged(QString)),
            SLOT(handleTargetStatusChange(QString)));
    refreshZoneBuildTargetsAction();
}

void
Controller::handleSessionTargetBuild(const synthclone::Target *target)
{
    connect(target, SIGNAL(buildWarning(const QString &)),
            SLOT(handleTargetBuildWarning(const QString &)));
    connect(target, SIGNAL(progressChanged(float)),
            SLOT(handleTargetBuildProgressChange(float)));
    connect(target, SIGNAL(statusChanged(const QString &)),
            SLOT(handleTargetBuildStatusChange(const QString &)));

    QString message = tr("Building target '%1' ...").arg(target->getName());
    progressView.addMessage(message);
    progressView.setProgress(0.0);
    progressView.setStatus(message);
    targetBuildWarningCount = 0;
    application.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void
Controller::
handleSessionTargetBuildCompletion(const synthclone::Target *target)
{
    disconnect(target, SIGNAL(buildWarning(const QString &)),
               this, SLOT(handleTargetBuildWarning(const QString &)));
    disconnect(target, SIGNAL(progressChanged(float)),
               this, SLOT(handleTargetBuildProgressChange(float)));
    disconnect(target, SIGNAL(statusChanged(const QString &)),
               this, SLOT(handleTargetBuildStatusChange(const QString &)));
    QString message = tr("Target '%1' built successfully with %2 warnings.").
        arg(target->getName(),
            QLocale::system().toString(targetBuildWarningCount));
    progressView.addMessage(message);
    application.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void
Controller::handleSessionTargetBuildError(const synthclone::Target *target,
                                          const QString &message)
{
    disconnect(target, SIGNAL(progressChanged(float)),
               this, SLOT(handleTargetBuildProgressChange(float)));
    disconnect(target, SIGNAL(statusChanged(const QString &)),
               this, SLOT(handleTargetBuildStatusChange(const QString &)));
    QString msg = tr("ERROR: Target '%1' build failed: %2").
        arg(target->getName(), message);
    progressView.addMessage(msg);
    application.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void
Controller::handleSessionTargetBuildOperation()
{
    progressView.setCloseEnabled(false);
    progressView.setProgress(0.0);
    progressView.setStatus(tr("Building targets ..."));
    progressView.setVisible(true);
    application.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void
Controller::handleSessionTargetBuildOperationCompletion()
{
    progressView.setCloseEnabled(true);
    progressView.setProgress(0.0);
    progressView.setStatus("");
}

void
Controller::handleSessionTargetMove(const synthclone::Target */*target*/,
                                    int fromIndex, int toIndex)
{
    mainView.getComponentViewlet()->moveTarget(fromIndex, toIndex);
}

void
Controller::handleSessionTargetRemoval(const synthclone::Target *target,
                                       int index)
{
    disconnect(target, SIGNAL(nameChanged(QString)),
               this, SLOT(handleTargetNameChange(QString)));
    disconnect(target, SIGNAL(progressChanged(float)),
               this, SLOT(handleTargetProgressChange(float)));
    disconnect(target, SIGNAL(statusChanged(QString)),
               this, SLOT(handleTargetStatusChange(QString)));
    mainView.getComponentViewlet()->removeTarget(index);
    if (session.getTargetCount() == 1) {
        mainView.getZoneViewlet()->setBuildTargetsEnabled(false);
    }
}

void
Controller::handleSessionZoneAddition(synthclone::Zone *zone, int index)
{
    ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();
    zoneViewlet->addZone(index);
    updateZoneViewlet(zone, index);

    connect(zone, SIGNAL(aftertouchChanged(synthclone::MIDIData)),
            SLOT(handleZoneAftertouchChange(synthclone::MIDIData)));
    connect(zone, SIGNAL(channelChanged(synthclone::MIDIData)),
            SLOT(handleZoneChannelChange(synthclone::MIDIData)));
    connect(zone, SIGNAL(channelPressureChanged(synthclone::MIDIData)),
            SLOT(handleZoneChannelPressureChange(synthclone::MIDIData)));
    connect(zone,
            SIGNAL(controlValueChanged(synthclone::MIDIData,
                                       synthclone::MIDIData)),
            SLOT(handleZoneControlValueChange(synthclone::MIDIData,
                                              synthclone::MIDIData)));
    connect(zone, SIGNAL(drySampleChanged(const synthclone::Sample *)),
            SLOT(handleZoneDrySampleChange(const synthclone::Sample *)));
    connect(zone, SIGNAL(drySampleStaleChanged(bool)),
            SLOT(handleZoneDrySampleStaleChange(bool)));
    connect(zone, SIGNAL(noteChanged(synthclone::MIDIData)),
            SLOT(handleZoneNoteChange(synthclone::MIDIData)));
    connect(zone, SIGNAL(releaseTimeChanged(synthclone::SampleTime)),
            SLOT(handleZoneReleaseTimeChange(synthclone::SampleTime)));
    connect(zone, SIGNAL(sampleTimeChanged(synthclone::SampleTime)),
            SLOT(handleZoneSampleTimeChange(synthclone::SampleTime)));
    connect(zone, SIGNAL(statusChanged(synthclone::Zone::Status)),
            SLOT(handleZoneStatusChange(synthclone::Zone::Status)));
    connect(zone, SIGNAL(velocityChanged(synthclone::MIDIData)),
            SLOT(handleZoneVelocityChange(synthclone::MIDIData)));
    connect(zone, SIGNAL(wetSampleChanged(const synthclone::Sample *)),
            SLOT(handleZoneWetSampleChange(const synthclone::Sample *)));
    connect(zone, SIGNAL(wetSampleStaleChanged(bool)),
            SLOT(handleZoneWetSampleStaleChange(bool)));

    zoneViewlet->setInvertSelectionEnabled(true);
    zoneViewlet->setSelectAllEnabled(true);
}

void
Controller::handleSessionZoneMove(synthclone::Zone */*zone*/, int fromIndex,
                                  int toIndex)
{
    mainView.getZoneViewlet()->moveZone(fromIndex, toIndex);
}

void
Controller::handleSessionZoneRemoval(synthclone::Zone */*zone*/, int index)
{
    ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();
    zoneViewlet->removeZone(index);
    if (! session.getZoneCount()) {
        zoneViewlet->setApplyEffectsEnabled(false);
        zoneViewlet->setClearSelectionEnabled(false);
        zoneViewlet->setCopyEnabled(false);
        zoneViewlet->setCutEnabled(false);
        zoneViewlet->setDeleteEnabled(false);
        zoneViewlet->setInvertSelectionEnabled(false);
        zoneViewlet->setPlayDrySampleEnabled(false);
        zoneViewlet->setPlayWetSampleEnabled(false);
        zoneViewlet->setRemoveEffectJobEnabled(false);
        zoneViewlet->setRemoveSamplerJobEnabled(false);
        zoneViewlet->setSampleEnabled(false);
        zoneViewlet->setSelectAllEnabled(false);
    }
}

void
Controller::handleSessionZoneSelectionChange(synthclone::Zone */*zone*/,
                                             int index, bool selected)
{
    ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();
    zoneViewlet->setSelected(index, selected);
    refreshZoneViewletActions();
}

////////////////////////////////////////////////////////////////////////////////
// SessionLoadView signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::
handleSessionLoadViewCreationDirectoryBrowseRequest(const QString &directory,
                                                    const QString &name)
{
    createSessionName = name;
    postDirectorySelectAction = POSTDIRECTORYSELECTACTION_CREATE_SESSION;
    directoryView.setDirectory(directory);
    directoryView.setOperation(synthclone::FileSelectionView::OPERATION_OPEN);
    directoryView.setTitle(tr("New Session"));
    directoryView.setVisible(true);
}

void
Controller::
handleSessionLoadViewCreationDirectoryChange(const QString &directory,
                                             const QString &name)
{
    verifySessionLoadViewCreation(directory, name);
}

void
Controller::
handleSessionLoadViewCreationRequest(const QString &directory,
                                     const QString &name,
                                     synthclone::SampleRate sampleRate,
                                     synthclone::SampleChannelCount count)
{
    if (verifySessionLoadViewCreation(directory, name)) {
        QDir sessionDir = QDir(directory).absoluteFilePath(name);
        Session::create(sessionDir, sampleRate, count);
        session.load(sessionDir);
    }
}

void
Controller::
handleSessionLoadViewOpenDirectoryBrowseRequest(const QString &directory)
{
    postDirectorySelectAction = POSTDIRECTORYSELECTACTION_OPEN_SESSION;
    directoryView.setDirectory(directory);
    directoryView.setOperation(synthclone::FileSelectionView::OPERATION_OPEN);
    directoryView.setTitle(tr("Open Session"));
    directoryView.setVisible(true);
}

void
Controller::handleSessionLoadViewOpenDirectoryChange(const QString &directory)
{
    verifySessionLoadViewOpen(directory);
}

void
Controller::handleSessionLoadViewOpenRequest(const QString &directory)
{
    if (verifySessionLoadViewOpen(directory)) {
        session.load(directory);
    }
}

////////////////////////////////////////////////////////////////////////////////
// SessionViewlet signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleSessionViewletLoadRequest()
{
    switch (session.getState()) {
    case synthclone::SESSIONSTATE_CURRENT:
        // Unloading the session will cause the session open view to be shown.
        session.unload();
        break;
    case synthclone::SESSIONSTATE_MODIFIED:
        postSaveChangesAction = POSTSAVECHANGESACTION_LOAD;
        saveChangesView.setVisible(true);
        break;
    default:
        assert(false);
    }
}

void
Controller::handleSessionViewletQuitRequest()
{
    processQuitRequest();
}

void
Controller::handleSessionViewletSaveAsRequest()
{
    postDirectorySelectAction = POSTDIRECTORYSELECTACTION_SAVE_SESSION;
    directoryView.setDirectory(session.getDirectory()->absolutePath());
    directoryView.setOperation(synthclone::FileSelectionView::OPERATION_SAVE);
    directoryView.setTitle(tr("Save Session"));
    directoryView.setVisible(true);
}

////////////////////////////////////////////////////////////////////////////////
// Target signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleTargetBuildProgressChange(float progress)
{
    progressView.setProgress(progress);
    application.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void
Controller::handleTargetBuildStatusChange(const QString &status)
{
    progressView.addMessage(status);
    application.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void
Controller::handleTargetBuildWarning(const QString &message)
{
    targetBuildWarningCount++;
    progressView.addMessage(tr("WARN: %1").arg(message));
    application.processEvents(QEventLoop::ExcludeUserInputEvents);
}

void
Controller::handleTargetNameChange(const QString &name)
{
    const synthclone::Target *target =
        qobject_cast<const synthclone::Target *>(sender());
    int index = session.getTargetIndex(target);
    mainView.getComponentViewlet()->setTargetName(index, name);
}

void
Controller::handleTargetProgressChange(float progress)
{
    const synthclone::Target *target =
        qobject_cast<const synthclone::Target *>(sender());
    int index = session.getTargetIndex(target);
    mainView.getComponentViewlet()->setTargetProgress(index, progress);
}

void
Controller::handleTargetStatusChange(const QString &status)
{
    const synthclone::Target *target =
        qobject_cast<const synthclone::Target *>(sender());
    int index = session.getTargetIndex(target);
    mainView.getComponentViewlet()->setTargetStatus(index, status);
}

////////////////////////////////////////////////////////////////////////////////
// ViewViewlet signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleViewViewletParticipantEditRequest()
{
    participantView.setVisible(true);
}

////////////////////////////////////////////////////////////////////////////////
// Zone signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleZoneAftertouchChange(synthclone::MIDIData aftertouch)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->
        setAftertouch(session.getZoneIndex(zone), aftertouch);
}

void
Controller::handleZoneChannelChange(synthclone::MIDIData channel)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->setChannel(session.getZoneIndex(zone), channel);
}

void
Controller::handleZoneChannelPressureChange(synthclone::MIDIData pressure)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->
        setChannelPressure(session.getZoneIndex(zone), pressure);
}

void
Controller::handleZoneControlValueChange(synthclone::MIDIData control,
                                         synthclone::MIDIData value)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->
        setControlValue(session.getZoneIndex(zone), control, value);
}

void
Controller::handleZoneDrySampleChange(const synthclone::Sample *sample)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    int index = session.getZoneIndex(zone);
    ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();
    if (sample) {
        SampleProfile profile(*sample);
        zoneViewlet->setDrySampleProfile(index, &profile);
    } else {
        zoneViewlet->setDrySampleProfile(index, 0);
    }
    if (session.isZoneSelected(zone)) {
        bool dryEnabled = static_cast<bool>(sample) &&
            static_cast<bool>(session.getSampler());
        if (dryEnabled) {
            for (int i = session.getSelectedZoneCount() - 1; i >= 0; i--) {
                synthclone::Zone *zone = session.getSelectedZone(i);
                if ((zone->getStatus() != synthclone::Zone::STATUS_NORMAL) ||
                    (! zone->getDrySample())) {
                    dryEnabled = false;
                    break;
                }
            }
        }
        zoneViewlet->
            setApplyEffectsEnabled(dryEnabled && session.getEffectCount());
        zoneViewlet->setPlayDrySampleEnabled(dryEnabled);
    }
}

void
Controller::handleZoneDrySampleStaleChange(bool stale)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->
        setDrySampleStale(session.getZoneIndex(zone), stale);
}

void
Controller::handleZoneNoteChange(synthclone::MIDIData note)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->setNote(session.getZoneIndex(zone), note);
}

void
Controller::handleZoneReleaseTimeChange(synthclone::SampleTime releaseTime)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->
        setReleaseTime(session.getZoneIndex(zone), releaseTime);
}

void
Controller::handleZoneSampleTimeChange(synthclone::SampleTime sampleTime)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->
        setSampleTime(session.getZoneIndex(zone), sampleTime);
}

void
Controller::handleZoneStatusChange(synthclone::Zone::Status status)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();
    zoneViewlet->setStatus(session.getZoneIndex(zone), status);
    refreshZoneViewletActions();
    refreshZoneBuildTargetsAction();
}

void
Controller::handleZoneVelocityChange(synthclone::MIDIData velocity)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->
        setVelocity(session.getZoneIndex(zone), velocity);
}

void
Controller::handleZoneWetSampleChange(const synthclone::Sample *sample)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    int index = session.getZoneIndex(zone);
    ZoneViewlet *zoneViewlet = mainView.getZoneViewlet();
    if (sample) {
        SampleProfile profile(*sample);
        zoneViewlet->setWetSampleProfile(index, &profile);
    } else {
        zoneViewlet->setWetSampleProfile(index, 0);
    }
    if (session.isZoneSelected(zone)) {
        bool wetEnabled = static_cast<bool>(sample) &&
            static_cast<bool>(session.getSampler());
        if (wetEnabled) {
            for (int i = session.getSelectedZoneCount() - 1; i >= 0; i--) {
                synthclone::Zone *zone = session.getSelectedZone(i);
                if ((zone->getStatus() != synthclone::Zone::STATUS_NORMAL) ||
                    (! zone->getWetSample())) {
                    wetEnabled = false;
                    break;
                }
            }
        }
        zoneViewlet->setPlayWetSampleEnabled(wetEnabled);
    }
}

void
Controller::handleZoneWetSampleStaleChange(bool stale)
{
    synthclone::Zone *zone = qobject_cast<synthclone::Zone *>(sender());
    mainView.getZoneViewlet()->
        setWetSampleStale(session.getZoneIndex(zone), stale);
}

////////////////////////////////////////////////////////////////////////////////
// ZoneListLoader signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::handleZoneListLoaderWarning(int line, int column,
                                        const QString &message)
{
    qWarning() << tr("error loading zone list from clipboard at line %1, "
                     "column %2: %3").arg(line).arg(column).arg(message);
}

////////////////////////////////////////////////////////////////////////////////
// ZoneViewlet signal handlers
////////////////////////////////////////////////////////////////////////////////

void
Controller::
handleZoneViewletAftertouchChangeRequest(int index,
                                         synthclone::MIDIData aftertouch)
{
    session.getZone(index)->setAftertouch(aftertouch);
}

void
Controller::handleZoneViewletAftertouchPropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_AFTERTOUCH);
    session.sortZones(comparer, ascending);
}

void
Controller::handleZoneViewletApplyEffectsRequest()
{
    int count = session.getSelectedZoneCount();
    for (int i = 0; i < count; i++) {
        session.addEffectJob(session.getSelectedZone(i));
    }
}

void
Controller::handleZoneViewletChannelChangeRequest(int index,
                                                  synthclone::MIDIData channel)
{
    session.getZone(index)->setChannel(channel);
}

void
Controller::
handleZoneViewletChannelPressureChangeRequest(int index,
                                              synthclone::MIDIData pressure)
{
    session.getZone(index)->setChannelPressure(pressure);
}

void
Controller::handleZoneViewletChannelPressurePropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_CHANNEL_PRESSURE);
    session.sortZones(comparer, ascending);
}

void
Controller::handleZoneViewletChannelPropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_CHANNEL);
    session.sortZones(comparer, ascending);
}

void
Controller::handleZoneViewletClearEffectJobsRequest()
{
    for (int i = session.getEffectJobCount() - 1; i >= 0; i--) {
        session.removeEffectJob(i);
    }
}

void
Controller::handleZoneViewletClearSamplerJobsRequest()
{
    for (int i = session.getSamplerJobCount() - 1; i >= 0; i--) {
        session.removeSamplerJob(i);
    }
}

void
Controller::handleZoneViewletClearSelectionRequest()
{
    for (int i = session.getZoneCount() - 1; i >= 0; i--) {
        session.setZoneSelected(i, false);
    }
}

void
Controller::
handleZoneViewletControlPropertySortRequest(synthclone::MIDIData control,
                                            bool ascending)
{
    int property = static_cast<int>(control) + ZoneComparer::PROPERTY_CONTROL_0;
    ZoneComparer comparer(property);
    session.sortZones(comparer, ascending);
}

void
Controller::
handleZoneViewletControlValueChangeRequest(int index,
                                           synthclone::MIDIData control,
                                           synthclone::MIDIData value)
{
    session.getZone(index)->setControlValue(control, value);
}

void
Controller::handleZoneViewletCopyRequest()
{
    copySelectedZones();
}

void
Controller::handleZoneViewletCutRequest()
{
    copySelectedZones();
    removeSelectedZones();
}

void
Controller::handleZoneViewletDeleteRequest()
{
    removeSelectedZones();
}

void
Controller::handleZoneViewletDrySamplePropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_DRY_SAMPLE);
    session.sortZones(comparer, ascending);
}

void
Controller::handleZoneViewletInsertRequest()
{
    session.addZone(session.getSelectedZoneCount() ?
                    session.getZoneIndex(session.getSelectedZone(0)) :
                    session.getZoneCount());
}

void
Controller::handleZoneViewletInvertSelectionRequest()
{
    for (int i = session.getZoneCount() - 1; i >= 0; i--) {
        session.setZoneSelected(i, ! session.isZoneSelected(i));
    }
}

void
Controller::handleZoneViewletNoteChangeRequest(int index,
                                               synthclone::MIDIData note)
{
    session.getZone(index)->setNote(note);
}

void
Controller::handleZoneViewletNotePropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_NOTE);
    session.sortZones(comparer, ascending);
}

void
Controller::handleZoneViewletPasteRequest()
{
    QDomDocument document;
    if (! loadClipboardZoneList(document)) {
        throw synthclone::Error(tr("clipboard does not contain zone list"));
    }
    int pasteIndex = session.getSelectedZoneCount() ?
        session.getZoneIndex(session.getSelectedZone(0)) :
        session.getZoneCount();
    ZoneListLoader zoneListLoader(session);
    connect(&zoneListLoader, SIGNAL(warning(int, int, QString)),
            SLOT(handleZoneListLoaderWarning(int, int, QString)));
    QDomElement element = document.documentElement();
    zoneListLoader.loadZones(element, pasteIndex);
}

void
Controller::handleZoneViewletPlayDrySampleRequest()
{
    int count = session.getSelectedZoneCount();
    for (int i = 0; i < count; i++) {
        session.addSamplerJob(synthclone::SamplerJob::TYPE_PLAY_DRY_SAMPLE,
                              session.getSelectedZone(i));
    }
}

void
Controller::handleZoneViewletPlayWetSampleRequest()
{
    int count = session.getSelectedZoneCount();
    for (int i = 0; i < count; i++) {
        session.addSamplerJob(synthclone::SamplerJob::TYPE_PLAY_WET_SAMPLE,
                              session.getSelectedZone(i));
    }
}

void
Controller::
handleZoneViewletReleaseTimeChangeRequest(int index,
                                          synthclone::SampleTime releaseTime)
{
    session.getZone(index)->setReleaseTime(releaseTime);
}

void
Controller::handleZoneViewletReleaseTimePropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_RELEASE_TIME);
    session.sortZones(comparer, ascending);
}

void
Controller::handleZoneViewletRemoveEffectJobRequest()
{
    for (int i = session.getSelectedZoneCount() - 1; i >= 0; i--) {
        session.removeEffectJob
            (session.getEffectJob(session.getSelectedZone(i)));
    }
}

void
Controller::handleZoneViewletRemoveSamplerJobRequest()
{
    for (int i = session.getSelectedZoneCount() - 1; i >= 0; i--) {
        session.removeSamplerJob
            (session.getSamplerJob(session.getSelectedZone(i)));
    }
}

void
Controller::handleZoneViewletSampleRequest()
{
    int count = session.getSelectedZoneCount();
    for (int i = 0; i < count; i++) {
        session.addSamplerJob(synthclone::SamplerJob::TYPE_SAMPLE,
                              session.getSelectedZone(i));
    }
}

void
Controller::
handleZoneViewletSampleTimeChangeRequest(int index,
                                         synthclone::SampleTime sampleTime)
{
    session.getZone(index)->setSampleTime(sampleTime);
}

void
Controller::handleZoneViewletSampleTimePropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_SAMPLE_TIME);
    session.sortZones(comparer, ascending);
}

void
Controller::handleZoneViewletSelectAllRequest()
{
    for (int i = session.getZoneCount() - 1; i >= 0; i--) {
        session.setZoneSelected(i, true);
    }
}

void
Controller::handleZoneViewletStatusPropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_STATUS);
    session.sortZones(comparer, ascending);
}

void
Controller::
handleZoneViewletVelocityChangeRequest(int index, synthclone::MIDIData velocity)
{
    session.getZone(index)->setVelocity(velocity);
}

void
Controller::handleZoneViewletVelocityPropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_VELOCITY);
    session.sortZones(comparer, ascending);
}

void
Controller::handleZoneViewletWetSamplePropertySortRequest(bool ascending)
{
    ZoneComparer comparer(ZoneComparer::PROPERTY_WET_SAMPLE);
    session.sortZones(comparer, ascending);
}
