/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011-2012 Devin Anderson
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

#include "context.h"
#include "controller.h"
#include "participantmanager.h"

// Session signals

static QByteArray ADDING_EFFECT_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingEffect(const synthclone::Effect *, int)));
static QByteArray EFFECT_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(effectAdded(const synthclone::Effect *, int)));
static QByteArray EFFECT_MOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(effectMoved(const synthclone::Effect *, int, int)));
static QByteArray EFFECT_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(effectRemoved(const synthclone::Effect *, int)));
static QByteArray MOVING_EFFECT_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(movingEffect(const synthclone::Effect *, int, int)));
static QByteArray REMOVING_EFFECT_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingEffect(const synthclone::Effect *, int)));

static QByteArray ADDING_SAMPLER_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingSampler(const synthclone::Sampler *)));
static QByteArray REMOVING_SAMPLER_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingSampler(const synthclone::Sampler *)));
static QByteArray SAMPLER_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(samplerAdded(const synthclone::Sampler *)));
static QByteArray SAMPLER_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(samplerRemoved(const synthclone::Sampler *)));

static QByteArray ADDING_TARGET_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingTarget(const synthclone::Target *, int)));
static QByteArray TARGET_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(targetAdded(const synthclone::Target *, int)));
static QByteArray TARGET_MOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(targetMoved(const synthclone::Target *, int, int)));
static QByteArray TARGET_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(targetRemoved(const synthclone::Target *, int)));
static QByteArray MOVING_TARGET_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(movingTarget(const synthclone::Target *, int, int)));
static QByteArray REMOVING_TARGET_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingTarget(const synthclone::Target *, int)));

static QByteArray ADDING_EFFECT_JOB_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingEffectJob(const synthclone::EffectJob *, int)));
static QByteArray EFFECT_JOB_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(effectJobAdded(const synthclone::EffectJob *, int)));
static QByteArray EFFECT_JOB_MOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(effectJobMoved(const synthclone::EffectJob *, int, int)));
static QByteArray EFFECT_JOB_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(effectJobRemoved(const synthclone::EffectJob *, int)));
static QByteArray MOVING_EFFECT_JOB_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(movingEffectJob(const synthclone::EffectJob *, int, int)));
static QByteArray REMOVING_EFFECT_JOB_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingEffectJob(const synthclone::EffectJob *, int)));

static QByteArray ADDING_SAMPLER_JOB_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingSamplerJob(const synthclone::SamplerJob *, int)));
static QByteArray SAMPLER_JOB_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(samplerJobAdded(const synthclone::SamplerJob *, int)));
static QByteArray SAMPLER_JOB_MOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(samplerJobMoved(const synthclone::SamplerJob *, int, int)));
static QByteArray SAMPLER_JOB_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(samplerJobRemoved(const synthclone::SamplerJob *, int)));
static QByteArray MOVING_SAMPLER_JOB_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(movingSamplerJob(const synthclone::SamplerJob *, int, int)));
static QByteArray REMOVING_SAMPLER_JOB_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingSamplerJob(const synthclone::SamplerJob *, int)));

static QByteArray SELECTED_EFFECT_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(selectedEffectChanged(const synthclone::Effect *, int)));
static QByteArray SELECTED_TARGET_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(selectedTargetChanged(const synthclone::Target *, int)));
static QByteArray CURRENT_EFFECT_JOB_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(currentEffectJobChanged(const synthclone::EffectJob *)));
static QByteArray CURRENT_SAMPLER_JOB_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(currentSamplerJobChanged(const synthclone::SamplerJob *)));
static QByteArray FOCUSED_COMPONENT_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(focusedComponentChanged(const synthclone::Component *)));

static QByteArray ADDING_ZONE_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingZone(synthclone::Zone *, int)));
static QByteArray MOVING_ZONE_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(movingZone(synthclone::Zone *, int, int)));
static QByteArray REMOVING_ZONE_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingZone(synthclone::Zone *, int)));
static QByteArray ZONE_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(zoneAdded(synthclone::Zone *, int)));
static QByteArray ZONE_MOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(zoneMoved(synthclone::Zone *, int, int)));
static QByteArray ZONE_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(zoneRemoved(synthclone::Zone *, int)));

static QByteArray ZONE_SELECTION_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(zoneSelectionChanged(synthclone::Zone *, int, bool)));

static QByteArray ADDING_MENU_ACTION_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingMenuAction(const synthclone::MenuAction *,
                             synthclone::Menu,
                             const QStringList &)));
static QByteArray ADDING_EFFECT_MENU_ACTION_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingMenuAction(const synthclone::MenuAction *,
                             const synthclone::Effect *,
                             const QStringList &)));
static QByteArray ADDING_SAMPLER_MENU_ACTION_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingMenuAction(const synthclone::MenuAction *,
                             const synthclone::Sampler *,
                             const QStringList &)));
static QByteArray ADDING_TARGET_MENU_ACTION_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingMenuAction(const synthclone::MenuAction *,
                             const synthclone::Target *,
                             const QStringList &)));

static QByteArray ADDING_MENU_SEPARATOR_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingMenuSeparator(const synthclone::MenuSeparator *,
                                synthclone::Menu,
                                const QStringList &)));
static QByteArray ADDING_EFFECT_MENU_SEPARATOR_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingMenuSeparator(const synthclone::MenuSeparator *,
                                const synthclone::Effect *,
                                const QStringList &)));
static QByteArray ADDING_SAMPLER_MENU_SEPARATOR_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingMenuSeparator(const synthclone::MenuSeparator *,
                                const synthclone::Sampler *,
                                const QStringList &)));
static QByteArray ADDING_TARGET_MENU_SEPARATOR_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingMenuSeparator(const synthclone::MenuSeparator *,
                                const synthclone::Target *,
                                const QStringList &)));

static QByteArray MENU_ACTION_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuActionAdded(const synthclone::MenuAction *,
                            synthclone::Menu,
                            const QStringList &)));
static QByteArray EFFECT_MENU_ACTION_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuActionAdded(const synthclone::MenuAction *,
                            const synthclone::Effect *,
                            const QStringList &)));
static QByteArray SAMPLER_MENU_ACTION_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuActionAdded(const synthclone::MenuAction *,
                            const synthclone::Sampler *,
                            const QStringList &)));
static QByteArray TARGET_MENU_ACTION_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuActionAdded(const synthclone::MenuAction *,
                            const synthclone::Target *,
                            const QStringList &)));

static QByteArray MENU_ACTION_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuActionRemoved(const synthclone::MenuAction *,
                              synthclone::Menu,
                              const QStringList &)));
static QByteArray EFFECT_MENU_ACTION_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuActionRemoved(const synthclone::MenuAction *,
                              const synthclone::Effect *,
                              const QStringList &)));
static QByteArray SAMPLER_MENU_ACTION_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuActionRemoved(const synthclone::MenuAction *,
                              const synthclone::Sampler *,
                              const QStringList &)));
static QByteArray TARGET_MENU_ACTION_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuActionRemoved(const synthclone::MenuAction *,
                              const synthclone::Target *,
                              const QStringList &)));

static QByteArray MENU_SEPARATOR_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuSeparatorAdded(const synthclone::MenuSeparator *,
                               synthclone::Menu,
                               const QStringList &)));
static QByteArray EFFECT_MENU_SEPARATOR_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuSeparatorAdded(const synthclone::MenuSeparator *,
                               const synthclone::Effect *,
                               const QStringList &)));
static QByteArray SAMPLER_MENU_SEPARATOR_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuSeparatorAdded(const synthclone::MenuSeparator *,
                               const synthclone::Sampler *,
                               const QStringList &)));
static QByteArray TARGET_MENU_SEPARATOR_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuSeparatorAdded(const synthclone::MenuSeparator *,
                               const synthclone::Target *,
                               const QStringList &)));

static QByteArray MENU_SEPARATOR_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuSeparatorRemoved(const synthclone::MenuSeparator *,
                                 synthclone::Menu,
                                 const QStringList &)));
static QByteArray EFFECT_MENU_SEPARATOR_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuSeparatorRemoved(const synthclone::MenuSeparator *,
                                 const synthclone::Effect *,
                                 const QStringList &)));
static QByteArray SAMPLER_MENU_SEPARATOR_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuSeparatorRemoved(const synthclone::MenuSeparator *,
                                 const synthclone::Sampler *,
                                 const QStringList &)));
static QByteArray TARGET_MENU_SEPARATOR_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(menuSeparatorRemoved(const synthclone::MenuSeparator *,
                                 const synthclone::Target *,
                                 const QStringList &)));

static QByteArray REMOVING_MENU_ACTION_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingMenuAction(const synthclone::MenuAction *,
                               synthclone::Menu,
                               const QStringList &)));
static QByteArray REMOVING_EFFECT_MENU_ACTION_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingMenuAction(const synthclone::MenuAction *,
                               const synthclone::Effect *,
                               const QStringList &)));
static QByteArray REMOVING_SAMPLER_MENU_ACTION_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingMenuAction(const synthclone::MenuAction *,
                               const synthclone::Sampler *,
                               const QStringList &)));
static QByteArray REMOVING_TARGET_MENU_ACTION_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingMenuAction(const synthclone::MenuAction *,
                               const synthclone::Target *,
                               const QStringList &)));

static QByteArray REMOVING_MENU_SEPARATOR_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingMenuSeparator(const synthclone::MenuSeparator *,
                                  synthclone::Menu,
                                  const QStringList &)));
static QByteArray REMOVING_EFFECT_MENU_SEPARATOR_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingMenuSeparator(const synthclone::MenuSeparator *,
                                  const synthclone::Effect *,
                                  const QStringList &)));
static QByteArray REMOVING_SAMPLER_MENU_SEPARATOR_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingMenuSeparator(const synthclone::MenuSeparator *,
                                  const synthclone::Sampler *,
                                  const QStringList &)));
static QByteArray REMOVING_TARGET_MENU_SEPARATOR_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingMenuSeparator(const synthclone::MenuSeparator *,
                                  const synthclone::Target *,
                                  const QStringList &)));

static QByteArray BUILDING_TARGET_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(buildingTarget(const synthclone::Target *)));
static QByteArray BUILDING_TARGETS_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(buildingTargets()));
static QByteArray TARGET_BUILD_ERROR_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(targetBuildError(const synthclone::Target *, const QString &)));
static QByteArray TARGET_BUILT_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(targetBuilt(const synthclone::Target *)));
static QByteArray TARGETS_BUILT_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(targetsBuilt()));

static QByteArray ACTIVATING_PARTICIPANT_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(activatingParticipant(const synthclone::Participant *,
                                  const synthclone::Participant *,
                                  const QByteArray &)));
static QByteArray ADDING_PARTICIPANT_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(addingParticipant(const synthclone::Participant *,
                              const synthclone::Participant *,
                              const QByteArray &)));
static QByteArray DEACTIVATING_PARTICIPANT_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(deactivatingParticipant(const synthclone::Participant *,
                                    const synthclone::Participant *,
                                    const QByteArray &)));
static QByteArray PARTICIPANT_ACTIVATED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(participantActivated(const synthclone::Participant *,
                                 const synthclone::Participant *,
                                 const QByteArray &)));
static QByteArray PARTICIPANT_ADDED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(participantAdded(const synthclone::Participant *,
                             const synthclone::Participant *,
                             const QByteArray &)));
static QByteArray PARTICIPANT_DEACTIVATED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(participantDeactivated(const synthclone::Participant *,
                                   const synthclone::Participant *,
                                   const QByteArray &)));
static QByteArray PARTICIPANT_REMOVED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(participantRemoved(const synthclone::Participant *,
                               const synthclone::Participant *,
                               const QByteArray &)));
static QByteArray REMOVING_PARTICIPANT_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(removingParticipant(const synthclone::Participant *,
                                const synthclone::Participant *,
                                const QByteArray &)));

static QByteArray AFTERTOUCH_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(aftertouchPropertyVisibilityChanged(bool)));
static QByteArray CHANNEL_PRESSURE_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(channelPressurePropertyVisibilityChanged(bool)));
static QByteArray CHANNEL_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(channelPropertyVisibilityChanged(bool)));
static QByteArray CONTROL_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(controlPropertyVisibilityChanged(synthclone::MIDIData, bool)));
static QByteArray DRY_SAMPLE_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(drySamplePropertyVisibilityChanged(bool)));
static QByteArray NOTE_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(notePropertyVisibilityChanged(bool)));
static QByteArray RELEASE_TIME_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(releaseTimePropertyVisibilityChanged(bool)));
static QByteArray SAMPLE_TIME_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(sampleTimePropertyVisibilityChanged(bool)));
static QByteArray STATUS_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(statusPropertyVisibilityChanged(bool)));
static QByteArray VELOCITY_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(velocityPropertyVisibilityChanged(bool)));
static QByteArray WET_SAMPLE_PROPERTY_VISIBILITY_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(wetSamplePropertyVisibilityChanged(bool)));

static QByteArray SAMPLE_CHANNEL_COUNT_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(sampleChannelCountChanged(synthclone::SampleChannelCount)));
static QByteArray SAMPLE_RATE_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(sampleRateChanged(synthclone::SampleRate)));

static QByteArray ERROR_REPORTED_SIGNAL =
    QMetaObject::normalizedSignature(SIGNAL(errorReported(const QString &)));

static QByteArray SESSION_STATE_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(sessionStateChanged(synthclone::SessionState, const QDir *)));

static QByteArray STATE_CHANGED_SIGNAL =
    QMetaObject::normalizedSignature
    (SIGNAL(stateChanged(synthclone::SessionState, const QDir *)));

// And now, back to our regularly scheduled class definition.

Context::Context(synthclone::Participant &participant,
                 ParticipantManager &participantManager, Controller &controller,
                 QObject *parent):
    synthclone::Context(parent),
    controller(controller),
    mainView(controller.getMainView()),
    menuManager(controller.getMenuManager()),
    participant(participant),
    participantManager(participantManager),
    session(controller.getSession())
{
    sampler = 0;

    // Controller signals
    connect_(&controller, ERROR_REPORTED_SIGNAL);

    // Menu manager signals
    connect_(&menuManager, ADDING_MENU_ACTION_SIGNAL);
    connect_(&menuManager, ADDING_EFFECT_MENU_ACTION_SIGNAL);
    connect_(&menuManager, ADDING_SAMPLER_MENU_ACTION_SIGNAL);
    connect_(&menuManager, ADDING_TARGET_MENU_ACTION_SIGNAL);

    connect_(&menuManager, ADDING_MENU_SEPARATOR_SIGNAL);
    connect_(&menuManager, ADDING_EFFECT_MENU_SEPARATOR_SIGNAL);
    connect_(&menuManager, ADDING_SAMPLER_MENU_SEPARATOR_SIGNAL);
    connect_(&menuManager, ADDING_TARGET_MENU_SEPARATOR_SIGNAL);

    connect_(&menuManager, MENU_ACTION_ADDED_SIGNAL);
    connect_(&menuManager, EFFECT_MENU_ACTION_ADDED_SIGNAL);
    connect_(&menuManager, SAMPLER_MENU_ACTION_ADDED_SIGNAL);
    connect_(&menuManager, TARGET_MENU_ACTION_ADDED_SIGNAL);

    connect_(&menuManager, MENU_ACTION_REMOVED_SIGNAL);
    connect_(&menuManager, EFFECT_MENU_ACTION_REMOVED_SIGNAL);
    connect_(&menuManager, SAMPLER_MENU_ACTION_REMOVED_SIGNAL);
    connect_(&menuManager, TARGET_MENU_ACTION_REMOVED_SIGNAL);

    connect_(&menuManager, MENU_SEPARATOR_ADDED_SIGNAL);
    connect_(&menuManager, EFFECT_MENU_SEPARATOR_ADDED_SIGNAL);
    connect_(&menuManager, SAMPLER_MENU_SEPARATOR_ADDED_SIGNAL);
    connect_(&menuManager, TARGET_MENU_SEPARATOR_ADDED_SIGNAL);

    connect_(&menuManager, MENU_SEPARATOR_REMOVED_SIGNAL);
    connect_(&menuManager, EFFECT_MENU_SEPARATOR_REMOVED_SIGNAL);
    connect_(&menuManager, SAMPLER_MENU_SEPARATOR_REMOVED_SIGNAL);
    connect_(&menuManager, TARGET_MENU_SEPARATOR_REMOVED_SIGNAL);

    connect_(&menuManager, REMOVING_MENU_ACTION_SIGNAL);
    connect_(&menuManager, REMOVING_EFFECT_MENU_ACTION_SIGNAL);
    connect_(&menuManager, REMOVING_SAMPLER_MENU_ACTION_SIGNAL);
    connect_(&menuManager, REMOVING_TARGET_MENU_ACTION_SIGNAL);

    connect_(&menuManager, REMOVING_MENU_SEPARATOR_SIGNAL);
    connect_(&menuManager, REMOVING_EFFECT_MENU_SEPARATOR_SIGNAL);
    connect_(&menuManager, REMOVING_SAMPLER_MENU_SEPARATOR_SIGNAL);
    connect_(&menuManager, REMOVING_TARGET_MENU_SEPARATOR_SIGNAL);

    // Participant manager signals
    connect_(&participantManager, ACTIVATING_PARTICIPANT_SIGNAL);
    connect_(&participantManager, ADDING_PARTICIPANT_SIGNAL);
    connect_(&participantManager, DEACTIVATING_PARTICIPANT_SIGNAL);
    connect_(&participantManager, PARTICIPANT_ACTIVATED_SIGNAL);
    connect_(&participantManager, PARTICIPANT_ADDED_SIGNAL);
    connect_(&participantManager, PARTICIPANT_DEACTIVATED_SIGNAL);
    connect_(&participantManager, PARTICIPANT_REMOVED_SIGNAL);
    connect_(&participantManager, REMOVING_PARTICIPANT_SIGNAL);

    // Session signals
    connect_(&session, ADDING_EFFECT_SIGNAL);
    connect_(&session, EFFECT_ADDED_SIGNAL);
    connect_(&session, EFFECT_MOVED_SIGNAL);
    connect_(&session, EFFECT_REMOVED_SIGNAL);
    connect_(&session, MOVING_EFFECT_SIGNAL);
    connect_(&session, REMOVING_EFFECT_SIGNAL);

    connect_(&session, ADDING_SAMPLER_SIGNAL);
    connect_(&session, SAMPLER_ADDED_SIGNAL);
    connect_(&session, SAMPLER_REMOVED_SIGNAL);
    connect_(&session, REMOVING_SAMPLER_SIGNAL);

    connect_(&session, ADDING_TARGET_SIGNAL);
    connect_(&session, TARGET_ADDED_SIGNAL);
    connect_(&session, TARGET_MOVED_SIGNAL);
    connect_(&session, TARGET_REMOVED_SIGNAL);
    connect_(&session, MOVING_TARGET_SIGNAL);
    connect_(&session, REMOVING_TARGET_SIGNAL);

    connect_(&session, ADDING_EFFECT_JOB_SIGNAL);
    connect_(&session, EFFECT_JOB_ADDED_SIGNAL);
    connect_(&session, EFFECT_JOB_MOVED_SIGNAL);
    connect_(&session, EFFECT_JOB_REMOVED_SIGNAL);
    connect_(&session, MOVING_EFFECT_JOB_SIGNAL);
    connect_(&session, REMOVING_EFFECT_JOB_SIGNAL);

    connect_(&session, ADDING_SAMPLER_JOB_SIGNAL);
    connect_(&session, SAMPLER_JOB_ADDED_SIGNAL);
    connect_(&session, SAMPLER_JOB_MOVED_SIGNAL);
    connect_(&session, SAMPLER_JOB_REMOVED_SIGNAL);
    connect_(&session, MOVING_SAMPLER_JOB_SIGNAL);
    connect_(&session, REMOVING_SAMPLER_JOB_SIGNAL);

    connect_(&session, SELECTED_EFFECT_CHANGED_SIGNAL);
    connect_(&session, SELECTED_TARGET_CHANGED_SIGNAL);

    connect_(&session, CURRENT_EFFECT_JOB_CHANGED_SIGNAL);
    connect_(&session, CURRENT_SAMPLER_JOB_CHANGED_SIGNAL);

    connect_(&session, FOCUSED_COMPONENT_CHANGED_SIGNAL);

    connect_(&session, ADDING_ZONE_SIGNAL);
    connect_(&session, ZONE_ADDED_SIGNAL);
    connect_(&session, ZONE_MOVED_SIGNAL);
    connect_(&session, ZONE_REMOVED_SIGNAL);
    connect_(&session, MOVING_ZONE_SIGNAL);
    connect_(&session, REMOVING_ZONE_SIGNAL);

    connect_(&session, ZONE_SELECTION_CHANGED_SIGNAL);

    connect_(&session, BUILDING_TARGET_SIGNAL);
    connect_(&session, BUILDING_TARGETS_SIGNAL);
    connect_(&session, TARGET_BUILD_ERROR_SIGNAL);
    connect_(&session, TARGET_BUILT_SIGNAL);
    connect_(&session, TARGETS_BUILT_SIGNAL);

    connect_(&session, AFTERTOUCH_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, CHANNEL_PRESSURE_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, CHANNEL_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, CONTROL_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, DRY_SAMPLE_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, NOTE_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, RELEASE_TIME_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, SAMPLE_TIME_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, STATUS_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, VELOCITY_PROPERTY_VISIBILITY_CHANGED_SIGNAL);
    connect_(&session, WET_SAMPLE_PROPERTY_VISIBILITY_CHANGED_SIGNAL);

    connect_(&session, SAMPLE_CHANNEL_COUNT_CHANGED_SIGNAL);
    connect_(&session, SAMPLE_RATE_CHANGED_SIGNAL);

    connect(&session, STATE_CHANGED_SIGNAL, SESSION_STATE_CHANGED_SIGNAL);
}

Context::~Context()
{
    int i;

    for (i = participants.count() - 1; i >= 0; i--) {
        participantManager.removeParticipant(participants[i]);
    }
    if (sampler) {
        session.removeSampler();
    }
    for (i = effects.count() - 1; i >= 0; i--) {
        session.removeEffect(effects[i]);
    }
    for (i = targets.count() - 1; i >= 0; i--) {
        session.removeTarget(targets[i]);
    }

    // Removing components created in this context will remove all of the menu
    // items associated with those components; however, there may also be menu
    // items associated with components that weren't created in this context.

    for (i = menuActions.count() - 1; i >= 0; i--) {
        // XXX: Change Remove from main view.
        menuManager.removeMenuAction(menuActions[i]);
    }
    for (i = menuSeparators.count() - 1; i >= 0; i--) {
        // XXX: Change to remove from main view.
        menuManager.removeMenuSeparator(menuSeparators[i]);
    }

    assert(! effects.count());
    assert(! menuActions.count());
    assert(! menuSeparators.count());
    assert(! participants.count());
    assert(! sampler);
    assert(! targets.count());
}

void
Context::abortCurrentSamplerJob()
{
    session.abortCurrentSamplerJob();
}

void
Context::activateParticipant(const synthclone::Participant *participant)
{
    participantManager.activateParticipant(participant);
}

const synthclone::Registration &
Context::addEffect(synthclone::Effect *effect, int index)
{
    const synthclone::Registration &registration =
        session.addEffect(effect, &participant, index);
    effects.append(effect);
    connect(&registration, SIGNAL(unregistered(QObject *)),
            SLOT(handleEffectRemoval(QObject *)));
    return registration;
}

const synthclone::EffectJob *
Context::addEffectJob(synthclone::Zone *zone)
{
    return session.addEffectJob(zone);
}

template<typename T>
const synthclone::Registration &
Context::addMenuAction(synthclone::MenuAction *action, T rootMenuItem,
                       const QStringList &subMenus)
{
    const synthclone::Registration &registration =
        menuManager.addMenuAction(action, rootMenuItem, subMenus);
    menuActions.append(action);
    connect(&registration, SIGNAL(unregistered(QObject *)),
            SLOT(handleMenuActionRemoval(QObject *)));
    return registration;
}

const synthclone::Registration &
Context::addMenuAction(synthclone::MenuAction *action, synthclone::Menu menu,
                       const QStringList &subMenus)
{
    return addMenuAction<synthclone::Menu>(action, menu, subMenus);
}

const synthclone::Registration &
Context::addMenuAction(synthclone::MenuAction *action,
                       const synthclone::Effect *effect,
                       const QStringList &subMenus)
{
    return addMenuAction<const synthclone::Effect *>(action, effect, subMenus);
}

const synthclone::Registration &
Context::addMenuAction(synthclone::MenuAction *action,
                       const synthclone::Sampler *sampler,
                       const QStringList &subMenus)
{
    return addMenuAction<const synthclone::Sampler *>(action, sampler,
                                                      subMenus);
}

const synthclone::Registration &
Context::addMenuAction(synthclone::MenuAction *action,
                       const synthclone::Target *target,
                       const QStringList &subMenus)
{
    return addMenuAction<const synthclone::Target *>(action, target, subMenus);
}

template<typename T>
const synthclone::Registration &
Context::addMenuSeparator(synthclone::MenuSeparator *separator, T rootMenuItem,
                          const QStringList &subMenus)
{
    const synthclone::Registration &registration =
        menuManager.addMenuSeparator(separator, rootMenuItem, subMenus);
    menuSeparators.append(separator);
    connect(&registration, SIGNAL(unregistered(QObject *)),
            SLOT(handleMenuSeparatorRemoval(QObject *)));
    return registration;
}

const synthclone::Registration &
Context::addMenuSeparator(synthclone::MenuSeparator *separator,
                          synthclone::Menu menu, const QStringList &subMenus)
{
    return addMenuSeparator<synthclone::Menu>(separator, menu, subMenus);
}

const synthclone::Registration &
Context::addMenuSeparator(synthclone::MenuSeparator *separator,
                          const synthclone::Effect *effect,
                          const QStringList &subMenus)
{
    return addMenuSeparator<const synthclone::Effect *>(separator, effect,
                                                        subMenus);
}

const synthclone::Registration &
Context::addMenuSeparator(synthclone::MenuSeparator *separator,
                          const synthclone::Sampler *sampler,
                          const QStringList &subMenus)
{
    return addMenuSeparator<const synthclone::Sampler *>(separator, sampler,
                                                         subMenus);
}

const synthclone::Registration &
Context::addMenuSeparator(synthclone::MenuSeparator *separator,
                          const synthclone::Target *target,
                          const QStringList &subMenus)
{
    return addMenuSeparator<const synthclone::Target *>(separator, target,
                                                        subMenus);
}

const synthclone::Registration &
Context::addParticipant(synthclone::Participant *participant,
                        const QByteArray &subId)
{
    const synthclone::Registration &registration =
        participantManager.addParticipant(participant, &(this->participant),
                                          subId);
    participants.append(participant);
    connect(&registration, SIGNAL(unregistered(QObject *)),
            SLOT(handleParticipantRemoval(QObject *)));
    return registration;
}

const synthclone::Registration &
Context::addSampler(synthclone::Sampler *sampler)
{
    const synthclone::Registration &registration =
        session.addSampler(sampler, &participant);
    this->sampler = sampler;
    connect(&registration, SIGNAL(unregistered(QObject *)),
            SLOT(handleSamplerRemoval(QObject *)));
    return registration;
}

const synthclone::SamplerJob *
Context::addSamplerJob(synthclone::SamplerJob::Type type,
                       synthclone::Zone *zone)
{
    return session.addSamplerJob(type, zone);
}

const synthclone::Registration &
Context::addTarget(synthclone::Target *target, int index)
{
    const synthclone::Registration &registration =
        session.addTarget(target, &participant, index);
    targets.append(target);
    connect(&registration, SIGNAL(unregistered(QObject *)),
            SLOT(handleTargetRemoval(QObject *)));
    return registration;
}

synthclone::Zone *
Context::addZone(int index)
{
    return session.addZone(index);
}

void
Context::buildTargets()
{
    session.buildTargets();
}

QMetaObject::Connection
Context::connect_(const QObject *sender, const char *signal)
{
    return connect(sender, signal, signal);
}

void
Context::createSession(const QDir &directory, synthclone::SampleRate sampleRate,
                       synthclone::SampleChannelCount count)
{
    Session::create(directory, sampleRate, count);
}

void
Context::deactivateParticipant(const synthclone::Participant *participant)
{
    participantManager.deactivateParticipant(participant);
}

const synthclone::EffectJob *
Context::getCurrentEffectJob() const
{
    return session.getCurrentEffectJob();
}

const synthclone::SamplerJob *
Context::getCurrentSamplerJob() const
{
    return session.getCurrentSamplerJob();
}

const synthclone::Effect *
Context::getEffect(int index) const
{
    return session.getEffect(index);
}

int
Context::getEffectCount() const
{
    return session.getEffectCount();
}

int
Context::getEffectIndex(const synthclone::Effect *effect) const
{
    return session.getEffectIndex(effect);
}

const synthclone::EffectJob *
Context::getEffectJob(int index) const
{
    return session.getEffectJob(index);
}

int
Context::getEffectJobCount() const
{
    return session.getEffectJobCount();
}

int
Context::getEffectJobIndex(const synthclone::EffectJob *job) const
{
    return session.getEffectJobIndex(job);
}

const synthclone::Component *
Context::getFocusedComponent() const
{
    return session.getFocusedComponent();
}

int
Context::getMajorVersion() const
{
    return session.getMajorVersion();
}

int
Context::getMinorVersion() const
{
    return session.getMinorVersion();
}

const synthclone::Participant *
Context::getParticipant(const QByteArray &id) const
{
    return participantManager.getParticipant(id);
}

const synthclone::Participant *
Context::getParticipant(int index, const synthclone::Participant *parent) const
{
    return participantManager.getParticipant(index, parent);
}

int
Context::getParticipantCount(const synthclone::Participant *parent) const
{
    return participantManager.getParticipantCount(parent);
}

QByteArray
Context::getParticipantId(const synthclone::Participant *participant) const
{
    return participantManager.getParticipantId(participant);
}

const synthclone::Participant *
Context::getParticipantParent(const synthclone::Participant *participant) const
{
    return participantManager.getParticipantParent(participant);
}

int
Context::getRevision() const
{
    return session.getRevision();
}

synthclone::SampleChannelCount
Context::getSampleChannelCount() const
{
    return session.getSampleChannelCount();
}

const synthclone::Sampler *
Context::getSampler() const
{
    return session.getSampler();
}

synthclone::SampleRate
Context::getSampleRate() const
{
    return session.getSampleRate();
}

const synthclone::SamplerJob *
Context::getSamplerJob(int index) const
{
    return session.getSamplerJob(index);
}

int
Context::getSamplerJobCount() const
{
    return session.getSamplerJobCount();
}

int
Context::getSamplerJobIndex(const synthclone::SamplerJob *job) const
{
    return session.getSamplerJobIndex(job);
}

const synthclone::Effect *
Context::getSelectedEffect() const
{
    return session.getSelectedEffect();
}

const synthclone::Target *
Context::getSelectedTarget() const
{
    return session.getSelectedTarget();
}

synthclone::Zone *
Context::getSelectedZone(int index)
{
    return session.getSelectedZone(index);
}

int
Context::getSelectedZoneCount() const
{
    return session.getSelectedZoneCount();
}

const QDir *
Context::getSessionDirectory() const
{
    return session.getDirectory();
}

synthclone::SessionState
Context::getSessionState() const
{
    return session.getState();
}

const synthclone::Target *
Context::getTarget(int index) const
{
    return session.getTarget(index);
}

int
Context::getTargetCount() const
{
    return session.getTargetCount();
}

int
Context::getTargetIndex(const synthclone::Target *target) const
{
    return session.getTargetIndex(target);
}

synthclone::Zone *
Context::getZone(int index)
{
    return session.getZone(index);
}

int
Context::getZoneCount() const
{
    return session.getZoneCount();
}

int
Context::getZoneIndex(const synthclone::Zone *zone) const
{
    return session.getZoneIndex(zone);
}

void
Context::handleEffectRemoval(QObject *obj)
{
    synthclone::Effect *effect = qobject_cast<synthclone::Effect *>(obj);
    assert(effect);
    bool removed = effects.removeOne(effect);
    assert(removed);
}

void
Context::handleMenuActionRemoval(QObject *obj)
{
    synthclone::MenuAction *action =
        qobject_cast<synthclone::MenuAction *>(obj);
    assert(action);
    bool removed = menuActions.removeOne(action);
    assert(removed);
}

void
Context::handleMenuSeparatorRemoval(QObject *obj)
{
    synthclone::MenuSeparator *separator =
        qobject_cast<synthclone::MenuSeparator *>(obj);
    assert(separator);
    bool removed = menuSeparators.removeOne(separator);
    assert(removed);
}

void
Context::handleParticipantRemoval(QObject *obj)
{
    synthclone::Participant *participant =
        qobject_cast<synthclone::Participant *>(obj);
    assert(participant);
    bool removed = participants.removeOne(participant);
    assert(removed);
}

void
Context::handleSamplerRemoval(QObject *obj)
{
    synthclone::Sampler *sampler = qobject_cast<synthclone::Sampler *>(obj);
    assert(sampler);
    assert(this->sampler == sampler);
    this->sampler = 0;
}

void
Context::handleTargetRemoval(QObject *obj)
{
    synthclone::Target *target = qobject_cast<synthclone::Target *>(obj);
    assert(target);
    bool removed = targets.removeOne(target);
    assert(removed);
}

bool
Context::isAftertouchPropertyVisible() const
{
    return session.isAftertouchPropertyVisible();
}

bool
Context::isChannelPressurePropertyVisible() const
{
    return session.isChannelPressurePropertyVisible();
}

bool
Context::isChannelPropertyVisible() const
{
    return session.isChannelPropertyVisible();
}

bool
Context::isControlPropertyVisible(synthclone::MIDIData control) const
{
    return session.isControlPropertyVisible(control);
}

bool
Context::isDrySamplePropertyVisible() const
{
    return session.isDrySamplePropertyVisible();
}

bool
Context::isNotePropertyVisible() const
{
    return session.isNotePropertyVisible();
}

bool
Context::
isParticipantActivated(const synthclone::Participant *participant) const
{
    return participantManager.isParticipantActivated(participant);
}

bool
Context::isReleaseTimePropertyVisible() const
{
    return session.isReleaseTimePropertyVisible();
}

bool
Context::isSampleTimePropertyVisible() const
{
    return session.isSampleTimePropertyVisible();
}

bool
Context::isSessionDirectory(const QDir &directory) const
{
    return Session::isDirectory(directory);
}

bool
Context::isStatusPropertyVisible() const
{
    return session.isStatusPropertyVisible();
}

bool
Context::isVelocityPropertyVisible() const
{
    return session.isVelocityPropertyVisible();
}

bool
Context::isWetSamplePropertyVisible() const
{
    return session.isWetSamplePropertyVisible();
}

bool
Context::isZoneSelected(const synthclone::Zone *zone) const
{
    return session.isZoneSelected(zone);
}

bool
Context::isZoneSelected(int index) const
{
    return session.isZoneSelected(index);
}

void
Context::loadSession(const QDir &directory)
{
    session.load(directory);
}

void
Context::moveEffect(int fromIndex, int toIndex)
{
    session.moveEffect(fromIndex, toIndex);
}

void
Context::moveEffectJob(int fromIndex, int toIndex)
{
    session.moveEffectJob(fromIndex, toIndex);
}

void
Context::moveSamplerJob(int fromIndex, int toIndex)
{
    session.moveSamplerJob(fromIndex, toIndex);
}

void
Context::moveTarget(int fromIndex, int toIndex)
{
    session.moveTarget(fromIndex, toIndex);
}

void
Context::moveZone(int fromIndex, int toIndex)
{
    session.moveZone(fromIndex, toIndex);
}

void
Context::quit()
{
    controller.quit();
}

void
Context::removeEffect(const synthclone::Effect *effect)
{
    session.removeEffect(effect);
}

void
Context::removeEffect(int index)
{
    session.removeEffect(index);
}

void
Context::removeEffectJob(const synthclone::EffectJob *job)
{
    session.removeEffectJob(job);
}

void
Context::removeEffectJob(int index)
{
    session.removeEffectJob(index);
}

void
Context::removeMenuAction(const synthclone::MenuAction *action)
{
    menuManager.removeMenuAction(action);
}

void
Context::removeMenuSeparator(const synthclone::MenuSeparator *separator)
{
    menuManager.removeMenuSeparator(separator);
}

void
Context::removeParticipant(const synthclone::Participant *participant)
{
    participantManager.removeParticipant(participant);
}

void
Context::removeSampler()
{
    session.removeSampler();
}

void
Context::removeSamplerJob(const synthclone::SamplerJob *job)
{
    session.removeSamplerJob(job);
}

void
Context::removeSamplerJob(int index)
{
    session.removeSamplerJob(index);
}

void
Context::removeTarget(const synthclone::Target *target)
{
    session.removeTarget(target);
}

void
Context::removeTarget(int index)
{
    session.removeTarget(index);
}

void
Context::removeZone(synthclone::Zone *zone)
{
    session.removeZone(zone);
}

void
Context::removeZone(int index)
{
    session.removeZone(index);
}

void
Context::reportError(const QString &message)
{
    controller.reportError(message);
}

void
Context::saveSession()
{
    session.save();
}

void
Context::saveSession(const QDir &directory)
{
    session.save(directory);
}

void
Context::setAftertouchPropertyVisible(bool visible)
{
    session.setAftertouchPropertyVisible(visible);
}

void
Context::setChannelPressurePropertyVisible(bool visible)
{
    session.setChannelPressurePropertyVisible(visible);
}

void
Context::setChannelPropertyVisible(bool visible)
{
    session.setChannelPropertyVisible(visible);
}

void
Context::setControlPropertyVisible(synthclone::MIDIData control, bool visible)
{
    session.setControlPropertyVisible(control, visible);
}

void
Context::setDrySamplePropertyVisible(bool visible)
{
    session.setDrySamplePropertyVisible(visible);
}

void
Context::setFocusedComponent(const synthclone::Component *component)
{
    session.setFocusedComponent(component);
}

void
Context::setNotePropertyVisible(bool visible)
{
    session.setNotePropertyVisible(visible);
}

void
Context::setReleaseTimePropertyVisible(bool visible)
{
    session.setReleaseTimePropertyVisible(visible);
}

void
Context::setSampleChannelCount(synthclone::SampleChannelCount count)
{
    session.setSampleChannelCount(count);
}

void
Context::setSampleRate(synthclone::SampleRate sampleRate)
{
    session.setSampleRate(sampleRate);
}

void
Context::setSampleTimePropertyVisible(bool visible)
{
    session.setSampleTimePropertyVisible(visible);
}

void
Context::setSelectedEffect(const synthclone::Effect *effect)
{
    session.setSelectedEffect(effect);
}

void
Context::setSelectedEffect(int index)
{
    session.setSelectedEffect(index);
}

void
Context::setSelectedTarget(const synthclone::Target *target)
{
    session.setSelectedTarget(target);
}

void
Context::setSelectedTarget(int index)
{
    session.setSelectedTarget(index);
}

void
Context::setSessionModified()
{
    session.setModified();
}

void
Context::setStatusPropertyVisible(bool visible)
{
    session.setStatusPropertyVisible(visible);
}

void
Context::setVelocityPropertyVisible(bool visible)
{
    session.setVelocityPropertyVisible(visible);
}

void
Context::setWetSamplePropertyVisible(bool visible)
{
    session.setWetSamplePropertyVisible(visible);
}

void
Context::setZoneSelected(const synthclone::Zone *zone, bool selected)
{
    session.setZoneSelected(zone, selected);
}

void
Context::setZoneSelected(int index, bool selected)
{
    session.setZoneSelected(index, selected);
}

void
Context::sortZones(const synthclone::ZoneComparer &comparer, bool ascending)
{
    session.sortZones(comparer, ascending);
}

void
Context::unloadSession()
{
    session.unload();
}
