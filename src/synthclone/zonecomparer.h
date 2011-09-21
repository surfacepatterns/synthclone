/*
 * synthclone - Synthesizer-cloning software
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

#ifndef __ZONECOMPARER_H__
#define __ZONECOMPARER_H__

#include <synthclone/sample.h>
#include <synthclone/zonecomparer.h>

#include "zonetablecolumn.h"

class ZoneComparer: public synthclone::ZoneComparer {

    Q_OBJECT

public:

    enum Property {
        PROPERTY_AFTERTOUCH = 0,
        PROPERTY_CHANNEL = 1,
        PROPERTY_DRY_SAMPLE = 2,
        PROPERTY_NOTE = 3,
        PROPERTY_RELEASE_TIME = 4,
        PROPERTY_SAMPLE_TIME = 5,
        PROPERTY_STATUS = 6,
        PROPERTY_VELOCITY = 7,
        PROPERTY_WET_SAMPLE = 8,
        PROPERTY_CONTROL_0 = 9,
        PROPERTY_CONTROL_127 = 136
    };

    ZoneComparer(int property, QObject *parent=0);

    ~ZoneComparer();

    bool
    isLessThan(const synthclone::Zone *zone1,
               const synthclone::Zone *zone2) const;

private:

    bool
    isLessThan(synthclone::MIDIData n1, synthclone::MIDIData n2) const;

    bool
    isLessThan(const synthclone::Sample *s1,
               const synthclone::Sample *s2) const;

    int property;

};

#endif
