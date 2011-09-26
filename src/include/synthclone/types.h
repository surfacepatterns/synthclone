/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011 Devin Anderson
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __SYNTHCLONE_TYPES_H__
#define __SYNTHCLONE_TYPES_H__

#include <QtCore/QMetaType>
#include <QtCore/QtGlobal>

namespace synthclone {

    /**
     * Menu identifiers for default `synthclone` menus.
     */

    enum Menu {
        MENU_ADD_EFFECT = 0,
        MENU_ADD_SAMPLER = 1,
        MENU_ADD_TARGET = 2,
        MENU_HELP = 3,
        MENU_SESSION = 4,
        MENU_TOOLS = 5,
        MENU_VIEW = 6,
        MENU_ZONES = 7
    };

    /**
     * Holds a byte of MIDI data.
     */

    typedef quint8 MIDIData;

    /**
     * A MIDIData value that refers to unset values.  This is only useful for
     * comparing unsigned 7-bit MIDI values.
     */

    const MIDIData MIDI_VALUE_NOT_SET = 0x80;

    /**
     * Holds a sample channel count.
     */

    typedef quint16 SampleChannelCount;

    /**
     * The minimum valid sample channel count.
     */

    const SampleChannelCount SAMPLE_CHANNEL_COUNT_MINIMUM = 1;

    /**
     * The maximum valid sample channel count.
     */

    const SampleChannelCount SAMPLE_CHANNEL_COUNT_MAXIMUM = 65535;

    /**
     * Holds a count of sample frames, or an offset with sample frames.
     */

    typedef qint64 SampleFrameCount;

    /**
     * Holds a sample rate.
     */

    typedef quint32 SampleRate;

    /**
     * The minimum valid sample rate.  I don't see 1 as being a practical
     * sample rate, but I don't see a reason why I shouldn't let it happen if
     * that's the user's wish.
     */

    const SampleRate SAMPLE_RATE_MINIMUM = 1;

    /**
     * The maximum valid sample rate.  I don't think anyone is going to request
     * a sample rate this high, but I don't see a good reason to stop a person
     * from requesting a sample rate if they think it's reasonable.
     *
     * The reason the sampling rate isn't (2 ** 32) - 1 is because libsndfile
     * holds its sample rate in an 'int'.
     */

    const SampleRate SAMPLE_RATE_MAXIMUM = 2147483647;

    /**
     * Holds a sample time (in seconds).
     */

    typedef float SampleTime;

    /**
     * The minimum valid sample time.  This number just needs to be above 0.0.
     */

    const SampleTime SAMPLE_TIME_MINIMUM = 1e-15;

    /**
     * The maximum valid sample time.  The value is arbitrary.  I'm open to
     * changing the value if there's a good reason to do so.
     */

    const SampleTime SAMPLE_TIME_MAXIMUM = 512.0;

    /**
     * Defines session states.
     */

    enum SessionState {
        SESSIONSTATE_CURRENT = 0,
        SESSIONSTATE_LOADING = 1,
        SESSIONSTATE_MODIFIED = 2,
        SESSIONSTATE_SAVING = 3,
        SESSIONSTATE_UNLOADING = 4
    };

}

#endif
