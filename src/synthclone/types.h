/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011-2012 Devin Anderson
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

enum ParticipantTreeColumn {
    PARTICIPANTTREECOLUMN_NAME = 0,
    PARTICIPANTTREECOLUMN_STATE = 1,
    PARTICIPANTTREECOLUMN_VERSION = 2,
    PARTICIPANTTREECOLUMN_AUTHOR = 3,
    PARTICIPANTTREECOLUMN_SUMMARY = 4,

    PARTICIPANTTREECOLUMN_TOTAL = 5
};

enum ZoneTableColumn {
    ZONETABLECOLUMN_STATUS = 0,
    ZONETABLECOLUMN_CHANNEL = 1,
    ZONETABLECOLUMN_NOTE = 2,
    ZONETABLECOLUMN_VELOCITY = 3,
    ZONETABLECOLUMN_CHANNEL_PRESSURE = 4,
    ZONETABLECOLUMN_AFTERTOUCH = 5,
    ZONETABLECOLUMN_CONTROL_0 = 6,

    ZONETABLECOLUMN_CONTROL_127 = 133,
    ZONETABLECOLUMN_SAMPLE_TIME = 134,
    ZONETABLECOLUMN_RELEASE_TIME = 135,
    ZONETABLECOLUMN_DRY_SAMPLE = 136,
    ZONETABLECOLUMN_WET_SAMPLE = 137,

    ZONETABLECOLUMN_BASE_TOTAL = 138
};

#endif
