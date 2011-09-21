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

#ifndef __ZONELISTLOADER_H__
#define __ZONELISTLOADER_H__

#include "session.h"

class ZoneListLoader: public QObject {

    Q_OBJECT

public:

    ZoneListLoader(Session &session, QObject *parent=0);

    ~ZoneListLoader();

    void
    loadZones(QDomElement &element, int startIndex=-1,
              const QDir *sampleDirectory=0);

signals:

    void
    loadingZone(int current, int total);

    void
    warning(int line, int column, const QString &message);

private:

    void
    emitWarning(const QDomElement &element, const QString &message);

    bool
    verifyBooleanAttribute(const QDomElement &element, const QString &name,
                           bool defaultValue);

    bool
    verifyMIDIAttribute(const QDomElement &element, const QString &name,
                        synthclone::MIDIData &value, bool required=true,
                        synthclone::MIDIData minimumValue=0,
                        synthclone::MIDIData maximumValue=0x7f);

    synthclone::Sample *
    verifySampleAttribute(const QDomElement &element, const QString &name,
                          const QDir *directory=0, QObject *parent=0);

    bool
    verifySampleTimeAttribute(const QDomElement &element, const QString &name,
                              synthclone::SampleTime &sampleTime);

    Session &session;

};

#endif
