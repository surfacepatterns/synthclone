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

    LV2Instance(const LilvPlugin *plugin, LilvWorld *world, double sampleRate,
                QObject *parent=0);

    ~LV2Instance();

    void
    activate();

    void
    connectPort(uint32_t portIndex, void *location);

    void
    deactivate();

    LV2State *
    getState() const;

    QString
    getURI() const;

    void
    run(uint32_t sampleCount);

    void
    setState(const LV2State *state);

private:

    LilvInstance *instance;
    const LilvPlugin *plugin;
    LilvWorld *world;

};

#endif
