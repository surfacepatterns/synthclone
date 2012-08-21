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

#ifndef __PARTICIPANTMANAGER_H__
#define __PARTICIPANTMANAGER_H__

#include <QtCore/QByteArray>
#include <QtCore/QVariant>

#include <synthclone/participant.h>

#include "context.h"
#include "registration.h"

class Controller;

class ParticipantManager: public QObject {

    Q_OBJECT

public:

    explicit
    ParticipantManager(Controller *controller, QObject *parent=0);

    ~ParticipantManager();

    int
    getActivatedParticipantCount(const synthclone::Participant *parent=0) const;

    const synthclone::Participant *
    getParticipant(const QByteArray &id) const;

    synthclone::Participant *
    getParticipant(const QByteArray &id);

    const synthclone::Participant *
    getParticipant(int index, const synthclone::Participant *parent=0) const;

    synthclone::Participant *
    getParticipant(int index, const synthclone::Participant *parent=0);

    int
    getParticipantCount(const synthclone::Participant *parent=0) const;

    QByteArray
    getParticipantId(const synthclone::Participant *participant) const;

    const synthclone::Participant *
    getParticipantParent(const synthclone::Participant *participant) const;

    synthclone::Participant *
    getParticipantParent(const synthclone::Participant *participant);

    bool
    isParticipantActivated(const synthclone::Participant *participant) const;

public slots:

    void
    activateParticipant(const synthclone::Participant *participant,
                        const QVariant &state=QVariant());

    const synthclone::Registration &
    addParticipant(synthclone::Participant *participant, const QByteArray &id);

    const synthclone::Registration &
    addParticipant(synthclone::Participant *participant,
                   const synthclone::Participant *parent,
                   const QByteArray &subId);

    void
    deactivateParticipant(const synthclone::Participant *participant);

    void
    removeParticipant(const synthclone::Participant *participant);

signals:

    void
    activatingParticipant(const synthclone::Participant *participant,
                          const synthclone::Participant *parent,
                          const QByteArray &id);

    void
    addingParticipant(const synthclone::Participant *participant,
                      const synthclone::Participant *parent,
                      const QByteArray &id);

    void
    deactivatingParticipant(const synthclone::Participant *participant,
                            const synthclone::Participant *parent,
                            const QByteArray &id);

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
    removingParticipant(const synthclone::Participant *participant,
                        const synthclone::Participant *parent,
                        const QByteArray &id);

private:

    typedef QList<synthclone::Participant *> ParticipantList;

    struct ParticipantData {
        ParticipantList children;
        Context *context;
        QByteArray id;
        synthclone::Participant *parent;
        synthclone::Participant *participant;
        Registration *registration;
    };

    typedef QMap<const synthclone::Participant *,
                 ParticipantData *> ParticipantDataMap;
    typedef QMap<QByteArray, synthclone::Participant *> ParticipantIdMap;

    bool
    verifySubId(const QByteArray &subId);

    Controller *controller;
    ParticipantDataMap participantDataMap;
    ParticipantIdMap participantIdMap;
    ParticipantList rootParticipants;

};

#endif
