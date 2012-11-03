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

#include <synthclone/error.h>

#include "lv2instance.h"

LV2Instance::LV2Instance(const LilvPlugin *plugin, LilvWorld *world,
                         LV2_URID_Map *map, LV2_URID_Unmap *unmap,
                         double sampleRate, QObject *parent):
    QObject(parent)
{
    instance = lilv_plugin_instantiate(plugin, sampleRate, 0);
    if (! instance) {
        throw synthclone::Error("failed to create plugin instance");
    }
    this->map = map;
    this->plugin = plugin;
    this->unmap = unmap;
    this->world = world;
}

LV2Instance::~LV2Instance()
{
    lilv_instance_free(instance);
}

void
LV2Instance::activate()
{
    lilv_instance_activate(instance);
}

void
LV2Instance::connectPort(uint32_t portIndex, void *location)
{
    lilv_instance_connect_port(instance, portIndex, location);
}

void
LV2Instance::deactivate()
{
    lilv_instance_deactivate(instance);
}

LV2State *
LV2Instance::getState() const
{
    return new LV2State(instance, plugin, world, map, unmap);
}

QString
LV2Instance::getURI() const
{
    return lilv_instance_get_uri(instance);
}

void
LV2Instance::run(uint32_t sampleCount)
{
    lilv_instance_run(instance, sampleCount);
}

void
LV2Instance::setState(const LV2State *state)
{
    lilv_state_restore(state->state, instance, 0, 0, 0, 0);
}
