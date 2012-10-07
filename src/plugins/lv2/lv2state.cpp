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

#include <cassert>

#include "lv2state.h"

LV2State::LV2State(LilvInstance *instance, const LilvPlugin *plugin,
                   LilvWorld *world, QObject *parent):
    QObject(parent)
{
    assert(instance);
    assert(plugin);
    assert(world);
    state = lilv_state_new_from_instance(plugin, instance, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0);
    assert(state);
    this->world = world;
}

LV2State::LV2State(const QString &state, LilvWorld *world, QObject *parent):
    QObject(parent)
{
    assert(world);
    QByteArray stateBytes = state.toAscii();
    this->state = lilv_state_new_from_string(world, 0, stateBytes.constData());
    this->world = world;
}

LV2State::~LV2State()
{
    lilv_state_free(state);
}

QString
LV2State::getString() const
{
    return QString(lilv_state_to_string(world, 0, 0, state, 0, 0));
}
