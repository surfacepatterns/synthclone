/*
 * libsynthclone_trimmer - Sample trimmer effect plugin for `synthclone`
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

#include <synthclone/participant.h>

#include "effect.h"
#include "effectview.h"

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
    getState(const synthclone::Effect *effect) const;

    void
    restoreEffect(const QVariant &state);

private slots:

    void
    handleConfiguredEffectTrimEndChange(bool trimEnd);

    void
    handleConfiguredEffectTrimStartChange(bool trimStart);

    void
    handleEffectAddition();

    void
    handleEffectConfiguration();

    void
    handleEffectViewCloseRequest();

    void
    handleUnregistration(QObject *obj);

private:

    Effect *
    addEffect();

    void
    configureEffect(Effect *effect);

    synthclone::MenuAction addEffectAction;
    synthclone::Context *context;
    Effect *configuredEffect;
    EffectView effectView;

};

#endif
