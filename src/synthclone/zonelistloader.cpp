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

#include <synthclone/error.h>

#include "util.h"
#include "zonelistloader.h"

ZoneListLoader::ZoneListLoader(Session &session, QObject *parent):
    QObject(parent),
    session(session)
{
    // Empty
}

ZoneListLoader::~ZoneListLoader()
{
    // Empty
}

void
ZoneListLoader::emitWarning(const QDomElement &element, const QString &message)
{
    emit warning(element.lineNumber(), element.columnNumber(), message);
}

void
ZoneListLoader::loadZones(QDomElement &element, int startIndex,
                          const QDir *samplesDirectory)
{
    assert((startIndex >= -1) && (startIndex <= session.getZoneCount()));
    if (startIndex == -1) {
        startIndex = session.getZoneCount();
    }
    int elementCount = element.elementsByTagName("zone").count();
    int i;
    QString message;
    for (i = 0, element = element.firstChildElement("zone"); ! element.isNull();
         element = element.nextSiblingElement("zone"), i++) {
        emit loadingZone(i + 1, elementCount);
        Zone *zone = qobject_cast<Zone *>(session.addZone(startIndex + i));

        synthclone::MIDIData data;
        if (verifyMIDIAttribute(element, "aftertouch", data, false)) {
            zone->setAftertouch(data);
        }
        if (verifyMIDIAttribute(element, "channel", data, true, 1, 16)) {
            zone->setChannel(data);
        }
        if (verifyMIDIAttribute(element, "note", data)) {
            zone->setNote(data);
        }
        if (verifyMIDIAttribute(element, "velocity", data, true, 1)) {
            zone->setVelocity(data);
        }

        synthclone::SampleTime sampleTime;
        if (verifySampleTimeAttribute(element, "release-time", sampleTime)) {
            zone->setReleaseTime(sampleTime);
        }
        if (verifySampleTimeAttribute(element, "sample-time", sampleTime)) {
            zone->setSampleTime(sampleTime);
        }

        synthclone::Sample *sample =
            verifySampleAttribute(element, "dry-sample", samplesDirectory);
        if (sample) {
            try {
                zone->setDrySample(sample);
            } catch (synthclone::Error &e) {
                emitWarning(element, e.getMessage());
            }
        }
        sample = verifySampleAttribute(element, "wet-sample", samplesDirectory);
        if (sample) {
            try {
                zone->setWetSample(sample);
            } catch (synthclone::Error &e) {
                emitWarning(element, e.getMessage());
            }
        }

        // Stale checks have to be done after the samples are set.
        zone->setDrySampleStale(verifyBooleanAttribute(element,
                                                       "dry-sample-stale",
                                                       false));
        zone->setWetSampleStale(verifyBooleanAttribute(element,
                                                       "wet-sample-stale",
                                                       false));

        QDomElement subElement = element.firstChildElement("controls");
        if (subElement.isNull()) {
            message = "'zone' element has no 'controls' element";
            emitWarning(element, message);
        } else {
            for (subElement = subElement.firstChildElement("control");
                 ! subElement.isNull();
                 subElement = subElement.nextSiblingElement("control")) {
                synthclone::MIDIData index;
                if (verifyMIDIAttribute(subElement, "index", index, true) &&
                    verifyMIDIAttribute(subElement, "value", data, true)) {
                    zone->setControlValue(index, data);
                }
            }
        }
    }
}

bool
ZoneListLoader::verifyBooleanAttribute(const QDomElement &element,
                                       const QString &name, bool defaultValue)
{
    try {
        return getBooleanAttribute(element, name);
    } catch (synthclone::Error &e) {
        emitWarning(element, e.getMessage());
    }
    return defaultValue;
}

bool
ZoneListLoader::verifyMIDIAttribute(const QDomElement &element,
                                    const QString &name,
                                    synthclone::MIDIData &value, bool required,
                                    synthclone::MIDIData minimumValue,
                                    synthclone::MIDIData maximumValue)
{
    assert(minimumValue <= maximumValue);
    assert(maximumValue <= 0x7f);
    QString message;
    QString strValue = element.attribute(name);
    if (strValue.isEmpty()) {
        if (! required) {
            value = synthclone::MIDI_VALUE_NOT_SET;
            return true;
        }
        message = qApp->tr("'%1' element does not contain '%2' attribute").
            arg(element.tagName(), name);
        emitWarning(element, message);
        return false;
    }
    bool success;
    ulong uValue = strValue.toULong(&success);
    if (! success) {
        message = qApp->tr("'%1' element contains non-integer '%2' attribute").
            arg(element.tagName(), name);
        emitWarning(element, message);
        return false;
    }
    if (! (((uValue >= minimumValue) && (uValue <= maximumValue)) ||
           ((! required) && (uValue == synthclone::MIDI_VALUE_NOT_SET)))) {
        message = qApp->tr("'%1' element contains out of range '%2' attribute").
            arg(element.tagName(), name);
        emitWarning(element, message);
        return false;
    }
    value = static_cast<synthclone::MIDIData>(uValue);
    return true;
}

synthclone::Sample *
ZoneListLoader::verifySampleAttribute(const QDomElement &element,
                                      const QString &name,
                                      const QDir *directory, QObject *parent)
{
    QString path = element.attribute(name);
    if (path.isEmpty()) {
        return 0;
    }
    QString message;
    if (directory) {
        if (path != QFileInfo(path).fileName()) {
            message = qApp->tr("'%1' attribute of '%2' element is not a valid "
                               "file name").arg(name, element.tagName());
            emitWarning(element, message);
            return 0;
        }
        path = directory->absoluteFilePath(path);
    }
    try {
        return new synthclone::Sample(path, parent);
    } catch (synthclone::Error &e) {
        message = qApp->tr("failed to load sample from '%1': %2").
            arg(path, e.getMessage());
        emitWarning(element, message);
    }
    return 0;
}

bool
ZoneListLoader::verifySampleTimeAttribute(const QDomElement &element,
                                          const QString &name,
                                          synthclone::SampleTime &sampleTime)
{
    QString message;
    QString strValue = element.attribute(name);
    if (strValue.isEmpty()) {
        message = qApp->tr("'%1' element does not contain '%2' attribute").
            arg(element.tagName(), name);
        emitWarning(element, message);
        return false;
    }
    bool success;
    float value = strValue.toFloat(&success);
    if (! success) {
        message = qApp->tr("'%1' element contains non-numeric '%2' attribute").
            arg(element.tagName()).arg(name);
        emitWarning(element, message);
        return false;
    }
    if (! ((value > 0.0) && (value <= synthclone::SAMPLE_TIME_MAXIMUM))) {
        message = qApp->tr("'%1' element contains out of range '%2' attribute").
            arg(element.tagName(), name);
        emitWarning(element, message);
        return false;
    }
    sampleTime = static_cast<synthclone::SampleTime>(value);
    return true;
}
