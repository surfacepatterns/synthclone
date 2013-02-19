/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone`
 * Copyright (C) 2012-2013 Devin Anderson
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

#ifndef __TYPES_H__
#define __TYPES_H__

#include <synthclone/types.h>

// There are times when the sample rate will not be set for synthclone, but we
// will need a sample rate to instantiate and configure LV2 effects.  As a hack,
// we use this value.
const synthclone::SampleRate FALLBACK_SAMPLE_RATE = 48000;

enum ChannelMapTableColumn {
    CHANNELMAPTABLECOLUMN_INPUT_CHANNEL = 0,
    CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL = 1,
    CHANNELMAPTABLECOLUMN_TOTAL = 2
};

enum ControlInputPortType {
    CONTROLINPUTPORTTYPE_BOOLEAN = 0,
    CONTROLINPUTPORTTYPE_ENUMERATION = 1,
    CONTROLINPUTPORTTYPE_FLOAT = 2,
    CONTROLINPUTPORTTYPE_INTEGER = 3
};

#endif
