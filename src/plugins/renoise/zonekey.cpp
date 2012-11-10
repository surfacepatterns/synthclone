/*
 * libsynthclone_renoise - Renoise target plugin for `synthclone`
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

#include "zonekey.h"

ZoneKey::ZoneKey(const synthclone::Zone &zone, QObject *parent):
    QObject(parent)
{
    aftertouch = zone.getAftertouch();
    channel = zone.getChannel();
    channelPressure = zone.getChannelPressure();
    controlBits1 = 0;
    controlBits2 = 0;
    note = zone.getNote();
    synthclone::MIDIData i;
    synthclone::MIDIData value;
    for (i = 0; i < 0x40; i++) {
        value = zone.getControlValue(i);
        controlValues[i] = value;
        if (value != synthclone::MIDI_VALUE_NOT_SET) {
            controlBits1 |= 0x8000000000000000 >> i;
        }
    }
    for (i = 0x40; i < 0x80; i++) {
        value = zone.getControlValue(i);
        controlValues[i] = value;
        if (value != synthclone::MIDI_VALUE_NOT_SET) {
            controlBits2 |= 0x8000000000000000 >> (i - 0x40);
        }
    }
}

ZoneKey::ZoneKey(const ZoneKey &key, QObject *parent):
    QObject(parent)
{
    *this = key;
}

ZoneKey &
ZoneKey::operator=(const ZoneKey &key)
{
    aftertouch = key.aftertouch;
    channel = key.channel;
    channelPressure = key.channelPressure;
    controlBits1 = key.controlBits1;
    controlBits2 = key.controlBits2;
    note = key.note;
    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        controlValues[i] = key.controlValues[i];
    }
    return *this;
}

bool
ZoneKey::operator<(const ZoneKey &key) const
{
    if (channel != key.channel) {
        return channel < key.channel;
    }
    if (note != key.note) {
        return note < key.note;
    }
    if (channelPressure != key.channelPressure) {
        return channelPressure == synthclone::MIDI_VALUE_NOT_SET ? true :
            key.channelPressure == synthclone::MIDI_VALUE_NOT_SET ? false :
            channelPressure < key.channelPressure;
    }
    if (aftertouch != key.aftertouch) {
        return aftertouch == synthclone::MIDI_VALUE_NOT_SET ? true :
            key.aftertouch == synthclone::MIDI_VALUE_NOT_SET ? false :
            aftertouch < key.aftertouch;
    }
    if (! (controlBits1 || controlBits2)) {
        if (key.controlBits1 || key.controlBits2) {
            return true;
        }
    } else if (! (key.controlBits1 || key.controlBits2)) {
        return false;
    } else if (controlBits1 != key.controlBits1) {
        return controlBits1 > key.controlBits1;
    } else if (controlBits2 != key.controlBits2) {
        return controlBits2 > key.controlBits2;
    }
    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        synthclone::MIDIData value1 = controlValues[i];
        synthclone::MIDIData value2 = key.controlValues[i];
        if (value1 != value2) {
            return value1 < value2;
        }
    }
    return false;
}

synthclone::MIDIData
ZoneKey::getNote() const
{
    return note;
}
