/*
 * libsynthclone_sfz - SFZ target plugin for `synthclone`
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

#ifndef __TYPES_H__
#define __TYPES_H__

#include <synthclone/types.h>

enum ControlLayerTableColumn {
    CONTROLLAYERTABLECOLUMN_CONTROL = 0,
    CONTROLLAYERTABLECOLUMN_TYPE = 1,
    CONTROLLAYERTABLECOLUMN_DEFAULT_VALUE = 2,
    CONTROLLAYERTABLECOLUMN_CROSSFADING_ENABLED = 3,

    CONTROLLAYERTABLECOLUMN_TOTAL = 4
};

enum CrossfadeCurve {
    CROSSFADECURVE_NONE = 0,
    CROSSFADECURVE_GAIN = 1,
    CROSSFADECURVE_POWER = 2
};

enum SampleFormat {
    SAMPLEFORMAT_OGG_VORBIS = 0,
    SAMPLEFORMAT_WAV_8BIT = 1,
    SAMPLEFORMAT_WAV_16BIT = 2,
    SAMPLEFORMAT_WAV_24BIT = 3,
    SAMPLEFORMAT_WAV_32BIT = 4,
    SAMPLEFORMAT_WAV_32BIT_FLOAT = 5
};

const synthclone::MIDIData CONTROL_AFTERTOUCH = 0x82;
const synthclone::MIDIData CONTROL_CHANNEL_PRESSURE = 0x81;

#endif
