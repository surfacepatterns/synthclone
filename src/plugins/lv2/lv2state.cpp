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

#include <lv2/lv2plug.in/ns/ext/state/state.h>

#include <QtCore/QDebug>

#include "lv2state.h"

static const char *STATE_URI =
    "http://synthclone.googlecode.com/plugins/lv2/state";

LV2State::LV2State(LilvInstance *instance, const LilvPlugin *plugin,
                   LilvWorld *world, LV2_URID_Map *map, LV2_URID_Unmap *unmap,
                   LilvGetPortValueFunc getPortValue, void *userData,
                   QObject *parent):
    QObject(parent)
{
    assert(instance);
    assert(map);
    assert(plugin);
    assert(unmap);
    assert(world);

    state = lilv_state_new_from_instance(plugin, instance, map, 0, 0, 0, 0,
                                         getPortValue, userData,
                                         LV2_STATE_IS_POD |
                                         LV2_STATE_IS_PORTABLE, 0);
    assert(state);

    this->map = map;
    this->unmap = unmap;
    this->world = world;
}

LV2State::LV2State(const QByteArray &state, LilvWorld *world, LV2_URID_Map *map,
                   LV2_URID_Unmap *unmap, QObject *parent):
    QObject(parent)
{
    assert(map);
    assert(unmap);
    assert(world);
    this->map = map;
    this->state = lilv_state_new_from_string(world, map, state.constData());
    this->unmap = unmap;
    this->world = world;
}

LV2State::~LV2State()
{
    lilv_state_free(state);
}

QByteArray
LV2State::getBytes() const
{
    qDebug() << "lilv_state_to_string";

    QByteArray bytes(lilv_state_to_string(world, map, unmap, state, STATE_URI,
                                          0));

    qDebug() << "/lilv_state_to_string";

    return bytes;
}
