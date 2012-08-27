/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011 Devin Anderson
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __SYNTHCLONE_CONTEXT_H__
#define __SYNTHCLONE_CONTEXT_H__

#include <QtCore/QDir>
#include <QtCore/QStringList>

#include <synthclone/effect.h>
#include <synthclone/effectjob.h>
#include <synthclone/menuaction.h>
#include <synthclone/menuseparator.h>
#include <synthclone/registration.h>
#include <synthclone/sampler.h>
#include <synthclone/target.h>
#include <synthclone/zonecomparer.h>

namespace synthclone {

    class Participant;

    /**
     * Context objects allow Participant objects to interact with a
     * `synthclone` session.  They allow Participant objects to do almost
     * everything to a session that the application itself can do.
     */

    class Context: public QObject {

        Q_OBJECT

    public:

        /**
         * Gets the EffectJob currently being executed by the registered Effect
         * objects.
         *
         * @returns
         *   The EffectJob, or 0 if there isn't a current EffectJob.
         */

        virtual const EffectJob *
        getCurrentEffectJob() const = 0;

        /**
         * Gets the SamplerJob currently being executed by the Sampler.
         *
         * @returns
         *   The SamplerJob, or 0 if there isn't a current SamplerJob.
         */

        virtual const SamplerJob *
        getCurrentSamplerJob() const = 0;

        /**
         * Gets the effect.
         *
         * @param index
         *   The index of the effect.
         *
         * @returns
         *   The effect.
         */

        virtual const Effect *
        getEffect(int index) const = 0;

        /**
         * Gets the count of registered effects.
         *
         * @returns
         *   The count.
         */

        virtual int
        getEffectCount() const = 0;

        /**
         * Gets the index of an effect.
         *
         * @param effect
         *   The effect.
         *
         * @returns
         *   The effect index.
         */

        virtual int
        getEffectIndex(const Effect *effect) const = 0;

        /**
         * Gets an EffectJob from the EffectJob queue.
         *
         * @param index
         *   The index of the EffectJob to get.
         *
         * @returns
         *   The EffectJob object.
         */

        virtual const EffectJob *
        getEffectJob(int index) const = 0;

        /**
         * Gets the count of EffectJob objects in the EffectJob queue.
         *
         * @returns
         *   The count.
         */

        virtual int
        getEffectJobCount() const = 0;

        /**
         * Gets the index of an EffectJob in the EffectJob queue.
         *
         * @param job
         *   The EffectJob object.
         *
         * @returns
         *   The index.
         */

        virtual int
        getEffectJobIndex(const synthclone::EffectJob *job) const = 0;

        /**
         * Gets the component that currently has focus.
         *
         * @returns
         *   The focused component, or 0 if no component currently has focus.
         */

        virtual const Component *
        getFocusedComponent() const = 0;

        /**
         * Gets the major portion of the version of `synthclone`.
         *
         * @returns
         *   The major version.
         */

        virtual int
        getMajorVersion() const = 0;

        /**
         * Gets the minor portion of the version of `synthclone`.
         *
         * @returns
         *   The minor version.
         */

        virtual int
        getMinorVersion() const = 0;

        /**
         * Gets a Participant.
         *
         * @param id
         *   The id of the Participant.
         *
         * @returns
         *   The Participant with the given id.
         */

        virtual const Participant *
        getParticipant(const QByteArray &id) const = 0;

        /**
         * Gets a Participant.
         *
         * @param index
         *   The index of the Participant to get in relation to the parent.
         *
         * @param parent
         *   The optional parent Participant.  If no Participant is specified,
         *   then the root Participant list will be scanned.
         *
         * @returns
         *   The index-th Participant with the given parent.
         */

        virtual const Participant *
        getParticipant(int index, const Participant *parent=0) const = 0;

        /**
         * Gets the count of child Participants of a Participant, or the count
         * of root Participant objects.
         *
         * @param parent
         *   The parent Participant.  If the parent isn't specified, then the
         *   root Participant list will be used.
         *
         * @returns
         *   The count.
         */

        virtual int
        getParticipantCount(const Participant *parent=0) const = 0;

        /**
         * Gets the id of a Participant.
         *
         * @param participant
         *   The Participant.
         *
         * @returns
         *   The Participant id.
         */

        virtual QByteArray
        getParticipantId(const Participant *participant) const = 0;

        /**
         * Gets the parent Participant of a Participant.
         *
         * @param participant
         *   The Participant.
         *
         * @returns
         *   The parent Participant, or 0 if the Participant is a root
         *   Participant.
         */

        virtual const Participant *
        getParticipantParent(const Participant *participant) const = 0;

        /**
         * Gets the revision portion of the version of synthclone.
         *
         * @returns
         *   The revision.
         */

        virtual int
        getRevision() const = 0;

        /**
         * Gets the current sample channel count.
         *
         * @returns
         *   The sample channel count.
         */

        virtual SampleChannelCount
        getSampleChannelCount() const = 0;

        /**
         * Gets the Sampler registered with the session.
         *
         * @returns
         *   The register Sampler, or 0 if there isn't a Sampler registered
         *   with the session.
         */

        virtual const Sampler *
        getSampler() const = 0;

        /**
         * Gets a SamplerJob from the SamplerJob queue.
         *
         * @param index
         *   The index of the SamplerJob to get.
         *
         * @returns
         *   The SamplerJob object.
         */

        virtual const SamplerJob *
        getSamplerJob(int index) const = 0;

        /**
         * Gets the count of SamplerJob objects in the SamplerJob queue.
         *
         * @returns
         *   The count.
         */

        virtual int
        getSamplerJobCount() const = 0;

        /**
         * Gets the index of a SamplerJob in the SamplerJob queue.
         *
         * @param job
         *   The SamplerJob object.
         *
         * @returns
         *   The index.
         */

        virtual int
        getSamplerJobIndex(const synthclone::SamplerJob *job) const = 0;

        /**
         * Gets the session sample rate.
         *
         * @returns
         *   The sample rate.
         */

        virtual SampleRate
        getSampleRate() const = 0;

        /**
         * Gets the currently selected Effect.
         *
         * @returns
         *   The selected Effect, or 0 if there isn't an Effect selected.
         */

        virtual const Effect *
        getSelectedEffect() const = 0;

        /**
         * Gets the currently selected target.
         *
         * @returns
         *   The selected Target, or 0 if there isn't a Target selected.
         */

        virtual const Target *
        getSelectedTarget() const = 0;

        /**
         * Gets a Zone from the selected Zone list.
         *
         * @param index
         *   The index of the Zone to get.
         *
         * @returns
         *   The Zone object.
         */

        virtual Zone *
        getSelectedZone(int index) = 0;

        /**
         * Gets the count of Zone objects in the selected Zone list.
         *
         * @returns
         *   The count.
         */

        virtual int
        getSelectedZoneCount() const = 0;

        /**
         * Gets the current session directory.
         *
         * @returns
         *   The current session directory, or 0 if there isn't an active
         *   session.
         */

        virtual const QDir *
        getSessionDirectory() const = 0;

        /**
         * Gets the state of the current session.
         *
         * @returns
         *   The SessionState.
         */

        virtual SessionState
        getSessionState() const = 0;

        /**
         * Gets a Target from the Target list.
         *
         * @param index
         *   The index of the Target to get.
         *
         * @returns
         *   The Target object.
         */

        virtual const Target *
        getTarget(int index) const = 0;

        /**
         * Gets the count of Target objects in the Target list.
         *
         * @returns
         *   The count.
         */

        virtual int
        getTargetCount() const = 0;

        /**
         * Gets the index of a Target in the Target list.
         *
         * @param target
         *   The Target object.
         *
         * @returns
         *   The Target index.
         */

        virtual int
        getTargetIndex(const Target *target) const = 0;

        /**
         * Gets a Zone.
         *
         * @param index
         *   The index of the Zone in the Zone list.
         *
         * @returns
         *   The Zone object.
         */

        virtual Zone *
        getZone(int index) = 0;

        /**
         * Gets the count of Zones in the Zone list.
         *
         * @returns
         *   The count.
         */

        virtual int
        getZoneCount() const = 0;

        /**
         * Gets the index of a Zone in the Zone list.
         *
         * @param zone
         *   The Zone object.
         *
         * @returns
         *   The index.
         */

        virtual int
        getZoneIndex(const Zone *zone) const = 0;

        /**
         * Gets a boolean indicating whether or not the aftertouch property is
         * visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isAftertouchPropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not the channel pressure
         * property is visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isChannelPressurePropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not the channel property is
         * visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isChannelPropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not a control property is
         * visible.
         *
         * @param control
         *   The control index.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isControlPropertyVisible(MIDIData control) const = 0;

        /**
         * Gets a boolean indicating whether or not the dry sample property is
         * visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isDrySamplePropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not the note property is
         * visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isNotePropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not a Participant is activated.
         *
         * @param participant
         *   The Participant object.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isParticipantActivated(const Participant *participant) const = 0;

        /**
         * Gets a boolean indicating whether or not the release time property
         * is visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isReleaseTimePropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not the sample time property is
         * visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isSampleTimePropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not a directory contains a
         * valid `synthclone` session.
         *
         * @param directory
         *   The directory to scan.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isSessionDirectory(const QDir &directory) const = 0;

        /**
         * Gets a boolean indicating whether or not the status property is
         * visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isStatusPropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not the velocity property is
         * visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isVelocityPropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not the wet sample property is
         * visible.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isWetSamplePropertyVisible() const = 0;

        /**
         * Gets a boolean indicating whether or not a Zone is selected.
         *
         * @param zone
         *   The Zone to check.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isZoneSelected(const Zone *zone) const = 0;

        /**
         * Gets a boolean indicating whether or not a Zone is selected.
         *
         * @param index
         *   The index in the Zone list of the Zone to check.
         *
         * @returns
         *   The boolean.
         */

        virtual bool
        isZoneSelected(int index) const = 0;

    public slots:

        /**
         * Tells the Sampler to abort the current SamplerJob.
         */

        virtual void
        abortCurrentSamplerJob() = 0;

        /**
         * Activates a Participant.  This allows a Participant object to
         * interact with the application.
         *
         * @param participant
         *   The Participant object to activate.
         */

        virtual void
        activateParticipant(const synthclone::Participant *participant) = 0;

        /**
         * Adds an Effect to the session.
         *
         * @param effect
         *   The Effect object to add.
         *
         * @param index
         *   The index at which the Effect object should be added.
         *
         * @returns
         *   A Registration object corresponding to the added Effect.
         */

        virtual const Registration &
        addEffect(Effect *effect, int index=-1) = 0;

        /**
         * Adds an EffectJob to the EffectJob queue.
         *
         * @param zone
         *   The Zone containing the dry Sample that effects will be applied
         *   to.
         *
         * @returns
         *   The new EffectJob.
         */

        virtual const EffectJob *
        addEffectJob(Zone *zone) = 0;

        /**
         * Adds a MenuAction to the application.
         *
         * @param action
         *   The MenuAction to add.
         *
         * @param menu
         *   The root `synthclone` menu to add the action to.
         *
         * @param subMenus
         *   A list of sub-menus to create from the root menu.  The MenuAction
         *   will be created at the deepest menu.
         *
         * @returns
         *   A Registration object corresponding to the added MenuAction.
         */

        virtual const Registration &
        addMenuAction(MenuAction *action, Menu menu,
                      const QStringList &subMenus=QStringList()) = 0;

        /**
         * Adds a MenuAction to the application.
         *
         * @param action
         *   The MenuAction to add.
         *
         * @param effect
         *   The Effect corresponding to the effect menu to add this action to.
         *
         * @param subMenus
         *   A list of sub-menus to create from the effect menu.  The
         *   MenuAction will be created at the deepest menu.
         *
         * @returns
         *   A Registration object corresponding to the added MenuAction.
         */

        virtual const Registration &
        addMenuAction(MenuAction *action, const Effect *effect,
                      const QStringList &subMenus=QStringList()) = 0;

        /**
         * Adds a MenuAction to the application.
         *
         * @param action
         *   The MenuAction to add.
         *
         * @param sampler
         *   The Sampler corresponding to the sampler menu to add this action
         *   to.
         *
         * @param subMenus
         *   A list of sub-menus to create from the sampler menu.  The
         *   MenuAction will be created at the deepest menu.
         *
         * @returns
         *   A Registration object corresponding to the added MenuAction.
         */

        virtual const Registration &
        addMenuAction(MenuAction *action, const Sampler *sampler,
                      const QStringList &subMenus=QStringList()) = 0;

        /**
         * Adds a MenuAction to the application.
         *
         * @param action
         *   The MenuAction to add.
         *
         * @param target
         *   The Target corresponding to the target menu to add this action to.
         *
         * @param subMenus
         *   A list of sub-menus to create from the target menu.  The
         *   MenuAction will be created at the deepest menu.
         *
         * @returns
         *   A Registration object corresponding to the added MenuAction.
         */

        virtual const Registration &
        addMenuAction(MenuAction *action, const Target *target,
                      const QStringList &subMenus=QStringList()) = 0;

        /**
         * Adds a MenuSeparator to the application.
         *
         * @param separator
         *   The MenuSeparator to add.
         *
         * @param menu
         *   The root `synthclone` menu to add the separator to.
         *
         * @param subMenus
         *   A list of sub-menus to create from the root menu.  The
         *   MenuSeparator will be created at the deepest menu.
         *
         * @returns
         *   A Registration object corresponding to the added MenuSeparator.
         */

        virtual const Registration &
        addMenuSeparator(MenuSeparator *separator, Menu menu,
                         const QStringList &subMenus=QStringList()) = 0;

        /**
         * Adds a MenuSeparator to the application.
         *
         * @param separator
         *   The MenuSeparator to add.
         *
         * @param effect
         *   The Effect corresponding to the effect menu to add this separator
         *   to.
         *
         * @param subMenus
         *   A list of sub-menus to create from the effect menu.  The
         *   MenuSeparator will be created at the deepest menu.
         *
         * @returns
         *   A Registration object corresponding to the added MenuSeparator.
         */

        virtual const Registration &
        addMenuSeparator(MenuSeparator *separator, const Effect *effect,
                         const QStringList &subMenus=QStringList()) = 0;

        /**
         * Adds a MenuSeparator to the application.
         *
         * @param separator
         *   The MenuSeparator to add.
         *
         * @param sampler
         *   The Sampler corresponding to the sampler menu to add this
         *   separator to.
         *
         * @param subMenus
         *   A list of sub-menus to create from the sampler menu.  The
         *   MenuSeparator will be created at the deepest menu.
         *
         * @returns
         *   A Registration object corresponding to the added MenuSeparator.
         */

        virtual const Registration &
        addMenuSeparator(MenuSeparator *separator, const Sampler *sampler,
                         const QStringList &subMenus=QStringList()) = 0;

        /**
         * Adds a MenuSeparator to the application.
         *
         * @param separator
         *   The MenuSeparator to add.
         *
         * @param target
         *   The Target corresponding to the target menu to add this separator
         *   to.
         *
         * @param subMenus
         *   A list of sub-menus to create from the target menu.  The
         *   MenuSeparator will be created at the deepest menu.
         *
         * @returns
         *   A Registration object corresponding to the added MenuSeparator.
         */

        virtual const Registration &
        addMenuSeparator(MenuSeparator *separator, const Target *target,
                         const QStringList &subMenus=QStringList()) = 0;

        /**
         * Adds a Participant to the session.  At this point, the Participant
         * is made available for activation, but it cannot interact with the
         * application until its Participant::activate() method is called.
         *
         * @param participant
         *   The Participant object to add.
         *
         * @param subId
         *   The sub-id of the Participant.  The sub-id will be used in
         *   conjunction with the parent Participant object's id to create this
         *   Participant object's full id, which will be in the form
         *   '[parentId].[subId]'.  The sub-id must start and end with a number
         *   or letter, and contain only numbers, letters, and hyphens.  See
         *   IPlugin::getId() for more information on ids.
         *
         * @returns
         *   A Registration object corresponding to the added Participant.
         *
         * @note
         *   The practical use case I have in mind for allowing a Participant
         *   object to add child Participant objects is to enable the creation
         *   of bindings to other programming languages, where a root
         *   Participant creates the resources necessary to use `libsynthclone`
         *   in another language, and then loads plugins as child Participant
         *   objects.  There may be other practical uses.
         */

        virtual const Registration &
        addParticipant(Participant *participant, const QByteArray &subId) = 0;

        /**
         * Adds a Sampler to the session, removing any Sampler that may already
         * be registered with the session.
         *
         * @param sampler
         *   The Sampler to add.
         *
         * @returns
         *   A Registration object corresponding to the added Sampler.
         */

        virtual const Registration &
        addSampler(Sampler *sampler) = 0;

        /**
         * Adds a SamplerJob object to the SamplerJob queue.
         *
         * @param type
         *   The job type.
         *
         * @param zone
         *   The Zone that the job will operate on.
         *
         * @returns
         *   The new SamplerJob object.
         *
         * @sa
         *   SamplerJob::Type
         */

        virtual const SamplerJob *
        addSamplerJob(SamplerJob::Type type, Zone *zone) = 0;

        /**
         * Adds a Target to the session.
         *
         * @param target
         *   The Target object to add.
         *
         * @param index
         *   The index at which the Target object should be added.
         *
         * @returns
         *   A Registration object corresponding to the added Target.
         */

        virtual const Registration &
        addTarget(Target *target, int index=-1) = 0;

        /**
         * Adds a new zone to the session.
         *
         * @param index
         *   The index at which the new Zone should be added.
         *
         * @returns
         *   The new Zone object.
         */

        virtual Zone *
        addZone(int index=-1) = 0;

        /**
         * Attempts to build all registered targets.
         */

        virtual void
        buildTargets() = 0;

        /**
         * Writes an empty `synthclone` session to a directory.
         *
         * @param directory
         *   The directory to write the session to.
         *
         * @param sampleRate
         *   The sample rate for the session.
         *
         * @param count
         *   The sample channel count for the session.
         */

        virtual void
        createSession(const QDir &directory, SampleRate sampleRate,
                      SampleChannelCount count) = 0;

        /**
         * Deactivates a Participant.  The Participant will cease interacting
         * with the application.
         *
         * @param participant
         *   The Participant to deactivate.
         */

        virtual void
        deactivateParticipant(const Participant *participant) = 0;

        /**
         * Loads a `synthclone` session.  The currently loaded session will be
         * unloaded first.  This call will NOT prompt the user.
         *
         * @param directory
         *   The directory to load the session from.
         *
         * @sa
         *   unloadSession()
         */

        virtual void
        loadSession(const QDir &directory) = 0;

        /**
         * Moves an Effect.
         *
         * @param fromIndex
         *   The index of the Effect to move.
         *
         * @param toIndex
         *   The index to move the Effect to.
         */

        virtual void
        moveEffect(int fromIndex, int toIndex) = 0;

        /**
         * Moves an EffectJob.
         *
         * @param fromIndex
         *   The index of the EffectJob to move.
         *
         * @param toIndex
         *   The index to move the EffectJob to.
         */

        virtual void
        moveEffectJob(int fromIndex, int toIndex) = 0;

        /**
         * Moves a SamplerJob.
         *
         * @param fromIndex
         *   The index of the SamplerJob to move.
         *
         * @param toIndex
         *   The index to move the SamplerJob to.
         */

        virtual void
        moveSamplerJob(int fromIndex, int toIndex) = 0;

        /**
         * Moves a Target.
         *
         * @param fromIndex
         *   The index of the Target to move.
         *
         * @param toIndex
         *   The index to move the Target to.
         */

        virtual void
        moveTarget(int fromIndex, int toIndex) = 0;

        /**
         * Moves a Zone.
         *
         * @param fromIndex
         *   The index of the Zone to move.
         *
         * @param toIndex
         *   The index to move the Zone to.
         */

        virtual void
        moveZone(int fromIndex, int toIndex) = 0;

        /**
         * Terminates `synthclone`.  The current session will be unloaded, and
         * the application will be terminated when the Participant returns
         * control to the application.
         *
         * @sa
         *   unloadSession()
         */

        virtual void
        quit() = 0;

        /**
         * Removes an Effect.
         *
         * @param effect
         *   The Effect to remove.
         */

        virtual void
        removeEffect(const Effect *effect) = 0;

        /**
         * Removes an Effect.
         *
         * @param index
         *   The index of the Effect to remove.
         */

        virtual void
        removeEffect(int index) = 0;

        /**
         * Removes an EffectJob from the EffectJob queue.
         *
         * @param job
         *   The EffectJob to remove.
         */

        virtual void
        removeEffectJob(const EffectJob *job) = 0;

        /**
         * Removes an EffectJob from the EffectJob queue.
         *
         * @param index
         *   The index of the EffectJob to remove.
         */

        virtual void
        removeEffectJob(int index) = 0;

        /**
         * Removes a MenuAction.
         *
         * @param action
         *   The MenuAction to remove.
         */

        virtual void
        removeMenuAction(const MenuAction *action) = 0;

        /**
         * Removes a MenuSeparator.
         *
         * @param separator
         *   The MenuSeparator to remove.
         */

        virtual void
        removeMenuSeparator(const MenuSeparator *separator) = 0;

        /**
         * Removes a Participant from the session.
         *
         * @param participant
         *   The Participant to remove.
         */

        virtual void
        removeParticipant(const Participant *participant) = 0;

        /**
         * Removes a Sampler registered with the session.
         */

        virtual void
        removeSampler() = 0;

        /**
         * Removes a SamplerJob from the SamplerJob queue.
         *
         * @param job
         *   The SamplerJob to remove.
         */

        virtual void
        removeSamplerJob(const SamplerJob *job) = 0;

        /**
         * Removes a sampler job from the SamplerJob queue.
         *
         * @param index
         *   The index of the SamplerJob to remove.
         */

        virtual void
        removeSamplerJob(int index) = 0;

        /**
         * Removes a Target from the Target list.
         *
         * @param target
         *   The Target to remove.
         */

        virtual void
        removeTarget(const Target *target) = 0;

        /**
         * Removes a Target from the Target list.
         *
         * @param index
         *   The index of the Target to remove.
         */

        virtual void
        removeTarget(int index) = 0;

        /**
         * Removes a Zone from the Zone list.
         *
         * @param zone
         *   The Zone to remove.
         */

        virtual void
        removeZone(Zone *zone) = 0;

        /**
         * Removes a Zone from the Zone list.
         *
         * @param index
         *   The index of the Zone to remove.
         */

        virtual void
        removeZone(int index) = 0;

        /**
         * Reports a session error.  The application responds by showing an
         * error dialog.
         *
         * @param message
         *   An error message describing the error that occurred.
         */

        virtual void
        reportError(const QString &message) = 0;

        /**
         * Saves the session in the current session directory.  Note that this
         * method does NOT prompt the user.
         */

        virtual void
        saveSession() = 0;

        /**
         * Saves the session, and changes the current session directory.  Note
         * that this method does NOT prompt the user.
         *
         * @param directory
         *   The directory to save the session in.
         */

        virtual void
        saveSession(const QDir &directory) = 0;

        /**
         * Sets the visibility of the aftertouch property.
         *
         * @param visible
         *   Whether or not the aftertouch property should be visible.
         */

        virtual void
        setAftertouchPropertyVisible(bool visible) = 0;

        /**
         * Sets the visibility of the channel pressure property.
         *
         * @param visible
         *   Whether or not the channel pressure property should be visible.
         */

        virtual void
        setChannelPressurePropertyVisible(bool visible) = 0;

        /**
         * Sets the visibility of the channel property.
         *
         * @param visible
         *   Whether or not the channel property should be visible.
         */

        virtual void
        setChannelPropertyVisible(bool visible) = 0;

        /**
         * Sets the visibility of a control property.
         *
         * @param control
         *   The control to change the visibility of.
         *
         * @param visible
         *   Whether or not the control property should be visible.
         */

        virtual void
        setControlPropertyVisible(MIDIData control, bool visible) = 0;

        /**
         * Sets the visibility of the dry sample property.
         *
         * @param visible
         *   Whether or not the dry sample property should be visible.
         */

        virtual void
        setDrySamplePropertyVisible(bool visible) = 0;

        /**
         * Sets the focused component.
         *
         * @param component
         *   The component that should gain focus.  If this argument is 0, then
         *   there will be no focused component.
         */

        virtual void
        setFocusedComponent(const Component *component) = 0;

        /**
         * Sets the visibility of the note property.
         *
         * @param visible
         *   Whether or not the note property should be visible.
         */

        virtual void
        setNotePropertyVisible(bool visible) = 0;

        /**
         * Sets the visibility of the release time property.
         *
         * @param visible
         *   Whether or not the release time property should be visible.
         */

        virtual void
        setReleaseTimePropertyVisible(bool visible) = 0;

        /**
         * Sets the SampleChannelCount for the session.  If there are Sample
         * objects already registered with this session, then behavior varies:
         *
         * - If the Sample objects are mono samples, then they will be
         * converted to samples with the updated SampleChannelCount.
         * - If the incoming SampleChannelCount is 1, then the Sample objects
         * will be converted to mono samples.
         * - Otherwise, the Sample objects will be deleted.
         *
         * All of this is done WITHOUT prompting the user.  You have been
         * warned.
         *
         * @param sampleChannelCount
         *   The new SampleChannelCount.
         */

        virtual void
        setSampleChannelCount(SampleChannelCount sampleChannelCount) = 0;

        /**
         * Sets the SampleRate for the session.  If there are Sample objects
         * already registered with this session, then they will be converted to
         * the new SampleRate WITHOUT prompting the user.  You have been
         * warned.
         *
         * @param sampleRate
         *   The new SampleRate.
         */

        virtual void
        setSampleRate(SampleRate sampleRate) = 0;

        /**
         * Sets the visibility of the sample time property.
         *
         * @param visible
         *   Whether or not the sample time property should be visible.
         */

        virtual void
        setSampleTimePropertyVisible(bool visible) = 0;

        /**
         * Sets the selected Effect.
         *
         * @param effect
         *   The Effect to select.  If the argument is set to 0, then any
         *   currently selected Effect object will be deselected.
         */

        virtual void
        setSelectedEffect(const Effect *effect) = 0;

        /**
         * Sets the selected Effect.
         *
         * @param index
         *   The index of the Effect to select.  If the argument is set to -1,
         *   then any currently selected Effect object will be deselected.
         */

        virtual void
        setSelectedEffect(int index) = 0;

        /**
         * Sets the selected Target.
         *
         * @param target
         *   The Target to select.  If the argument is set to 0, then any
         *   currently selected Target object will be deselected.
         */

        virtual void
        setSelectedTarget(const Target *target) = 0;

        /**
         * Sets the selected Target.
         *
         * @param index
         *   The index of the Target to select.  If the argument is set to -1,
         *   then any currently selected Target object will be deselected.
         */

        virtual void
        setSelectedTarget(int index) = 0;

        /**
         * Changes the session state to SESSIONSTATE_MODIFIED if the session
         * state is SESSIONSTATE_CURRENT.
         *
         * @sa
         *   synthclone::SessionState
         */

        virtual void
        setSessionModified() = 0;

        /**
         * Sets the visibility of the status property.
         *
         * @param visible
         *   Whether or not the status property should be visible.
         */

        virtual void
        setStatusPropertyVisible(bool visible) = 0;

        /**
         * Sets the visibility of the velocity property.
         *
         * @param visible
         *   Whether or not the velocity property should be visible.
         */

        virtual void
        setVelocityPropertyVisible(bool visible) = 0;

        /**
         * Sets the visibility of the wet sample property.
         *
         * @param visible
         *   Whether or not the wet sample property should be visible.
         */

        virtual void
        setWetSamplePropertyVisible(bool visible) = 0;

        /**
         * (De)selects a Zone.
         *
         * @param zone
         *   The Zone to (de)select.
         *
         * @param selected
         *   Whether or not the Zone should be selected.
         */

        virtual void
        setZoneSelected(const Zone *zone, bool selected) = 0;

        /**
         * (De)selects a Zone.
         *
         * @param index
         *   The index of the Zone to (de)select.
         *
         * @param selected
         *   Whether or not the Zone should be selected.
         */

        virtual void
        setZoneSelected(int index, bool selected) = 0;

        /**
         * Sorts the Zone list.
         *
         * @param comparer
         *   The ZoneComparer used to sort the Zone list.
         *
         * @param ascending
         *   Whether or not to sort the Zone objects in ascending order.
         */

        virtual void
        sortZones(const ZoneComparer &comparer, bool ascending=true) = 0;

        /**
         * Unloads the current session.  The user will NOT be prompted.
         *
         * @note
         *   The Participant unloading the session will be deactivated before
         *   this method returns, and this Context object will be deleted at
         *   some point after the Participant returns control to the
         *   application.  Use this method responsibly.
         */

        virtual void
        unloadSession() = 0;

    signals:

        /**
         * Emitted when a Participant is being activated.
         *
         * @param participant
         *   The Participant being activated.
         *
         * @param parent
         *   The parent Participant of the Participant being activated.  If
         *   this argument is set to 0, then the Participant being activated is
         *   a root Participant.
         *
         * @param id
         *   The id of the Participant being activated.
         */

        void
        activatingParticipant(const synthclone::Participant *participant,
                              const synthclone::Participant *parent,
                              const QByteArray &id);

        /**
         * Emitted when an Effect is being added to the Effect list.
         *
         * @param effect
         *   The Effect being added.
         *
         * @param index
         *   The index at which the Effect will be added.
         */

        void
        addingEffect(const synthclone::Effect *effect, int index);

        /**
         * Emitted when an EffectJob is being added to the EffectJob queue.
         *
         * @param job
         *   The EffectJob being added.
         *
         * @param index
         *   The index at which the EffectJob will be added.
         */

        void
        addingEffectJob(const synthclone::EffectJob *job, int index);

        /**
         * Emitted when a MenuAction is being added to a root menu.
         *
         * @param action
         *   The MenuAction being added.
         *
         * @param menu
         *   The root menu.
         *
         * @param subMenus
         *   A list of sub-menus that will be created from the root menu.  The
         *   MenuAction will be created at the deepest menu.
         */

        void
        addingMenuAction(const synthclone::MenuAction *action,
                         synthclone::Menu menu, const QStringList &subMenus);

        /**
         * Emitted when a MenuAction is being added to an Effect menu.
         *
         * @param action
         *   The MenuAction being added.
         *
         * @param effect
         *   The Effect corresponding to the Effect menu to add this action to.
         *
         * @param subMenus
         *   A list of sub-menus that will be created from the Effect menu.
         *   The MenuAction will be created at the deepest menu.
         */

        void
        addingMenuAction(const synthclone::MenuAction *action,
                         const synthclone::Effect *effect,
                         const QStringList &subMenus);

        /**
         * Emitted when a MenuAction is being added to a Sampler menu.
         *
         * @param action
         *   The MenuAction being added.
         *
         * @param sampler
         *   The Sampler corresponding to the Sampler menu to add this action
         *   to.
         *
         * @param subMenus
         *   A list of sub-menus that will be created from the Sampler menu.
         *   The MenuAction will be created at the deepest menu.
         */

        void
        addingMenuAction(const synthclone::MenuAction *action,
                         const synthclone::Sampler *sampler,
                         const QStringList &subMenus);

        /**
         * Emitted when a MenuAction is being added to a Target menu.
         *
         * @param action
         *   The MenuAction being added.
         *
         * @param target
         *   The Target corresponding to the Target menu to add this action to.
         *
         * @param subMenus
         *   A list of sub-menus that will be created from the Target menu.
         *   The MenuAction will be created at the deepest menu.
         */

        void
        addingMenuAction(const synthclone::MenuAction *action,
                         const synthclone::Target *target,
                         const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator is being added to a root menu.
         *
         * @param separator
         *   The MenuSeparator being added.
         *
         * @param menu
         *   The root menu.
         *
         * @param subMenus
         *   A list of sub-menus that will be created from the root menu.  The
         *   MenuSeparator will be created at the deepest menu.
         */

        void
        addingMenuSeparator(const synthclone::MenuSeparator *separator,
                            synthclone::Menu menu,
                            const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator is being added to an Effect menu.
         *
         * @param separator
         *   The MenuSeparator being added.
         *
         * @param effect
         *   The Effect corresponding to the Effect menu to add this separator
         *   to.
         *
         * @param subMenus
         *   A list of sub-menus that will be created from the Effect menu.
         *   The MenuSeparator will be created at the deepest menu.
         */

        void
        addingMenuSeparator(const synthclone::MenuSeparator *separator,
                            const synthclone::Effect *effect,
                            const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator is being added to a Sampler menu.
         *
         * @param separator
         *   The MenuSeparator being added.
         *
         * @param sampler
         *   The Sampler corresponding to the Sampler menu to add this
         *   separator to.
         *
         * @param subMenus
         *   A list of sub-menus that will be created from the Sampler menu.
         *   The MenuSeparator will be created at the deepest menu.
         */

        void
        addingMenuSeparator(const synthclone::MenuSeparator *separator,
                            const synthclone::Sampler *sampler,
                            const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator is being added to a Target menu.
         *
         * @param separator
         *   The MenuSeparator being added.
         *
         * @param target
         *   The Target corresponding to the Target menu to add this separator
         *   to.
         *
         * @param subMenus
         *   A list of sub-menus that will be created from the Target menu.
         *   The MenuSeparator will be created at the deepest menu.
         */

        void
        addingMenuSeparator(const synthclone::MenuSeparator *separator,
                            const synthclone::Target *target,
                            const QStringList &subMenus);

        /**
         * Emitted when a Participant is being added.
         *
         * @param participant
         *   The Participant being added.
         *
         * @param parent
         *   The parent Participant of the Participant being added.  If this
         *   argument is set to 0, then the Participant being added is a root
         *   Participant.
         *
         * @param id
         *   The id of the Participant being added.
         */

        void
        addingParticipant(const synthclone::Participant *participant,
                          const synthclone::Participant *parent,
                          const QByteArray &id);

        /**
         * Emitted when a Sampler is being added.
         *
         * @param sampler
         *   The Sampler being added.
         */

        void
        addingSampler(const synthclone::Sampler *sampler);

        /**
         * Emitted when a SamplerJob is being added to the SamplerJob queue.
         *
         * @param job
         *   The SamplerJob being added.
         *
         * @param index
         *   The index at which the SamplerJob will be added.
         */

        void
        addingSamplerJob(const synthclone::SamplerJob *job, int index);

        /**
         * Emitted when a Target is being added to the Target list.
         *
         * @param target
         *   The Target being added.
         *
         * @param index
         *   The index at which the Target will be added.
         */

        void
        addingTarget(const synthclone::Target *target, int index);

        /**
         * Emitted when a Zone is being added to the Zone list.
         *
         * @param zone
         *   The Zone being added.
         *
         * @param index
         *   The index at which the Zone will be added.
         */

        void
        addingZone(const synthclone::Zone *zone, int index);

        /**
         * Emitted when the visibility of the aftertouch property is changed.
         *
         * @param visible
         *   The visibility of the aftertouch property.
         */

        void
        aftertouchPropertyVisibilityChanged(bool visible);

        /**
         * Emitted when a Target is being built.
         *
         * @param target
         *   The Target being built.
         */

        void
        buildingTarget(const synthclone::Target *target);

        /**
         * Emitted when a Target build operation is starting.
         */

        void
        buildingTargets();

        /**
         * Emitted when the visibility of the channel pressure property is
         * changed.
         *
         * @param visible
         *   The visibility of the channel pressure property.
         */

        void
        channelPressurePropertyVisibilityChanged(bool visible);

        /**
         * Emitted when the visibility of the channel property is changed.
         *
         * @param visible
         *   The visibility of the channel property.
         */

        void
        channelPropertyVisibilityChanged(bool visible);

        /**
         * Emitted when the visibility of a control property is changed.
         *
         * @param control
         *   The control.
         *
         * @param visible
         *   The visibility of the control property.
         */

        void
        controlPropertyVisibilityChanged(synthclone::MIDIData control,
                                         bool visible);

        /**
         * Emitted when the current effect job is changed.
         *
         * @param job
         *   The current effect job.
         */

        void
        currentEffectJobChanged(const synthclone::EffectJob *job);

        /**
         * Emitted when the current sampler job is changed.
         *
         * @param job
         *   The current sampler job.
         */

        void
        currentSamplerJobChanged(const synthclone::SamplerJob *job);

        /**
         * Emitted when a Participant is being deactivated.
         *
         * @param participant
         *   The Participant being deactivated.
         *
         * @param parent
         *   The parent Participant of the Participant being deactivated.  If
         *   this argument is set to 0, then the Participant being deactivated
         *   is a root Participant.
         *
         * @param id
         *   The id of the Participant being deactivated.
         */

        void
        deactivatingParticipant(const synthclone::Participant *participant,
                                const synthclone::Participant *parent,
                                const QByteArray &id);

        /**
         * Emitted when the visibility of the dry sample property is changed.
         *
         * @param visible
         *   The visibility of the dry sample property.
         */

        void
        drySamplePropertyVisibilityChanged(bool visible);

        /**
         * Emitted when an Effect has been added to the Effect list.
         *
         * @param effect
         *   The added Effect.
         *
         * @param index
         *   The index of the Effect.
         */

        void
        effectAdded(const synthclone::Effect *effect, int index);

        /**
         * Emitted when an EffectJob has been added to the EffectJob queue.
         *
         * @param job
         *   The added EffectJob.
         *
         * @param index
         *   The index of the EffectJob.
         */

        void
        effectJobAdded(const synthclone::EffectJob *job, int index);

        /**
         * Emitted when an EffectJob is moved in the EffectJob queue.
         *
         * @param job
         *   The EffectJob.
         *
         * @param fromIndex
         *   The index the EffectJob was moved from.
         *
         * @param toIndex
         *   The current index of the EffectJob.
         */

        void
        effectJobMoved(const synthclone::EffectJob *job, int fromIndex,
                       int toIndex);

        /**
         * Emitted when an EffectJob has been removed from the EffectJob queue.
         *
         * @param job
         *   The removed EffectJob.
         *
         * @param index
         *   The index the EffectJob was removed from.
         */

        void
        effectJobRemoved(const synthclone::EffectJob *job, int index);

        /**
         * Emitted when an Effect has been moved in the Effect list.
         *
         * @param effect
         *   The moved Effect.
         *
         * @param fromIndex
         *   The index the Effect was moved from.
         *
         * @param toIndex
         *   The current index of the Effect.
         */

        void
        effectMoved(const synthclone::Effect *effect, int fromIndex,
                    int toIndex);

        /**
         * Emitted when an Effect has been removed from the Effect list.
         *
         * @param effect
         *   The removed Effect.
         *
         * @param index
         *   The index at which the Effect was removed.
         */

        void
        effectRemoved(const synthclone::Effect *effect, int index);

        /**
         * Emitted when an error is reported.
         *
         * @param message
         *   The error message.
         */

        void
        errorReported(const QString &message);

        /**
         * Emitted when the focused component is changed.
         *
         * @param component
         *   The component that has gained focus.  If this argument is set to
         *   0, then there currently isn't a focused component.
         */

        void
        focusedComponentChanged(const synthclone::Component *component);

        /**
         * Emitted when a MenuAction has been added to a root menu.
         *
         * @param action
         *   The added MenuAction.
         *
         * @param menu
         *   The root menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the root menu.  The MenuAction is
         *   at the deepest menu.
         */

        void
        menuActionAdded(const synthclone::MenuAction *action,
                        synthclone::Menu menu, const QStringList &subMenus);

        /**
         * Emitted when a MenuAction has been added to an Effect menu.
         *
         * @param action
         *   The added MenuAction.
         *
         * @param effect
         *   The Effect corresponding to the Effect menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Effect menu.  The MenuAction
         *   is at the deepest menu.
         */

        void
        menuActionAdded(const synthclone::MenuAction *action,
                        const synthclone::Effect *effect,
                        const QStringList &subMenus);

        /**
         * Emitted when a MenuAction has been added to a Sampler menu.
         *
         * @param action
         *   The added MenuAction.
         *
         * @param sampler
         *   The Sampler corresponding to the Sampler menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Sampler menu.  The MenuAction
         *   is at the deepest menu.
         */

        void
        menuActionAdded(const synthclone::MenuAction *action,
                        const synthclone::Sampler *sampler,
                        const QStringList &subMenus);

        /**
         * Emitted when a MenuAction has been added to a Target menu.
         *
         * @param action
         *   The added MenuAction.
         *
         * @param target
         *   The Target corresponding to the Target menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Target menu.  The MenuAction
         *   is at the deepest menu.
         */

        void
        menuActionAdded(const synthclone::MenuAction *action,
                        const synthclone::Target *target,
                        const QStringList &subMenus);

        /**
         * Emitted when a MenuAction has been removed from a root menu.
         *
         * @param action
         *   The MenuAction.
         *
         * @param menu
         *   The root menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the root menu.  The MenuAction
         *   was at the deepest menu.  Any empty sub-menus have been removed.
         */

        void
        menuActionRemoved(const synthclone::MenuAction *action,
                          synthclone::Menu menu, const QStringList &subMenus);

        /**
         * Emitted when a MenuAction has been removed from an Effect menu.
         *
         * @param action
         *   The MenuAction.
         *
         * @param effect
         *   The Effect corresponding to the Effect menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Effect menu.  The MenuAction
         *   was at the deepest menu.  Any empty sub-menus have been removed.
         */

        void
        menuActionRemoved(const synthclone::MenuAction *action,
                          const synthclone::Effect *effect,
                          const QStringList &subMenus);

        /**
         * Emitted when a MenuAction has been removed from a Sampler menu.
         *
         * @param action
         *   The MenuAction.
         *
         * @param sampler
         *   The Sampler corresponding to the Sampler menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Sampler menu.  The MenuAction
         *   was at the deepest menu.  Any empty sub-menus have been removed.
         */

        void
        menuActionRemoved(const synthclone::MenuAction *action,
                          const synthclone::Sampler *sampler,
                          const QStringList &subMenus);

        /**
         * Emitted when a MenuAction has been removed from a Target menu.
         *
         * @param action
         *   The MenuAction.
         *
         * @param target
         *   The Target corresponding to the Target menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Target menu.  The MenuAction
         *   was at the deepest menu.  Any empty sub-menus have been removed.
         */

        void
        menuActionRemoved(const synthclone::MenuAction *action,
                          const synthclone::Target *target,
                          const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator has been added to a root menu.
         *
         * @param separator
         *   The added MenuSeparator.
         *
         * @param menu
         *   The root menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the root menu.  The MenuSeparator
         *   is at the deepest menu.
         */

        void
        menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                           synthclone::Menu menu, const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator has been added to an Effect menu.
         *
         * @param separator
         *   The added MenuSeparator.
         *
         * @param effect
         *   The Effect corresponding to the Effect menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Effect menu.  The
         *   MenuSeparator is at the deepest menu.
         */

        void
        menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                           const synthclone::Effect *effect,
                           const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator has been added to a Sampler menu.
         *
         * @param separator
         *   The added MenuSeparator.
         *
         * @param sampler
         *   The Sampler corresponding to the Sampler menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Sampler menu.  The
         *   MenuSeparator is at the deepest menu.
         */

        void
        menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                           const synthclone::Sampler *sampler,
                           const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator has been added to a Target menu.
         *
         * @param separator
         *   The added MenuSeparator.
         *
         * @param target
         *   The Target corresponding to the Target menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Target menu.  The
         *   MenuSeparator is at the deepest menu.
         */

        void
        menuSeparatorAdded(const synthclone::MenuSeparator *separator,
                           const synthclone::Target *target,
                           const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator has been removed from a root menu.
         *
         * @param separator
         *   The MenuSeparator.
         *
         * @param menu
         *   The root menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the root menu.  The MenuSeparator
         *   was at the deepest menu.  Any empty sub-menus have been removed.
         */

        void
        menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                             synthclone::Menu menu,
                             const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator has been removed from an Effect menu.
         *
         * @param separator
         *   The MenuSeparator.
         *
         * @param effect
         *   The Effect corresponding to the Effect menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Effect menu.  The
         *   MenuSeparator was at the deepest menu.  Any empty sub-menus have
         *   been removed.
         */

        void
        menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                             const synthclone::Effect *effect,
                             const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator has been removed from a Sampler menu.
         *
         * @param separator
         *   The MenuSeparator.
         *
         * @param sampler
         *   The Sampler corresponding to the Sampler menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Sampler menu.  The
         *   MenuSeparator was at the deepest menu.  Any empty sub-menus have
         *   been removed.
         */

        void
        menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                             const synthclone::Sampler *sampler,
                             const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator has been removed from a Target menu.
         *
         * @param separator
         *   The MenuSeparator.
         *
         * @param target
         *   The Target corresponding to the Target menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Target menu.  The
         *   MenuSeparator was at the deepest menu.  Any empty sub-menus have
         *   been removed.
         */

        void
        menuSeparatorRemoved(const synthclone::MenuSeparator *separator,
                             const synthclone::Target *target,
                             const QStringList &subMenus);

        /**
         * Emitted when an Effect is being moved.
         *
         * @param effect
         *   The Effect being moved.
         *
         * @param fromIndex
         *   The Effect object's current index.
         *
         * @param toIndex
         *   The index the Effect will be moved to.
         */

        void
        movingEffect(const synthclone::Effect *effect, int fromIndex,
                     int toIndex);

        /**
         * Emitted when an EffectJob is being moved.
         *
         * @param job
         *   The EffectJob being moved.
         *
         * @param fromIndex
         *   The EffectJob object's current index.
         *
         * @param toIndex
         *   The index the EffectJob will be moved to.
         */

        void
        movingEffectJob(const synthclone::EffectJob *job, int fromIndex,
                        int toIndex);

        /**
         * Emitted when a SamplerJob is being moved.
         *
         * @param job
         *   The SamplerJob being moved.
         *
         * @param fromIndex
         *   The SamplerJob object's current index.
         *
         * @param toIndex
         *   The index the SamplerJob will be moved to.
         */

        void
        movingSamplerJob(const synthclone::SamplerJob *job, int fromIndex,
                         int toIndex);

        /**
         * Emitted when a Target is being moved.
         *
         * @param target
         *   The Target being moved.
         *
         * @param fromIndex
         *   The Target object's current index.
         *
         * @param toIndex
         *   The index the Target will be moved to.
         */

        void
        movingTarget(const synthclone::Target *target, int fromIndex,
                     int toIndex);

        /**
         * Emitted when a Zone is being moved.
         *
         * @param zone
         *   The Zone being moved.
         *
         * @param fromIndex
         *   The Zone object's current index.
         *
         * @param toIndex
         *   The index the Zone will be moved to.
         */

        void
        movingZone(const synthclone::Zone *zone, int fromIndex, int toIndex);

        /**
         * Emitted when the visibility of the note property is changed.
         *
         * @param visible
         *   The visibility of the note property.
         */

        void
        notePropertyVisibilityChanged(bool visible);

        /**
         * Emitted when a Participant has being activated.
         *
         * @param participant
         *   The activated Participant.
         *
         * @param parent
         *   The parent Participant of the activated Participant.  If this
         *   argument is set to 0, then the activated Participant is a root
         *   Participant.
         *
         * @param id
         *   The id of the activated Participant.
         */

        void
        participantActivated(const synthclone::Participant *participant,
                             const synthclone::Participant *parent,
                             const QByteArray &id);

        /**
         * Emitted when a Participant has being added.
         *
         * @param participant
         *   The added Participant.
         *
         * @param parent
         *   The parent Participant of the added Participant.  If this argument
         *   is set to 0, then the added Participant is a root Participant.
         *
         * @param id
         *   The id of the added Participant.
         */

        void
        participantAdded(const synthclone::Participant *participant,
                         const synthclone::Participant *parent,
                         const QByteArray &id);

        /**
         * Emitted when a Participant has being deactivated.
         *
         * @param participant
         *   The deactivated Participant.
         *
         * @param parent
         *   The parent Participant of the deactivated Participant.  If this
         *   argument is set to 0, then the deactivated Participant is a root
         *   Participant.
         *
         * @param id
         *   The id of the deactivated Participant.
         */

        void
        participantDeactivated(const synthclone::Participant *participant,
                               const synthclone::Participant *parent,
                               const QByteArray &id);

        /**
         * Emitted when a Participant has being removed.
         *
         * @param participant
         *   The removed Participant.
         *
         * @param parent
         *   The parent Participant of the removed Participant.  If this
         *   argument is set to 0, then the removed Participant is a root
         *   Participant.
         *
         * @param id
         *   The id of the removed Participant.
         */

        void
        participantRemoved(const synthclone::Participant *participant,
                           const synthclone::Participant *parent,
                           const QByteArray &id);

        /**
         * Emitted when the visibility of the release time property is changed.
         *
         * @param visible
         *   The visibility of the release time property.
         */

        void
        releaseTimePropertyVisibilityChanged(bool visible);

        /**
         * Emitted when an Effect is being removed from the Effect list.
         *
         * @param effect
         *   The Effect being removed.
         *
         * @param index
         *   The index of the Effect being removed.
         */

        void
        removingEffect(const synthclone::Effect *effect, int index);

        /**
         * Emitted when an EffectJob is being removed from the EffectJob queue.
         *
         * @param job
         *   The EffectJob being removed.
         *
         * @param index
         *   The index of the EffectJob being removed.
         */

        void
        removingEffectJob(const synthclone::EffectJob *job, int index);

        /**
         * Emitted when a MenuAction is being removed from a root menu.
         *
         * @param action
         *   The MenuAction being removed.
         *
         * @param menu
         *   The root menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the root menu.  The MenuAction is
         *   at the deepest menu.  Any empty sub-menus will be removed.
         */

        void
        removingMenuAction(const synthclone::MenuAction *action,
                           synthclone::Menu menu, const QStringList &subMenus);

        /**
         * Emitted when a MenuAction is being removed from an Effect menu.
         *
         * @param action
         *   The MenuAction being removed.
         *
         * @param effect
         *   The Effect corresponding to the Effect menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Effect menu.  The MenuAction
         *   is at the deepest menu.  Any empty sub-menus will be removed.
         */

        void
        removingMenuAction(const synthclone::MenuAction *action,
                           const synthclone::Effect *effect,
                           const QStringList &subMenus);

        /**
         * Emitted when a MenuAction is being removed from a Sampler menu.
         *
         * @param action
         *   The MenuAction being removed.
         *
         * @param sampler
         *   The Sampler corresponding to the Sampler menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Sampler menu.  The MenuAction
         *   is at the deepest menu.  Any empty sub-menus will be removed.
         */

        void
        removingMenuAction(const synthclone::MenuAction *action,
                           const synthclone::Sampler *sampler,
                           const QStringList &subMenus);

        /**
         * Emitted when a MenuAction is being removed from a Target menu.
         *
         * @param action
         *   The MenuAction being removed.
         *
         * @param target
         *   The Target corresponding to the Target menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Target menu.  The MenuAction
         *   is at the deepest menu.  Any empty sub-menus will be removed.
         */

        void
        removingMenuAction(const synthclone::MenuAction *action,
                           const synthclone::Target *target,
                           const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator is being removed from a root menu.
         *
         * @param separator
         *   The MenuSeparator being removed.
         *
         * @param menu
         *   The root menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the root menu.  The MenuSeparator
         *   is at the deepest menu.  Any empty sub-menus will be removed.
         */

        void
        removingMenuSeparator(const synthclone::MenuSeparator *separator,
                              synthclone::Menu menu,
                              const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator is being removed from an Effect menu.
         *
         * @param separator
         *   The MenuSeparator being removed.
         *
         * @param effect
         *   The Effect corresponding to the Effect menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Effect menu.  The
         *   MenuSeparator is at the deepest menu.  Any empty sub-menus will be
         *   removed.
         */

        void
        removingMenuSeparator(const synthclone::MenuSeparator *separator,
                              const synthclone::Effect *effect,
                              const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator is being removed from a Sampler menu.
         *
         * @param separator
         *   The MenuSeparator being removed.
         *
         * @param sampler
         *   The Sampler corresponding to the Sampler menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Sampler menu.  The
         *   MenuSeparator is at the deepest menu.  Any empty sub-menus will be
         *   removed.
         */

        void
        removingMenuSeparator(const synthclone::MenuSeparator *separator,
                              const synthclone::Sampler *sampler,
                              const QStringList &subMenus);

        /**
         * Emitted when a MenuSeparator is being removed from a Target menu.
         *
         * @param separator
         *   The MenuSeparator being removed.
         *
         * @param target
         *   The Target corresponding to the Target menu.
         *
         * @param subMenus
         *   A list of sub-menus created from the Target menu.  The
         *   MenuSeparator is at the deepest menu.  Any empty sub-menus will be
         *   removed.
         */

        void
        removingMenuSeparator(const synthclone::MenuSeparator *separator,
                              const synthclone::Target *target,
                              const QStringList &subMenus);

        /**
         * Emitted when a Participant is being removed.
         *
         * @param participant
         *   The Participant being removed.
         *
         * @param parent
         *   The parent Participant of the Participant being removed.  If this
         *   argument is set to 0, then the Participant being removed is a root
         *   Participant.
         *
         * @param id
         *   The id of the Participant being removed.
         */

        void
        removingParticipant(const synthclone::Participant *participant,
                            const synthclone::Participant *parent,
                            const QByteArray &id);

        /**
         * Emitted when a Sampler is being removed.
         *
         * @param sampler
         *   The Sampler being removed.
         */

        void
        removingSampler(const synthclone::Sampler *sampler);

        /**
         * Emitted when a SamplerJob is being removed from the SamplerJob
         * queue.
         *
         * @param job
         *   The SamplerJob being removed.
         *
         * @param index
         *   The index of the SamplerJob being removed.
         */

        void
        removingSamplerJob(const synthclone::SamplerJob *job, int index);

        /**
         * Emitted when a Target is being removed from the Target list.
         *
         * @param target
         *   The Target being removed.
         *
         * @param index
         *   The index of the Target being removed.
         */

        void
        removingTarget(const synthclone::Target *target, int index);

        /**
         * Emitted when an Zone is being removed from the Zone list.
         *
         * @param zone
         *   The Zone being removed.
         *
         * @param index
         *   The index of the Zone being removed.
         */

        void
        removingZone(const synthclone::Zone *zone, int index);

        /**
         * Emitted when the session's synthclone::SampleChannelCount is
         * changed.
         *
         * @param count
         *   The synthclone::SampleChannelCount.
         */

        void
        sampleChannelCountChanged(synthclone::SampleChannelCount count);

        /**
         * Emitted when a Sampler has been added.
         *
         * @param sampler
         *   The added Sampler.
         */

        void
        samplerAdded(const synthclone::Sampler *sampler);

        /**
         * Emitted when the session's synthclone::SampleRate is changed.
         *
         * @param sampleRate
         *   The synthclone::SampleRate.
         */

        void
        sampleRateChanged(synthclone::SampleRate sampleRate);

        /**
         * Emitted when a SamplerJob has been added to the SamplerJob queue.
         *
         * @param job
         *   The added SamplerJob.
         *
         * @param index
         *   The index of the SamplerJob.
         */

        void
        samplerJobAdded(const synthclone::SamplerJob *job, int index);

        /**
         * Emitted when a SamplerJob is moved in the SamplerJob queue.
         *
         * @param job
         *   The SamplerJob.
         *
         * @param fromIndex
         *   The index the SamplerJob was moved from.
         *
         * @param toIndex
         *   The current index of the SamplerJob.
         */

        void
        samplerJobMoved(const synthclone::SamplerJob *job, int fromIndex,
                        int toIndex);

        /**
         * Emitted when a SamplerJob has been removed from the SamplerJob
         * queue.
         *
         * @param job
         *   The removed SamplerJob.
         *
         * @param index
         *   The index the SamplerJob was removed from.
         */

        void
        samplerJobRemoved(const synthclone::SamplerJob *job, int index);

        /**
         * Emitted when a Sampler is removed.
         *
         * @param sampler
         *   The removed Sampler.
         */

        void
        samplerRemoved(const synthclone::Sampler *sampler);

        /**
         * Emitted when the visibility of the sample time property is changed.
         *
         * @param visible
         *   The visibility of the sample time property.
         */

        void
        sampleTimePropertyVisibilityChanged(bool visible);

        /**
         * Emitted when the selected Effect is changed.
         *
         * @param effect
         *   The selected Effect.
         *
         * @param index
         *   The index of the selected Effect.
         */

        void
        selectedEffectChanged(const synthclone::Effect *effect, int index);

        /**
         * Emitted when the selected Target is changed.
         *
         * @param target
         *   The selected Target.
         *
         * @param index
         *   The index of the selected Target.
         */

        void
        selectedTargetChanged(const synthclone::Target *target, int index);

        /**
         * Emitted when the synthclone::SessionState changes.
         *
         * @param state
         *   The synthclone::SessionState.
         *
         * @param directory
         *   The session directory.  If this argument is 0, then a session
         *   isn't currently loaded.
         */

        void
        sessionStateChanged(synthclone::SessionState state,
                            const QDir *directory);

        /**
         * Emitted when the visibility of the status property is changed.
         *
         * @param visible
         *   The visibility of the status property.
         */

        void
        statusPropertyVisibilityChanged(bool visible);

        /**
         * Emitted when a Target has been added to the Target list.
         *
         * @param target
         *   The added Target.
         *
         * @param index
         *   The index of the Target.
         */

        void
        targetAdded(const synthclone::Target *target, int index);

        /**
         * Emitted when an error occurs while attempting to build a Target.
         *
         * @param target
         *   The Target that was being built.
         *
         * @param message
         *   The error message.
         */

        void
        targetBuildError(const synthclone::Target *target,
                         const QString &message);

        /**
         * Emitted when a Target is successfully built.
         *
         * @param target
         *   The Target that was successfully built.
         */

        void
        targetBuilt(const synthclone::Target *target);

        /**
         * Emitted when a Target build operation is completed.
         */

        void
        targetsBuilt();

        /**
         * Emitted when a Target has been moved in the Target list.
         *
         * @param target
         *   The moved Target.
         *
         * @param fromIndex
         *   The index the Target was moved from.
         *
         * @param toIndex
         *   The current index of the Target.
         */

        void
        targetMoved(const synthclone::Target *target, int fromIndex,
                    int toIndex);

        /**
         * Emitted when a Target has been removed from the Target list.
         *
         * @param target
         *   The removed Target.
         *
         * @param index
         *   The index the Target was removed from.
         */

        void
        targetRemoved(const synthclone::Target *target, int index);

        /**
         * Emitted when the visibility of the velocity property is changed.
         *
         * @param visible
         *   The visibility of the velocity property.
         */

        void
        velocityPropertyVisibilityChanged(bool visible);

        /**
         * Emitted when the visibility of the wet sample property is changed.
         *
         * @param visible
         *   The visibility of the wet sample property.
         */

        void
        wetSamplePropertyVisibilityChanged(bool visible);

        /**
         * Emitted when a Zone has been added to the Zone list.
         *
         * @param zone
         *   The added Zone.
         *
         * @param index
         *   The index of the Zone.
         */

        void
        zoneAdded(const synthclone::Zone *zone, int index);

        /**
         * Emitted when a Zone has been moved in the Zone list.
         *
         * @param zone
         *   The moved Zone.
         *
         * @param fromIndex
         *   The index the Zone was moved from.
         *
         * @param toIndex
         *   The current index of the Zone.
         */

        void
        zoneMoved(const synthclone::Zone *zone, int fromIndex, int toIndex);

        /**
         * Emitted when a Zone has been removed from the Zone list.
         *
         * @param zone
         *   The removed Zone.
         *
         * @param index
         *   The index the Zone was removed from.
         */

        void
        zoneRemoved(const synthclone::Zone *zone, int index);

        /**
         * Emitted when the selection of a Zone changes.
         *
         * @param zone
         *   The Zone being (de)selected.
         *
         * @param selected
         *   Whether or not the Zone is now selected.
         */

        void
        zoneSelectionChanged(const synthclone::Zone *zone, bool selected);

    protected:

        /**
         * Constructs a new Context object.  This constructor cannot be called
         * directly.  Context objects are provided to Participant objects when
         * the Participant::activate() method is called.
         *
         * @param parent
         *   The parent object of the new Context object.
         */

        explicit
        Context(QObject *parent=0);

        /**
         * Destroys the Context object.  The memory of Context objects is
         * managed by the application.
         */

        virtual
        ~Context();

    };

}

#endif
