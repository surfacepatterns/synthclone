/*
 * libsynthclone_fader - Fader effect plugin for `synthclone`
 * Copyright (C) 2013 Devin Anderson
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
    synthclone::Participant(tr("Fader"), 0, 0, 1, "Devin Anderson",
                            tr("Adds a fade-in/fade-out effect to samples"),
                            parent),
    addEffectAction(tr("Fader"))
{
    connect(&addEffectAction, SIGNAL(triggered()),
            SLOT(handleEffectAddition()));
    connect(&effectView, SIGNAL(closeRequest()),
            SLOT(handleEffectViewCloseRequest()));
    configuredEffect = 0;
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
    Effect *effect = new Effect(tr("Fader"), this);
    connect(effect, SIGNAL(fadeInEnabledChanged(bool)),
            context, SLOT(setSessionModified()));
    connect(effect, SIGNAL(fadeInStartVolumeChanged(float)),
            context, SLOT(setSessionModified()));
    connect(effect, SIGNAL(fadeInTimeChanged(float)),
            context, SLOT(setSessionModified()));
    connect(effect, SIGNAL(fadeOutEnabledChanged(bool)),
            context, SLOT(setSessionModified()));
    connect(effect, SIGNAL(fadeOutEndVolumeChanged(float)),
            context, SLOT(setSessionModified()));
    connect(effect, SIGNAL(fadeOutTimeChanged(float)),
            context, SLOT(setSessionModified()));
    connect(effect, SIGNAL(nameChanged(const QString &)),
            context, SLOT(setSessionModified()));

    synthclone::MenuAction *action =
        new synthclone::MenuAction(tr("Configure"), effect);
    connect(action, SIGNAL(triggered()), SLOT(handleEffectConfiguration()));

    const synthclone::Registration &effectRegistration =
        context->addEffect(effect);
    connect(&effectRegistration, SIGNAL(unregistered(QObject *)),
            SLOT(handleUnregistration(QObject *)));

    const synthclone::Registration &actionRegistration =
        context->addMenuAction(action, effect);
    connect(&actionRegistration, SIGNAL(unregistered(QObject *)),
            SLOT(handleUnregistration(QObject *)));

    return effect;
}

void
Participant::configureEffect(Effect *effect)
{
    connect(effect, SIGNAL(fadeInEnabledChanged(bool)),
            &effectView, SLOT(setFadeInEnabled(bool)));
    connect(effect, SIGNAL(fadeInStartVolumeChanged(float)),
            &effectView, SLOT(setFadeInStartVolume(float)));
    connect(effect, SIGNAL(fadeInTimeChanged(float)),
            &effectView, SLOT(setFadeInTime(float)));
    connect(effect, SIGNAL(fadeOutEnabledChanged(bool)),
            &effectView, SLOT(setFadeOutEnabled(bool)));
    connect(effect, SIGNAL(fadeOutEndVolumeChanged(float)),
            &effectView, SLOT(setFadeOutEndVolume(float)));
    connect(effect, SIGNAL(fadeOutTimeChanged(float)),
            &effectView, SLOT(setFadeOutTime(float)));
    connect(effect, SIGNAL(nameChanged(const QString &)),
            &effectView, SLOT(setName(const QString &)));

    connect(&effectView, SIGNAL(fadeInEnabledChanged(bool)),
            effect, SLOT(setFadeInEnabled(bool)));
    connect(&effectView, SIGNAL(fadeInStartVolumeChanged(float)),
            effect, SLOT(setFadeInStartVolume(float)));
    connect(&effectView, SIGNAL(fadeInTimeChanged(float)),
            effect, SLOT(setFadeInTime(float)));
    connect(&effectView, SIGNAL(fadeOutEnabledChanged(bool)),
            effect, SLOT(setFadeOutEnabled(bool)));
    connect(&effectView, SIGNAL(fadeOutEndVolumeChanged(float)),
            effect, SLOT(setFadeOutEndVolume(float)));
    connect(&effectView, SIGNAL(fadeOutTimeChanged(float)),
            effect, SLOT(setFadeOutTime(float)));
    connect(&effectView, SIGNAL(nameChanged(const QString &)),
            effect, SLOT(setName(const QString &)));

    effectView.setFadeInEnabled(effect->isFadeInEnabled());
    effectView.setFadeInStartVolume(effect->getFadeInStartVolume());
    effectView.setFadeInTime(effect->getFadeInTime());
    effectView.setFadeOutEnabled(effect->isFadeOutEnabled());
    effectView.setFadeOutEndVolume(effect->getFadeOutEndVolume());
    effectView.setFadeOutTime(effect->getFadeOutTime());
    effectView.setName(effect->getName());

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
    const Effect *fader = qobject_cast<const Effect *>(effect);
    assert(fader);
    QVariantMap map;
    map["fadeInEnabled"] = fader->isFadeInEnabled();
    map["fadeInStartVolume"] = fader->getFadeInStartVolume();
    map["fadeInTime"] = fader->getFadeInTime();
    map["fadeOutEnabled"] = fader->isFadeOutEnabled();
    map["fadeOutEndVolume"] = fader->getFadeOutEndVolume();
    map["fadeOutTime"] = fader->getFadeOutTime();
    map["name"] = fader->getName();
    return map;
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

    disconnect(configuredEffect, SIGNAL(fadeInEnabledChanged(bool)),
               &effectView, SLOT(setFadeInEnabled(bool)));
    disconnect(configuredEffect, SIGNAL(fadeInStartVolumeChanged(float)),
               &effectView, SLOT(setFadeInStartVolume(float)));
    disconnect(configuredEffect, SIGNAL(fadeInTimeChanged(float)),
               &effectView, SLOT(setFadeInTime(float)));
    disconnect(configuredEffect, SIGNAL(fadeOutEnabledChanged(bool)),
               &effectView, SLOT(setFadeOutEnabled(bool)));
    disconnect(configuredEffect, SIGNAL(fadeOutEndVolumeChanged(float)),
               &effectView, SLOT(setFadeOutEndVolume(float)));
    disconnect(configuredEffect, SIGNAL(fadeOutTimeChanged(float)),
               &effectView, SLOT(setFadeOutTime(float)));
    disconnect(configuredEffect, SIGNAL(nameChanged(const QString &)),
               &effectView, SLOT(setName(const QString &)));

    disconnect(&effectView, SIGNAL(fadeInEnabledChanged(bool)),
               configuredEffect, SLOT(setFadeInEnabled(bool)));
    disconnect(&effectView, SIGNAL(fadeInStartVolumeChanged(float)),
               configuredEffect, SLOT(setFadeInStartVolume(float)));
    disconnect(&effectView, SIGNAL(fadeInTimeChanged(float)),
               configuredEffect, SLOT(setFadeInTime(float)));
    disconnect(&effectView, SIGNAL(fadeOutEnabledChanged(bool)),
               configuredEffect, SLOT(setFadeOutEnabled(bool)));
    disconnect(&effectView, SIGNAL(fadeOutEndVolumeChanged(float)),
               configuredEffect, SLOT(setFadeOutEndVolume(float)));
    disconnect(&effectView, SIGNAL(fadeOutTimeChanged(float)),
               configuredEffect, SLOT(setFadeOutTime(float)));
    disconnect(&effectView, SIGNAL(nameChanged(const QString &)),
               configuredEffect, SLOT(setName(const QString &)));

    configuredEffect = 0;
}

void
Participant::handleUnregistration(QObject *obj)
{
    delete obj;
}

void
Participant::restoreEffect(const QVariant &state)
{
    Effect *effect = addEffect();
    const QVariantMap map = state.toMap();
    effect->setFadeInEnabled(map.value("fadeInEnabled", true).toBool());
    effect->setFadeInStartVolume
        (map.value("fadeInStartVolume", -64).toFloat());
    effect->setFadeInTime(map.value("fadeInTime", 0.01).toFloat());
    effect->setFadeOutEnabled(map.value("fadeOutEnabled", true).toBool());
    effect->setFadeOutEndVolume(map.value("fadeOutEndVolume", -64).toFloat());
    effect->setFadeOutTime(map.value("fadeOutTime", 0.01).toFloat());
    effect->setName(map.value("name", tr("Fader")).toString());
}
