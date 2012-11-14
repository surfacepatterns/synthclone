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

#ifndef __LV2STATE_H__
#define __LV2STATE_H__

#include <QtCore/QByteArray>
#include <QtCore/QObject>

#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

#include <lilv/lilv.h>

class LV2Instance;

class LV2State: public QObject {

    Q_OBJECT

    friend class LV2Instance;

public:

    LV2State(const QByteArray &state, LilvWorld *world, LV2_URID_Map *map,
             LV2_URID_Unmap *unmap, QObject *parent=0);

    LV2State(LilvInstance *instance, const LilvPlugin *plugin, LilvWorld *world,
             LV2_URID_Map *map, LV2_URID_Unmap *unmap,
             LilvGetPortValueFunc getPortValue=0, void *userData=0,
             QObject *parent=0);

    ~LV2State();

    QByteArray
    getBytes() const;

private:

    LV2_URID_Map *map;
    LilvState *state;
    LV2_URID_Unmap *unmap;
    LilvWorld *world;

};

#endif
