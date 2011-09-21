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

#include <cassert>

#include <synthclone/sampleinputstream.h>

#include "zonecomparer.h"

ZoneComparer::ZoneComparer(int property, QObject *parent):
    synthclone::ZoneComparer(parent)
{
    assert((property >= 0) && (property <= PROPERTY_CONTROL_127));
    this->property = property;
}

ZoneComparer::~ZoneComparer()
{
    // Empty
}

bool
ZoneComparer::isLessThan(synthclone::MIDIData n1, synthclone::MIDIData n2) const
{
    int value1 = n1 == synthclone::MIDI_VALUE_NOT_SET ? -1 : ((int) n1);
    int value2 = n2 == synthclone::MIDI_VALUE_NOT_SET ? -1 : ((int) n2);
    return value1 < value2;
}

bool
ZoneComparer::isLessThan(const synthclone::Sample *s1,
                         const synthclone::Sample *s2) const
{
    // Weirdness.  There's no *proper* way to compare samples.
    if (s1 == s2) {
        return false;
    }
    if (! s1) {
        return true;
    }
    if (! s2) {
        return false;
    }
    synthclone::SampleInputStream stream1(*s1);
    synthclone::SampleInputStream stream2(*s2);
    int channels1 = stream1.getChannels();
    int channels2 = stream2.getChannels();
    if (channels1 != channels2) {
        return channels1 < channels2;
    }
    synthclone::SampleFrameCount frames1 = stream1.getFrames();
    synthclone::SampleFrameCount frames2 = stream2.getFrames();
    if (frames1 != frames2) {
        return frames1 < frames2;
    }

    // ::Shrug::
    return false;
}

bool
ZoneComparer::isLessThan(const synthclone::Zone *zone1,
                         const synthclone::Zone *zone2) const
{
    switch (property) {
    case PROPERTY_AFTERTOUCH:
        return isLessThan(zone1->getAftertouch(), zone2->getAftertouch());
    case PROPERTY_CHANNEL:
        return zone1->getChannel() < zone2->getChannel();
    case PROPERTY_DRY_SAMPLE:
        return isLessThan(zone1->getDrySample(), zone2->getDrySample());
    case PROPERTY_NOTE:
        return zone1->getNote() < zone2->getNote();
    case PROPERTY_RELEASE_TIME:
        return zone1->getReleaseTime() < zone2->getReleaseTime();
    case PROPERTY_SAMPLE_TIME:
        return zone1->getSampleTime() < zone2->getSampleTime();
    case PROPERTY_STATUS:
        return zone1->getStatus() < zone2->getStatus();
    case PROPERTY_VELOCITY:
        return zone1->getVelocity() < zone2->getVelocity();
    case PROPERTY_WET_SAMPLE:
        return isLessThan(zone1->getWetSample(), zone2->getWetSample());
    }
    synthclone::MIDIData control =
        static_cast<synthclone::MIDIData>(property - PROPERTY_CONTROL_0);
    return isLessThan(zone1->getControlValue(control),
                      zone2->getControlValue(control));
}
