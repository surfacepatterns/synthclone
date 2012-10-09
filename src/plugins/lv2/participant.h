/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone`
 * Copyright (C) 2012 Devin Anderson
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

#include <synthclone/menuaction.h>
#include <synthclone/participant.h>

#include "effect.h"
#include "effectview.h"
#include "lv2world.h"

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
    handleActionUnregistration(QObject *obj);

    void
    handleControlInputPortValueChange(int index, float value);

    void
    handleControlOutputPortValueChange(int index, float value);

    void
    handleEffectActionUnregistration(QObject *obj);

    void
    handleEffectAddition();

    void
    handleEffectConfiguration();

    void
    handleEffectUnregistration(QObject *obj);

    void
    handleEffectViewCloseRequest();

    void
    handlePortValueChange(uint32_t portIndex, uint32_t bufferSize,
                          uint32_t protocol, void const *buffer);

private:

    struct PluginUIData {
        QString typeURI;
        const LV2UIData *uiData;
    };

    Effect *
    addEffect(const LV2Plugin *plugin);

    void
    addPluginActions();

    void
    configureEffect(Effect *effect);

    void
    removePluginActions();

    QMap<synthclone::MenuAction *, const LV2Plugin *> actionPluginMap;
    Effect *configuredEffect;
    synthclone::Context *context;
    QMap<uint32_t, int> controlInputPortIndexMap;
    QMap<uint32_t, int> controlOutputPortIndexMap;
    EffectView effectView;
    QMap<const LV2Plugin *, PluginUIData *> pluginUIMap;
    QList<Effect *> registeredEffects;
    QMap<QString, const LV2Plugin *> uriPluginMap;
    LV2World *world;

};

#endif
