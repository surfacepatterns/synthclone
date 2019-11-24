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

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <QtCore/QList>
#include <QtCore/QMap>

#include <synthclone/context.h>

class Controller;
class MenuManager;
class ParticipantManager;
class Session;
class MainView;

class Context: public synthclone::Context {

    Q_OBJECT

public:

    Context(synthclone::Participant &participant,
            ParticipantManager &participantManager, Controller &controller,
            QObject *parent=0);

    ~Context();

    const synthclone::EffectJob *
    getCurrentEffectJob() const;

    const synthclone::SamplerJob *
    getCurrentSamplerJob() const;

    const synthclone::Effect *
    getEffect(int index) const;

    int
    getEffectCount() const;

    int
    getEffectIndex(const synthclone::Effect *effect) const;

    const synthclone::EffectJob *
    getEffectJob(int index) const;

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

    synthclone::SampleRate
    getSampleRate() const;

    const synthclone::SamplerJob *
    getSamplerJob(int index) const;

    int
    getSamplerJobCount() const;

    int
    getSamplerJobIndex(const synthclone::SamplerJob *job) const;

    const synthclone::Effect *
    getSelectedEffect() const;

    const synthclone::Target *
    getSelectedTarget() const;

    synthclone::Zone *
    getSelectedZone(int index);

    int
    getSelectedZoneCount() const;

    const QDir *
    getSessionDirectory() const;

    synthclone::SessionState
    getSessionState() const;

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
    isSessionDirectory(const QDir &directory) const;

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
    addEffect(synthclone::Effect *effect, int index=-1);

    const synthclone::EffectJob *
    addEffectJob(synthclone::Zone *zone);

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
    addParticipant(synthclone::Participant *, const QByteArray &subId);

    const synthclone::Registration &
    addSampler(synthclone::Sampler *sampler);

    const synthclone::SamplerJob *
    addSamplerJob(synthclone::SamplerJob::Type type, synthclone::Zone *zone);

    const synthclone::Registration &
    addTarget(synthclone::Target *target, int index=-1);

    synthclone::Zone *
    addZone(int index=-1);

    void
    buildTargets();

    void
    createSession(const QDir &directory, synthclone::SampleRate sampleRate,
                  synthclone::SampleChannelCount count);

    void
    deactivateParticipant(const synthclone::Participant *participant);

    void
    loadSession(const QDir &directory);

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
    saveSession();

    void
    saveSession(const QDir &directory);

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
    setSessionModified();

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
    unloadSession();

protected:

#if 0
    void
    connectNotify(const char *signal);

    void
    disconnectNotify(const char *signal);
#endif

private slots:

    void
    handleEffectRemoval(QObject *obj);

    void
    handleMenuActionRemoval(QObject *obj);

    void
    handleMenuSeparatorRemoval(QObject *obj);

    void
    handleParticipantRemoval(QObject *obj);

    void
    handleSamplerRemoval(QObject *obj);

    void
    handleTargetRemoval(QObject *obj);

private:

    typedef QList<synthclone::Effect *> EffectList;
    typedef QList<synthclone::MenuAction *> MenuActionList;
    typedef QList<synthclone::MenuSeparator *> MenuSeparatorList;
    typedef QList<synthclone::Participant *> ParticipantList;
    typedef QList<synthclone::Target *> TargetList;

    template<typename T>
    const synthclone::Registration &
    addMenuAction(synthclone::MenuAction *action, T rootMenuItem,
                  const QStringList &subMenus);

    template<typename T>
    const synthclone::Registration &
    addMenuSeparator(synthclone::MenuSeparator *separator, T rootMenuItem,
                     const QStringList &subMenus);

    Controller &controller;
    EffectList effects;
    MainView &mainView;
    MenuActionList menuActions;
    MenuManager &menuManager;
    MenuSeparatorList menuSeparators;
    synthclone::Participant &participant;
    ParticipantList participants;
    ParticipantManager &participantManager;
    synthclone::Sampler *sampler;
    Session &session;
    TargetList targets;

};

#endif
