/*
 * libsynthclone_hydrogen - Hydrogen target plugin for `synthclone`
 * Copyright (C) 2011-2013 Devin Anderson
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

#include <QtCore/QFileInfo>
#include <QtCore/QLocale>

#include <synthclone/error.h>
#include <synthclone/samplecopier.h>
#include <synthclone/util.h>

#include "target.h"
#include "velocitycomparer.h"

Target::Target(const QString &name, QObject *parent):
    synthclone::Target(name, parent)
{
    layerAlgorithm = LAYERALGORITHM_LINEAR_INTERPOLATION;
    sampleFormat = SAMPLEFORMAT_FLAC_24BIT;
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
    QDir directory(path);
    if (! directory.exists()) {
        message = tr("the path '%1' does not point to an existing directory").
            arg(path);
        throw synthclone::Error(message);
    }
    if (kitName.isEmpty()) {
        message = tr("the kit name is not set");
        throw synthclone::Error(message);
    }
    if (QFileInfo(kitName).fileName() != kitName) {
        message = tr("'%1' is not a valid kit name").arg(kitName);
        throw synthclone::Error(message);
    }
    ArchiveWriter archiveWriter
        (directory.absoluteFilePath(QString("%1.h2drumkit").arg(kitName)));

    // This plugin builds different instruments for every different combination
    // of channel, note, channel pressure, aftertouch, and control values.  If
    // one or more zones have the same above data, then they will become layers
    // in the same instrument, which will be sorted by velocity values.

    emit statusChanged(tr("Building zone map ..."));
    QLocale locale = QLocale::system();
    int zoneCount = zones.count();
    QMultiMap<ZoneKey, const synthclone::Zone *> zoneMap;
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
        zoneMap.insert(ZoneKey(*zone), zone);
    }
    emit progressChanged(0.5);

    QList<ZoneKey> keys = zoneMap.uniqueKeys();
    int instrumentCount = keys.count();
    if (instrumentCount > 1000) {
        message = tr("The current zone list contains %1 potential drum kit "
                     "instruments.  Hydrogen only supports %2 instruments per "
                     "drum kit.  Some instruments will not be generated.").
            arg(locale.toString(instrumentCount), locale.toString(1000));
        emit buildWarning(message);
        instrumentCount = 1000;
    }

    // Write drumkit data.
    QString configuration;
    QXmlStreamWriter confWriter(&configuration);
    confWriter.setAutoFormatting(true);
    confWriter.setAutoFormattingIndent(4);
    confWriter.writeStartElement("drumkit_info");

    writeElement(confWriter, "name", getName());
    writeElement(confWriter, "author", author);
    writeElement(confWriter, "info", info);
    writeElement(confWriter, "license", license);
    confWriter.writeStartElement("instrumentList");

    emit statusChanged(tr("Writing instrument list ..."));
    int layerOverflows = 0;
    for (int i = 0; i < instrumentCount; i++) {
        float startProgress =
            ((static_cast<float>(i) / instrumentCount) * 0.5) + 0.5;
        float endProgress =
            ((static_cast<float>(i + 1) / instrumentCount) * 0.5) + 0.5;
        float difference = endProgress - startProgress;

        emit progressChanged(startProgress);
        emit statusChanged(tr("Writing instrument %1 of %2 ...").
                           arg(locale.toString(i + 1),
                               locale.toString(instrumentCount)));

        // Write instrument data.
        confWriter.writeStartElement("instrument");
        writeElement(confWriter, "id", QString::number(i));
        writeElement(confWriter, "name",
                     tr("Instrument #%1").arg(locale.toString(i + 1)));
        writeElement(confWriter, "Attack", "0.0");
        writeElement(confWriter, "Decay", "0.0");
        writeElement(confWriter, "Sustain", "1.0");
        writeElement(confWriter, "Release", "1000.0");
        writeElement(confWriter, "filterActive", "false");
        writeElement(confWriter, "filterCutoff", "0.0");
        writeElement(confWriter, "filterResonance", "0.0");
        writeElement(confWriter, "gain", "1.0");
        writeElement(confWriter, "isMuted", "false");
        writeElement(confWriter, "muteGroup", "-1");
        writeElement(confWriter, "pan_L", "1.0");
        writeElement(confWriter, "pan_R", "1.0");
        writeElement(confWriter, "randomPitchFactor", "0.0");
        writeElement(confWriter, "isStopNote", "false");

        QList<const synthclone::Zone *> zones = zoneMap.values(keys[i]);
        int layerCount = zones.count();
        assert(layerCount);
        if (layerCount > 16) {
            layerCount = 16;
            layerOverflows++;
        }
        qStableSort(zones.begin(), zones.end(), VelocityComparer());
        float lowVelocity = 0.0;

        // Write instrument layer data.
        for (int j = 0; j < layerCount - 1; j++) {
            emit progressChanged(((static_cast<float>(j) / layerCount) *
                                  difference) + startProgress);
            emit statusChanged(tr("Writing layer %1 of %2 for instrument %3 "
                                  "of %4 ...").
                               arg(locale.toString(j + 1),
                                   locale.toString(layerCount),
                                   locale.toString(i + 1),
                                   locale.toString(instrumentCount)));

            const synthclone::Zone *currentZone = zones[j];
            const synthclone::Zone *nextZone = zones[j + 1];

            float highVelocity;
            switch (layerAlgorithm) {
            case LAYERALGORITHM_LINEAR_INTERPOLATION:
                // 127.0 * 2.0 = 254.0
                highVelocity = (static_cast<float>(nextZone->getVelocity()) +
                                currentZone->getVelocity()) / 254.0;
                break;
            case LAYERALGORITHM_MAXIMUM:
                highVelocity = currentZone->getVelocity() / 127.0;
                break;
            case LAYERALGORITHM_MINIMUM:
                highVelocity = nextZone->getVelocity() / 127.0;
                break;
            default:
                assert(false);
            }
            try {
                writeLayer(archiveWriter, confWriter, i, j, lowVelocity,
                           highVelocity, currentZone);
            } catch (...) {
                emit progressChanged(0.0);
                emit statusChanged("Idle.");
                throw;
            }
            lowVelocity = highVelocity;
        }
        emit progressChanged(((static_cast<float>(layerCount - 1) /
                               layerCount) * difference) + startProgress);
        emit statusChanged(tr("Writing layer %1 of %2 for instrument %3 of "
                              "%4 ...").
                           arg(locale.toString(layerCount),
                               locale.toString(layerCount),
                               locale.toString(i + 1),
                               locale.toString(instrumentCount)));

        try {
            writeLayer(archiveWriter, confWriter, i, layerCount - 1,
                       lowVelocity, 1.0, zones[layerCount - 1]);
        } catch (...) {
            emit progressChanged(0.0);
            emit statusChanged("Idle.");
            throw;
        }
        confWriter.writeEndElement();
    }
    confWriter.writeEndElement();

    confWriter.writeEndElement();
    confWriter.writeEndDocument();

    // Add the configuration to the archive writer.
    QByteArray configurationBytes = configuration.toLocal8Bit();
    ArchiveHeader header(QString("%1/drumkit.xml").arg(kitName),
                         configurationBytes.count());
    archiveWriter.writeHeader(header);
    archiveWriter.writeData(configurationBytes);

    if (layerOverflows) {
        message = tr("%1 instruments contained more than %2 layers.  Hydrogen "
                     "only supports %2 layers per instrument.  Some layers "
                     "were not be generated.").
            arg(locale.toString(layerOverflows), locale.toString(16));
        emit buildWarning(message);
    }

    emit progressChanged(0.0);
    emit statusChanged("Idle.");
}

QString
Target::getAuthor() const
{
    return author;
}

QString
Target::getInfo() const
{
    return info;
}

QString
Target::getKitName() const
{
    return kitName;
}

LayerAlgorithm
Target::getLayerAlgorithm() const
{
    return layerAlgorithm;
}

QString
Target::getLicense() const
{
    return license;
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

void
Target::setAuthor(const QString &author)
{
    if (this->author != author) {
        this->author = author;
        emit authorChanged(author);
    }
}

void
Target::setInfo(const QString &info)
{
    if (this->info != info) {
        this->info = info;
        emit infoChanged(info);
    }
}

void
Target::setKitName(const QString &name)
{
    if (kitName != name) {
        kitName = name;
        emit kitNameChanged(name);
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
Target::setLicense(const QString &license)
{
    if (this->license != license) {
        this->license = license;
        emit licenseChanged(license);
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
Target::writeElement(QXmlStreamWriter &writer, const QString &name,
                     const QString &value)
{
    writer.writeStartElement(name);
    writer.writeCharacters(value);
    writer.writeEndElement();
}

void
Target::writeLayer(ArchiveWriter &archiveWriter, QXmlStreamWriter &confWriter,
                   int instrument, int layer, float lowVelocity,
                   float highVelocity, const synthclone::Zone *zone)
{
    synthclone::SampleStream::SubType sampleStreamSubType;
    synthclone::SampleStream::Type sampleStreamType;
    switch (sampleFormat) {
    case SAMPLEFORMAT_AIFF_8BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_S8;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AIFF_16BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_16;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AIFF_24BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_24;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AIFF_32BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_32;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AIFF_32BIT_FLOAT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_FLOAT;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AU_8BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_S8;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_AU_16BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_16;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_AU_24BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_24;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_AU_32BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_32;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_AU_32BIT_FLOAT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_FLOAT;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_FLAC_8BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_S8;
        sampleStreamType = synthclone::SampleStream::TYPE_FLAC;
        break;
    case SAMPLEFORMAT_FLAC_16BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_16;
        sampleStreamType = synthclone::SampleStream::TYPE_FLAC;
        break;
    case SAMPLEFORMAT_FLAC_24BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_24;
        sampleStreamType = synthclone::SampleStream::TYPE_FLAC;
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

    QString sampleName = tr("instrument%1-layer%2.%3").
        arg(instrument + 1).arg(layer + 1).
        arg(synthclone::getSampleFilenameExtension(sampleStreamType));
    const synthclone::Sample *sample = zone->getWetSample();
    if (! sample) {
        sample = zone->getDrySample();
        assert(sample);
    }

    synthclone::Sample outSample;
    synthclone::SampleInputStream inputStream(*sample);
    synthclone::SampleChannelCount channels = inputStream.getChannels();
    synthclone::SampleOutputStream outputStream
        (outSample, inputStream.getSampleRate(), channels, sampleStreamType,
         sampleStreamSubType);
    synthclone::SampleCopier copier;
    copier.copy(inputStream, outputStream, inputStream.getFrames());
    outputStream.close();

    // Write sample to archive.
    QString path = outSample.getPath();
    QFileInfo info(path);
    assert(info.exists());
    assert(info.isFile());
    ArchiveHeader header(QString("%1/%2").arg(kitName, sampleName),
                         info.size());
    archiveWriter.writeHeader(header);
    QFile file(path);
    if (! file.open(QFile::ReadOnly)) {
        QString message = tr("could not open '%1': %2").
            arg(path, file.errorString());
        throw synthclone::Error(message);
    }
    for (;;) {
        QByteArray data = file.read(8192);
        if (data.isEmpty()) {
            break;
        }
        archiveWriter.writeData(data);
    }

    confWriter.writeStartElement("layer");
    writeElement(confWriter, "filename", sampleName);
    writeElement(confWriter, "min", QString::number(lowVelocity));
    writeElement(confWriter, "max", QString::number(highVelocity));
    writeElement(confWriter, "gain", "1.0");
    writeElement(confWriter, "pitch", "0.0");
    confWriter.writeEndElement();
}
