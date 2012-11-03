/*
 * libsynthclone_reverser - `synthclone` effect plugin that renders a sample
 * stream in reverse
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

#include <cassert>

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Reverser"), 0, 0, 1, "Devin Anderson",
                            tr("Renders samples in reverse"), parent),
    addEffectAction(tr("Reverser"))
{
    connect(&addEffectAction, SIGNAL(triggered()), SLOT(addEffect()));
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
    this->context = &context;
}

Effect *
Participant::addEffect()
{
    Effect *effect = new Effect(tr("Reverser"), this);
    connect(effect, SIGNAL(nameChanged(QString)),
            context, SLOT(setSessionModified()));

    const synthclone::Registration &effectRegistration =
        context->addEffect(effect);
    connect(&effectRegistration, SIGNAL(unregistered(QObject *)),
            SLOT(handleUnregistration(QObject *)));

    return effect;
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
    const Effect *reverser = qobject_cast<const Effect *>(effect);
    assert(reverser);

    QVariantMap map;
    map["name"] = reverser->getName();
    return map;
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
    effect->setName(map.value("name", tr("Reverser")).toString());
}
