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

#include <cassert>

#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QScopedPointer>
#include <QtCore/QTextStream>

#include <synthclone/error.h>
#include <synthclone/samplecopier.h>
#include <synthclone/util.h>

#include "target.h"

// Static data

// These destructors don't actually destroy their objects; instead, they
// destroy the dynamically allocated contents of the objects, and leave the
// objects themselves intact.

struct ControlZoneMapDestructor {

    static void
    cleanup(Target::ControlZoneMap *controlZoneMap)
    {
        if (controlZoneMap) {
            Target::ControlZoneMap::iterator end = controlZoneMap->end();
            for (Target::ControlZoneMap::iterator iter =
                     controlZoneMap->begin();
                 iter != end; iter++) {
                delete iter.value();
            }
        }
    }

};

struct ZoneMapDestructor {

    static void
    cleanup(Target::ZoneMap *zoneMap)
    {
        if (zoneMap) {
            Target::ZoneMap::iterator zoneMapEnd = zoneMap->end();
            for (Target::ZoneMap::iterator zoneMapIter = zoneMap->begin();
                 zoneMapIter != zoneMapEnd; zoneMapIter++) {
                Target::NoteZoneMap *noteMap = zoneMapIter.value();
                Target::NoteZoneMap::iterator noteMapEnd = noteMap->end();
                for (Target::NoteZoneMap::iterator noteMapIter =
                         noteMap->begin();
                     noteMapIter != noteMapEnd; noteMapIter++) {
                    Target::VelocityZoneMap *velocityMap = noteMapIter.value();
                    Target::VelocityZoneMap::iterator velocityMapEnd =
                        velocityMap->end();
                    for (Target::VelocityZoneMap::iterator velocityMapIter =
                             velocityMap->begin();
                         velocityMapIter != velocityMapEnd;
                         velocityMapIter++) {
                        delete velocityMapIter.value();
                    }
                    delete velocityMap;
                }
                delete noteMap;
            }
        }
    }

};

// Class implementation

Target::Target(const QString &name, QObject *parent):
    synthclone::Target(name, parent)
{
    controlCrossfadeCurve = CROSSFADECURVE_GAIN;
    drumKit = false;
    noteCrossfadeCurve = CROSSFADECURVE_GAIN;
    sampleFormat = SAMPLEFORMAT_WAV_24BIT;
    velocityCrossfadeCurve = CROSSFADECURVE_GAIN;
    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        availableControls.append(i);
    }
    availableControls.append(CONTROL_AFTERTOUCH);
    availableControls.append(CONTROL_CHANNEL_PRESSURE);
    qStableSort(availableControls.begin(), availableControls.end());
}

Target::~Target()
{
    // Empty
}

ControlLayer *
Target::addControlLayer(synthclone::MIDIData control)
{
    assert(availableControls.contains(control));
    ControlLayer *layer = new ControlLayer(control, this);
    availableControls.removeOne(control);
    controlLayers.append(layer);
    controlLayerMap.insert(control, layer);
    emit controlLayerAdded(layer, controlLayers.count() - 1);
    return layer;
}

void
Target::build(const QList<synthclone::Zone *> &zones)
{
    emit progressChanged(0.0);
    QString message;
    if (path.isEmpty()) {
        message = tr("the build path is not set");
        throw synthclone::Error(message);
    }
    QDir directory(path);
    if (! directory.exists()) {
        message = tr("the path '%1' does not point to an existing directory").
            arg(path);
        throw synthclone::Error(message);
    }
    QFile file(directory.absoluteFilePath("patch.sfz"));
    if (! file.open(QIODevice::WriteOnly)) {
        throw synthclone::Error(file.errorString());
    }

    // This plugin builds a region for every zone.  This allows the user to
    // choose to add a group later for all regions that will contain global
    // parameters.

    emit statusChanged(tr("Building zone map ..."));
    QLocale locale = QLocale::system();
    int zoneCount = zones.count();
    ZoneMap zoneMap;
    QScopedPointer<ZoneMap, ZoneMapDestructor> zoneMapPtr(&zoneMap);

    // Assemble the zone map.  It looks something like this:
    //
    // {
    //     channel1:
    //     {
    //         note1:
    //         {
    //             velocity1: zoneList1,
    //             velocity2: zoneList2,
    //             ...
    //             velocityN: zoneListN
    //         },
    //         note2: {...},
    //         ...
    //         noteN: {...}
    //     },
    //     channel2: {...},
    //     ...
    //     channelN: {...}
    // }

    for (int i = 0; i < zoneCount; i++) {
        emit progressChanged((static_cast<float>(i) / zoneCount) * 0.5);
        synthclone::Zone *zone = zones[i];
        const synthclone::Sample *sample = zone->getWetSample();
        if (! sample) {
            sample = zone->getDrySample();
            if (! sample) {
                message = tr("Skipping zone %1 - no sample").
                    arg(locale.toString(i + 1));
                emit buildWarning(message);
                continue;
            }
        }
        synthclone::MIDIData channel = zone->getChannel();
        NoteZoneMap *noteZoneMap = zoneMap.value(channel, 0);
        if (! noteZoneMap) {
            noteZoneMap = new NoteZoneMap();
            zoneMap.insert(channel, noteZoneMap);
        }
        synthclone::MIDIData note = zone->getNote();
        VelocityZoneMap *velocityZoneMap = noteZoneMap->value(note, 0);
        if (! velocityZoneMap) {
            velocityZoneMap = new VelocityZoneMap();
            noteZoneMap->insert(note, velocityZoneMap);
        }
        synthclone::MIDIData velocity = zone->getVelocity();
        ZoneList *zoneList = velocityZoneMap->value(velocity, 0);
        if (! zoneList) {
            zoneList = new ZoneList();
            velocityZoneMap->insert(velocity, zoneList);
        }
        zoneList->append(zone);
    }
    emit progressChanged(0.5);

    synthclone::SampleStream::Type sampleStreamType;
    synthclone::SampleStream::SubType sampleStreamSubType;
    switch (sampleFormat) {
    case SAMPLEFORMAT_OGG_VORBIS:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_VORBIS;
        sampleStreamType = synthclone::SampleStream::TYPE_OGG;
        break;
    case SAMPLEFORMAT_WAV_8BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_S8;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    case SAMPLEFORMAT_WAV_16BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_16;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    case SAMPLEFORMAT_WAV_24BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_24;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    case SAMPLEFORMAT_WAV_32BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_32;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    case SAMPLEFORMAT_WAV_32BIT_FLOAT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_FLOAT;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    default:
        assert(false);
    }
    QString extension =
        synthclone::getSampleFilenameExtension(sampleStreamType);

    // Holy nested for loops, Batman!  Yuck.  I tried writing this code in a
    // few different ways before settling on this structure.  This seemed the
    // easiest and most readable way to go about it.  If you're looking for
    // bugs, then I apologize in advance.  It goes something like this:
    //
    // for each channel do:
    //     for each note associated with channel do:
    //         for each velocity associated with note do:
    //             write regions

    QTextStream stream(&file);
    QList<synthclone::MIDIData> channels = zoneMap.keys();
    int channelCount = channels.count();

    emit statusChanged("Writing SFZ patch ...");
    int zonesWritten = 0;
    for (int channelIndex = 0; channelIndex < channelCount; channelIndex++) {

        synthclone::MIDIData channel = channels[channelIndex];
        QString channelStr = QString::number(channel);
        NoteZoneMap *noteZoneMap = zoneMap.value(channel);
        QList<synthclone::MIDIData> notes = noteZoneMap->keys();
        int noteCount = notes.count();

        for (int noteIndex = 0; noteIndex < noteCount; noteIndex++) {

            synthclone::MIDIData note = notes[noteIndex];
            QString noteStr = QString::number(note);
            VelocityZoneMap *velocityZoneMap = noteZoneMap->value(note);
            QList<synthclone::MIDIData> velocities = velocityZoneMap->keys();
            int velocityCount = velocities.count();

            for (int velocityIndex = 0; velocityIndex < velocityCount;
                 velocityIndex++) {

                synthclone::MIDIData velocity = velocities[velocityIndex];
                QString velocityStr = QString::number(velocity);

                QStringList commonRegionData;
                commonRegionData << "<region>\n";

                // Channels are easy.
                writeOpcode(commonRegionData, "lochan", channelStr);
                writeOpcode(commonRegionData, "hichan", channelStr);

                // Notes are a little more complex.
                synthclone::MIDIData hiKey;
                synthclone::MIDIData loKey;
                if (drumKit) {
                    writeOpcode(commonRegionData, "key", noteStr);
                    writeOpcode(commonRegionData, "pitch_keytrack", "0");
                    hiKey = note;
                    loKey = note;
                } else {
                    if (! noteIndex) {
                        loKey = 0;
                    } else if (noteCrossfadeCurve == CROSSFADECURVE_NONE) {
                        loKey = ((notes[noteIndex - 1] + note) / 2) + 1;
                    } else {
                        loKey = notes[noteIndex - 1] + 1;
                        writeOpcode(commonRegionData, "xfin_lokey",
                                    QString::number(loKey - 1));
                        writeOpcode(commonRegionData, "xfin_hikey", noteStr);
                    }
                    if (noteIndex == noteCount - 1) {
                        hiKey = 127;
                    } else if (noteCrossfadeCurve == CROSSFADECURVE_NONE) {
                        hiKey = (notes[noteIndex + 1] + note) / 2;
                    } else {
                        hiKey = notes[noteIndex + 1] - 1;
                        writeOpcode(commonRegionData, "xfout_lokey", noteStr);
                        writeOpcode(commonRegionData, "xfout_hikey",
                                    QString::number(hiKey + 1));
                    }
                    if (noteCount != 1) {
                        switch (noteCrossfadeCurve) {
                        case CROSSFADECURVE_GAIN:
                            writeOpcode(commonRegionData, "xf_keycurve",
                                        "gain");
                            break;
                        case CROSSFADECURVE_POWER:
                            writeOpcode(commonRegionData, "xf_keycurve",
                                        "power");
                            // Fallthrough on purpose.
                        case CROSSFADECURVE_NONE:
                            break;
                        default:
                            assert(false);
                        }
                    }
                    writeOpcode(commonRegionData, "pitch_keycenter", noteStr);
                    writeOpcode(commonRegionData, "lokey",
                                QString::number(loKey));
                    writeOpcode(commonRegionData, "hikey",
                                QString::number(hiKey));
                }

                // Low and high velocities are generated in the same way that
                // low and high notes are generated when a drum-kit isn't being
                // built, save for the 'amp_velcurve_N' definitions, which are
                // used when velocity crossfading is not enabled.
                synthclone::MIDIData hiVelocity;
                synthclone::MIDIData loVelocity;
                if (! velocityIndex) {
                    loVelocity = 1;
                } else if (velocityCrossfadeCurve == CROSSFADECURVE_NONE) {
                    loVelocity = ((velocities[velocityIndex - 1] +
                                   velocity) / 2) + 1;
                } else {
                    loVelocity = velocities[velocityIndex - 1] + 1;
                    writeOpcode(commonRegionData, "xfin_lovel",
                                QString::number(loVelocity - 1));
                    writeOpcode(commonRegionData, "xfin_hivel", velocityStr);
                }
                if (velocityIndex == velocityCount - 1) {
                    hiVelocity = 127;
                } else if (velocityCrossfadeCurve == CROSSFADECURVE_NONE) {
                    hiVelocity = (velocities[velocityIndex + 1] +
                                  velocity) / 2;
                } else {
                    hiVelocity = velocities[velocityIndex + 1] - 1;
                    writeOpcode(commonRegionData, "xfout_lovel", velocityStr);
                    writeOpcode(commonRegionData, "xfout_hivel",
                                QString::number(hiVelocity + 1));
                }
                if (velocityCount != 1) {
                    QString temp("amp_velcurve_%1");
                    switch (velocityCrossfadeCurve) {
                    case CROSSFADECURVE_NONE:
                        if (velocity != 127) {
                            writeOpcode(commonRegionData, temp.arg(velocity),
                                        "1.0");
                            if (hiVelocity != velocity) {
                                float amp =
                                    static_cast<float>(hiVelocity) / velocity;
                                writeOpcode(commonRegionData, temp.arg(hiKey),
                                            QString::number(amp));
                            }
                        }
                        break;
                    case CROSSFADECURVE_GAIN:
                        writeOpcode(commonRegionData, "xf_velcurve", "gain");
                        break;
                    case CROSSFADECURVE_POWER:
                        writeOpcode(commonRegionData, "xf_velcurve", "power");
                        break;
                    default:
                        assert(false);
                    }
                }
                writeOpcode(commonRegionData, "lovel",
                            QString::number(loVelocity));
                writeOpcode(commonRegionData, "hivel",
                            QString::number(hiVelocity));

                switch (controlCrossfadeCurve) {
                case CROSSFADECURVE_GAIN:
                    writeOpcode(commonRegionData, "xf_cccurve", "gain");
                    break;
                case CROSSFADECURVE_POWER:
                    writeOpcode(commonRegionData, "xf_cccurve", "power");
                    // Fallthrough on purpose.
                case CROSSFADECURVE_NONE:
                    break;
                default:
                    assert(false);
                }

                // Write samples to the instrument directory.  Create
                // individual regions (everything up until now is common to one
                // or more regions), and add them to a region map.

                RegionMap regionMap;
                ZoneList *zoneList = velocityZoneMap->value(velocity);
                for (int i = zoneList->count() - 1; i >= 0; i--) {
                    const synthclone::Zone *zone = zoneList->at(i);

                    // Write the sample to the target directory.
                    QString sampleName =
                        tr("channel%1-note%2-velocity%3-%4.%5").
                        arg(QString::number(channel), QString::number(note),
                            QString::number(velocity), QString::number(i + 1),
                            extension);
                    const synthclone::Sample *sample = zone->getWetSample();
                    if (! sample) {
                        sample = zone->getDrySample();
                        assert(sample);
                    }
                    synthclone::Sample outSample
                        (directory.absoluteFilePath(sampleName));
                    synthclone::SampleInputStream inputStream(*sample);
                    synthclone::SampleChannelCount channels =
                        inputStream.getChannels();
                    synthclone::SampleOutputStream outputStream
                        (outSample, inputStream.getSampleRate(), channels,
                         sampleStreamType, sampleStreamSubType);
                    synthclone::SampleCopier copier;
                    copier.copy(inputStream, outputStream,
                                inputStream.getFrames());

                    QStringList regionData = commonRegionData;
                    writeOpcode(regionData, "sample", sampleName);
                    regionMap.insert(zone, regionData);
                }

                // Now, we deal with control ranges.
                constructControlRanges(regionMap, *zoneList, 0);

                // Write each constructed region to the file.
                for (int i = zoneList->count() - 1; i >= 0; i--) {
                    stream << regionMap.value(zoneList->at(i)).join("")
                           << '\n';
                }

                zonesWritten += zoneList->count();
                emit progressChanged(((static_cast<float>(zonesWritten) /
                                       zoneCount) * 0.5) + 0.5);
            }
        }
    }
    file.close();
    emit progressChanged(0.0);
    emit statusChanged("Idle.");
}

void
Target::constructControlRanges(RegionMap &regionMap, ZoneList &zoneList,
                               int controlLayerIndex)
{
    int zoneCount = zoneList.count();
    assert(zoneCount);

    // If there is only one zone left, then don't create redundant ranges.
    if (zoneCount == 1) {
        return;
    }

    int controlLayerCount = controlLayers.count();
    assert(controlLayerCount >= controlLayerIndex);

    // If we've gone beyond the control layers but still have more than 1 zone,
    // then make each zone trigger randomly.
    if (controlLayerCount == controlLayerIndex) {
        for (int i = 0; i < zoneCount; i++) {
            const synthclone::Zone *zone = zoneList[i];
            QStringList region = regionMap.value(zone);
            writeOpcode(region, "lorand",
                        QString::number(static_cast<float>(i) / zoneCount));
            writeOpcode(region, "hirand",
                        QString::number(static_cast<float>(i + 1) /
                                        zoneCount));
            regionMap[zone] = region;
        }
        return;
    }

    ControlZoneMap controlZoneMap;
    QScopedPointer<ControlZoneMap, ControlZoneMapDestructor>
        controlZoneMapPtr(&controlZoneMap);

    // Separate zones by control value.
    ControlLayer *controlLayer = controlLayers[controlLayerIndex];
    synthclone::MIDIData control = controlLayer->getControl();
    bool crossfadingEnabled = controlLayer->isCrossfadingEnabled();
    synthclone::MIDIData defaultValue = controlLayer->getDefaultValue();
    synthclone::ControlType type = controlLayer->getType();
    for (int i = 0; i < zoneCount; i++) {
        const synthclone::Zone *zone = zoneList[i];
        synthclone::MIDIData controlValue =
            (control == CONTROL_AFTERTOUCH) ? zone->getAftertouch() :
            (control == CONTROL_CHANNEL_PRESSURE) ?
            zone->getChannelPressure() : zone->getControlValue(control);
        if (controlValue == synthclone::MIDI_VALUE_NOT_SET) {
            controlValue = defaultValue;
        }
        switch (type) {
        case synthclone::CONTROLTYPE_SWITCH:
            controlValue = (controlValue >= 64) ? 127 : 0;
            // Fallthrough on purpose.
        case synthclone::CONTROLTYPE_CONTINUOUS:
            break;
        default:
            assert(false);
        }
        ZoneList *controlZoneList = controlZoneMap.value(controlValue, 0);
        if (! controlZoneList) {
            controlZoneList = new ZoneList();
            controlZoneMap.insert(controlValue, controlZoneList);
        }
        controlZoneList->append(zone);
    }

    QList<synthclone::MIDIData> controlValues = controlZoneMap.keys();
    int controlValueCount = controlValues.count();
    assert(controlValueCount);

    // If there is only one distinct control value, then we don't need to write
    // region data for this control.
    if (controlValueCount != 1) {
        for (int controlValueIndex = 0; controlValueIndex < controlValueCount;
             controlValueIndex++) {
            synthclone::MIDIData controlValue =
                controlValues[controlValueIndex];
            QString controlValueStr = QString::number(controlValue);
            synthclone::MIDIData loControlValue;
            synthclone::MIDIData hiControlValue;
            ZoneList *zones = controlZoneMap.value(controlValue);
            int zoneCount = zones->count();
            assert(zones);
            if (! controlValueIndex) {
                loControlValue = 0;
            } else if ((controlCrossfadeCurve == CROSSFADECURVE_NONE) ||
                       (! crossfadingEnabled)) {
                loControlValue = ((controlValues[controlValueIndex - 1] +
                                   controlValue) / 2) + 1;
            } else {
                loControlValue = controlValues[controlValueIndex - 1] + 1;
                for (int i = zoneCount - 1; i >= 0; i--) {
                    const synthclone::Zone *zone = zones->at(i);
                    QStringList regionData = regionMap.value(zone);
                    writeOpcode(regionData,
                                QString("xfin_locc%1").arg(control),
                                QString::number(loControlValue - 1));
                    writeOpcode(regionData,
                                QString("xfin_hicc%1").arg(control),
                                controlValueStr);
                    regionMap[zone] = regionData;
                }
            }
            if (controlValueIndex == controlValueCount - 1) {
                hiControlValue = 127;
            } else if ((controlCrossfadeCurve == CROSSFADECURVE_NONE) ||
                       (! crossfadingEnabled)) {
                hiControlValue = (controlValues[controlValueIndex + 1] +
                                  controlValue) / 2;
            } else {
                hiControlValue = controlValues[controlValueIndex + 1] - 1;
                for (int i = zoneCount - 1; i >= 0; i--) {
                    const synthclone::Zone *zone = zones->at(i);
                    QStringList regionData = regionMap.value(zone);
                    writeOpcode(regionData,
                                QString("xfout_locc%1").arg(control),
                                controlValueStr);
                    writeOpcode(regionData,
                                QString("xfout_hicc%1").arg(control),
                                QString::number(hiControlValue + 1));
                    regionMap[zone] = regionData;
                }
            }
            if (controlCrossfadeCurve != CROSSFADECURVE_NONE) {
                for (int i = zones->count() - 1; i >= 0; i--) {
                    const synthclone::Zone *zone = zones->at(i);
                    QStringList regionData = regionMap.value(zone);
                    writeOpcode(regionData, QString("locc%1").arg(control),
                                QString::number(loControlValue));
                    writeOpcode(regionData, QString("hicc%1").arg(control),
                                QString::number(hiControlValue));
                    regionMap[zone] = regionData;
                }
            }
        }
    }

    // Move on to the next control layer.
    int nextControlLayerIndex = controlLayerIndex + 1;
    for (int i = 0; i < controlValueCount; i++) {
        constructControlRanges(regionMap,
                               *(controlZoneMap.value(controlValues[i])),
                               nextControlLayerIndex);
    }
}

CrossfadeCurve
Target::getControlCrossfadeCurve() const
{
    return controlCrossfadeCurve;
}

const ControlLayer *
Target::getControlLayer(int index) const
{
    assert((index >= 0) && (index < controlLayers.count()));
    return controlLayers[index];
}

ControlLayer *
Target::getControlLayer(int index)
{
    assert((index >= 0) && (index < controlLayers.count()));
    return controlLayers[index];
}

int
Target::getControlLayerCount() const
{
    return controlLayers.count();
}

int
Target::getControlLayerIndex(const ControlLayer *layer) const
{
    int index = controlLayers.indexOf(const_cast<ControlLayer *>(layer));
    assert(index != -1);
    return index;
}

CrossfadeCurve
Target::getNoteCrossfadeCurve() const
{
    return noteCrossfadeCurve;
}

QString
Target::getPath() const
{
    return path;
}

SampleFormat
Target::getSampleFormat() const
{
    return sampleFormat;
}

CrossfadeCurve
Target::getVelocityCrossfadeCurve() const
{
    return velocityCrossfadeCurve;
}

bool
Target::isDrumKit() const
{
    return drumKit;
}

void
Target::moveControlLayer(int fromIndex, int toIndex)
{
    assert((fromIndex >= 0) && (fromIndex < controlLayers.count()));
    assert((toIndex >= 0) && (toIndex < controlLayers.count()));
    assert(fromIndex != toIndex);
    controlLayers.move(fromIndex, toIndex);
    emit controlLayerMoved(controlLayers[toIndex], fromIndex, toIndex);
}

void
Target::removeControlLayer(int index)
{
    assert((index >= 0) && (index < controlLayers.count()));
    ControlLayer *layer = controlLayers.takeAt(index);
    synthclone::MIDIData control = layer->getControl();
    controlLayerMap.remove(control);
    emit controlLayerRemoved(layer, index);
    availableControls.append(control);
    qStableSort(availableControls.begin(), availableControls.end());
    delete layer;
}

void
Target::setControlCrossfadeCurve(CrossfadeCurve curve)
{
    if (controlCrossfadeCurve != curve) {
        controlCrossfadeCurve = curve;
        emit controlCrossfadeCurveChanged(curve);
    }
}

void
Target::setDrumKit(bool drumKit)
{
    if (this->drumKit != drumKit) {
        this->drumKit = drumKit;
        emit drumKitChanged(drumKit);
    }
}

void
Target::setNoteCrossfadeCurve(CrossfadeCurve curve)
{
    if (noteCrossfadeCurve != curve) {
        noteCrossfadeCurve = curve;
        emit noteCrossfadeCurveChanged(curve);
    }
}

void
Target::setPath(const QString &path)
{
    if (this->path != path) {
        this->path = path;
        emit pathChanged(path);
    }
}

void
Target::setSampleFormat(SampleFormat format)
{
    if (sampleFormat != format) {
        sampleFormat = format;
        emit sampleFormatChanged(format);
    }
}

void
Target::setVelocityCrossfadeCurve(CrossfadeCurve curve)
{
    if (velocityCrossfadeCurve != curve) {
        velocityCrossfadeCurve = curve;
        emit velocityCrossfadeCurveChanged(curve);
    }
}

void
Target::writeOpcode(QStringList &stringList, const QString &name,
                    const QString &value)
{
    stringList << name << "=" << value << "\n";
}
