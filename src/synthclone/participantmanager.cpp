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
#include <cctype>

#include <QtCore/QScopedPointer>

#include <synthclone/error.h>
#include <synthclone/util.h>

#include "context.h"
#include "controller.h"

ParticipantManager::ParticipantManager(Controller &controller, QObject *parent):
    QObject(parent),
    controller(controller)
{
    // Empty
}

ParticipantManager::~ParticipantManager()
{
    for (int i = rootParticipants.count() - 1; i >= 0; i--) {
        ParticipantData *data = participantDataMap[rootParticipants[i]];
        if (! data->parent) {
            removeParticipant(data->participant);
        }
    }
    assert(! participantDataMap.count());
    assert(! participantIdMap.count());
    assert(! rootParticipants.count());
}

void
ParticipantManager::
activateParticipant(const synthclone::Participant *participant,
                    const QVariant &state)
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with participant manager"));
    CONFIRM(! data->context, tr("participant is already activated"));

    QByteArray &id = data->id;
    const synthclone::Participant *parent = data->parent;
    synthclone::Participant *mutableParticipant = data->participant;
    emit activatingParticipant(participant, parent, id);
    Context *context = new Context(*mutableParticipant, *this, controller);
    mutableParticipant->activate(*context, state);
    data->context = context;
    emit participantActivated(participant, parent, id);
}

const synthclone::Registration &
ParticipantManager::addParticipant(synthclone::Participant *participant,
                                   const QByteArray &id)
{
    CONFIRM(participant, tr("participant is set to NULL"));
    CONFIRM(! participantDataMap.value(participant, 0),
            tr("participant is already registered with participant manager"));
    QList<QByteArray> parts = id.split('.');
    for (int i = parts.count() - 1; i >= 0; i--) {
        CONFIRM(verifySubId(parts[i]),
                tr("'%1': invalid participant id").arg(id.constData()));
    }

    emit addingParticipant(participant, 0, id);
    ParticipantData *data = new ParticipantData();
    Registration *registration = new Registration(participant, this);
    data->context = 0;
    data->id = id;
    data->parent = 0;
    data->participant = participant;
    data->registration = registration;
    participantDataMap[participant] = data;
    participantIdMap[id] = participant;
    rootParticipants.append(participant);
    emit participantAdded(participant, 0, id);
    return *registration;
}

const synthclone::Registration &
ParticipantManager::addParticipant(synthclone::Participant *participant,
                                   const synthclone::Participant *parent,
                                   const QByteArray &subId)
{
    CONFIRM(participant, tr("participant is set to NULL"));
    CONFIRM(participantDataMap.contains(participant),
            tr("participant is already registered with participant manager"));
    CONFIRM(verifySubId(subId),
            tr("'%1': invalid participant sub-id").arg(subId.constData()));
    assert(parent);

    ParticipantData *parentData = participantDataMap.value(parent, 0);

    assert(parentData);
    assert(parentData->context);

    QByteArray id;
    id.append(parentData->id);
    id.append('.');
    id.append(subId);
    emit addingParticipant(participant, parent, id);
    ParticipantData *data = new ParticipantData();
    Registration *registration = new Registration(participant, this);
    data->context = 0;
    data->id = id;
    data->parent = parentData->participant;
    data->participant = participant;
    data->registration = registration;
    participantDataMap[participant] = data;
    participantIdMap[id] = participant;
    parentData->children.append(participant);
    emit participantAdded(participant, parent, id);
    return *registration;
}

void
ParticipantManager::
deactivateParticipant(const synthclone::Participant *participant)
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with participant manager"));

    Context *context = data->context;

    CONFIRM(data->context, tr("participant is not currently activated"));

    ParticipantList &children = data->children;
    for (int i = children.count() - 1; i >= 0; i--) {
        removeParticipant(children[i]);
    }
    const QByteArray &id = data->id;
    const synthclone::Participant *parent = data->parent;
    emit deactivatingParticipant(participant, parent, id);
    data->participant->deactivate(*context);
    data->context = 0;
    delete context;
    emit participantDeactivated(participant, parent, id);
}

int
ParticipantManager::
getActivatedParticipantCount(const synthclone::Participant *parent) const
{
    const ParticipantList *participants;
    if (! parent) {
        participants = &rootParticipants;
    } else {
        ParticipantData *data = participantDataMap.value(parent, 0);
        CONFIRM(data, tr("the given parent is not a registered participant"));
        participants = &(data->children);
    }
    int activatedCount = 0;
    for (int i = participants->count() - 1; i >= 0; i--) {
        if (isParticipantActivated(participants->at(i))) {
            activatedCount++;
        }
    }
    return activatedCount;
}

const synthclone::Participant *
ParticipantManager::getParticipant(const QByteArray &id) const
{
    const synthclone::Participant *participant = participantIdMap.value(id, 0);
    if (! participant) {
        throw synthclone::Error(tr("'%1' is not a registered participant id").
                                arg(id.data()));
    }
    return participant;
}

synthclone::Participant *
ParticipantManager::getParticipant(const QByteArray &id)
{
    synthclone::Participant *participant = participantIdMap.value(id, 0);
    if (! participant) {
        throw synthclone::Error(tr("'%1' is not a registered participant id").
                                arg(id.data()));
    }
    return participant;
}

const synthclone::Participant *
ParticipantManager::getParticipant(int index,
                                   const synthclone::Participant *parent) const
{
    const ParticipantList *participants;
    if (! parent) {
        participants = &rootParticipants;
    } else {
        ParticipantData *data = participantDataMap.value(parent, 0);

        CONFIRM(data, tr("the given parent is not a registered participant"));

        participants = &(data->children);
    }

    CONFIRM((index >= 0) && (index < participants->count()),
            tr("'%1': index is out of range").arg(index));

    return participants->at(index);
}

synthclone::Participant *
ParticipantManager::getParticipant(int index,
                                   const synthclone::Participant *parent)
{
    const ParticipantList *participants;
    if (! parent) {
        participants = &rootParticipants;
    } else {
        ParticipantData *data = participantDataMap.value(parent, 0);

        CONFIRM(data, tr("the given parent is not a registered participant"));

        participants = &(data->children);
    }

    CONFIRM((index >= 0) && (index < participants->count()),
            tr("'%1': index is out of range").arg(index));

    return participants->at(index);
}

int
ParticipantManager::
getParticipantCount(const synthclone::Participant *parent) const
{
    if (! parent) {
        return rootParticipants.count();
    }
    ParticipantData *data = participantDataMap.value(parent, 0);

    CONFIRM(data, tr("the given parent is not a registered participant"));

    return data->children.count();
}

QByteArray
ParticipantManager::
getParticipantId(const synthclone::Participant *participant) const
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with participant manager"));

    return data->id;
}

const synthclone::Participant *
ParticipantManager::
getParticipantParent(const synthclone::Participant *participant) const
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with participant manager"));

    return data->parent;
}

synthclone::Participant *
ParticipantManager::
getParticipantParent(const synthclone::Participant *participant)
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with participant manager"));

    return data->parent;
}

bool
ParticipantManager::
isParticipantActivated(const synthclone::Participant *participant) const
{
    ParticipantData *data = participantDataMap.value(participant, 0);
    CONFIRM(data, tr("participant is not registered with participant manager"));
    return static_cast<bool>(data->context);
}

void
ParticipantManager::
removeParticipant(const synthclone::Participant *participant)
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with participant manager"));

    if (data->context) {
        deactivateParticipant(participant);
    }
    QByteArray &id = data->id;
    const synthclone::Participant *parent = data->parent;
    emit removingParticipant(participant, parent, id);
    assert(participantDataMap.contains(participant));
    assert(participantIdMap.contains(id));
    participantDataMap.remove(participant);
    participantIdMap.remove(id);
    bool removed;
    if (parent) {
        ParticipantData *parentData = participantDataMap.value(parent, 0);
        assert(parentData);
        removed = parentData->children.removeOne(data->participant);
    } else {
        removed = rootParticipants.removeOne(data->participant);
    }
    assert(removed);
    QScopedPointer<Registration> registrationPtr(data->registration);
    delete data;
    emit participantRemoved(participant, parent, id);
}

bool
ParticipantManager::verifySubId(const QByteArray &subId)
{
    if (subId.isEmpty()) {
        return false;
    }
    char c = subId[0];
    if (! (std::islower(c) || std::isdigit(c))) {
        return false;
    }
    int lastIndex = subId.count() - 1;
    for (int i = 1; i < lastIndex; i++) {
        c = subId[i];
        if (! (std::islower(c) || std::isdigit(c) || (c == '-'))) {
            return false;
        }
    }
    c = subId[lastIndex];
    return std::islower(c) || std::isdigit(c);
}
