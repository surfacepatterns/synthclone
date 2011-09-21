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

#include <cassert>

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Trimmer"), 0, 0, 1, "Devin Anderson",
                            tr("Trims silence from start/end of samples"),
                            parent),
    addEffectAction(tr("Trimmer"))
{
    connect(&addEffectAction, SIGNAL(triggered()),
            SLOT(handleEffectAddition()));
    connect(&effectView, SIGNAL(closeRequest()),
            SLOT(handleEffectViewCloseRequest()));
    context = 0;
}

Participant::~Participant()
{
    // Empty
}

void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    context.addMenuAction(&addEffectAction, synthclone::MENU_ADD_EFFECT);
    configuredEffect = 0;
    this->context = &context;
}

Effect *
Participant::addEffect()
{
    Effect *effect = new Effect(tr("Trimmer"), this);
    connect(effect, SIGNAL(nameChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(effect, SIGNAL(sampleFloorChanged(float)),
            context, SLOT(setSessionModified()));
    connect(effect, SIGNAL(trimEndChanged(bool)),
            context, SLOT(setSessionModified()));
    connect(effect, SIGNAL(trimStartChanged(bool)),
            context, SLOT(setSessionModified()));

    synthclone::MenuAction *action =
        new synthclone::MenuAction(tr("Configure"), effect);
    connect(action, SIGNAL(triggered()), SLOT(handleEffectConfiguration()));

    const synthclone::Registration &effectRegistration =
        context->addEffect(effect);
    connect(&effectRegistration, SIGNAL(unregistered(QObject *)),
            effect, SLOT(deleteLater()));

    const synthclone::Registration &actionRegistration =
        context->addMenuAction(action, effect);
    connect(&actionRegistration, SIGNAL(unregistered(QObject *)),
            action, SLOT(deleteLater()));

    return effect;
}

void
Participant::configureEffect(Effect *effect)
{
    connect(effect, SIGNAL(nameChanged(const QString &)),
            &effectView, SLOT(setName(const QString &)));
    connect(effect, SIGNAL(sampleFloorChanged(float)),
            &effectView, SLOT(setSampleFloor(float)));

    connect(effect, SIGNAL(trimEndChanged(bool)),
            SLOT(handleConfiguredEffectTrimEndChange(bool)));
    connect(effect, SIGNAL(trimStartChanged(bool)),
            SLOT(handleConfiguredEffectTrimStartChange(bool)));

    connect(&effectView, SIGNAL(nameChanged(const QString &)),
            effect, SLOT(setName(const QString &)));
    connect(&effectView, SIGNAL(sampleFloorChanged(float)),
            effect, SLOT(setSampleFloor(float)));
    connect(&effectView, SIGNAL(trimEndChanged(bool)),
            effect, SLOT(setTrimEnd(bool)));
    connect(&effectView, SIGNAL(trimStartChanged(bool)),
            effect, SLOT(setTrimStart(bool)));

    effectView.setName(effect->getName());
    effectView.setSampleFloor(effect->getSampleFloor());
    bool trimEnd = effect->getTrimEnd();
    bool trimStart = effect->getTrimStart();
    effectView.setSampleFloorEditorEnabled(trimStart || trimEnd);
    effectView.setTrimEnd(trimEnd);
    effectView.setTrimStart(trimStart);

    configuredEffect = effect;

    effectView.setVisible(true);
}

void
Participant::deactivate(synthclone::Context &context)
{
    context.removeMenuAction(&addEffectAction);
    this->context = 0;
}

QVariant
Participant::getState(const synthclone::Effect *effect) const
{
    const Effect *trimmer = qobject_cast<const Effect *>(effect);
    assert(trimmer);
    QVariantMap map;
    map["name"] = trimmer->getName();
    map["sampleFloor"] = trimmer->getSampleFloor();
    map["trimEnd"] = trimmer->getTrimEnd();
    map["trimStart"] = trimmer->getTrimStart();
    return map;
}

void
Participant::handleConfiguredEffectTrimEndChange(bool trimEnd)
{
    effectView.setTrimEnd(trimEnd);
    effectView.setSampleFloorEditorEnabled
        (trimEnd || configuredEffect->getTrimStart());
}

void
Participant::handleConfiguredEffectTrimStartChange(bool trimStart)
{
    effectView.setTrimStart(trimStart);
    effectView.setSampleFloorEditorEnabled
        (trimStart || configuredEffect->getTrimEnd());
}

void
Participant::handleEffectAddition()
{
    configureEffect(addEffect());
}

void
Participant::handleEffectConfiguration()
{
    synthclone::MenuAction *action =
        qobject_cast<synthclone::MenuAction *>(sender());
    configureEffect(qobject_cast<Effect *>(action->parent()));
}

void
Participant::handleEffectViewCloseRequest()
{
    effectView.setVisible(false);
    disconnect(configuredEffect, SIGNAL(sampleFloorChanged(float)),
               &effectView, SLOT(setSampleFloor(float)));
    disconnect(configuredEffect, SIGNAL(nameChanged(const QString &)),
               &effectView, SLOT(setName(const QString &)));

    disconnect(&effectView, SIGNAL(nameChanged(const QString &)),
               configuredEffect, SLOT(setName(const QString &)));
    disconnect(&effectView, SIGNAL(sampleFloorChanged(float)),
               configuredEffect, SLOT(setSampleFloor(float)));
    disconnect(&effectView, SIGNAL(trimEndChanged(bool)),
               configuredEffect, SLOT(setTrimEnd(bool)));
    disconnect(&effectView, SIGNAL(trimStartChanged(bool)),
               configuredEffect, SLOT(setTrimStart(bool)));

    configuredEffect = 0;
}

void
Participant::restoreEffect(const QVariant &state)
{
    Effect *effect = addEffect();
    const QVariantMap map = state.toMap();
    effect->setName(map.value("name", tr("Trimmer")).toString());
    effect->setSampleFloor(map.value("sampleFloor", -70.0).toFloat());
    effect->setTrimEnd(map.value("trimEnd", true).toBool());
    effect->setTrimStart(map.value("trimStart", true).toBool());
}
