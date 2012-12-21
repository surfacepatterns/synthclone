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

#include <cassert>

#include <QtCore/QLocale>

#include <synthclone/error.h>
#include <synthclone/samplecopier.h>
#include <synthclone/util.h>

#include "target.h"
#include "velocitycomparer.h"

Target::Target(const QString &name, QObject *parent):
    synthclone::Target(name, parent)
{
    drumKit = false;
    instrumentName = name;
    layerAlgorithm = LAYERALGORITHM_LINEAR_INTERPOLATION;
    pitchInterpolation = PITCHINTERPOLATION_CUBIC;
}

Target::~Target()
{
    // Empty
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
    if (instrumentName.isEmpty()) {
        message = tr("the instrument name is not set");
        throw synthclone::Error(message);
    }

    ArchiveWriter archiveWriter(path, instrumentName);
    QString configuration;
    QXmlStreamWriter confWriter(&configuration);
    confWriter.setAutoFormatting(true);
    confWriter.setAutoFormattingIndent(2);
    confWriter.setCodec("UTF-8");
    confWriter.writeStartDocument();
    confWriter.writeStartElement("RenoiseInstrument");
    confWriter.writeAttribute("doc_version", "13");
    writeElement(confWriter, "ActiveGeneratorTab", "Samples");
    writeElement(confWriter, "Name", instrumentName);

    // If this instrument is a drum kit, then this plugin builds different
    // sub-instruments for every different combination of channel, note, channel
    // pressure, aftertouch, and control values.  If one or more zones have the
    // same above data, then they will become layers in the same instrument,
    // which will be sorted by velocity values.
    //
    // If this instrument is not a drum kit, then it's a bit more complex.
    // All of the zones will be examined to find the most commonly used MIDI
    // channel.  Any zone that does not have that MIDI channel and/or has a
    // valid channel pressure, a valid aftertouch, or valid control values, will
    // be represented in the Renoise instrument as a sample without a note-on
    // mapping.

    synthclone::MIDIData midiChannel = 0;
    int zoneCount = zones.count();
    if (! drumKit) {
        emit statusChanged(tr("Finding most common MIDI channel ..."));
        int channelCounts[16];
        for (int i = 0; i < 16; i++) {
            channelCounts[i] = 0;
        }
        for (int i = 0; i < zoneCount; i++) {
            channelCounts[zones[i]->getChannel() - 1]++;
        }
        int greatestChannelCount = channelCounts[0];
        midiChannel = 0;
        for (int i = 1; i < 16; i++) {
            int channelCount = channelCounts[i];
            if (channelCount > greatestChannelCount) {
                greatestChannelCount = channelCount;
                midiChannel = i;
            }
        }
        midiChannel++;
    }

    emit statusChanged(tr("Adding samples to archive ..."));
    QList<synthclone::Zone *> includedZones;
    QLocale locale = QLocale::system();
    QMultiMap<ZoneKey, synthclone::Zone *> zoneMap;
    confWriter.writeStartElement("Samples");
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

        writeSample(archiveWriter, confWriter, zone);
        includedZones.append(zone);

        if (! drumKit) {
            if (zone->getChannel() != midiChannel) {
                message = tr("Zone %1 will not be mapped - MIDI channel is not "
                             "%2").
                    arg(locale.toString(i + 1)).
                    arg(locale.toString(midiChannel));
                emit buildWarning(message);
                continue;
            }
            if (zone->getAftertouch() != synthclone::MIDI_VALUE_NOT_SET) {
                message = tr("Zone %1 will not be mapped - Renoise doesn't "
                             "support aftertouch specific samples").
                    arg(locale.toString(i + 1));
                emit buildWarning(message);
                continue;
            }
            if (zone->getChannelPressure() != synthclone::MIDI_VALUE_NOT_SET) {
                message = tr("Zone %1 will not be mapped - Renoise doesn't "
                             "support channel pressure specific samples").
                    arg(locale.toString(i + 1));
                emit buildWarning(message);
                continue;
            }
            if (zone->getControlMap().count()) {
                message = tr("Zone %1 will not be mapped - Renoise doesn't "
                             "support controller specific samples").
                    arg(locale.toString(i + 1));
                emit buildWarning(message);
                continue;
            }
        }

        zoneMap.insert(ZoneKey(*zone), zone);
    }
    confWriter.writeEndElement();
    emit progressChanged(0.5);

    QList<ZoneKey> keys = zoneMap.uniqueKeys();
    int keyCount = keys.count();
    if (keyCount > 128) {
        message = tr("The current zone list contains %1 potential note "
                     "mappings.  Renoise only supports %2 note mappings per "
                     "instrument.  Some note mappings will not be generated.").
            arg(locale.toString(keyCount), locale.toString(128));
        emit buildWarning(message);
        keyCount = 128;
    }

    // Renoise instruments separate MIDI mappings from the samples themselves,
    // which is definitely cool.  However, Renoise does not have specific sample
    // mappings for controllers, aftertouch, channel pressure, etc., which is
    // unfortunate.
    confWriter.writeStartElement("SampleSplitMap");
    confWriter.writeStartElement("NoteOnMappings");
    emit statusChanged(tr("Writing note mappings ..."));
    synthclone::MIDIData lowNote = 0;
    for (int i = 0; i < keyCount; i++) {
        float startProgress = ((static_cast<float>(i) / keyCount) * 0.5) + 0.5;
        float endProgress = ((static_cast<float>(i + 1) / keyCount) * 0.5) +
            0.5;
        float difference = endProgress - startProgress;

        emit progressChanged(startProgress);
        emit statusChanged(tr("Writing note %1 of %2 ...").
                           arg(locale.toString(i + 1),
                               locale.toString(keyCount)));

        synthclone::MIDIData highNote;
        ZoneKey zoneKey = keys[i];
        if (drumKit) {
            highNote = lowNote;
        } else if (i != (keyCount - 1)) {
            highNote = (keys[i + 1].getNote() + zoneKey.getNote()) / 2;
        } else {
            highNote = 127;
        }

        // Write instrument layer data.
        QList<synthclone::Zone *> zones = zoneMap.values(zoneKey);
        int layerCount = zones.count();
        assert(layerCount);
        qStableSort(zones.begin(), zones.end(), VelocityComparer());
        synthclone::Zone *currentZone;
        synthclone::MIDIData lowVelocity = 0;
        for (int j = 0; j < layerCount - 1; j++) {
            emit progressChanged(((static_cast<float>(j) / layerCount) *
                                  difference) + startProgress);
            emit statusChanged(tr("Writing layer %1 of %2 for note %3 of %4 "
                                  "...").
                               arg(locale.toString(j + 1),
                                   locale.toString(layerCount),
                                   locale.toString(i + 1),
                                   locale.toString(keyCount)));

            currentZone = zones[j];
            synthclone::Zone *nextZone = zones[j + 1];

            synthclone::MIDIData highVelocity;
            switch (layerAlgorithm) {
            case LAYERALGORITHM_LINEAR_INTERPOLATION:
                highVelocity = (nextZone->getVelocity() +
                                currentZone->getVelocity()) / 2;
                break;
            case LAYERALGORITHM_MAXIMUM:
                highVelocity = currentZone->getVelocity();
                break;
            case LAYERALGORITHM_MINIMUM:
                highVelocity = nextZone->getVelocity();
                break;
            default:
                assert(false);
            }

            writeMapping(confWriter, currentZone,
                         includedZones.indexOf(currentZone), lowNote, highNote,
                         lowVelocity, highVelocity);
            lowVelocity = highVelocity;
        }

        emit progressChanged(((static_cast<float>(layerCount - 1) /
                               layerCount) * difference) + startProgress);
        emit statusChanged(tr("Writing layer %1 of %2 for note %3 of %4 ...").
                           arg(locale.toString(layerCount),
                               locale.toString(layerCount),
                               locale.toString(i + 1),
                               locale.toString(keyCount)));

        currentZone = zones[layerCount - 1];
        writeMapping(confWriter, currentZone,
                     includedZones.indexOf(currentZone), lowNote, highNote,
                     lowVelocity, 127);

        if (drumKit) {
            lowNote++;
        } else {
            lowNote = highNote + 1;
        }
    }
    confWriter.writeEndElement();
    confWriter.writeEndElement();

    // Write sample envelopes
    confWriter.writeStartElement("SampleEnvelopes");
    confWriter.writeStartElement("Volume");
    writeElement(confWriter, "Decay", "0");
    writeElement(confWriter, "IsActive", "true");
    writeElement(confWriter, "SustainIsActive", drumKit ? "false" : "true");
    writeElement(confWriter, "SustainPos", "0");
    confWriter.writeEndElement();
    confWriter.writeEndElement();

    // Write plugin properties
    confWriter.writeStartElement("PluginProperties");
    confWriter.writeStartElement("OutputRoutings");
    confWriter.writeStartElement("OutputRouting");
    confWriter.writeEndElement();
    confWriter.writeEndElement();
    confWriter.writeEndElement();

    // Finish configuration
    confWriter.writeEndElement();
    confWriter.writeEndDocument();

    archiveWriter.addConfiguration(configuration);

    emit progressChanged(0.0);
    emit statusChanged("Idle.");
}

QString
Target::getInstrumentName() const
{
    return instrumentName;
}

LayerAlgorithm
Target::getLayerAlgorithm() const
{
    return layerAlgorithm;
}

QString
Target::getPath() const
{
    return path;
}

PitchInterpolation
Target::getPitchInterpolation() const
{
    return pitchInterpolation;
}

bool
Target::isDrumKit() const
{
    return drumKit;
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
Target::setInstrumentName(const QString &name)
{
    if (instrumentName != name) {
        instrumentName = name;
        emit instrumentNameChanged(name);
    }
}

void
Target::setLayerAlgorithm(LayerAlgorithm algorithm)
{
    if (layerAlgorithm != algorithm) {
        layerAlgorithm = algorithm;
        emit layerAlgorithmChanged(algorithm);
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
Target::setPitchInterpolation(PitchInterpolation interpolation)
{
    if (pitchInterpolation != interpolation) {
        pitchInterpolation = interpolation;
        emit pitchInterpolationChanged(interpolation);
    }
}

void
Target::writeElement(QXmlStreamWriter &writer, const QString &name,
                     const QString &value)
{
    writer.writeStartElement(name);
    writer.writeCharacters(value);
    writer.writeEndElement();
}

void
Target::writeMapping(QXmlStreamWriter &confWriter, const synthclone::Zone *zone,
                     int index, synthclone::MIDIData noteStart,
                     synthclone::MIDIData noteEnd,
                     synthclone::MIDIData velocityStart,
                     synthclone::MIDIData velocityEnd)
{
    synthclone::MIDIData baseNote = drumKit ? noteStart : zone->getNote();
    confWriter.writeStartElement("NoteOnMapping");
    writeElement(confWriter, "SampleIndex", QString::number(index));
    writeElement(confWriter, "MapVelocityToVolume", "false");
    writeElement(confWriter, "BaseNote", QString::number(baseNote));
    writeElement(confWriter, "NoteStart", QString::number(noteStart));
    writeElement(confWriter, "NoteEnd", QString::number(noteEnd));
    writeElement(confWriter, "VelocityStart", QString::number(velocityStart));
    writeElement(confWriter, "VelocityEnd", QString::number(velocityEnd));
    confWriter.writeEndElement();
}

void
Target::writeSample(ArchiveWriter &archiveWriter, QXmlStreamWriter &confWriter,
                    const synthclone::Zone *zone)
{
    const synthclone::Sample *sample = zone->getWetSample();
    if (! sample) {
        sample = zone->getDrySample();
        assert(sample);
    }

    synthclone::MIDIData note = zone->getNote();
    synthclone::Sample outSample;
    synthclone::SampleInputStream inputStream(*sample);
    synthclone::SampleChannelCount channels = inputStream.getChannels();
    synthclone::SampleOutputStream outputStream
        (outSample, inputStream.getSampleRate(), channels,
         synthclone::SampleStream::TYPE_FLAC,
         synthclone::SampleStream::SUBTYPE_PCM_24);
    synthclone::SampleCopier copier;
    copier.copy(inputStream, outputStream, inputStream.getFrames());
    outputStream.close();
    QString sampleName = tr("%1-%2").arg(note).arg(zone->getVelocity());
    archiveWriter.addSample(sampleName, outSample);

    QString interpolation;
    switch (pitchInterpolation) {
    case PITCHINTERPOLATION_NONE:
        interpolation = "None";
        break;
    case PITCHINTERPOLATION_LINEAR:
        interpolation = "Linear";
        break;
    case PITCHINTERPOLATION_CUBIC:
        interpolation = "Cubic";
        break;
    default:
        assert(false);
    }

    confWriter.writeStartElement("Sample");
    writeElement(confWriter, "Name", sampleName);
    writeElement(confWriter, "BaseNote", QString::number(note));
    writeElement(confWriter, "InterpolationMode", interpolation);
    confWriter.writeEndElement();
}
