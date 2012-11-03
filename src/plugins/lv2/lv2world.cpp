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

#include <synthclone/error.h>

#include "lv2world.h"

// Class definition

LV2World::LV2World(QObject *parent):
    QObject(parent)
{
    world = lilv_world_new();
    if (! world) {
        throw synthclone::Error("failed to load lilv world");
    }
    lilv_world_load_all(world);
    const LilvPlugins *plugins = lilv_world_get_all_plugins(world);
    assert(plugins);
    LV2_URID_Map *map = uriMap.getMap();
    LV2_URID_Unmap *unmap = uriMap.getUnmap();
    LILV_FOREACH(plugins, iter, plugins) {
        pluginList.append(new LV2Plugin(lilv_plugins_get(plugins, iter), world,
                                        map, unmap, this));
    }
}

LV2World::~LV2World()
{
    for (int i = pluginList.count() - 1; i >= 0; i--) {
        delete pluginList.takeLast();
    }
    lilv_world_free(world);
}

LV2State *
LV2World::createState(const QString &state)
{
    return new LV2State(state, world, uriMap.getMap(), uriMap.getUnmap());
}

const LV2Plugin &
LV2World::getPlugin(int index) const
{
    assert((index >= 0) && (index < pluginList.count()));
    return *(pluginList.at(index));
}

int
LV2World::getPluginCount() const
{
    return pluginList.count();
}
