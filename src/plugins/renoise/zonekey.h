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

#ifndef __ZONEKEY_H__
#define __ZONEKEY_H__

#include <synthclone/zone.h>

class ZoneKey: public QObject {

    Q_OBJECT

public:

    explicit
    ZoneKey(const synthclone::Zone &zone, QObject *parent=0);

    ZoneKey(const ZoneKey &key, QObject *parent=0);

    ZoneKey &
    operator=(const ZoneKey &key);

    bool
    operator<(const ZoneKey &key) const;

    synthclone::MIDIData
    getNote() const;

private:

    synthclone::MIDIData aftertouch;
    synthclone::MIDIData channel;
    synthclone::MIDIData channelPressure;
    quint64 controlBits1;
    quint64 controlBits2;
    synthclone::MIDIData controlValues[0x80];
    synthclone::MIDIData note;

};

#endif
