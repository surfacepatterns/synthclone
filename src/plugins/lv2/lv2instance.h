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

#ifndef __LV2INSTANCE_H__
#define __LV2INSTANCE_H__

#include "lv2state.h"

class LV2Instance: public QObject {

    Q_OBJECT

public:

    LV2Instance(const LilvPlugin *plugin, LilvWorld *world, LV2_URID_Map *map,
                LV2_URID_Unmap *unmap, double sampleRate, QObject *parent=0);

    ~LV2Instance();

    void
    activate();

    void
    connectPort(uint32_t portIndex, void *location);

    void
    deactivate();

    LV2State *
    getState(LilvGetPortValueFunc getPortValue=0, void *userData=0) const;

    QString
    getURI() const;

    void
    run(uint32_t sampleCount);

    void
    setState(const LV2State *state, LilvSetPortValueFunc setPortValue=0,
             void *userData=0);

private:

    LilvInstance *instance;
    LV2_URID_Map *map;
    const LilvPlugin *plugin;
    LV2_URID_Unmap *unmap;
    LilvWorld *world;

};

#endif
