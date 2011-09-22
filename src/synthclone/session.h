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

#ifndef __SESSION_H__
#define __SESSION_H__

#include <limits>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QSemaphore>
#include <QtCore/QStringList>
#include <QtCore/QXmlStreamWriter>
#include <QtXml/QDomDocument>

#include <synthclone/effect.h>
#include <synthclone/menuaction.h>
#include <synthclone/menuseparator.h>
#include <synthclone/participant.h>
#include <synthclone/sampleoutputstream.h>
#include <synthclone/sampler.h>
#include <synthclone/samplerjob.h>
#include <synthclone/target.h>
#include <synthclone/zonecomparer.h>

#include "effectjobthread.h"
#include "registration.h"
#include "zone.h"
#include "zoneindexcomparer.h"

class Context;

class Session: public QObject {

    Q_OBJECT

    friend class EffectJobThread;

public:

    static void
    create(const QDir &directory, synthclone::SampleRate sampleRate,
           synthclone::SampleChannelCount count);

    static bool
    isDirectory(const QDir &directory);

    Session(QCoreApplication &application, QObject *parent=0);

    ~Session();

    int
    getActivatedParticipantCount(const synthclone::Participant *parent=0) const;

    const synthclone::EffectJob *
    getCurrentEffectJob() const;

    const synthclone::SamplerJob *
    getCurrentSamplerJob() const;

    const QDir *
    getDirectory() const;

    const synthclone::Effect *
    getEffect(int index) const;

    int
    getEffectCount() const;

    int
    getEffectIndex(const synthclone::Effect *effect) const;

    const synthclone::EffectJob *
    getEffectJob(int index) const;

    const synthclone::EffectJob *
    getEffectJob(const synthclone::Zone *zone) const;

    int
    getEffectJobCount() const;

    int
    getEffectJobIndex(const synthclone::EffectJob *job) const;

    const synthclone::Component *
    getFocusedComponent() const;

    int
    getMajorVersion() const;

    int
    getMinorVersion() const;

    const synthclone::Participant *
    getParticipant(const QByteArray &id) const;

    const synthclone::Participant *
    getParticipant(int index, const synthclone::Participant *parent=0) const;

    int
    getParticipantCount(const synthclone::Participant *parent=0) const;

    QByteArray
    getParticipantId(const synthclone::Participant *participant) const;

    const synthclone::Participant *
    getParticipantParent(const synthclone::Participant *participant) const;

    int
    getRevision() const;

    synthclone::SampleChannelCount
    getSampleChannelCount() const;

    const synthclone::Sampler *
    getSampler() const;

    const synthclone::SamplerJob *
    getSamplerJob(int index) const;

    const synthclone::SamplerJob *
    getSamplerJob(const synthclone::Zone *zone) const;

    int
    getSamplerJobCount() const;

    int
    getSamplerJobIndex(const synthclone::SamplerJob *job) const;

    synthclone::SampleRate
    getSampleRate() const;

    const synthclone::Effect *
    getSelectedEffect() const;

    const synthclone::Target *
    getSelectedTarget() const;

    synthclone::Zone *
    getSelectedZone(int index);

    int
    getSelectedZoneCount() const;

    synthclone::SessionState
    getState() const;

    const synthclone::Target *
    getTarget(int index) const;

    int
    getTargetCount() const;

    int
    getTargetIndex(const synthclone::Target *target) const;

    synthclone::Zone *
    getZone(int index);

    int
    getZoneCount() const;

    int
    getZoneIndex(const synthclone::Zone *zone) const;

    bool
    isAftertouchPropertyVisible() const;

    bool
    isChannelPressurePropertyVisible() const;

    bool
    isChannelPropertyVisible() const;

    bool
    isControlPropertyVisible(synthclone::MIDIData control) const;

    bool
    isDrySamplePropertyVisible() const;

    bool
    isNotePropertyVisible() const;

    bool
    isParticipantActivated(const synthclone::Participant *participant) const;

    bool
    isReleaseTimePropertyVisible() const;

    bool
    isSampleTimePropertyVisible() const;

    bool
    isStatusPropertyVisible() const;

    bool
    isVelocityPropertyVisible() const;

    bool
    isWetSamplePropertyVisible() const;

    bool
    isZoneSelected(const synthclone::Zone *zone) const;

    bool
    isZoneSelected(int index) const;

public slots:

    void
    abortCurrentSamplerJob();

    void
    activateParticipant(const synthclone::Participant *participant);

    const synthclone::Registration &
    addEffect(synthclone::Effect *effect,
              const synthclone::Participant *participant, int index=-1);

    const synthclone::EffectJob *
    addEffectJob(synthclone::Zone *zone, int index=-1);

    const synthclone::Registration &
    addMenuAction(synthclone::MenuAction *action, synthclone::Menu menu,
                  const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuAction(synthclone::MenuAction *action,
                  const synthclone::Effect *effect,
                  const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuAction(synthclone::MenuAction *action,
                  const synthclone::Sampler *sampler,
                  const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuAction(synthclone::MenuAction *action,
                  const synthclone::Target *target,
                  const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuSeparator(synthclone::MenuSeparator *separator,
                     synthclone::Menu menu,
                     const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuSeparator(synthclone::MenuSeparator *separator,
                     const synthclone::Effect *effect,
                     const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuSeparator(synthclone::MenuSeparator *separator,
                     const synthclone::Sampler *sampler,
                     const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addMenuSeparator(synthclone::MenuSeparator *separator,
                     const synthclone::Target *target,
                     const QStringList &subMenus=QStringList());

    const synthclone::Registration &
    addParticipant(synthclone::Participant *participant, const QByteArray &id);

    const synthclone::Registration &
    addParticipant(synthclone::Participant *participant,
                   const synthclone::Participant *parent,
                   const QByteArray &subId);

    const synthclone::Registration &
    addSampler(synthclone::Sampler *sampler,
               const synthclone::Participant *participant);

    const synthclone::SamplerJob *
    addSamplerJob(synthclone::SamplerJob::Type type, synthclone::Zone *zone,
                  int index=-1);

    const synthclone::Registration &
    addTarget(synthclone::Target *target,
              const synthclone::Participant *participant, int index=-1);

    synthclone::Zone *
    addZone(int index=-1);

    void
    buildTargets();

    void
    deactivateParticipant(const synthclone::Participant *participant);

    void
    load(const QDir &directory);

    void
    moveEffect(int fromIndex, int toIndex);

    void
    moveEffectJob(int fromIndex, int toIndex);

    void
    moveSamplerJob(int fromIndex, int toIndex);

    void
    moveTarget(int fromIndex, int toIndex);

    void
    moveZone(int fromIndex, int toIndex);

    void
    quit();

    void
    removeEffect(const synthclone::Effect *effect);

    void
    removeEffect(int index);

    void
    removeEffectJob(const synthclone::EffectJob *job);

    void
    removeEffectJob(int index);

    void
    removeMenuAction(const synthclone::MenuAction *action);

    void
    removeMenuSeparator(const synthclone::MenuSeparator *separator);

    void
    removeParticipant(const synthclone::Participant *participant);

    void
    removeSampler();

    void
    removeSamplerJob(const synthclone::SamplerJob *job);

    void
    removeSamplerJob(int index);

    void
    removeTarget(const synthclone::Target *target);

    void
    removeTarget(int index);

    void
    removeZone(synthclone::Zone *zone);

    void
    removeZone(int index);

    void
    reportError(const QString &message);

    void
    save();

    void
    save(const QDir &directory);

    void
    setAftertouchPropertyVisible(bool visible);

    void
    setChannelPressurePropertyVisible(bool visible);

    void
    setChannelPropertyVisible(bool visible);

    void
    setControlPropertyVisible(synthclone::MIDIData control, bool visible);

    void
    setDrySamplePropertyVisible(bool visible);

    void
    setFocusedComponent(const synthclone::Component *component);

    void
    setModified();

    void
    setNotePropertyVisible(bool visible);

    void
    setReleaseTimePropertyVisible(bool visible);

    void
    setSampleChannelCount(synthclone::SampleChannelCount count);

    void
    setSampleRate(synthclone::SampleRate sampleRate);

    void
    setSampleTimePropertyVisible(bool visible);

    void
    setSelectedEffect(const synthclone::Effect *effect);

    void
    setSelectedEffect(int index);

    void
    setSelectedTarget(const synthclone::Target *target);

    void
    setSelectedTarget(int index);

    void
    setStatusPropertyVisible(bool visible);

    void
    setVelocityPropertyVisible(bool visible);

    void
    setWetSamplePropertyVisible(bool visible);

    void
    setZoneSelected(const synthclone::Zone *zone, bool selected);

    void
    setZoneSelected(int index, bool selected);

    void
    sortZones(const synthclone::ZoneComparer &comparer, bool ascending=true);

    void
    unload();

signals:

    void
    activatingParticipant(const synthclone::Participant *participant,
                          const synthclone::Participant *parent,
                          const QByteArray &id);

    void
    addingEffect(const synthclone::Effect *effect, int index);

    void
    addingEffectJob(const synthclone::EffectJob *job, int index);

    void
    addingMenuAction(const synthclone::MenuAction *action,
                     synthclone::Menu menu, const QStringList &subMenus);

    void
    addingMenuAction(const synthclone::MenuAction *action,
                     const synthclone::Effect *effect,
                     const QStringList &subMenus);

    void
    addingMenuAction(const synthclone::MenuAction *action,
                     const synthclone::Sampler *sampler,
                     const QStringList &subMenus);

    void
    addingMenuAction(const synthclone::MenuAction *action,
                     const synthclone::Target *target,
                     const QStringList &subMenus);

    void
    addingMenuSeparator(const synthclone::MenuSeparator *separator,
                        synthclone::Menu menu, const QStringList &subMenus);

    void
    addingMenuSeparator(const synthclone::MenuSeparator *separator,
                        const synthclone::Effect *effect,
                        const QStringList &subMenus);

    void
    addingMenuSeparator(const synthclone::MenuSeparator *separator,
                        const synthclone::Sampler *sampler,
                        const QStringList &subMenus);

    void
    addingMenuSeparator(const synthclone::MenuSeparator *separator,
                        const synthclone::Target *target,
                        const QStringList &subMenus);

    void
    addingParticipant(const synthclone::Participant *participant,
                      const synthclone::Participant *parent,
                      const QByteArray &id);

    void
    addingSampler(const synthclone::Sampler *sampler);

    void
    addingSamplerJob(const synthclone::SamplerJob *job, int index);

    void
    addingTarget(const synthclone::Target *target, int index);

    void
    addingZone(synthclone::Zone *zone, int index);

    void
    aftertouchPropertyVisibilityChanged(bool visible);

    void
    buildingTargets();

    void
    channelPressurePropertyVisibilityChanged(bool visible);

    void
    channelPropertyVisibilityChanged(bool visible);

    void
    controlPropertyVisibilityChanged(synthclone::MIDIData control,
                                     bool visible);

    void
    currentEffectJobChanged(const synthclone::EffectJob *job);

    void
    currentSamplerJobChanged(const synthclone::SamplerJob *job);

    void
    deactivatingParticipant(const synthclone::Participant *participant,
                            const synthclone::Participant *parent,
                            const QByteArray &id);

    void
    drySamplePropertyVisibilityChanged(bool visible);

    void
    effectAdded(const synthclone::Effect *effect, int index);

    void
    effectJobAdded(const synthclone::EffectJob *job, int index);

    void
    effectJobCompletion();

    void
    effectJobError(const QString &message);

    void
    effectJobMoved(const synthclone::EffectJob *job, int fromIndex,
                   int toIndex);

    void
    effectJobRemoved(const synthclone::EffectJob *job, int index);

    void
    effectMoved(const synthclone::Effect *effect, int fromIndex, int toIndex);

    void
    effectRemoved(const synthclone::Effect *effect, int index);

    void
    errorReported(const QString &message);

    void
    focusedComponentChanged(const synthclone::Component *component);

    void
    loadWarning(int line, int column, const QString &message);

    void
    menuActionAdded(const synthclone::MenuAction *action, synthclone::Menu menu,
                    const QStringList &subMenus);

    void
    menuActionAdded(const synthclone::MenuAction *action,
                    const synthclone::Effect *effect,
                    const QStringList &subMenus);

    void
    menuActionAdded(const synthclone::MenuAction *action,
                    const synthclone::Sampler *sampler,
                    const QStringList &subMenus);

    void
    menuActionAdded(const synthclone::MenuAction *action,
                    const synthclone::Target *target,
                    const QStringList &subMenus);

    void
    menuActionRemoved(const synthclone::MenuAction *action,
                      synthclone::Menu menu, const QStringList &subMenus);

    void
    menuActionRemoved(const synthclone::MenuAction *action,
                      const synthclone::Effect *effect,
                      const QStringList &subMenus);

    void
    menuActionRemoved(const synthclone::MenuAction *action,
                      const synthclone::Sampler *sampler,
                      const QStringList &subMenus);

    void
    menuActionRemoved(const synthclone::MenuAction *action,
                      const synthclone::Target *target,
                      const QStringList &subMenus);

    void
    menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                       synthclone::Menu menu, const QStringList &subMenus);

    void
    menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                       const synthclone::Effect *effect,
                       const QStringList &subMenus);

    void
    menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                       const synthclone::Sampler *sampler,
                       const QStringList &subMenus);

    void
    menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                       const synthclone::Target *target,
                       const QStringList &subMenus);

    void
    menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                         synthclone::Menu menu, const QStringList &subMenus);

    void
    menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                         const synthclone::Effect *effect,
                         const QStringList &subMenus);

    void
    menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                         const synthclone::Sampler *sampler,
                         const QStringList &subMenus);

    void
    menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                         const synthclone::Target *target,
                         const QStringList &subMenus);

    void
    movingEffect(const synthclone::Effect *effect, int fromIndex, int toIndex);

    void
    movingEffectJob(const synthclone::EffectJob *job, int fromIndex,
                    int toIndex);

    void
    movingSamplerJob(const synthclone::SamplerJob *job, int fromIndex,
                     int toIndex);

    void
    movingTarget(const synthclone::Target *target, int fromIndex, int toIndex);

    void
    movingZone(synthclone::Zone *zone, int fromIndex, int toIndex);

    void
    notePropertyVisibilityChanged(bool visible);

    void
    participantActivated(const synthclone::Participant *participant,
                         const synthclone::Participant *parent,
                         const QByteArray &id);

    void
    participantAdded(const synthclone::Participant *participant,
                     const synthclone::Participant *parent,
                     const QByteArray &id);

    void
    participantDeactivated(const synthclone::Participant *participant,
                           const synthclone::Participant *parent,
                           const QByteArray &id);

    void
    participantRemoved(const synthclone::Participant *participant,
                       const synthclone::Participant *parent,
                       const QByteArray &id);

    void
    progressChanged(float progress, const QString &status);

    void
    releaseTimePropertyVisibilityChanged(bool visible);

    void
    removingEffect(const synthclone::Effect *effect, int index);

    void
    removingEffectJob(const synthclone::EffectJob *job, int index);

    void
    removingMenuAction(const synthclone::MenuAction *action,
                       synthclone::Menu menu, const QStringList &subMenus);

    void
    removingMenuAction(const synthclone::MenuAction *action,
                       const synthclone::Effect *effect,
                       const QStringList &subMenus);

    void
    removingMenuAction(const synthclone::MenuAction *action,
                       const synthclone::Sampler *sampler,
                       const QStringList &subMenus);

    void
    removingMenuAction(const synthclone::MenuAction *action,
                       const synthclone::Target *target,
                       const QStringList &subMenus);

    void
    removingMenuSeparator(const synthclone::MenuSeparator *separator,
                          synthclone::Menu menu, const QStringList &subMenus);

    void
    removingMenuSeparator(const synthclone::MenuSeparator *separator,
                          const synthclone::Effect *effect,
                          const QStringList &subMenus);

    void
    removingMenuSeparator(const synthclone::MenuSeparator *separator,
                          const synthclone::Sampler *sampler,
                          const QStringList &subMenus);

    void
    removingMenuSeparator(const synthclone::MenuSeparator *separator,
                          const synthclone::Target *target,
                          const QStringList &subMenus);

    void
    removingParticipant(const synthclone::Participant *participant,
                        const synthclone::Participant *parent,
                        const QByteArray &id);

    void
    removingSampler(const synthclone::Sampler *sampler);

    void
    removingSamplerJob(const synthclone::SamplerJob *job, int index);

    void
    removingTarget(const synthclone::Target *target, int index);

    void
    removingZone(synthclone::Zone *zone, int index);

    void
    sampleChannelCountChanged(synthclone::SampleChannelCount count);

    void
    samplerAdded(const synthclone::Sampler *sampler);

    void
    sampleRateChanged(synthclone::SampleRate sampleRate);

    void
    samplerJobAdded(const synthclone::SamplerJob *job, int index);

    void
    samplerJobError(const QString &message);

    void
    samplerJobMoved(const synthclone::SamplerJob *job, int fromIndex,
                    int toIndex);

    void
    samplerJobRemoved(const synthclone::SamplerJob *job, int index);

    void
    samplerRemoved(const synthclone::Sampler *sampler);

    void
    sampleTimePropertyVisibilityChanged(bool visible);

    void
    savingTarget(const synthclone::Target *target);

    void
    selectedEffectChanged(const synthclone::Effect *effect, int index);

    void
    selectedTargetChanged(const synthclone::Target *target, int index);

    void
    stateChanged(synthclone::SessionState state, const QDir *directory);

    void
    statusPropertyVisibilityChanged(bool visible);

    void
    targetAdded(const synthclone::Target *target, int index);

    void
    targetBuildingCompleted();

    void
    targetMoved(const synthclone::Target *target, int fromIndex, int toIndex);

    void
    targetRemoved(const synthclone::Target *target, int index);

    void
    targetSaved(const synthclone::Target *target);

    void
    targetSaveError(const synthclone::Target *target, const QString &message);

    void
    targetValidationCompleted(const synthclone::Target *target);

    void
    validatingTarget(const synthclone::Target *target);

    void
    velocityPropertyVisibilityChanged(bool visible);

    void
    wetSamplePropertyVisibilityChanged(bool visible);

    void
    zoneAdded(synthclone::Zone *zone, int index);

    void
    zoneMoved(synthclone::Zone *zone, int fromIndex, int toIndex);

    void
    zoneRemoved(synthclone::Zone *zone, int index);

    void
    zoneSelectionChanged(synthclone::Zone *zone, int index, bool selected);

private slots:

    void
    handleEffectJobCompletion();

    void
    handleEffectJobError(const QString &message);

    void
    handleSamplerJobAbort();

    void
    handleSamplerJobCompletion();

    void
    handleSamplerJobError(const QString &message);

    void
    handleTargetValidationError();

    void
    handleZoneLoad(int current, int total);

private:

    enum MenuBaseType {
        MENUBASETYPE_EFFECT,
        MENUBASETYPE_MENU,
        MENUBASETYPE_SAMPLER,
        MENUBASETYPE_TARGET
    };

    typedef QList<synthclone::EffectJob *> EffectJobList;
    typedef QList<synthclone::Effect *> EffectList;
    typedef QList<const synthclone::MenuAction *> MenuActionList;
    typedef QList<const synthclone::MenuSeparator *> MenuSeparatorList;
    typedef QList<synthclone::Participant *> ParticipantList;
    typedef QList<synthclone::SamplerJob *> SamplerJobList;
    typedef QList<synthclone::Target *> TargetList;
    typedef QList<synthclone::Zone *> ZoneList;

    struct ComponentData {
        MenuActionList menuActions;
        MenuSeparatorList menuSeparators;
        const synthclone::Participant *participant;
        Registration *registration;
    };

    struct MenuItemData {
        union {
            synthclone::Menu menu;
            const synthclone::Effect *effect;
            const synthclone::Sampler *sampler;
            const synthclone::Target *target;
        } menuBase;
        MenuBaseType menuBaseType;
        Registration *registration;
        QStringList subMenus;
    };

    struct ParticipantData {
        ParticipantList children;
        Context *context;
        QByteArray id;
        const synthclone::Participant *parent;
        synthclone::Participant *participant;
        Registration *registration;
    };

    typedef QMap<const synthclone::Effect *, ComponentData *> EffectDataMap;
    typedef QMap<const synthclone::MenuAction *,
                 MenuItemData *> MenuActionDataMap;
    typedef QMap<const synthclone::MenuSeparator *,
                 MenuItemData *> MenuSeparatorDataMap;
    typedef QMap<const synthclone::Participant *,
                 ParticipantData *> ParticipantDataMap;
    typedef QMap<QByteArray, synthclone::Participant *> ParticipantIdMap;
    typedef QMap<const synthclone::Target *, ComponentData *> TargetDataMap;
    typedef QMap<const synthclone::Zone *,
                 synthclone::EffectJob *> ZoneEffectJobMap;
    typedef QMap<const synthclone::Zone *,
                 synthclone::SamplerJob *> ZoneSamplerJobMap;

    static void
    initializeDirectory(const QDir &directory);

    static void
    initializeWriter(QXmlStreamWriter &writer, QFile &file,
                     const QDir &directory);

    static bool
    loadXML(const QDir &directory, QDomDocument &document);

    void
    activateParticipant(const synthclone::Participant *participant,
                        const QVariant &state);

    QString
    createUniqueSampleFile(const QDir &sessionDirectory);

    void
    emitLoadWarning(const QDomElement &element, const QString &message);

    synthclone::Participant *
    getActivatedParticipant(const QDomElement &element);

    QDir
    getSamplesDirectory(const QDir &sessionDirectory);

    void
    insertSelectedZone(synthclone::Zone *zone);

    QVariant
    readXMLState(const QDomElement &element);

    QVariant
    readXMLVariant(const QDomElement &element);

    void
    recycleCurrentEffectJob();

    void
    recycleCurrentSamplerJob();

    void
    refreshWetSample(Zone *zone);

    void
    removeComponentMenuItems(ComponentData *data);

    void
    runEffectJobs();

    void
    sortZones(const synthclone::ZoneComparer &comparer, bool ascending,
              int leftIndex, int rightIndex);

    void
    swapZones(int firstIndex, int secondIndex);

    synthclone::EffectJob *
    takeEffectJob(int index);

    synthclone::SamplerJob *
    takeSamplerJob(int index);

    void
    updateEffectJobs();

    void
    updateSamplerJobs();

    bool
    verifyBooleanAttribute(const QDomElement &element, const QString &name,
                           bool defaultValue);

    bool
    verifySubId(const QByteArray &subId);

    bool
    verifyWholeNumberAttribute(const QDomElement &element, const QString &name,
                               quint32 &value, quint32 minimumValue=0,
                               quint32 maximumValue=
                               std::numeric_limits<quint32>::max());

    const synthclone::Participant *
    writeXMLParticipantId(QXmlStreamWriter &writer,
                          const synthclone::Participant *participant);

    void
    writeXMLParticipantList(QXmlStreamWriter &writer,
                            const ParticipantList &participants);

    void
    writeXMLState(QXmlStreamWriter &writer, const QVariant &value);

    bool aftertouchPropertyVisible;
    QCoreApplication &application;
    bool channelPressurePropertyVisible;
    bool channelPropertyVisible;
    bool controlPropertiesVisible[0x80];
    synthclone::EffectJob *currentEffectJob;
    synthclone::Sample *currentEffectJobWetSample;
    synthclone::SamplerJob *currentSamplerJob;
    synthclone::Sample *currentSamplerJobSample;
    synthclone::SampleStream *currentSamplerJobStream;
    QDir *directory;
    bool drySamplePropertyVisible;
    EffectDataMap effectDataMap;
    EffectJobList effectJobs;
    QSemaphore effectJobSemaphore;
    EffectJobThread effectJobThread;
    EffectList effects;
    const synthclone::Component *focusedComponent;
    bool notePropertyVisible;
    MenuActionDataMap menuActionDataMap;
    MenuSeparatorDataMap menuSeparatorDataMap;
    ParticipantDataMap participantDataMap;
    ParticipantIdMap participantIdMap;
    bool releaseTimePropertyVisible;
    ParticipantList rootParticipants;
    synthclone::Sampler *sampler;
    ComponentData samplerData;
    SamplerJobList samplerJobs;
    bool sampleTimePropertyVisible;
    const synthclone::Effect *selectedEffect;
    const synthclone::Target *selectedTarget;
    ZoneList selectedZones;
    SessionSampleData sessionSampleData;
    synthclone::SessionState state;
    bool statusPropertyVisible;
    TargetList targets;
    TargetDataMap targetDataMap;
    bool targetValidationErrorsOccurred;
    bool velocityPropertyVisible;
    bool wetSamplePropertyVisible;
    ZoneList zones;
    ZoneEffectJobMap zoneEffectJobMap;
    ZoneIndexComparer zoneIndexComparer;
    ZoneSamplerJobMap zoneSamplerJobMap;

};

#endif
