/*
 * libsynthclone_hydrogen - Hydrogen target plugin for `synthclone`
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

#ifndef __SAMPLEFORMAT_H__
#define __SAMPLEFORMAT_H__

enum SampleFormat {
    SAMPLEFORMAT_AU_8BIT = 0,
    SAMPLEFORMAT_AU_16BIT = 1,
    SAMPLEFORMAT_AU_24BIT = 2,
    SAMPLEFORMAT_AU_32BIT = 3,
    SAMPLEFORMAT_AU_32BIT_FLOAT = 4,
    SAMPLEFORMAT_AIFF_8BIT = 5,
    SAMPLEFORMAT_AIFF_16BIT = 6,
    SAMPLEFORMAT_AIFF_24BIT = 7,
    SAMPLEFORMAT_AIFF_32BIT = 8,
    SAMPLEFORMAT_AIFF_32BIT_FLOAT = 9,
    SAMPLEFORMAT_FLAC_8BIT = 10,
    SAMPLEFORMAT_FLAC_16BIT = 11,
    SAMPLEFORMAT_FLAC_24BIT = 12,
    SAMPLEFORMAT_WAV_8BIT = 13,
    SAMPLEFORMAT_WAV_16BIT = 14,
    SAMPLEFORMAT_WAV_24BIT = 15,
    SAMPLEFORMAT_WAV_32BIT = 16,
    SAMPLEFORMAT_WAV_32BIT_FLOAT = 17
};

#endif
